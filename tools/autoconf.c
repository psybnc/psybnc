/* $Id: autoconf.c,v 1.2 2005/06/04 18:01:31 hisi Exp $ */
/************************************************************************
 *   psybnc2.3, tools/autoconf.c
 *   Copyright (C) 2001 the most psychoid  and
 *                      the cool lam3rz IRC Group, IRCnet
 *			http://www.psychoid.lam3rz.de
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 1, or (at your option)
 *   any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#ifndef lint
static char rcsid[] = "@(#)$Id: autoconf.c,v 1.2 2005/06/04 18:01:31 hisi Exp $";
#endif

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <arpa/inet.h>
#include <ctype.h>
#include <signal.h>
#include <errno.h>
#include "config.h"
#include "src/p_version.h"

int ssl=0;
int dns=0;
int bigendian=0;
int ipv6=0;
int off_time=0;
int snpr=0;
int needsock=0;
int extenv=0;
int needbind=0;
char bigopt[100];
char ipv6opt[100];
char snopt[100];
char socklib[100];
char snbuf[100];
char os[200];
char env[200];
char sunosopt[200];
char timeopt[100];
char sslopt[400];
char ssllib[400];
char sslbin[400];
char dnsopt[100];
char dnslib[100];

#define DN " 2>tools/.chk "

int checkcmp()
{
    FILE *cmp;
    char buf[400];
    cmp=fopen("tools/.chk","r");
    if(cmp!=NULL)
    {
	if(fgets(buf,sizeof(buf),cmp)!=NULL)
	{
	    fclose(cmp);
	    return 0x1;
	}    
	fclose(cmp);
    }
    return 0x0;
}

int fexists(char *fname)
{
    FILE *fn;
    int rc=0;
    fn=fopen(fname,"r");
    if(fn!=NULL)
    {
	fclose(fn);
	rc=1;
    }
    return rc;
}

int getos()
{
    FILE *ss;
    system("uname -a >tools/sys");
    ss=fopen("tools/sys","r");
    if(ss==NULL)
    {
	strcpy(os,"Can't guess");
	return 0x1;
    }
    else
    {
	fgets(os,sizeof(os),ss);
	sscanf(os,"%s\n",os);
    }
    return 0x0;
}

int checksocklib()
{
    int rc;
    unlink("tools/.chk");
    system("gcc tools/chksock.c -o tools/chksock" DN);
    return checkcmp();
}

int checkresolve()
{
    int rc;
    unlink("tools/.chk");
    system("gcc tools/chkresolv.c -o tools/chkresolv -lresolv" DN);
    return checkcmp();
}

int checktime()
{
    int rc;
    unlink("tools/.chk");
    system("gcc tools/chktime.c -o tools/chktime" DN);
    return checkcmp();
}

int checkbind()
{
    int rc;
    unlink("tools/.chk");
    system("gcc tools/chkbind.c -lnsl -ldl -lsocket -o tools/chkbind" DN);
    return checkcmp();
}

int checkenv()
{
    int rc;
    unlink("tools/.chk");
    system("gcc tools/chkenv.c -o tools/chkenv" DN);
    return checkcmp();
}

int checkssl()
{
    int rc;
#ifdef SSLPATH
    char mbuf[300];
    char sysbuf[1024+strlen(SSLPATH)];
#endif
    unlink("tools/.chk");
#ifdef SSLPATH
    strcpy(mbuf,SSLPATH);
    if(mbuf[strlen(mbuf)-1]!='/')
	strcat(mbuf,"/");
    strcpy(sysbuf,"gcc tools/chkssl.c -I");
    strcat(sysbuf,mbuf);
    strcat(sysbuf,"include -L");
    strcat(sysbuf,mbuf);
    strcat(sysbuf,"lib -lssl -lcrypto -o tools/chkssl ");
    strcat(sysbuf,DN);
    system(sysbuf);
#else
    system("gcc tools/chkssl.c -I/usr/local/ssl/include -L/usr/local/ssl/lib -lssl -lcrypto -o tools/chkssl" DN);
#endif
    return checkcmp();
}

int checkipv6usable()
{
    system("tools/chkipv6 >tools/.chk");
    return checkcmp();
}

int checkipv6()
{
    int rc;
    unlink("tools/.chk");
    if(strstr(os,"SunOS")==os)
    {
	if(needsock)
	{	
	    if(needbind)
		system("gcc tools/chkipv6.c -o tools/chkipv6 -lsocket -lnsl -ldl -lbind -DSUNOS " DN);
	    else
		system("gcc tools/chkipv6.c -o tools/chkipv6 -lsocket -lnsl -ldl -DSUNOS " DN);
	}
        else
    	    system("gcc tools/chkipv6.c -DSUNOS -o tools/chkipv6 " DN);
    } else {
	if(needsock)
	{		
	    if(needbind)
		system("gcc tools/chkipv6.c -o tools/chkipv6 -lsocket -lnsl -ldl -lbind " DN);
	    else
		system("gcc tools/chkipv6.c -o tools/chkipv6 -lsocket -lnsl -ldl " DN);
	}
	else
	    system("gcc tools/chkipv6.c -o tools/chkipv6 " DN);
    }
    return checkcmp();
}

int checkendian()
{
    char xyz[]="\x01\x02\x03\x04";
    unsigned long *lp;    
    unsigned long result;
    lp=(unsigned long *)xyz;
    result=*lp;
    if(result==67305985) return 0x1;
    return 0x0;
}

int main()
{
    FILE *makefile;
    FILE *config;
    FILE *sslrnd;
    FILE *uran;
#ifndef SSLPATH
    char *sslblist[]={
	"/bin/openssl",
	"/usr/bin/openssl",
	"/usr/sbin/openssl",
	"/usr/local/bin/openssl",
	"/usr/local/ssl/bin/openssl",
	NULL
    };
#endif
    int sslin=0;
    int provi=0;
    unsigned char rchar,orchar;
    int ic;
#ifdef SSLPATH
    char mbuf[strlen(SSLPATH)+30];
    char ibuf[strlen(SSLPATH)+20];
#else
    char mbuf[200];
    char ibuf[200];
#endif
    bigopt[0]=0;
    sunosopt[0]=0;
    timeopt[0]=0;
    ipv6opt[0]=0;
    ssllib[0]=0;
    socklib[0]=0;
    snbuf[0]=0;
    snopt[0]=0;
    env[0]=0;
    sslbin[0]=0;
    sslopt[0]=0;
    dnsopt[0]=0;
    dnslib[0]=0;
    printf("System:");
    getos();    
    printf(" %s\n",os);
    if(strstr(os,"SunOS")==os) strcpy(sunosopt,"-DSUNOS ");
    printf("Socket Libs: ");
    fflush(stdout);
    needsock=checksocklib();
    if(needsock!=0)
    {
	/* in the case of external socket-libs, sol8 needs to check for lbind */
	strcpy(socklib,"-lnsl -ldl -lsocket ");
	needbind=checkbind();
	if(needbind!=0)
	{
	    strcat(socklib,"-lbind ");
	    printf("External, -lbind required.\n");
	} else
	    printf("External.\n");
    }
    else
	printf("Internal.\n");
    fflush(stdout);
    printf("Environment: ");
    fflush(stdout);
    extenv=checkenv();
    if(extenv!=0)
    {
	printf("No internal Routines.\n");
	strcpy(env," src/bsd-setenv.o ");
    }
    else
    {
	printf("Internal.\n");
    }
    fflush(stdout);
    printf("Time-Headers: ");
    fflush(stdout);
    off_time=checktime();
    if(off_time!=0)
    {
	printf("in time.h and sys/time.h\n");
	strcpy(timeopt," -DNOSYSTIME ");
    }
    else
    {
	printf("in sys/time.h.\n");
    }
    fflush(stdout);
    bigendian=checkendian();
    if(bigendian)
    {
	printf("Byte order: Big Endian.\n");
	strcpy(bigopt,"-DBIGENDIAN ");
    }
    else
	printf("Byte order: Low Endian.\n");
    printf("IPv6-Support: ");
    fflush(stdout);
    ipv6=checkipv6();
    if(ipv6==0)
    {
#ifdef AF_INET6
	ic=checkipv6usable();
	if(ic==0)
	{
	    printf("Yes.\n");
	    strcpy(ipv6opt,"-DIPV6 ");
	} else {
	    printf("Yes, general support. But no interface configured.\n");
	    ipv6=1;
	}
#else
	printf("No.\n");
	ipv6=1;
#endif
    }
    else
	printf("No.\n");
    printf("async-DNS-Support: ");
    fflush(stdout);
    dns=checkresolve();
    if(dns==0)
    {
    	printf("Yes.\n");
	strcpy(dnslib,"-lresolv ");
    }
    else
    {
	strcpy(dnsopt,"-DBLOCKDNS ");
	printf("No, using blocking DNS.\n");
    }
    printf("SSL-Support: ");
    fflush(stdout);
    ssl=checkssl();
    if(ssl==0)
    {
#ifndef SSLPATH
	while(sslblist[sslin]!=NULL)
	{
	    if(fexists(sslblist[sslin]))
		break;
	    sslin++;
	}
	if(sslblist[sslin]==NULL)
	{
	    printf("Yes, but no openssl binary found.");
	    ssl=-1;
	} else {
	    strcpy(sslbin,sslblist[sslin]);
	    printf("Yes.\n");
	    strcpy(sslopt,"-DHAVE_SSL ");
	    strcpy(ssllib,"-L/usr/local/ssl/lib -lssl -lcrypto ");
	}
#else
	if(strlen(SSLPATH)+13<sizeof(mbuf))
	{
	    strcpy(mbuf,SSLPATH);
	    if(mbuf[strlen(mbuf)-1]!='/')
		strcat(mbuf,"/");
	    strcpy(ibuf,mbuf);
	    strcat(mbuf,"bin/openssl");
	    if(fexists(mbuf)==0)
	    {
		printf("Yes, but no openssl binary found in \"%s\".",SSLPATH);
		ssl=-1;
	    } else {
		strcpy(sslbin,mbuf);
		printf("Yes.\n");
		strcpy(sslopt,"-DHAVE_SSL ");
		strcpy(ssllib,"-L");
		strcat(ssllib,ibuf);
		strcat(ssllib,"lib -lssl -lcrypto ");
	    }
	} else {
	    printf("Possibly. But the configured path \"%s\" is too long.\n",SSLPATH);
	}	
#endif
    }
    else
	printf("No openssl found. Get openssl at www.openssl.org\n");
    config=fopen("/psybnc/config.h","r");
    if(config!=NULL)
    {
	fclose(config);
	printf("Found Provider-Config - Using this for compilation\n");
	provi=1;
    }
    printf("Creating Makefile\n");
    makefile=fopen("makefile.out","w");
    if(makefile==NULL)
    {
	printf("Can't create makefile.out .. aborting\n");
	exit(0x1);
    }
    fprintf(makefile,"CC	= gcc\n");
    fprintf(makefile,"SRC	= src/\n");
