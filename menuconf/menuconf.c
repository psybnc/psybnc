/* $Id: menuconf.c,v 1.3 2005/06/04 17:54:17 hisi Exp $ */
/************************************************************************
 *   psybnc2.3.1, menuconf/menuconf.c
 *   Copyright (C) 2002 the most psychoid  and
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

/* Note: The GUI-Routines are made by Savio Lam (lam836@cs.cuhk.hk)
 *       Some changes were made to those routines to give back the
 *       choosen parameter into an internal variable and to allow
 *       Delete and New for User and Linklist
 */

#ifndef lint
static char rcsid[] = "@(#)$Id: menuconf.c,v 1.3 2005/06/04 17:54:17 hisi Exp $";
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
#include "src/p_version.h"

int ap_snprintf(char *dest,size_t size, char *format, ...);
#define snprintf ap_snprintf

/* internals */

char selection[4096];
int param;
int true=-1;
int false=0;

int delb=0;
int newb=0;
int confbase=0;

/* Compiling Menu - Options */

struct comopt {
    int supenc;
    int supenctype;
    int suptrans;
    int supint;
    int suptrl;
    int suplinks;
    int dccfiles;
    int dccchat;
    int mduser;
    int maxuser;
    int maxcon;
    int supscript;
    int supoidentd;
    int supsharebans;
    int supnetwork;
    int supproxy;
    int blockdns;
    int anonymous;
    int dynamic;
    int loglevel;
    int partychannel;
    int supsslsec;
    char ctcpversion[100];
    char sslpath[200];
};

struct comopt *comoptions;

char bm1[200];
char bm2[200];
char bm3[200];

/* string copy with len and zero delimit */

char *strmncpy(char *dest, char *source, unsigned int len)
{
    char *pt;
    if(dest==NULL | source==NULL) return NULL;
    pt=strncpy(dest,source,len-1);
    if(strlen(source)+1>=len)
	dest[len-1]=0;
    return pt;
}

/* Bouncerconfig - Options */

const char * const bouncemenu[]={
    "301:",
    "Listening Ports --->",
    "302:",
    "Links --->",
    "303:",
    bm1,
    "304:",
    "Host Allows --->",
    "305:",
    "Users --->",
    "306:",
    bm2,
    "307:",
    bm3
};

/* Main Menu - Options */

const char * const mainmenu[]={
    "101:",
    "Compiling Options --->",
    "102:",
    "Bouncer-Config --->",
    "103:",
    "Read the main log",
    "104:",
    "--- ",
    "105:",
    "Read-Me for Version " APPVER,
    "106:",
    "FAQ for Version " APPVER,
    "107:",
    "Changes for Version " APPVER
};

/*
 * parameters for the compiling menu
 */

char cm1[100];
char cm2[100];
char cm3[100];
char cm4[100];
char cm5[100];
char cm6[100];
char cm7[100];
char cm8[100];
char cm9[100];
char cmA[100];
char cmB[100];
char cmC[100];
char cmD[100];
char cmE[100];
char cmF[100];
char cmG[100];
char cmH[100];
char cmI[100];
char cmJ[100];
char cmK[100];
char cmL[140];
char cmM[140];
char cmN[140];

/* the compiling option menu */

const char * const compilingmenu[]={
    "201:",
    cm1,
    "202:",
    cm2,
    "203:",
    cm3,
    "204:",
    cm4,
    "205:",
    cm5,
    "206:",
    cm6,
    "207:",
    cm7,
    "208:",
    cm8,
    "209:",
    cm9,
    "210:",
    cmA,
    "211:",
    cmB,
    "212:",
    cmC,
    "213:",
    cmD,
    "214:",
    cmE,
    "215:",
    cmF,
    "216:",
    cmG,
    "217:",
    cmH,
    "218:",
    cmI,
    "219:",
    cmJ,
    "220:",
    cmK,
    "221:",
    cmL,
    "222:",
    cmM,
    "223:",
    cmN
};

/* link menu and items */

char lm1[100];
char lm2[100];
char lm3[100];
char lm4[100];
char lm5[100];
char lm6[100];
char lm7[100];
char lm8[100];

const char * const linkmenu[]={
    "501:",
    lm1,
    "502:",
    lm2,
    "503:",
    lm3,
    "504:",
    lm4,
    "505:",
    lm5
};

/* vhost menu and items */

char vm1[200];
char vm2[4096];

const char * const vhostmenu[]={
    "601:",
    vm1,
    "602:",
    vm2
};

/* user menu and items */

char um1[200];
char um2[200];
char um3[200];
char um4[200];
char um5[200];
char um6[200];
char um7[200];
char um8[200];
char um9[200];
char umA[200];
char umB[200];
char umC[200];
char umD[200];
char umE[200];
char umF[200];
char umG[200];

const char * const usermenu[]={
    "701:",
    um1,
    "702:",
    um2,
    "703:",
    um3,
    "704:",
    um4,
    "705:",
    um5,
    "706:",
    um6,
    "707:",
    um7,
    "708:",
    um8,
    "709:",
    um9,
    "710:",
    umA,
    "711:",
    umB,
    "712:",
    umC,
    "713:",
    umD,
    "714:",
    umE,
    "715:",
    umF,
    "716:",
    umG
};

/* choosenlink is a helper for saving the last selected link */

int choosenlink;

/* choosenuser is another helper */

int choosenuser;

/* provi is 1, if the host offers a provider configuration */

int provi=0;

/* conf-file routines */

#define INIFILE "PSYBNC"

char configfile[]="psybnc.conf";

/* Data Definitions */

struct stringarray {
    char *entry;
    struct stringarray *next;
};

struct stringarray *conf;

char value[400];


/* externals */

/*
 * Display text from a file in a dialog box.
 */
int
dialog_textbox (const char *title, const char *file, int height, int width);

/*
 * Displays a menu and waits for choose
 */

extern int
dialog_menu (const char *title, const char *prompt, int height, int width,
		int menu_height, const char *current, int item_no,
		const char * const * items);
/*
 * Display a dialog box for inputing a string
 */
extern int
dialog_inputbox (const char *title, const char *prompt, int height, int width,
		 const char *init);
/*
 * Display a dialog box with a list of options that can be turned on or off
 * The `flag' parameter is used to select between radiolist and checklist.
 */
extern int
dialog_checklist (const char *title, const char *prompt, int height, int width,
	int list_height, int item_no, const char * const * items, int flag);

/* start and end of dialogues */

extern int init_dialog();
extern int end_dialog();

unsigned long *pmalloc(unsigned long size)
{
    unsigned long *rc;
    if (!(rc=(unsigned long *)malloc(size)))
    {
	exit(0x1);
    }
    memset(rc,0x0,size);
    return rc;
}

/* reset the config cache */

int resetconfig()
{
    struct stringarray *wconf,*owconf;
    wconf=conf;
    while(wconf!=NULL)
    {
	if (wconf->entry!=NULL)
	    free(wconf->entry);
	owconf=wconf;
	wconf=wconf->next;
	free(owconf);
    }
    conf=NULL;
    return 0x0;
}

/* read the config file */

int readconfig()
{
    FILE *handle;
    struct stringarray *wconf;
    char inistring[400];
    char *pt;
    wconf=conf;
    if(conf!=NULL) resetconfig();
    handle=fopen(configfile,"r");
    if(handle==NULL) return -1;
    while(fgets(inistring,sizeof(inistring),handle))
    {
	pt=strchr(inistring,'\n');
	if(pt!=NULL) *pt=0;
	if(wconf==NULL)
	{
	    wconf=(struct stringarray *)pmalloc(sizeof(struct stringarray));
	    conf=wconf;
	} else {
	    wconf->next=(struct stringarray *)pmalloc(sizeof(struct stringarray));
	    wconf=wconf->next;
	}
	wconf->entry=(char *)pmalloc(strlen(inistring)+1);
	strmncpy(wconf->entry,inistring,strlen(inistring)+1);
    }
    fclose(handle);
    return 0x0;
}

