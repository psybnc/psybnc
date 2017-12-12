/* $Id: p_inifunc.c,v 1.3 2005/06/04 18:00:14 hisi Exp $ */
/************************************************************************
 *   psybnc2.3.2, src/p_inifunc.c
 *   Copyright (C) 2003 the most psychoid  and
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
static char rcsid[] = "@(#)$Id: p_inifunc.c,v 1.3 2005/06/04 18:00:14 hisi Exp $";
#endif

#define P_INIFUNC

#include <p_global.h>

/* reset the config cache */

int resetconfig()
{
    struct stringarray *wconf,*owconf;
    pcontext;
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
    pcontext;
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
	strcpy(wconf->entry,inistring);
    }
    fclose(handle);
    return 0x0;
}

/* write the config file */

int flushconfig()
{
    FILE *handle;
    struct stringarray *wconf;
    pcontext;
    oldfile(configfile);
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
    pcontext;
    wconf=conf;
    xconf=conf;
    pcontext;
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
    pcontext;
    return 0x0;
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
		strmncpy(value,po,sizeof(value));
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
    ap_snprintf(ppuf,sizeof(ppuf),lngtxt(371),inidat,section,param);
    ap_snprintf(spuf,sizeof(spuf),lngtxt(372),inidat,section);
    if(data_p!=NULL)
	ap_snprintf(buf,sizeof(buf),"%s%s",ppuf,data_p);
    xconf=conf;
    sectconf=conf;
    while (wconf) 
    {
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
		    wconf->entry=(char *)pmalloc(strlen(buf)+2);
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
    xconf->entry=(char *)pmalloc(strlen(buf)+2);
    strmncpy(xconf->entry,buf,strlen(buf)+1);
    return 0x0;
}

int lastfree;
#define MAXITEML 256

int countconfentries(char *section, char *entry, char *fname)
{
    int cnt=0;
    char buf[256];
    char uitem[256];
    static int itemlist[MAXITEML];
    int i,rc;
    struct stringarray *wconf=conf;
    lastfree=0;
    ap_snprintf(buf,sizeof(buf),"%s.%s.%s=%%255s",fname,section,entry);
    memset(&itemlist[0],0x0,sizeof(itemlist));
    while (wconf)
    {
	rc=sscanf(wconf->entry,buf,&i,uitem);
	if(rc==2) 
	{
	    if(i<MAXITEML) itemlist[i]=1;
	    cnt++; 
	}
	wconf=wconf->next;
    }
    for(i=0;i<MAXITEML;i++)
    {
	if(itemlist[i]==0)
	{
	    lastfree=i;
	    break;
	}
    }
    return cnt;
}

/* get server from ini by number */

int getserver(int srvnr, int usernum)
{
   char fnmuser[20];
   char buf[100];
   int ern;
   ap_snprintf(fnmuser,sizeof(fnmuser),lngtxt(373),usernum);
   ap_snprintf(buf,sizeof(buf),lngtxt(374),srvnr);
   ern = getini(lngtxt(375),buf,fnmuser);
   if (ern != 0) { return ern; }
   ap_snprintf(user(usernum)->server,sizeof(user(usernum)->server),"%s",value);
   ap_snprintf(buf,sizeof(buf),lngtxt(376),srvnr);
   ern = getini(lngtxt(377),buf,fnmuser);
   if (ern != 0) { user(usernum)->port = 6667; } else { user(usernum)->port = atoi(value); }
   ap_snprintf(buf,sizeof(buf),lngtxt(378),srvnr);
   ern = getini(lngtxt(379),buf,fnmuser);
   if (ern != 0) { *user(usernum)->spass=0; return 0; }
   ap_snprintf(user(usernum)->spass,sizeof(user(usernum)->spass),"%s",value);
   return 0;
}

/* get next server */

int getnextserver(int nuser)
{
    int first;
    first = user(nuser)->currentserver;
    while(1)
    {
	user(nuser)->currentserver++;
	if (getserver(user(nuser)->currentserver,nuser) ==0)
	{
	    return 0; /* found it, bye */
	}
	if (user(nuser)->currentserver>20) user(nuser)->currentserver = 0;
	if (user(nuser)->currentserver == first) return -1;
    }
    return 1;
}

/* delete user files */
int deluser(int uind)
{
    char buf[60];
    pcontext;
#ifdef TRAFFICLOG
    /* close the trafficlog before deleting */
    if(user(uind)->trafficlog!=NULL)
    {
	fclose(user(uind)->trafficlog);
	user(uind)->trafficlog=NULL;
    }
#endif
    ap_snprintf(buf,sizeof(buf),lngtxt(380),uind);
    oldfile(buf);
    ap_snprintf(buf,sizeof(buf),lngtxt(381),uind);
    oldfile(buf);
    pcontext;
    ap_snprintf(buf,sizeof(buf),lngtxt(382),uind);
    clearsectionconfig(buf);
    flushconfig();
    pcontext;
    return 0x0;
}
