/* $Id: convconf.c,v 1.3 2005/06/04 18:01:32 hisi Exp $ */
/************************************************************************
 *   psybnc2.2.2, tools/convconf.c
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
static char rcsid[] = "@(#)$Id: convconf.c,v 1.3 2005/06/04 18:01:32 hisi Exp $";
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

int cofile(FILE *infile, FILE *outfile, char *fname,int ppo)
{
    char buf[2048];
    char section[200];
    char *pt,*pt2;
    while(fgets(buf,sizeof(buf),infile))
    {
	pt=strchr(buf,'\n');
	if(pt!=NULL) *pt=0;
	if (!(strchr(buf,'#')==buf))
	{ 
	if (*buf=='[')
	{
	    pt=buf+1;
	    pt2=strchr(pt,']');
	    if(pt2!=NULL)
	    {
		*pt2=0;
		if(strlen(pt)<sizeof(section))
		    strcpy(section,pt);
		else
		    section[0]=0;
	    }
	} else {
	    if(*buf!=';' && strlen(buf)>2)
		if(strstr(buf,"PORT=")==buf && ppo==1)
		{
		   pt=strchr(buf,'=');
		   pt++;
		   fprintf(outfile,"%s.%s.PORT1=%s\n",fname,section,pt);
		   fprintf(outfile,"%s.%s.HOST1=*\n",fname,section);
		} else {
		   fprintf(outfile,"%s.%s.%s\n",fname,section,buf);
		}
	}
	}
    }
    return 0x0;
}

int convertlist(char *section, char *parampattern, char *fname, char *fil, FILE *outfile)
{
    char xbuf[4096];
    char buf[2048];
    char entry[2000];
    int cnt=0;
    char *pt;
    FILE *listfile;
    listfile=fopen(fil,"r");
    if(listfile==NULL) return 0x0;
    while(fgets(buf,sizeof(buf),listfile))
    {
	pt=strchr(buf,'\n');
	if(pt!=NULL) *pt=0;
	pt=strchr(buf,':');
	if(pt!=NULL) *pt=';';
	if(strchr(buf,'#')!=buf)
	{
	    sprintf(entry,parampattern,cnt);
	    fprintf(outfile,"%s.%s.%s=%s\n",fname,section,entry,buf);
	    cnt++;
	}
    }
    fclose(listfile);
    unlink(fil);
    return 0x0;
}

int convertlists(int uid, FILE *outfile)
{
    char buf[400];
    char fname[200];
    sprintf(fname,"USER%d",uid);
    sprintf(buf,"USER%d.OP",uid);
    convertlist("OP","ENTRY%d",fname,buf,outfile);
    sprintf(buf,"USER%d.ASK",uid);
    convertlist("ASK","ENTRY%d",fname,buf,outfile);
    sprintf(buf,"USER%d.BAN",uid);
    convertlist("BAN","ENTRY%d",fname,buf,outfile);
    sprintf(buf,"USER%d.DCC",uid);
    convertlist("DCC","ENTRY%d",fname,buf,outfile);
    sprintf(buf,"USER%d.LGI",uid);
    convertlist("LGI","ENTRY%d",fname,buf,outfile);
    sprintf(buf,"USER%d.AOP",uid);
    convertlist("AOP","ENTRY%d",fname,buf,outfile);
    sprintf(buf,"USER%d.TRA",uid);
    convertlist("TRA","ENTRY%d",fname,buf,outfile);
    sprintf(buf,"USER%d.ENC",uid);
    convertlist("ENC","ENTRY%d",fname,buf,outfile);
    return 0x0;
}

int main(int argc,char **argv)
{
    int i;
    FILE *handle;
    FILE *fconfif;
    char file[200];
    char sn[200];
    system("mv *.LOG log 2>/dev/null");
    system("mv *.LOG.old log 2>/dev/null");
    system("mv *.log log 2>/dev/null");
    system("mv *.log.old log 2>/dev/null");
    system("mv *.TRL log 2>/dev/null");
    system("mv *.MOTD motd 2>/dev/null");
    system("mv *.MOTD.old motd 2>/dev/null");
    system("rm -rf *.INI.old 2>/dev/null");
    handle=fopen("psbnc.ini","r");
    if(handle!=NULL)
    {
	fclose(handle);
	printf("psyBNC2.1 -> psyBNC2.2 Conversion Utility\nChecking old Ini-Files\n");
	fconfif=fopen("psybnc.conf","a");
	if(fconfif==NULL)
	{
	    printf("Can't open psybnc.conf, aborting\n");
	    exit(0x1); /* error, break making. */
	}
	handle=fopen("psbnc.ini","r");
	cofile(handle,fconfif,"PSYBNC",1);
	fclose(handle);
	unlink("psbnc.ini");
	handle=fopen("LINKS.INI","r");
	if(handle!=NULL)
	{
	    cofile(handle,fconfif,"LINKS",0);
	    fclose(handle);
	    unlink("LINKS.INI");
	}
	for(i=1;i<1000;i++)
	{
	    sprintf(file,"USER%d.INI",i);
	    sprintf(sn,"USER%d",i);
	    handle=fopen(file,"r");
	    if(handle!=NULL)
	    {
		cofile(handle,fconfif,sn,0);
		fclose(handle);
		convertlists(i,fconfif);
		unlink(file);
	    }	
	}
	convertlist("HOSTALLOWS","ENTRY%d","PSYBNC","psbnc.hosts",fconfif);
	fclose(fconfif);
    } else {
	fconfif=fopen("psybnc.conf","r");
	if(fconfif!=NULL)
	{
	    printf("Using existent configuration File.\n");
	    fclose(fconfif);
	} else {
	    fconfif=fopen("psybnc.conf","w");
	    fprintf(fconfif,"PSYBNC.SYSTEM.PORT1=31337\n");
	    fprintf(fconfif,"PSYBNC.SYSTEM.HOST1=*\n");
	    fprintf(fconfif,"PSYBNC.HOSTALLOWS.ENTRY0=*;*\n");
	    fclose(fconfif);
	}
    }
    fconfif=fopen("config.h","r");
    if(fconfif==NULL)
    {
	fconfif=fopen("config.h","w");
	fprintf(fconfif,"/* Empty Config File */\n");
	fclose(fconfif);
    } else {
	fclose(fconfif);
    }
    exit(0x0);
}