#ifdef BOUNDCHECK
    fprintf(makefile,"CFLAGS  = -O -fbounds-checking -fno-builtin\n");
#else
    fprintf(makefile,"CFLAGS  = -O\n");
#endif
    fprintf(makefile,"LIBS	= -lm %s %s %s\n",socklib,ssllib,dnslib); /* math lib needed for snprintf of ap */
    if(ssl==0)
#ifdef SSLPATH
	fprintf(makefile,"INCLUDE = -I./src/ -I. -I%sinclude\n",SSLPATH);
#else
	fprintf(makefile,"INCLUDE = -I./src/ -I. -I/usr/local/ssl/include\n");
#endif
    else
	fprintf(makefile,"INCLUDE = -I./src/ -I.\n");
    fprintf(makefile,"OBJS	= src/psybnc.o src/match.o src/p_client.o src/p_crypt.o src/p_dcc.o src/p_hash.o src/p_idea.o src/p_inifunc.o src/p_link.o src/p_log.o src/p_memory.o src/p_network.o src/p_parse.o src/p_peer.o src/p_server.o src/p_socket.o src/p_string.o src/p_sysmsg.o src/p_userfile.o src/p_uchannel.o src/p_script.o src/p_topology.o src/p_intnet.o src/p_blowfish.o src/p_translate.o src/p_coredns.o src/snprintf.o %s\n",env);
    if(provi==0)
	fprintf(makefile,"DEFINE	= -DHAVE_CONFIG %s%s%s%s%s%s\n",sunosopt,bigopt,ipv6opt,timeopt,sslopt,dnsopt);
    else
	fprintf(makefile,"DEFINE	= -DHAVE_PROV_CONFIG %s%s%s%s%s%s\n",sunosopt,bigopt,ipv6opt,timeopt,sslopt,dnsopt);
    fprintf(makefile,"TARGET	= psybnc\n");
    fprintf(makefile,"\n");
    fprintf(makefile,"all:	$(OBJS)\n");
    fprintf(makefile,"	$(CC) -o $(TARGET) $(CFLAGS) $(OBJS) $(LIBS)\n");
    fprintf(makefile,"	@strip $(TARGET)\n");
    if(ssl==0)
    {
	if(!fexists("key/psybnc.cert.pem")) /* only create, if not exist */
	{
	    mkdir("key",0700);
	    fprintf(makefile,"	@echo \"*** GENERATING SSL-KEYS FROM CERTIFICATE **\"\n");
	    fprintf(makefile,"	@echo \"* You will be prompted for Cert-Contents  *\"\n");
	    fprintf(makefile,"	@echo \"*  This Infos will be used only for SSL   *\"\n");
	    fprintf(makefile,"	@echo \"* Alter the informations to your values   *\"\n");
	    fprintf(makefile,"	@echo \"* for the sake of correct Cert-Checking   *\"\n");
	    fprintf(makefile,"	@echo \"*******************************************\"\n");
	    fprintf(makefile,"	@echo \"Generating certificate request .. \"\n");
	    fprintf(makefile,"	@%s req -new -config src/ssl.cnf -out key/psybnc.req.pem \\\n",sslbin);
	    fprintf(makefile,"      	-keyout key/psybnc.key.pem -nodes\n");
	    fprintf(makefile,"	@echo \"Generating self-signed certificate .. \"\n");
	    fprintf(makefile,"	@%s req -x509 -days 365 -in key/psybnc.req.pem \\\n",sslbin);
    	    fprintf(makefile,"       	-key key/psybnc.key.pem -out key/psybnc.cert.pem\n");
	    fprintf(makefile,"	@echo \"Generating fingerprint ..\"\n");
	    fprintf(makefile,"	@%s x509 -subject -dates -fingerprint -noout \\\n",sslbin);
	    fprintf(makefile,"		-in key/psybnc.cert.pem\n");
	}
	if(!fexists("src/ssl.rnd"))
	{
	    sslrnd=fopen("src/ssl.rnd","w");
	    if(sslrnd!=NULL)
	    {
		uran=fopen("/dev/urandom","r");
		if(uran==NULL)
		{
		    printf("No urandom Device found. Random-Seed possibly could be reconstructable.\n");
		    srand(time(NULL)+getpid());
		    for(ic=0;ic<2048;ic++)
		    {
			while(rchar==orchar)
			    rchar=rand()%256;
			fputc(rchar,sslrnd);    					
		    }
		} else {
		    for(ic=0;ic<2048;ic++)
		    {
			rchar=fgetc(uran);
			fputc(rchar,sslrnd);
		    }	
		    fclose(uran);
		}
		printf("Random Seed created.\n");
		fclose(sslrnd);	
	    } else {
		printf("Cannot create Random-seed-file. Aborting.\n");
		exit(0x1);
	    }
	}
    }
    fprintf(makefile,"	@echo " APPNAME APPVER "-%s ready. Please read the README before you run psybnc.\n",os);
    fprintf(makefile,"\n");
    fprintf(makefile,"include ./targets.mak\n");
    fclose(makefile);
    exit(0x0);
}