/* write the config file */

int flushconfig()
{
    FILE *handle;
    struct stringarray *wconf;
    handle=fopen(configfile,"w");
    wconf=conf;
    while(wconf)
    {
	if(wconf->entry!=NULL) {
	   if(strlen(wconf->entry)>1)
	       fprintf(handle,"%s\n",wconf->entry);
	}
	wconf=wconf->next;
    }
    fclose(handle);
    return 0x0;
}

/* erases a section from the config */

int clearsectionconfig(char *pattern)
{
    struct stringarray *wconf,*xconf;
    wconf=conf;
    xconf=conf;
    while(wconf)
    {
	if(wconf->entry!=NULL)
	{
	    if(strstr(wconf->entry,pattern)==wconf->entry)
	    {
		if(wconf==conf)
		{
		    conf=wconf->next;
		    xconf=conf;
		} else {
		    xconf->next=wconf->next;
		}
		free(wconf->entry);
		free(wconf);
		wconf=xconf;
	    }
	}
	xconf=wconf;
	wconf=wconf->next;
    }
}

/* get entry from conf-file */

int getini(char *section, char *param,char *inidat)
{
   char ppuf[400];
   struct stringarray *wconf;
   char *po;
   wconf=conf;
   memset(value,0x0,sizeof(value));
   ap_snprintf(ppuf,sizeof(ppuf),"%s.%s.%s=",inidat,section,param);
   while (wconf!=NULL)
   {
	if(wconf->entry!=NULL)
	{
	    po = strstr(wconf->entry,ppuf);
	    if (po == wconf->entry) {
		po = po + strlen(ppuf);
		ap_snprintf(value,sizeof(value),"%s",po);
		return 0x0; /* found, returning */
	    }
	}
	wconf=wconf->next;
   }
   /* not found */
   return -2;
}

/* write entry to configcache or delete if if data = NULL */

int writeini(char *section, char *param, char *inidat, char *data)
{
    char ppuf[200];
    char spuf[200];
    char buf[2048];
    char tx[20];
    char *po;
    int wasinsection;
    char *data_p;
    struct stringarray *wconf,*xconf,*sectconf;
    wconf=conf;
    data_p = data;
    if (data_p != NULL)
       if (strlen(data) == 0) data_p = NULL;
    wasinsection = 0;
    ap_snprintf(ppuf,sizeof(ppuf),"%s.%s.%s=",inidat,section,param);
    ap_snprintf(spuf,sizeof(spuf),"%s.%s.",inidat,section);
    if(data_p!=NULL)
	ap_snprintf(buf,sizeof(buf),"%s%s",ppuf,data_p);
    xconf=conf;
    sectconf=conf;
    while (wconf) {
      if(wconf->entry!=NULL)
      {
	   po = strstr(wconf->entry,spuf);
           if (po == wconf->entry)
	   {
	      sectconf=xconf; /* save last entry of section */
	      wasinsection = 1; /* we had been in the section */
	      po = strstr(wconf->entry,ppuf);
	      if (po == wconf->entry) {
		 if(data_p==NULL)
		 {
		    if(wconf==conf)
		    {
			conf=wconf->next;
			xconf=conf;
		    } else {
			xconf->next=wconf->next;
		    }
		    free(wconf->entry);
		    free(wconf);
		    wconf=xconf;
		    return 0x0;
		 } else {
		    free(wconf->entry);
		    wconf->entry=(char *)pmalloc(strlen(buf)+1);
		    strmncpy(wconf->entry,buf,strlen(buf)+1);
		    return 0x0;
		 }
	      }
	   }
      }
      xconf=wconf;
      wconf=wconf->next;
    }
    if(data_p==NULL) return 0x0;
    if(wasinsection==0)
    {
	xconf->next=(struct stringarray *)pmalloc(sizeof(struct stringarray));
	xconf=xconf->next;
    } else {
	xconf=sectconf;
	wconf=(struct stringarray *)pmalloc(sizeof(struct stringarray));
	wconf->next=xconf->next;
	xconf->next=wconf;
	xconf=wconf;
    }
    xconf->entry=(char *)pmalloc(strlen(buf)+1);
    strmncpy(xconf->entry,buf,strlen(buf)+1);
    return 0x0;
}

int lastfree;

int countconfentries(char *section, char *entry, char *fname)
{
    int cnt=0;
    char buf[200];
    int i,rc;
    lastfree=999;
    for (i=confbase;i<1000;i++)
    {
	if(strchr(section,'%')!=NULL)
	{
	    ap_snprintf(buf,sizeof(buf),section,i);
	    rc=getini(buf,entry,fname);
	} else
	if(strchr(entry,'%')!=NULL)
	{
	    ap_snprintf(buf,sizeof(buf),entry,i);
	    rc=getini(section,buf,fname);
	} else
	if(strchr(fname,'%')!=NULL)
	{
	    ap_snprintf(buf,sizeof(buf),fname,i);
	    rc=getini(section,entry,buf);
	}
	if(rc==0) cnt++; else if(lastfree>i) lastfree=i;
    }
    return cnt;
}

/*
 * createvariablemenu()
 * creates a menu driven by config parameters
 */

int createvariablemenu(char *header, char *info, char *section,
		       char *entry, char *entry2, char *fname,
		       int offset, char *dpattern)
{
    int entries=countconfentries(section,entry,fname);
    int ubnd=entries*2;
    char pbuf[entries+1][200];
    char dbuf[entries+1][200];
    char vvalue[300];
    char buf[200];
    char *varmenu[ubnd+2];
    int i,cnt=0,xcnt=0;
    int rc,rc2;
    pbuf[0][0]=0;
    dbuf[0][0]=0;
    for(i=confbase;i<1000;i++)
    {
	vvalue[0]=0;
	value[0]=0;
	if(strchr(section,'%')!=NULL)
	{
	    ap_snprintf(buf,sizeof(buf),section,i);
	    rc=getini(buf,entry,fname);
	    if(rc==0)
		strmncpy(vvalue,value,sizeof(vvalue));
	    else
		vvalue[0]=0;
	    rc2=getini(buf,entry2,fname);
	} else
	if(strchr(entry,'%')!=NULL)
	{
	    ap_snprintf(buf,sizeof(buf),entry,i);
	    rc=getini(section,buf,fname);
	    if(rc==0)
		strmncpy(vvalue,value,sizeof(vvalue));
	    else
		vvalue[0]=0;
	    ap_snprintf(buf,sizeof(buf),entry2,i);
	    rc2=getini(section,buf,fname);
	} else
	if(strchr(fname,'%')!=NULL)
	{
	    ap_snprintf(buf,sizeof(buf),fname,i);
	    rc=getini(section,entry,buf);
	    if(rc==0)
		strmncpy(vvalue,value,sizeof(vvalue));
	    else
		vvalue[0]=0;
	    rc2=getini(section,entry2,buf);
	}
	if(rc2!=0) value[0]=0;
        if(rc==0)
        {
	    ap_snprintf(pbuf[cnt],sizeof(pbuf[cnt]),"%d:",i);
	    ap_snprintf(dbuf[cnt],sizeof(dbuf[cnt]),dpattern,i,value, vvalue);
	    varmenu[xcnt++]=pbuf[cnt];
	    varmenu[xcnt++]=dbuf[cnt];
	    cnt++;
	}
    }
    if(entries==0)
    {
	varmenu[0]=pbuf[0]; /* in some circumstances, psyBNC crashed because */
	varmenu[1]=dbuf[0]; /* of a missing first parameter */
    }
    newb=1;delb=1;
    init_dialog();
    rc= dialog_menu( header,info,21,75,12,"",entries,(const char * const *)&varmenu);
    end_dialog();
    newb=0;delb=0;
    return rc;
}
/*
 * readconfigh()
 * reads the current config.h
 */

