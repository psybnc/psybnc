/* $Id: p_translate.c,v 1.4 2005/06/04 18:00:14 hisi Exp $ */
/************************************************************************
 *   psybnc2.3.2, src/p_translate.c
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
static char rcsid[] = "@(#)$Id: p_translate.c,v 1.4 2005/06/04 18:00:14 hisi Exp $";
#endif

#define P_TRANSLATE

#include <p_global.h>

#ifdef TRANSLATE

int translateconnected(int uid);
int translatederror(int uid, int er);
int translatedone(int uid);
int translatedpart1(int uid);
int translatedpart2(int uid);
int translatedpart3(int uid);
int translatedpart4(int uid);

#define MAXSYNTRANS	120 /* number of maximum syncrone running translations */

int addtranslate(int usern, char *totranslate, char *from, char *dest, int direction, char *lang, char *command)
{
    struct translatet *lkm;
    int lastuid;
    int cnt=0;
    int sock;
    int rc;
    char buf[200];
    pcontext;
    if(translate==NULL)
    {
	translate=(struct translatet *)pmalloc(sizeof(struct translatet));
	lkm=translate;
	lastuid=0;
    } else {
	lkm=translate;
	lastuid=translate->uid;
	while(lkm->next!=NULL)
	{
	    lastuid=lkm->next->uid;
	    lkm=lkm->next;
	    cnt++;
	}
	lkm->next=(struct translatet *)pmalloc(sizeof(struct translatet));
	lkm=lkm->next;
    }
    if(cnt>MAXSYNTRANS) return -1;
    lastuid++;
    lkm->uid=lastuid;
    lkm->delayed=30; /* before it times out, doubled, altavista got slow */
    lkm->translatetext=(char *)pmalloc(strlen(totranslate)+1);
    strcpy(lkm->translatetext,totranslate);
    if(direction==TR_TO)
    {
	ap_snprintf(buf,sizeof(buf),lngtxt(861),command,dest);
    } else {
	if(strchr("&!#+",*dest)!=NULL)
	    ap_snprintf(buf,sizeof(buf),lngtxt(862),from,command,dest);
	else	
	    ap_snprintf(buf,sizeof(buf),lngtxt(863),from,command,user(usern)->nick);
    }
    lkm->translatedtext=(char *)pmalloc(strlen(buf)+1);
    strcpy(lkm->translatedtext,buf);
    lkm->dest=(char *)pmalloc(strlen(dest)+1);
    strcpy(lkm->dest,dest);
    lkm->source=(char *)pmalloc(strlen(from)+1);
    strcpy(lkm->source,from);
    lkm->lang=(char *)pmalloc(strlen(lang)+1);
    strcpy(lkm->lang,lang);
    lkm->direction=direction;
    lkm->usern=usern;
    lkm->sock=createsocket(0,ST_CONNECT,lastuid,SGR_NONE,NULL,translateconnected,translatederror,translatedpart1,translatedone,NULL,AF_INET,SSL_OFF);
    lkm->sock=connectto(lkm->sock,lngtxt(864),80,NULL);
    return 0x0;    
}

struct translatet *pre;

struct translatet *gettranslate(int uid)
{
    struct translatet *lkm;
    pcontext;
    lkm=translate;
    pre=NULL;
    while(lkm!=NULL)
    {
	if(lkm->uid==uid) return lkm;
	pre=lkm;
	lkm=lkm->next;
    }
    pcontext;
    return NULL;
}

int erasetranslate(int uid)
{
    struct translatet *lkm;
    pcontext;
    lkm=gettranslate(uid);
    pcontext;
    if(lkm==NULL) 
    {
	currentsocket->sock->destructor=NULL;
	killsocket(currentsocket->sock->syssock);
	return 0x0;
    }
    if(pre!=NULL)
	pre->next=lkm->next;
    else
	translate=lkm->next;
    free(lkm->translatetext);
    free(lkm->translatedtext);
    free(lkm->dest);
    free(lkm->source);
    free(lkm->lang);
    killsocket(lkm->sock);
    free(lkm);
    return 0x0;
}

int translateconnected(int uid)
{
    struct translatet *lkm;
    char buf[4096];
    char buf2[5190];
    char *pt;
    pcontext;
    lkm=gettranslate(uid);
    pcontext;
    if(lkm==NULL)
	return 0x0;
    pt=lkm->translatetext;
    while(*pt) /* $%&@ */
    {
	if(*pt==' ') *pt='+';
	pt++;
    }
    /*
     * Altavista changed to Microsoft-Products. This is bad,
     * i have to give more Header-Infos.
     *
     */
    ap_snprintf(buf,sizeof(buf),lngtxt(865),lkm->translatetext,lkm->lang);
    ap_snprintf(buf2,sizeof(buf2),lngtxt(866),strlen(buf),buf);
    writesock_URGENT(lkm->sock,buf2);
}

int translatederror(int uid, int err)
{
    char buf[200];
    struct translatet *lkm;
    pcontext;
    p_log(LOG_ERROR,-1,lngtxt(867),uid,err);
    lkm=gettranslate(uid);
    pcontext;
    if(lkm==NULL)
	return 0x0;
    erasetranslate(uid);    
}

int translatedone(int uid)
{
    pcontext;
    /* we dont say anyting on disconnection */
    erasetranslate(uid);
}

int translatedpart1(int uid)
{
    struct socketnodes *lkm;
    struct translatet *th;
    pcontext;
    if(strstr(ircbuf,lngtxt(868))!=NULL)
    {
	th=gettranslate(uid);
	if(th!=NULL)
	{
	    lkm=getpsocketbysock(th->sock);
	    if(lkm!=NULL)
	    {
		lkm->sock->handler=translatedpart2;
	    }
	}
    }	
}