int readconfigh()
{
    FILE *config1;
    FILE *config2;
    char ebuf[20];
    char buf[200];
    char *pt;
    config1=fopen("/psybnc/config.h","r");
    if(config1!=NULL)
    {
	provi=1;
	config2=config1;
    } else {
	config2=fopen("config.h","r");
	if(config2==NULL)
	{
	    /* standard defintions, all on except oIdentd */
	    comoptions->supenc=true;
	    comoptions->supenctype=false;
	    comoptions->suptrans=true;
	    comoptions->supint=true;
	    comoptions->suptrl=true;
	    comoptions->suplinks=true;
	    comoptions->dccfiles=true;
	    comoptions->dccchat=true;
	    comoptions->mduser=true;
	    comoptions->maxuser=50;
	    comoptions->maxcon=99;
	    comoptions->supscript=false;
	    comoptions->supoidentd=false;
	    comoptions->blockdns=true;
	    comoptions->supnetwork=false;
	    comoptions->supproxy=false;
	    comoptions->anonymous=false;
	    comoptions->dynamic=false;
	    comoptions->loglevel=0;
	    comoptions->partychannel=false;
	    comoptions->ctcpversion[0]=0;
	    comoptions->sslpath[0]=0;
	    comoptions->supsslsec=2; /* save public keys, and save certs */
	    return -1;
	}
    }
    comoptions->ctcpversion[0]=0;
    system("clear");
    while(fgets(buf,sizeof(buf),config2))
    {
	if (strstr(buf,"define")==buf+1 || strstr(buf,"DEFINE")==buf+1)
	{
	    if(strstr(buf," CRYPT")!=NULL)
		comoptions->supenc=true;
	    if(strstr(buf," IDEA")!=NULL)
		comoptions->supenctype=true;
	    if(strstr(buf," TRANSLATE")!=NULL)
		comoptions->suptrans=true;
	    if(strstr(buf," INTNET")!=NULL)
		comoptions->supint=true;
	    if(strstr(buf," TRAFFICLOG")!=NULL)
		comoptions->suptrl=true;
	    if(strstr(buf," LINKAGE")!=NULL)
		comoptions->suplinks=true;
	    if(strstr(buf," DCCFILES")!=NULL)
		comoptions->dccfiles=true;
	    if(strstr(buf," DCCCHAT")!=NULL)
		comoptions->dccchat=true;
	    if(strstr(buf," MULTIUSER")!=NULL)
		comoptions->mduser=true;
	    pt=strstr(buf," MAXUSER ");
	    if(pt!=NULL)
	    {
		pt+=9;
		comoptions->maxuser=atoi(pt);
	    }
	    pt=strstr(buf," MAXCONN ");
	    if(pt!=NULL)
	    {
		pt+=9;
		comoptions->maxcon=atoi(pt);
	    }
	    pt=strstr(buf," CTCPVERSION ");
	    if(pt!=NULL)
	    {
		pt+=13;
		while(*pt==' ') pt++;
		if(*pt=='"') pt++;
		strmncpy(comoptions->ctcpversion,pt,sizeof(comoptions->ctcpversion));
		pt=strchr(comoptions->ctcpversion,'"');
		if(pt!=NULL) *pt=0;
	    }
	    pt=strstr(buf," SSLPATH ");
	    if(pt!=NULL)
	    {
		pt+=9;
		while(*pt==' ') pt++;
		if(*pt=='"') pt++;
		strmncpy(comoptions->sslpath,pt,sizeof(comoptions->sslpath));
		pt=strchr(comoptions->sslpath,'"');
		if(pt!=NULL) *pt=0;
	    }
	    if(strstr(buf," SCRIPTING")!=NULL)
		comoptions->supscript=true;
	    if(strstr(buf," OIDENTD")!=NULL)
		comoptions->supoidentd=true;
	    if(strstr(buf," BLOCKDNS")!=NULL)
		comoptions->blockdns=true;
	    if(strstr(buf," NETWORK")!=NULL)
		comoptions->supnetwork=true;
	    if(strstr(buf," PROXYS")!=NULL)
		comoptions->supproxy=true;
	    if(strstr(buf," ANONYMOUS")!=NULL)
		comoptions->anonymous=true;
	    if(strstr(buf," DYNAMIC")!=NULL)
		comoptions->dynamic=true;
	    if(strstr(buf," PARTYCHANNEL")!=NULL)
		comoptions->partychannel=true;
	    pt=strstr(buf," LOGLEVEL ");
	    if(pt!=NULL)
	    {
		pt+=10;
		comoptions->loglevel=atoi(pt);
	    }
	    pt=strstr(buf," SSLSEC ");
	    if(pt!=NULL)
	    {
		pt+=8;
		comoptions->supsslsec=atoi(pt);
	    }
	}
    }
    fclose(config2);
}

/*
 * writeconfig()
 * writes the current config.h
 */

int writeconfig()
{
    FILE *config;
    if(getuid()==0 && provi==1) /* root may save it */
	config=fopen("/psybnc/config.h","w");
    else
    {
	if(provi==1) return 0x0;
	config=fopen("config.h","w");
    }
    if(config==NULL)
	return 0x0;
    fprintf(config,"/*\n");
    fprintf(config," * Configuration file for " APPNAME ", created by menuconf\n");
    fprintf(config," */\n\n");
    if(comoptions->supenc)
    {
	fprintf(config,"/* Encryption */\n\n#define CRYPT\n\n");
	if (comoptions->supenctype)
	    fprintf(config,"/* Encryption Type*/\n\n#define IDEA\n\n");
	else
	    fprintf(config,"/* Encryption Type*/\n\n#define BLOWFISH\n\n");
    }
    if(comoptions->suptrans)
	fprintf(config,"/* Allow Translation */\n\n#define TRANSLATE\n\n");
    if(comoptions->supint)
	fprintf(config,"/* Allow internal network */\n\n#define INTNET\n\n");
    if(comoptions->suptrl)
	fprintf(config,"/* Allow traffic logging */\n\n#define TRAFFICLOG\n\n");
    if(comoptions->suplinks)
	fprintf(config,"/* Allow linkage of bouncers */\n\n#define LINKAGE\n\n");
    if(comoptions->dccfiles)
	fprintf(config,"/* Allow the dcc File-Functions */\n\n#define DCCFILES\n\n");
    if(comoptions->dccchat)
	fprintf(config,"/* Pipe dcc Chats */\n\n#define DCCCHAT\n\n");
    if(comoptions->mduser)
	fprintf(config,"/* Allow to add more users */\n\n#define MULTIUSER\n\n");
    fprintf(config,"/* Number of max. Users */\n\n#define MAXUSER %d\n\n",comoptions->maxuser);
    fprintf(config,"/* Number of max. Connections per User */\n\n#define MAXCONN %d\n\n",comoptions->maxcon);
    if(comoptions->supscript)
	fprintf(config,"/* Allow the usage of scripts */\n\n#define SCRIPTING\n\n");
    if(comoptions->supoidentd)
	fprintf(config,"/* Support oIdentd */\n\n#define OIDENTD\n\n");
    if(comoptions->blockdns)
	fprintf(config,"/* Use blocking DNS */\n\n#define BLOCKDNS\n\n");
    if(comoptions->supnetwork)
	fprintf(config,"/* Allow multiple irc connections per user */\n\n#define NETWORK\n\n");
    if(comoptions->supproxy)
	fprintf(config,"/* Allow Proxy Support */\n\n#define PROXYS\n\n");
    if(comoptions->anonymous)
	fprintf(config,"/* Set " APPNAME " anonymous */\n\n#define ANONYMOUS\n\n");
    if(comoptions->dynamic)
	fprintf(config,"/* Connections arent permanent */\n\n#define DYNAMIC\n\n");
    fprintf(config,"/* The logging level */\n\n#define LOGLEVEL %d\n\n",comoptions->loglevel);
    if(comoptions->partychannel)
	fprintf(config,"/* We still use the 2.1.1 stylish partychannel */\n\n#define PARTYCHANNEL\n\n");
    if(comoptions->ctcpversion[0]!=0)
	fprintf(config,"/* The ctcp Version reply */\n\n#define CTCPVERSION \"%s\"\n\n",comoptions->ctcpversion);
    if(comoptions->sslpath[0]!=0)
    {
	if(comoptions->sslpath[strlen(comoptions->sslpath)-1]!='/')
	    strcat(comoptions->sslpath,"/");
	fprintf(config,"/* The Path to SSL */\n\n#define SSLPATH \"%s\"\n\n",comoptions->sslpath);
    }
    fprintf(config,"/* SSL-Security */\n\n#define SSLSEC %d\n\n",comoptions->supsslsec);
    fclose(config);
    return 0x1;
}

/*
 * setupcompilingoptions()
 * sets the texts from the set options
 */

int setupcompilingoptions()
{
    if(comoptions->supenc)
    {
	strcpy(cm1,"[X] Support Encryption");
	if(comoptions->supenctype==0)
	    strcpy(cm2,"    Encryption Type: Blowfish");
	else
	    strcpy(cm2,"    Encryption Type: IDEA");
    }
    else
    {
	strcpy(cm1,"[ ] Support Encryption");
	strcpy(cm2,"    Encryption Type: None");
    }
    if(comoptions->suptrans)
	strcpy(cm3,"[X] Support Translation");
    else
	strcpy(cm3,"[ ] Support Translation");
    if(comoptions->supint)
	strcpy(cm4,"[X] Support internal Network");
    else
	strcpy(cm4,"[ ] Support internal Network");
    if(comoptions->suptrl)
	strcpy(cm5,"[X] Support Traffic-Logging");
    else
	strcpy(cm5,"[ ] Support Traffic-Logging");
    if(comoptions->suplinks)
	strcpy(cm6,"[X] Support Linkage");
    else
	strcpy(cm6,"[ ] Support Linkage");
    if(comoptions->dccfiles)
	strcpy(cm7,"[X] Support DCC Files");
    else
	strcpy(cm7,"[ ] Support DCC Files");
    if(comoptions->dccchat)
	strcpy(cm8,"[X] Support DCC Chat");
    else
	strcpy(cm8,"[ ] Support DCC Chat");
    if(comoptions->mduser)
    {
	strcpy(cm9,"    Mode: Multiuser");
    }
    else
    {
	strcpy(cm9,"    Mode: Singleuser");
	comoptions->maxuser=1;
    }
    ap_snprintf(cmA,sizeof(cmA),"    Maximum Users: %d",comoptions->maxuser);
    ap_snprintf(cmB,sizeof(cmB),"    Maximum Connections: %d",comoptions->maxcon);
    if(comoptions->supscript)
	strcpy(cmC,"[X] Support Scripting - NOT RELIABLE -");
    else
	strcpy(cmC,"[ ] Support Scripting - NOT RELIABLE -");
    if(comoptions->supoidentd)
	strcpy(cmD,"[X] Support oIdentd");
    else
	strcpy(cmD,"[ ] Support oIdentd");
    if(comoptions->blockdns)
	strcpy(cmE,"[ ] Use asynchroneous resolving - EXPERIMENTAL -");
    else
	strcpy(cmE,"[X] Use asynchroneous resolving - EXPERIMENTAL -");
    if(comoptions->supnetwork)
	strcpy(cmF,"[X] Support multiple IRC-Networks");
    else
	strcpy(cmF,"[ ] Support multiple IRC-Networks");
    if(comoptions->supproxy)
	strcpy(cmG,"[X] Support Proxy Usage");
    else
	strcpy(cmG,"[ ] Support Proxy Usage");
    if(comoptions->anonymous)
	strcpy(cmH,"[X] Anonymous Bouncer Usage");
    else
	strcpy(cmH,"[ ] Anonymous Bouncer Usage");
    if(comoptions->dynamic)
	strcpy(cmI,"[X] None permanent IRC-Connections");
    else
	strcpy(cmI,"[ ] None permanent IRC-Connections");
    switch(comoptions->loglevel)
    {
	case 0:
	    strcpy(cmJ,"    Loglevel: Errors, Warnings and Infos");
	    break;
	case 1:
	    strcpy(cmJ,"    Loglevel: Errors and Warnings");
	    break;
	case 2:
	    strcpy(cmJ,"    Loglevel: Errors only");
	    break;
    }
    if(comoptions->partychannel)
	strcpy(cmK,"[X] Use the 2.1.1 compatible Partychannel");
    else
	strcpy(cmK,"[ ] Use the 2.1.1 compatible Partychannel");
    if(comoptions->ctcpversion[0]==0)
	strcpy(cmL,"    Version Reply: None");
    else
    {
	ap_snprintf(cmL,sizeof(cmL),"    Version Reply: %s",comoptions->ctcpversion);
    }
    if(comoptions->sslpath[0]==0)
	strcpy(cmM,"    SSL-Path: /usr/local/ssl");
    else
    {
	ap_snprintf(cmM,sizeof(cmM),"    SSL-Path: %s",comoptions->sslpath);
    }
    switch(comoptions->supsslsec)
    {
	case 0:
	    strcpy(cmN,"    SSL-SecLevel: None");
	    break;
	case 1:
	    strcpy(cmN,"    SSL-SecLevel: Check Certs");
	    break;
	case 2:
	    strcpy(cmN,"    SSL-SecLevel: Check Certs and Keys (NOT IMPLEMENTED)");
	    break;
	default:
	    strcpy(cmN,"    SSL-SecLevel: Undefined");
	    break;
    }
    return 0x0;
}


/* helper for extracting the option number */

int extractparam()
{
    char *pt;
    pt=strchr(selection,':');
    if(pt!=NULL)
    {
	*pt=0;
	param=atol(selection);
	*pt=':';
    } else {
	param=0;
    }
}

/*
 * help()
 * displays a help text for an option
 */

int help()
{
    FILE *check;
    char head[100];
    char fil[100];
    char *pt;
    int rc;
    pt=strchr(selection,':');
    if(pt!=NULL)
    {
	pt++;
	strmncpy(head,pt,sizeof(head));
	ap_snprintf(fil,sizeof(fil),"menuconf/help/h%d.txt",param);
	ap_snprintf(selection,sizeof(selection),"%d:",param);
	check=fopen(fil,"r");
	if(check==NULL)
	    return -1;
	fclose(check);
	rc=dialog_textbox (head, fil, 20, 75);
	return rc;
    }
    return 0x0;
}

/*
 * compilingoptions()
 * displays the options for compilation
 */