int translatedpart2(int uid)
{
    char *ept,*upt,*pt;
    struct translatet *th;
    struct socketnodes *lkm;
    pcontext;
    th=gettranslate(uid);
    pcontext;
    if(th==NULL)
	return 0x0;
    lkm=getpsocketbysock(th->sock);
    if(lkm==NULL)
    {
	erasetranslate(uid);
	return 0x0;
    }
    pcontext;
    ept=strstr(ircbuf,lngtxt(869));
    if(ept==NULL)
    {
	ept=ircbuf;
	upt=strchr(ept,':');
	if(upt!=NULL)
	{
	    upt++;*upt=' ';
	    upt++;*upt=0;
	    upt=strchr(th->translatedtext,'\r');
	    if (upt==NULL) upt=strchr(th->translatedtext,'\n');
	    if (upt!=NULL) *upt=0;
	    pt=strchr(ept,'&');
	    if(pt!=NULL)
	    {
		upt=strchr(pt+1,';');
		if(upt!=NULL)
		{
		    upt++;
		    while(*upt)
		    {
			*pt=*upt;
			pt++;
			upt++;
		    }
		    *pt=0;
		}
	    }
	    if(th->direction!=TR_TO)
		th->translatedtext=strmcat(th->translatedtext,ept);
	    lkm->sock->handler=translatedpart3;
	}
    } else {
	lkm->sock->handler=translatedpart3;
    }
}

int translatedpart3(int uid)
{
    char *ept,*apt;
    struct translatet *th;
    struct socketnodes *lkm;
    pcontext;
    th=gettranslate(uid);
    pcontext;
    if(th==NULL)
	return 0x0;
    lkm=getpsocketbysock(th->sock);
    if(lkm==NULL)
    {
	erasetranslate(uid);
	return 0x0;
    }
    pcontext;
    ept=nobreak(ircbuf);
    ept=strstr(ept,lngtxt(870));
    if(ept!=NULL)
    {
	ept=strchr(ept,'>');
	if(ept!=NULL)
	{
	    ept++;
	    th->translatedtext=strmcat(th->translatedtext,ept);
	    th->translatedtext=strmcat(th->translatedtext," ");
	}
	lkm->sock->handler=translatedpart4;
    } else {
	if(strstr(ircbuf,"</strong>")!=NULL)
	    lkm->sock->handler=translatedpart4;
    }
}

int translatedpart4(int uid)
{
    char *ept,*pt;
    struct translatet *th;
    struct socketnodes *lkm;
    pcontext;
    th=gettranslate(uid);
    pcontext;
    if(th==NULL)
	return 0x0;
    lkm=getpsocketbysock(th->sock);
    if(lkm==NULL)
    {
	erasetranslate(uid);
	return 0x0;
    }
    pcontext;
    ept=strstr(ircbuf,lngtxt(871));
    if(ept==NULL)
    {
	ept=strstr(ircbuf,"<p>");
    }
    if(ept!=NULL)
    {
	ept=strstr(th->translatedtext,lngtxt(872));
	if(ept!=NULL)
	{
	    ept=strchr(th->translatedtext,':');
	    if(ept==th->translatedtext) ept=strchr(ept+1,':');
	    if(ept!=NULL)
	    {
		*ept=0;
		th->translatedtext=strmcat(th->translatedtext,lngtxt(873));
	    }
	}
	ept=strstr(th->translatedtext,"<P>");
	if(ept!=NULL)
	{ 
	    *ept=0;
	}
	th->translatedtext=strmcat(th->translatedtext,"\r\n");
	if(th->direction==TR_TO)
	{
	    if(user(th->usern)->outstate==STD_CONN)
	    {
		writesock(user(th->usern)->outsock,th->translatedtext);
		if(user(th->usern)->instate>STD_NOCON)
		{
		    pt=strchr(th->translatedtext,':');
		    if(pt!=NULL)
		    {
			if(strchr("#!&+",*th->dest)!=NULL)
			{
			    ssnprintf(user(th->usern)->insock,lngtxt(874),user(th->usern)->nick,user(th->usern)->login,user(th->usern)->host,th->dest,pt);
			} else {
			    ssnprintf(user(th->usern)->insock,lngtxt(875),th->dest,user(th->usern)->nick,pt);
			}
		    }
		}
	    }
	} else {
	    if(user(th->usern)->instate>STD_NOCON)
	    {
		writesock(user(th->usern)->insock,th->translatedtext);
	    }
	}
	erasetranslate(uid); /* job done, socket gone */
    } else {    
	ept=nobreak(ircbuf);
	pt=strstr(ept,"<textarea");
	if(pt!=NULL)
	{
	    pt=strchr(pt,'>');
	    if(pt!=NULL)
	    {
		ept=pt+1;
	    }
	}
	while(*ept==' ' || *ept==9) ept++;
	if(*ept)
	{
	    th->translatedtext=strmcat(th->translatedtext,ept);
	    th->translatedtext=strmcat(th->translatedtext," ");
	}
    }
}

/* periodically checking the translation sockets */

int cleartranslates()
{
    struct translatet *lkm,*pre;
    char buf[100];
    lkm=translate;
    pre=NULL;
    pcontext;
    while(lkm!=NULL)
    {
	lkm->delayed-=delayinc;
	if(lkm->delayed<=0)
	{
	    p_log(LOG_WARNING,-1,lngtxt(876),lkm->uid,lkm->dest,lkm->lang);
	    erasetranslate(lkm->uid);
	    pcontext;
	    if(pre==NULL)
		lkm=translate;
	    else
		lkm=pre->next;
	} else {
	    pre=lkm;
	    lkm=lkm->next;
	}
    }
    return 0x0;
}

#endif