int compilingoptions()
{
    int rc;
    char lchoice[200];
    char buf[20];
    strcpy(lchoice,"201:");
    readconfigh();
    while(1)
    {
	selection[0]=0;
	setupcompilingoptions();
	init_dialog();
	if(provi)
	{
	    if(getuid()==0)
		rc=dialog_menu( "Compiling Options","This is the compiling section of the " APPNAME " Configuration. Your Host runs a provider-config. You are root, changes will be saved in the Provider-wide psybnc-Config.",
		    21,75,12,lchoice,23,compilingmenu);
	    else
		rc=dialog_menu( "Compiling Options","This is the compiling section of the " APPNAME " Configuration. Your Host runs a provider-config. You can`t change the compiling Options.",
		    21,75,12,lchoice,23,compilingmenu);
	}
	else
	    rc=dialog_menu( "Compiling Options","This is the compiling section of the " APPNAME " Configuration. Please select an item and press enter to change settings.",
		    21,75,12,lchoice,23,compilingmenu);
	extractparam();
	end_dialog();

	if(rc==0) /* choose */
	{
	    strmncpy(lchoice,selection,sizeof(lchoice));
	    switch(param)
	    {
		case 201:
		    comoptions->supenc=!comoptions->supenc;
		    break;
		case 202:
		    comoptions->supenctype=!comoptions->supenctype;
		    break;
		case 203:
		    comoptions->suptrans=!comoptions->suptrans;
		    break;
		case 204:
		    comoptions->supint=!comoptions->supint;
		    break;
		case 205:
		    comoptions->suptrl=!comoptions->suptrl;
		    break;
		case 206:
		    comoptions->suplinks=!comoptions->suplinks;
		    break;
		case 207:
		    comoptions->dccfiles=!comoptions->dccfiles;
		    break;
		case 208:
		    comoptions->dccchat=!comoptions->dccchat;
		    break;
		case 209:
		    comoptions->mduser=!comoptions->mduser;
		    break;
		case 210:
		    ap_snprintf(buf,sizeof(buf),"%d",comoptions->maxuser);
		    selection[0]=0;rc=0;
		    while(atoi(selection)==0 && rc==0)
		    {
		        rc=dialog_inputbox ( "Maximum Users", "Enter a value", 21, 75,
		                         buf);
		    }
		    if(rc==0) comoptions->maxuser=atoi(selection);
		    break;
		case 211:
		    ap_snprintf(buf,sizeof(buf),"%d",comoptions->maxcon);
		    selection[0]=0;rc=0;
		    while(atoi(selection)==0 && rc==0)
		    {
		        rc=dialog_inputbox ( "Maximum Connections", "Enter a value", 21, 75,
		                         buf);
		    }
		    if(rc==0) comoptions->maxcon=atoi(selection);
		    break;
		case 212:
		    comoptions->supscript=!comoptions->supscript;
		    break;
		case 213:
		    comoptions->supoidentd=!comoptions->supoidentd;
		    break;
		case 214:
		    comoptions->blockdns=!comoptions->blockdns;
		    break;
		case 215:
		    comoptions->supnetwork=!comoptions->supnetwork;
		    break;
		case 216:
		    comoptions->supproxy=!comoptions->supproxy;
		    break;
		case 217:
		    comoptions->anonymous=!comoptions->anonymous;
		    break;
		case 218:
		    comoptions->dynamic=!comoptions->dynamic;
		    break;
		case 219:
		    comoptions->loglevel++;
		    if (comoptions->loglevel>2) comoptions->loglevel=0;
		    break;
		case 220:
		    comoptions->partychannel=!comoptions->partychannel;
		    break;
		case 221:
		    rc=dialog_inputbox ( "CTCP Version Reply", "Enter a reply Text", 21, 75,
		                         APPNAME " " APPVER " by the most psychoid");
		    strmncpy(comoptions->ctcpversion,selection,sizeof(comoptions->ctcpversion));
		    break;
		case 222:
		    if(comoptions->sslpath[0]==0)
			rc=dialog_inputbox ( "Path to OpenSSL", "Enter the Path of your OpenSSL-Installation", 21, 75,
		                         "/usr/local/ssl");
		    else
			rc=dialog_inputbox ( "Path to OpenSSL", "Enter the Path of your OpenSSL-Installation", 21, 75,
		                         comoptions->sslpath);
		    strmncpy(comoptions->sslpath,selection,sizeof(comoptions->sslpath));
		    break;
		case 223:
		    comoptions->supsslsec++;
		    if (comoptions->supsslsec>2) comoptions->supsslsec=0;
		    break;
	    }
	}
	if(rc==1)
	{
	    writeconfig();
	    return 0x0; /* exit */
	}
	if(rc==2)
	{
	    help();
	    strmncpy(lchoice,selection,sizeof(lchoice));
	}
    }
}

/*
 * setupuseroptions()
 * setups a user mask
 */

int setupuseroptions()
{
    char buf[200];
    int rc;
    ap_snprintf(buf,sizeof(buf),"USER%d",choosenuser);
    /* single options */
    strcpy(um1,"Login: ");
    rc=getini("USER","LOGIN",buf);
    if(rc==0)
	if(strlen(um1)+strlen(value)<sizeof(um1)) strcat(um1,value);
    else
	strcat(um1,"Choose");
    strcpy(um2,"Nick: ");
    rc=getini("USER","NICK",buf);
    if(rc==0)
	if(strlen(um2)+strlen(value)<sizeof(um2)) strcat(um2,value);
    else
	strcat(um2,"Choose");
    strcpy(um3,"Username: ");
    rc=getini("USER","USER",buf);
    if(rc==0)
	if(strlen(um3)+strlen(value)<sizeof(um3)) strcat(um3,value);
    else
	strcat(um3,"Choose");
    strcpy(um4,"Password: ");
    rc=getini("USER","PASS",buf);
    if(rc==0)
	if(strlen(um4)+strlen(value)<sizeof(um4)) strcat(um4,value);
    else
	strcat(um4,"Choose");
    strcpy(um5,"VHost: ");
    rc=getini("USER","VHOST",buf);
    if(rc==0)
	if(strlen(um5)+strlen(value)<sizeof(um5)) strcat(um5,value);
    else
	strcat(um5,"Choose");
    strcpy(um6,"Rights: ");
    rc=getini("USER","RIGHTS",buf);
    if(rc==0)
    {
	if(atoi(value)==1)
	    strcat(um6,"Admin");
	else
	    strcat(um6,"User");
    }
    else
	strcat(um6,"User");
    strcpy(um7,"Relaying over Link #: ");
    rc=getini("USER","VLINK",buf);
    if(rc==0)
	if(atoi(value)==0)
	    strcat(um7,"None");
	else
	    if(strlen(um7)+strlen(value)<sizeof(um7)) strcat(um7,value);
    else
	strcat(um7,"None");
    strcpy(um8,"Using Proxy: ");
    rc=getini("USER","PROXY",buf);
    if(rc==0)
	if(strlen(um8)+strlen(value)<sizeof(um8)) strcat(um8,value);
    else
	strcat(um8,"None");
    strcpy(um9,"Proxy Port: ");
    rc=getini("USER","PPORT",buf);
    if(rc==0)
    {
	if(atoi(value)>0)
	    if(strlen(um9)+strlen(value)<sizeof(um9)) strcat(um9,value);
	else
	    strcat(um9,"None");
    }
    else
	strcat(um9,"None");
    strcpy(umA,"Network User of User #: ");
    rc=getini("USER","PARENT",buf);
    if(rc==0)
    {
	if(atoi(value)>0)
	    if(strlen(umA)+strlen(value)<sizeof(umA)) strcat(umA,value);
	else
	    strcat(umA,"None");
    }
    else
	strcat(umA,"None");
    strcpy(umB,"User marked as quitted: ");
    rc=getini("USER","QUITTED",buf);
    if(rc==0)
    {
	if(atoi(value)!=0)
	    strcat(umB,"Yes");
	else
	    strcat(umB,"No");
    }
    else
	strcat(umB,"No");
    strcpy(umC,"Auto-Accept DCC-Files: ");
    rc=getini("USER","AUTOGETDCC",buf);
    if(rc==0)
    {
	if(atoi(value)!=0)
	    strcat(umC,"Yes");
	else
	    strcat(umC,"No");
    }
    else
	strcat(umC,"No");
    strcpy(umD,"Receive Systemmessages: ");
    rc=getini("USER","SYSMSG",buf);
    if(rc==0)
    {
	if(atoi(value)!=0)
	    strcat(umD,"Yes");
	else
	    strcat(umD,"No");
    }
    else
	strcat(umD,"No");
    strcpy(umE,"Network name of Network User: ");
    rc=getini("USER","NETWORK",buf);
    if(rc==0)
	if(strlen(umE)+strlen(value)<sizeof(umE)) strcat(umE,value);
    else
	strcat(umE,"None");
    strcpy(umF,"Reset LastLog");
    strcpy(umG,"Servers --->");
    return 0x0;
}

/*
 * useredit()
 * shows the useredit mask
 */

int useredit()
{
    int rc;
    char lchoice[200];
    char header[200];
    char buf[200];
    char ebuf[200];
    char dbuf[400];
    char *pt;
    int pa,rrc;
    choosenuser=param;
    strcpy(lchoice,"701:");
    while(1)
    {
	ap_snprintf(header,sizeof(header),"User #%d",choosenuser);
	selection[0]=0;
	setupuseroptions();
	init_dialog();
	rc=dialog_menu( header,"Choose the option to change and press ENTER.",
		    21,75,12,lchoice,16,usermenu);
	extractparam();
	end_dialog();

	if(rc==0) /* choose */
	{
	    ap_snprintf(buf,sizeof(buf),"USER%d",choosenuser);
	    strmncpy(lchoice,selection,sizeof(lchoice));
	    switch(param)
	    {
		case 701:
		    rrc=getini("USER","LOGIN",buf);
		    if(rrc!=0)
			strcpy(value,"");
		    rrc=dialog_inputbox ( "User Login", "Enter the Login Name", 21, 75,
		                         value);
		    if(rrc==0 && strlen(selection)>0)
			writeini("USER","LOGIN",buf,selection);
		    break;
		case 702:
		    rrc=getini("USER","NICK",buf);
		    if(rrc!=0)
			strcpy(value,"");
		    rrc=dialog_inputbox ( "User Nick", "Enter the Nickname", 21, 75,
		                         value);
		    if(rrc==0 && strlen(selection)>0)
			writeini("USER","NICK",buf,selection);
		    break;
		case 703:
		    rrc=getini("USER","USER",buf);
		    if(rrc!=0)
			strcpy(value,"");
		    rrc=dialog_inputbox ( "User Realname", "Enter the Real Name", 21, 75,
		                         value);
		    if(rrc==0 && strlen(selection)>0)
			writeini("USER","USER",buf,selection);
		    break;
		case 704:
		    rrc=getini("USER","PASS",buf);
		    if(rrc!=0)
			strcpy(value,"");
		    if(*value=='=' || *value=='+')
		    {
			strcpy(value,"");
			rrc=dialog_inputbox ( "User Password - Cannot Edit, Password was encrypted. Enter a new Password in cleartext.", "Enter the User Password", 21, 75,
		                         value);
		    }
		    else
			rrc=dialog_inputbox ( "User Password", "Enter the User Password", 21, 75,
		                         value);
		    if(rrc==0 && strlen(selection)>0)
			writeini("USER","PASS",buf,selection);
		    break;
		case 705:
		    rrc=getini("USER","VHOST",buf);
		    if(rrc!=0)
			strcpy(value,"");
		    rrc=dialog_inputbox ( "User VHost", "Enter the VHost for the User (IP or Hostname)", 21, 75,
		                         value);
		    if(rrc==0)
			writeini("USER","VHOST",buf,selection);
		    break;
		case 706:
		    rrc=getini("USER","RIGHTS",buf);
		    if(rrc!=0)
			pa=0;
		    else
			pa=atoi(value);
		    pa++;
		    if(pa==2) pa=0;
		    ap_snprintf(value,sizeof(value),"%d",pa);
		    writeini("USER","RIGHTS",buf,value);
		    break;
		case 707:
		    rrc=getini("USER","VLINK",buf);
		    if(rrc!=0)
			strcpy(value,"0");
		    rrc=dialog_inputbox ( "User Relay over Link", "Enter the Link Number to relay", 21, 75,
		                         value);
		    if(rrc==0)
		    {
			pa=atoi(selection);
			ap_snprintf(value,sizeof(value),"%d",pa);
			writeini("USER","VLINK",buf,value);
		    }
		    break;
		case 708:
		    rrc=getini("USER","PROXY",buf);
		    if(rrc!=0)
			strcpy(value,"");
		    rrc=dialog_inputbox ( "User Proxy", "Enter the Proxy for the User (IP or Hostname)", 21, 75,
		                         value);
		    if(rrc==0)
			writeini("USER","PROXY",buf,selection);
		    break;
		case 709:
		    rrc=getini("USER","PPORT",buf);
		    if(rrc!=0)
			strcpy(value,"0");
		    rrc=dialog_inputbox ( "User Proxy Port", "Enter the Proxy Port to relay", 21, 75,
		                         value);
		    if(rrc==0)
		    {
			pa=atoi(selection);
			ap_snprintf(value,sizeof(value),"%d",pa);
			writeini("USER","PPORT",buf,value);
		    }
		    break;
		case 710:
		    rrc=getini("USER","PARENT",buf);
		    if(rrc!=0)
			strcpy(value,"0");
		    rrc=dialog_inputbox ( "User Parent User", "Enter the Number of the Parent User (defines this User as network User)", 21, 75,
		                         value);
		    if(rrc==0)
		    {
			pa=atoi(selection);
			ap_snprintf(value,sizeof(value),"%d",pa);
			writeini("USER","PARENT",buf,value);
		    }
		    break;
		case 711:
		    rrc=getini("USER","QUITTED",buf);
		    if(rrc!=0)
			pa=0;
		    else
			pa=atoi(value);
		    pa++;
		    if(pa==2) pa=0;
		    ap_snprintf(value,sizeof(value),"%d",pa);
		    writeini("USER","QUITTED",buf,value);
		    break;
		case 712:
		    rrc=getini("USER","AUTOGETDCC",buf);
		    if(rrc!=0)
			pa=0;
		    else
			pa=atoi(value);
		    pa++;
		    if(pa==2) pa=0;
		    ap_snprintf(value,sizeof(value),"%d",pa);
		    writeini("USER","AUTOGETDCC",buf,value);
		    break;
		case 713:
		    rrc=getini("USER","SYSMSG",buf);
		    if(rrc!=0)
			pa=0;
		    else
			pa=atoi(value);
		    pa++;
		    if(pa==2) pa=0;
		    ap_snprintf(value,sizeof(value),"%d",pa);
		    writeini("USER","SYSMSG",buf,value);
		    break;
		case 714:
		    rrc=getini("USER","NETWORK",buf);
		    if(rrc!=0)
			strcpy(value,"");
		    rrc=dialog_inputbox ( "User Network", "Enter the Network Identifier for the User (IP or Hostname)", 21, 75,
		                         value);
		    if(rrc==0)
			writeini("USER","NETWORK",buf,selection);
		    break;
		case 715:
		    writeini("USER","LASTLOG",buf,"0");
		    break;
		case 716:
		    rrc=0;
		    while(rrc>=0)
		    {
		       confbase=1;
		       rrc=createvariablemenu("Servers", "Please choose the Server-Entry you want to edit/delete or choose new to add a new entry. Press ESCAPE to return to the User.", "SERVERS", 
		       "PORT%d", "SERVER%d", buf, 20000, "#%d :%s %s");
		       extractparam();
		       switch(rrc)
		       {
		           case 1:
			       param=lastfree;
			   case 0:
			       strcpy(dbuf,"");
			       ap_snprintf(ebuf,sizeof(ebuf),"SERVER%d",param);
			       rrc=getini("SERVERS",ebuf,buf);
			       if(rrc==0)
			       {
			           strmncpy(dbuf,value,sizeof(dbuf));
				   if(strlen(dbuf)+1<sizeof(dbuf)) strcat(dbuf," ");
			           ap_snprintf(ebuf,sizeof(ebuf),"PORT%d",param);
				   rrc=getini("SERVERS",ebuf,buf);
				   if(rrc==0)
				       strcat(dbuf,value);
				   else
				       strcat(dbuf,"0");
			       }
			       rrc=dialog_inputbox ( "IRC Server and Port", "Enter the Hostname or IP and a Port delimited by a space", 21, 75,
		                         dbuf);
			       if(rrc==0 && strlen(selection)>0)
			       {
			           pt=strchr(selection,' ');
				   if(pt!=NULL)
			           {
				       pt++;
				       if(atoi(pt)>0)
				       {
			                   ap_snprintf(ebuf,sizeof(ebuf),"PORT%d",param);
				           writeini("SERVERS",ebuf,buf,pt);
					   pt--;
					   *pt=0;
			                   ap_snprintf(ebuf,sizeof(ebuf),"SERVER%d",param);
					   writeini("SERVERS",ebuf,buf,selection);
				       }
				   }
			       }
			       break;
			   case 2:
			       ap_snprintf(ebuf,sizeof(ebuf),"%s.SERVERS.PORT%d",buf,param);
			       clearsectionconfig(buf);
			       ap_snprintf(ebuf,sizeof(ebuf),"%s.SERVERS.SERVER%d",buf,param);
			       clearsectionconfig(buf);
			       break;
		       }
		    }
		    break;
	    }
	}
	if(rc==1)
	    return 0x0; /* exit */
	if(rc==2)
	{
	    help();
	    strmncpy(lchoice,selection,sizeof(lchoice));
	}
    }

}

/*
 * setuplinkoptions()
 * setups a link mask
 */

int setuplinkoptions()
{
    char buf[200];
    int rc;
    ap_snprintf(buf,sizeof(buf),"LINK%d",choosenlink);
    /* single options */
    strcpy(lm1,"Link Type: ");
    rc=getini(buf,"TYPE","LINKS");
    if(rc==0)
    {
	if(atoi(value)==1)
	    strcat(lm1,"Listening for the Link");
	else
	    strcat(lm1,"Connecting the Link");
    } else {
	strcat(lm1,"Choose");
    }
    strcpy(lm2,"Host :");
    rc=getini(buf,"HOST","LINKS");
    if(rc==0)
	if(strlen(lm2)+strlen(value)<sizeof(lm2)) strcat(lm2,value);
    else
	strcat(lm2,"Unknown");
    strcpy(lm3,"Port :");
    rc=getini(buf,"PORT","LINKS");
    if(rc==0)
	if(strlen(lm3)+strlen(value)<sizeof(lm3)) strcat(lm3,value);
    else
	strcat(lm3,"Choose");
    strcpy(lm4,"Relay allowed :");
    rc=getini(buf,"ALLOWRELAY","LINKS");
    if(rc==0)
    {
	if(atoi(value)==0)
	    strcat(lm4,"No");
	else
	    strcat(lm4,"Yes");
    } else
	strcat(lm4,"No");
    strcpy(lm5,"Reset Password");
}
/*
 * hostedit()
 * edits a choosen or new hostallow
 */

int hostedit()
{
    int rc;
    int choosenallow;
    char lchoice[200];
    char header[200];
    char buf[200];
    char ebuf[200];
    int pa,rrc;
    choosenallow=param;
    ap_snprintf(header,sizeof(header),"Allowed Host #%d",choosenallow);
    ap_snprintf(buf,sizeof(buf),"ENTRY%d",choosenallow);
    rrc=getini("HOSTALLOWS",buf,"PSYBNC");
    if(rrc!=0)
	strcpy(value,"");
    rrc=dialog_inputbox ( "IP", "Enter the allowed IP in format IP;IP (e.g. 172.16.1.2;172.16.1.2)", 21, 75,
                         value);
    if(rrc==0 && strlen(selection)>0)
	writeini("HOSTALLOWS",buf,"PSYBNC",selection);
    return 0x0;
}

/*
 * linkedit()
 * edits a choosen or new link
 */

int linkedit()
{
    int rc;
    char lchoice[200];
    char header[200];
    char buf[20];
    char ebuf[200];
    int pa,rrc;
    choosenlink=param;
    strcpy(lchoice,"501:");
    while(1)
    {
	ap_snprintf(header,sizeof(header),"Link #%d",choosenlink);
	selection[0]=0;
	setuplinkoptions();
	init_dialog();
	rc=dialog_menu( header,"Choose the option to change and press ENTER.",
		    21,75,12,lchoice,5,linkmenu);
	extractparam();
	end_dialog();

	if(rc==0) /* choose */
	{
	    ap_snprintf(buf,sizeof(buf),"LINK%d",choosenlink);
	    strmncpy(lchoice,selection,sizeof(lchoice));
	    switch(param)
	    {
		case 501:
		    rrc=getini(buf,"TYPE","LINKS");
		    if(rrc!=0)
			strcpy(value,"0");
		    pa=atoi(value)+1;
		    if(pa==3) pa=1;
		    ap_snprintf(value,sizeof(value),"%d",pa);
		    writeini(buf,"TYPE","LINKS",value);
		    break;
		case 502:
		    rrc=getini(buf,"HOST","LINKS");
		    if(rrc!=0)
			strcpy(value,"");
		    rrc=dialog_inputbox ( "Host Name", "Enter the Hostname", 21, 75,
		                         value);
		    if(rrc==0)
			writeini(buf,"HOST","LINKS",selection);
		    break;
		case 503:
		    rrc=getini(buf,"PORT","LINKS");
		    if(rrc!=0)
			strcpy(value,"31337");
		    selection[0]=0;rrc=0;
		    while(atoi(selection)==0 && rrc==0)
			rrc=dialog_inputbox ( "Port", "Enter the Port", 21, 75,
		                         value);
		    if(rrc==0)
			writeini(buf,"PORT","LINKS",selection);
		    break;
		case 504:
		    rrc=getini(buf,"ALLOWRELAY","LINKS");
		    if(rrc!=0)
			strcpy(value,"0");
		    pa=atoi(value)+1;
		    if(pa==2) pa=0;
		    ap_snprintf(value,sizeof(value),"%d",pa);
		    writeini(buf,"ALLOWRELAY","LINKS",value);
		    break;
		case 505:
		    writeini(buf,"PASS","LINKS",NULL);
		    break;
	    }
	}
	if(rc==1)
	    return 0x0; /* exit */
	if(rc==2)
	{
	    help();
	    strmncpy(lchoice,selection,sizeof(lchoice));
	}
    }
}

/*
 * bounceroptions()
 * displays general options for the bouncer
 */

int bounceroptions()
{
    int rc;
    char lchoice[200];
    char *pt;
    char sysme[200];
    char buf[200];
    char pbuf[400];
    FILE *atest;
    strcpy(lchoice,"101:");
    while(1)
    {
	selection[0]=0;
	rc=getini("SYSTEM","ME","PSYBNC");
	if(rc!=0)
	    strcpy(value,"(none)");
	ap_snprintf(bm1,sizeof(bm1),"Bouncer Name :%s",value);
	strmncpy(sysme,value,sizeof(sysme));
	rc=getini("SYSTEM","DCCHOST","PSYBNC");
	if(rc!=0)
	    strcpy(value,"(none)");
	ap_snprintf(bm2,sizeof(bm2),"DCC Host :%s",value);
	rc=getini("SYSTEM","LANGUAGE","PSYBNC");
	if(rc!=0)
	    strcpy(value,"english");
	ap_snprintf(bm3,sizeof(bm3),"Language :%s",value);
	init_dialog();
	rc=dialog_menu( "Bouncer-Config","Please select the Option you want to change and press Enter.",
		    21,75,12,lchoice,7,bouncemenu);
	extractparam();
	end_dialog();
	if(rc==0) /* choose */
	{
	    strmncpy(lchoice,selection,sizeof(lchoice));
	    switch(param)
	    {
		case 301:
		    rc=0;
		    while(rc>=0)
		    {
		       confbase=1;
		       rc=createvariablemenu("Listening Ports", "Please choose the Port-Entry you want to edit/delete or choose new to add a new entry. Press ESCAPE to return to the Option Menu.", "SYSTEM",
		       "PORT%d", "HOST%d", "PSYBNC", 10000, "#%d :%s %s");
		       extractparam();
		       switch(rc)
		       {
		           case 1:
			       param=lastfree;
			   case 0:
			       ap_snprintf(buf,sizeof(buf),"HOST%d",param);
			       rc=getini("SYSTEM",buf,"PSYBNC");
			       if(rc!=0)
			           strcpy(pbuf,"*");
			       else
			           strmncpy(pbuf,value,sizeof(pbuf));
			       if(strlen(pbuf)+1<sizeof(pbuf)) strcat(pbuf," ");
			       ap_snprintf(buf,sizeof(buf),"PORT%d",param);
			       rc=getini("SYSTEM",buf,"PSYBNC");
			       if(rc!=0)
			           strcat(pbuf,"31337");
			       else
			           if(strlen(pbuf)+strlen(value)<sizeof(pbuf)) strcat(pbuf,value);
			       selection[0]=0;rc=0;
			       while(strchr(selection,' ')==NULL && rc==0)
			       {
				    rc=dialog_inputbox ( "Listening Host and Port", "Enter a Host and a Port delimited by a space", 21, 75,
		                                     pbuf);
			       }
			       if(rc==0) {
				    pt=strchr(selection,' ');
				    if(pt!=NULL)
				    {
					pt++;
					if(atoi(pt))
					{
					    ap_snprintf(buf,sizeof(buf),"PORT%d",param);
					    writeini("SYSTEM",buf,"PSYBNC",pt);
					    pt--;
					    *pt=0;
					    ap_snprintf(buf,sizeof(buf),"HOST%d",param);
					    writeini("SYSTEM",buf,"PSYBNC",selection);
					}
				    }
			       }
			       break;
			   case 2:
			       ap_snprintf(buf,sizeof(buf),"PORT%d",param);
			       writeini("SYSTEM",buf,"PSYBNC",NULL);
			       ap_snprintf(buf,sizeof(buf),"HOST%d",param);
			       writeini("SYSTEM",buf,"PSYBNC",NULL);
		       }
		    }
		    break;
		case 302:
		    rc=0;
		    while(rc>=0)
		    {
		       confbase=1;
		       rc=createvariablemenu("Links", "Please choose the Link-Entry you want to edit/delete or choose new to add a new entry. Press ESCAPE to return to the Option Menu.", "LINK%d",
		       "PORT", "HOST", "LINKS", 10000, "#%d :%s %s");
		       extractparam();
		       switch(rc)
		       {
		           case 1:
			       param=lastfree;
			   case 0:
			       linkedit();
			       break;
			   case 2:
			       ap_snprintf(buf,sizeof(buf),"LINKS.LINK%d.",param);
			       clearsectionconfig(buf);
			       break;
		       }
		    }
		    break;
		case 303:
		    rc=dialog_inputbox ( "Bouncer Name", "Enter a name", 21, 75,
		                         sysme);
		    if(rc==0) writeini("SYSTEM","ME","PSYBNC",selection);
		    break;
		case 304:
		    rc=0;
		    while(rc>=0)
		    {
		       confbase=0;
		       rc=createvariablemenu("Host Allows", "This section allows editing the allowed Hosts, which may connect to the bouncer. Press ESCAPE to return to the Option Menu.", "HOSTALLOWS", 
		       "ENTRY%d", "ENTRY%d", "PSYBNC", 10000, "#%d :%s %s");
		       extractparam();
		       switch(rc)
		       {
		           case 1:
			       param=lastfree;
			   case 0:
			       hostedit();
			       break;
			   case 2:
			       ap_snprintf(buf,sizeof(buf),"PSYBNC.HOSTALLOWS.ENTRY%d",param);
			       clearsectionconfig(buf);
			       break;
		       }
		    }
		    break;
		case 305:
		    rc=0;
		    while(rc>=0)
		    {
		       confbase=1;
		       rc=createvariablemenu("Users", "This Section allows adding, editing and deletion of Users. Select a user or choose New to create a new user. Press ESCAPE to return to Bouncer-Config.", "USER", 
		       "USER", "LOGIN", "USER%d", 10000, "#%d :%s %s");
		       extractparam();
		       choosenuser=param;
		       switch(rc)
		       {
		           case 1:
			       param=lastfree;
			   case 0:
			       useredit();
			       break;
			   case 2:
			       ap_snprintf(buf,sizeof(buf),"USER%d.",param);
			       clearsectionconfig(buf);
			       break;
		       }
		    }
		    rc=0;
		    break;
		case 306:
		    rc=getini("SYSTEM","DCCHOST","PSYBNC");
		    rc=dialog_inputbox ( "DCC Host", "Enter the IP which should be used for DCC Chats and Files", 21, 75,
		                         value);
		    if(rc==0) writeini("SYSTEM","DCCHOST","PSYBNC",selection);
		    break;
		case 307:
		    rc=getini("SYSTEM","LANGUAGE","PSYBNC");
		    rc=dialog_inputbox ( "Language", "Enter the name of your language (e. g. english, german)", 21, 75,
		                         value);
		    if(rc==0)
		    {
			ap_snprintf(buf,sizeof(buf),"lang/%s.lng",selection);
			atest=fopen(buf,"r");
			if(atest!=NULL)
			{
			    writeini("SYSTEM","LANGUAGE","PSYBNC",selection);
			    fclose(atest);
			}
		    }
		    break;
	    }
	}
	if(rc==1)
	    return 0x0; /* exit */
	if(rc==2)
	{
	    help();
	    strmncpy(lchoice,selection,sizeof(lchoice));
	}
    }
}

/*
 * mmenu()
 * main menu routine
 */

int mmenu()
{
    int rc;
    char lchoice[200];
    FILE *log;
    strcpy(lchoice,"101:");
    while(1)
    {
	selection[0]=0;
	init_dialog();
	rc=dialog_menu( APPNAME " " APPVER " - Configuration","Welcome to the " APPNAME " Configuration-Tool. Please select the section of Options you want to change.",
		    21,75,12,lchoice,7,mainmenu);
	extractparam();
	end_dialog();
	if(rc==0) /* choose */
	{
	    strmncpy(lchoice,selection,sizeof(lchoice));
	    switch(param)
	    {
		case 101:
		    compilingoptions();
		    break;
		case 102:
		    bounceroptions();
		    break;
		case 103:
		    log=fopen("log/psybnc.log","r");
		    if(log!=NULL) /* only, if exists */
		    {
			fclose(log);
			rc=dialog_textbox ("The current Log", "log/psybnc.log", 21, 78);
		    }
		    break;
		case 104:
		    break;
		case 105:
		    rc=dialog_textbox ("Read-Me", "README", 21, 78);
		    break;
		case 106:
		    rc=dialog_textbox ("Frequently Asked Questions", "FAQ", 21, 78);
		    break;
		case 107:
		    rc=dialog_textbox ("Last Changes", "CHANGES", 21, 78);
		    break;
	    }
	}
	if(rc==1)
	    return 0x0; /* exit */
	if(rc==2)
	{
	    help();
	    strmncpy(lchoice,selection,sizeof(lchoice));
	}
    }
}

/* main loop */

int main ()
{
    comoptions=malloc(sizeof(struct comopt));
    umask( ~S_IRUSR & ~S_IWUSR );
    readconfig();
    mmenu();
    system("clear");
    flushconfig();
    free(comoptions);
    exit(0x0);
}
