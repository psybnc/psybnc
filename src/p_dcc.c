/* $Id: p_dcc.c,v 1.7 2005/06/04 18:00:14 hisi Exp $ */
/************************************************************************
 *   psybnc2.3.2, src/p_dcc.c
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
static char rcsid[] = "@(#)$Id: p_dcc.c,v 1.7 2005/06/04 18:00:14 hisi Exp $";
#endif

#define P_DCC

#include <p_global.h>

#ifdef DCCCHAT

/* DCC - Support */
/* adding a dcc */

int adddcc(int usern, char *host, int port, char *luser, char *pass, char *name,int noini)
{
    char buf[400];
    char buf2[400];
    struct linknodes *thisdcc;
    char afile[30];
    char *ppt;
    int userp;
    pcontext;
    if (user(usern)->parent!=0) userp=user(usern)->parent; else userp=usern;
    ap_snprintf(afile,sizeof(afile),lngtxt(286),usern);
    thisdcc=user(usern)->dcc;
    while (1)
    {
	if (thisdcc->link==NULL) {
	    thisdcc->link=(struct datalinkt *)pmalloc(sizeof(struct datalinkt));
	    break;
	}
	if (thisdcc->next==NULL)
	{
	    thisdcc->next=(struct linknodes *)pmalloc(sizeof(struct linknodes));
	    thisdcc->next->link=(struct datalinkt *)pmalloc(sizeof(struct datalinkt));
	    thisdcc=thisdcc->next;
	    break;
	}
	thisdcc=thisdcc->next;
    }
    thisdcc->link->outstate==0;
    thisdcc->link->outsock=0;
    thisdcc->link->port=port;
    thisdcc->uid=usern;
    thisdcc->link->type=LI_DCC;
    thisdcc->link->delayed=1;
    strmncpy(thisdcc->link->user,luser,sizeof(thisdcc->link->user));
    strmncpy(thisdcc->link->name,name,sizeof(thisdcc->link->user));
    strmncpy(thisdcc->link->pass,pass,sizeof(thisdcc->link->user));
    strmncpy(thisdcc->link->host,host,sizeof(thisdcc->link->user));
    if (noini==1) return 0x0;
    strmncpy(buf,pass,sizeof(buf));
    ppt=cryptit(buf);
    ap_snprintf(buf,sizeof(buf),lngtxt(287),thisdcc->link->name,thisdcc->link->user,ppt);
    ap_snprintf(buf2,sizeof(buf2),lngtxt(288),thisdcc->link->host,thisdcc->link->port);
    writelist(buf,buf2,afile,NULL);
    p_log(LOG_INFO,usern,lngtxt(289),thisdcc->link->name,thisdcc->link->host,thisdcc->link->port,user(usern)->login);
    return 0x0;
}

#endif

/* load all dccs of a user */

int loaddccs(int usern)
{
#ifdef DCCCHAT
    char buf[400];
    struct linknodes *thisdcc;
    char afile[30];
    char section[30];
    char entry[30];
    char *hpt;
    char *upt;
    char *ppt;
    char *npt;
    char *spt;
    int port;
    int cnt=0;
    ap_snprintf(afile,sizeof(afile),lngtxt(290),usern);
    strcpy(section,"DCC");
    for(cnt=0;cnt<20;cnt++)
    {
      ap_snprintf(entry,sizeof(entry),lngtxt(291),cnt);
      if(getini(section,entry,afile)==0) 
      {
	npt=value;
	upt=strchr(npt,' ');
	if (upt!=NULL)
	{
	    *upt=0;upt++;
	    ppt=strchr(upt,' ');
	    if (ppt!=NULL)
	    {
		*ppt=0;ppt++;
		hpt=strchr(ppt,';');
		if (hpt!=NULL)
		{
		    *hpt=0;hpt++;
		    spt=strchr(hpt,':');
		    if(spt!=NULL)
		    {
			while(strchr(spt+1,':')!=NULL)
			{
			    spt=strchr(spt+1,':');
			}
		    }
		    if (spt!=NULL)
		    {
			*spt=0;spt++;
			port=atoi(spt);
			adddcc(usern,hpt,port,upt,decryptit(ppt),npt,1);
		    }	    
		}
	    }	
	}
      }      
    }
#endif
    return 0x0;
}

#ifdef DCCCHAT

/* listing dccs */

int listdccs(int usern)
{
    char buf[400];
    struct linknodes *thisdcc;
    int cnt;
    char l;
    int userp;
    pcontext;
    if (user(usern)->parent!=0) userp=user(usern)->parent; else userp=usern;
    cnt =1;
    thisdcc=user(usern)->dcc;
    ssnprintf(user(usern)->insock,lngtxt(292),user(userp)->nick);
    while (thisdcc != NULL)
    {
	if (thisdcc->link!=NULL)
	{
	    if (thisdcc->link->outstate==STD_CONN)
		l='*';
	    else
		l=' ';
	    ssnprintf(user(usern)->insock,lngtxt(293),user(userp)->nick,cnt,l,thisdcc->link->name,thisdcc->link->host,thisdcc->link->port);
	    cnt++;
	}
	thisdcc=thisdcc->next;
    }
    ssnprintf(user(usern)->insock,lngtxt(294),user(userp)->nick);
    listpdccs(usern);
    return 0x0;
}


/* erasing dccs */

int erasedcc(int usern,int dccn)
{
    struct linknodes *thisdcc;
    struct linknodes *lastdcc;
    char l;
    FILE *infile;
    FILE *tmp;
    char fbuf[40];
    char afile[30];
    char section[30];
    char entry[30];
    int userp;
    int cnt;
    pcontext;
    if (user(usern)->parent!=0) userp=user(usern)->parent; else userp=usern;
    cnt =1;
    thisdcc=user(usern)->dcc;
    lastdcc=thisdcc;
    while (thisdcc != NULL)
    {
	if (thisdcc->link!=NULL)
	{
	    if (cnt==dccn) break;
	    lastdcc=thisdcc;
	    cnt++;
	}
	thisdcc=thisdcc->next;
    }
    if (thisdcc==NULL || thisdcc->link==NULL)
    {
	ssnprintf(user(usern)->insock,lngtxt(295),user(userp)->nick);
	return 0x0;    
    }
    if (thisdcc->link->outstate==STD_CONN)
    {
	killsocket(thisdcc->link->outsock);
	ssnprintf(user(usern)->insock,lngtxt(296),user(userp)->nick);
    }
    if (thisdcc==user(usern)->dcc)
    {
	if (thisdcc->next==NULL)
	{
	    free(thisdcc->link);
	    thisdcc->link=NULL;
	} else {
	    user(usern)->dcc=thisdcc->next;
	    free(thisdcc->link);
	    free(thisdcc);
	}
    } else {
	lastdcc->next=thisdcc->next;
	free(thisdcc->link);
	free(thisdcc);
    }
    ap_snprintf(afile,sizeof(afile),lngtxt(297),usern);
    ap_snprintf(entry,sizeof(entry),lngtxt(298),dccn-1);
    strcpy(section,"DCC");
    writeini(section,entry,afile,NULL);
    cnt=dccn;
    ap_snprintf(entry,sizeof(entry),lngtxt(299),cnt);
    while(getini(section,entry,afile)==0)
    {
	writeini(section,entry,afile,NULL);
	ap_snprintf(entry,sizeof(entry),lngtxt(300),cnt-1);
	writeini(section,entry,afile,value);
	cnt++;
	ap_snprintf(entry,sizeof(entry),lngtxt(301),cnt);
    }
    ssnprintf(user(usern)->insock,lngtxt(302),user(userp)->nick,dccn);
    return 0x0;
}

#endif

/* check a dcc connection */

struct datalinkt *checkdcc(int usern, char *dccname)
{
    struct linknodes *th;
    th=user(usern)->dcc;
    while (th!=NULL)
    {
	if (th->link != NULL)
	{
	    if (strlen(th->link->name)==strlen(dccname))
	    {
		if (strstr(th->link->name,dccname)!=NULL)
		    return th->link;
	    }
	}
	th=th->next;
    }
    return NULL;
}

#ifdef DCCCHAT

int dcchandler(int usern)
{
    struct linknodes *lkm;
    struct datalinkt *th;
    char l=')';
    char netc[15];
    int rc;
    netc[0]=0;
    pcontext;
    if (user(usern)->parent!=0)
	ap_snprintf(netc,sizeof(netc),"%s'",user(usern)->network);
    else
	memset(netc,0x0,sizeof(netc));
    lkm=user(usern)->dcc;
    while(lkm!=NULL)
    {
	if(lkm->link!=NULL)
	    if(lkm->link->outsock==currentsocket->sock->syssock) break;
	lkm=lkm->next;    
    }
    if (lkm==NULL) return 0x0;
    th=lkm->link;
    if(th->outstate==STD_CONN)
    {
	if (user(usern)->instate==STD_CONN)
	{
	    ssnprintf(user(usern)->insock,lngtxt(303),netc,l,th->name,th->host,user(usern)->nick,ircbuf);
	    return 0x0;
	}
    }
}

/* connected dcc */

int connecteddcc(int usern)
{
    struct linknodes *lkm;
    struct datalinkt *th;
    char *pt;
    char buf[100];
    pcontext;
    lkm=user(usern)->dcc;
    while(lkm!=NULL)
    {
	if(lkm->link!=NULL)
	    if(lkm->link->outsock==currentsocket->sock->syssock) break;
	lkm=lkm->next;    
    }
    if (lkm==NULL) return 0x0;
    th=lkm->link;
    p_log(LOG_INFO,usern,lngtxt(304),user(usern)->login,th->name,th->host,th->port);
    th->delayed=1;
    ap_snprintf(buf,sizeof(buf),lngtxt(305),th->user);
    writesock_DELAY(th->outsock,buf,10); /* should be enough */
    pt=rtrim(th->pass);
    ap_snprintf(buf,sizeof(buf),lngtxt(306),pt);
    writesock_DELAY(th->outsock,buf,5);
    th->outstate=STD_CONN;
    th->delayed=1;
}

/* remap dcc socket */

int remapdcc(int usern, int newsocket)
{
    struct linknodes *lkm;
    struct datalinkt *th;
    char *pt;
    char buf[100];
    pcontext;
    lkm=user(usern)->dcc;
    while(lkm!=NULL)
    {
	if(lkm->link!=NULL)
	    if(lkm->link->outsock==currentsocket->sock->syssock) break;
	lkm=lkm->next;    
    }
    if (lkm==NULL) return 0x0;
    th=lkm->link;
    if(th)
        th->outsock=newsocket;
}

/* connection terminated */

int killeddcc(int usern)
{
    struct linknodes *lkm;
    struct datalinkt *th;
    pcontext;
    lkm=user(usern)->dcc;
    while(lkm!=NULL)
    {
	if(lkm->link!=NULL)
	    if(lkm->link->outsock==currentsocket->sock->syssock) break;
	lkm=lkm->next;    
    }
    if (lkm==NULL) return 0x0;
    th=lkm->link;
    p_log(LOG_WARNING,usern,lngtxt(307),user(usern)->login,th->name,th->host,th->port);
    th->delayed=1;
    killsocket(th->outsock);
    th->outstate=0;
    return 0x0;	
}

/* connection could not be established */

int errordcc(int usern, int errn)
{
    struct linknodes *lkm;
    struct datalinkt *th;
    pcontext;
    lkm=user(usern)->dcc;
    while(lkm!=NULL)
    {
	if(lkm->link!=NULL)
	    if(lkm->link->outsock==currentsocket->sock->syssock) break;
	lkm=lkm->next;    
    }
    if (lkm==NULL) return 0x0;
    currentsocket->sock->destructor=NULL;
    th=lkm->link;
    p_log(LOG_ERROR,usern,lngtxt(308),user(usern)->login,th->name,th->host,th->port);
    th->delayed=1;
    th->outstate=0;
    killsocket(th->outsock);
    return 0x0;	
}



/* checking a single dcc link */

int checkdcclink(int usern, struct datalinkt *th)
{
    char buf[8192];
    char l=')';
    char netc[15];
    int proto=AF_INET;
    char *ho;
    int rc;
#ifdef HAVE_SSL
    int issl=SSL_OFF;
#else
    int issl=0;
#endif
    netc[0]=0;
#ifdef DYNAMIC
    if(user(usern)->instate!=STD_CONN) return 0x0;
#endif
    if (user(usern)->parent!=0)
	ap_snprintf(netc,sizeof(netc),"%s'",user(usern)->network);
    else
	memset(netc,0x0,sizeof(netc));
    if (th->outstate==0) {
	if (th->delayed >0)
	{
	    th->delayed-=delayinc;
	    return 0x0;
	}
	pcontext;
	/* if no ssl was installed, ho was not set. Bugged dcc then.. */
	ho=th->host;
#ifdef HAVE_SSL
	if(strstr(ho,"S=")==ho)
	{
	    ho=ho+2;
	    issl=SSL_ON;
	}
	if(issl==SSL_ON)
    	    p_log(LOG_INFO,usern,lngtxt(309),user(usern)->login,th->name,ho,th->port);
	else
#endif
    	    p_log(LOG_INFO,usern,lngtxt(310),user(usern)->login,th->name,ho,th->port);
	th->outsock=createsocket(0,ST_CONNECT,usern,SGR_NONE,NULL,connecteddcc,errordcc,dcchandler,killeddcc,remapdcc,proto,issl);
	th->outsock=connectto(th->outsock,ho,th->port,user(usern)->vhost);
	th->outstate=STD_NEWCON;
	if (th->outsock==0)
	{
    	    p_log(LOG_ERROR,usern,lngtxt(311),user(usern)->login,th->name,ho,th->port);
	    th->delayed=1;
	    return 0x0;	
	}
	return 0x1;
    }
    return 0x0;
}

#endif

/* checking all dcc links */

int checkdccs()
{
#ifdef DCCCHAT
    struct usernodes *th;
    static struct linknodes *lastdh;
    int cn=0;
    struct linknodes *dh;
    th=usernode;
    while (th!=NULL)
    {
	if (th->user != NULL)
	{
#ifdef DYNAMIC
	    if(user(th->uid)->instate==STD_CONN)
	    {
#endif
		dh=th->user->dcc;
		while (dh!=NULL)
		{
		    if (dh->link!=NULL)
		    {
			if(lastdh != dh && checkdcclink(th->uid,dh->link)==1) {
			    lastdh=dh;
			    cn=1;
			    break;
			}
		    }
		    dh=dh->next;
		}
#ifdef DYNAMIC
	    }
#endif
	}
	th=th->next;
    }
    if(!cn) lastdh=NULL;
#endif
    return 0x0;
}

/*
 * pending dcc support for files and chats
 * this allows the user to send and receive files from and to the
 * bouncer host and to receive or to send chats.
 *
 */


int randport()
{
    unsigned short port;
    port=random();
    while(port<1024)
	port+=1024;
    return port;
}

char stdcc[2048];

/* strip the filename without . and / */

char *stripdccfile(char *realname,int rec)
{
    char *pt;
    pt=realname;
    if(rec==0)
    {
	if(strstr(pt,lngtxt(312))==pt) /* so we send a device ? yes, right. */
	    return NULL;
	if(strstr(pt,lngtxt(313))==pt) /* so we send from etc ? yes, right. */
	    return NULL;
    }
    if(strchr(pt,'?')!=NULL) return NULL;
    if(strchr(pt,'*')!=NULL) return NULL;
    while(strchr(pt,'/')!= NULL)
    {
	if(rec==1) return NULL;
	pt=strchr(pt,'/');
	pt++;
    }
    while(strchr(pt,'\\')!= NULL)
    {
	pt=strchr(pt,'\\');
	pt++;
    }
    if(rec==1)
    {
	if(strstr(pt,"..")!=NULL)
	    return NULL;
    }
    strmncpy(stdcc,pt,sizeof(stdcc));
    return pt;    
}

/* create a new pdcc entry */

struct dcct *createpdcc(int usern)
{
    struct dcct *pd;
    pd=user(usern)->pdcc;
    if(pd==NULL)
    {
	user(usern)->pdcc=(struct dcct *)pmalloc(sizeof(struct dcct));
	pd=user(usern)->pdcc;
    } else {
	while(pd->next!=NULL) pd=pd->next;
	pd->next=(struct dcct *)pmalloc(sizeof(struct dcct));
	pd=pd->next;
    }
    return pd;
}

/* get the current dcc struct */

struct dcct *getpsocketpdcc(int usern, struct socketnodes *psock)
{
    struct dcct *pdcc;
    pdcc=user(usern)->pdcc;
    while(pdcc!=NULL && psock!=NULL)
    {
	if(psock->sock->syssock==pdcc->sock) return pdcc;
	pdcc=pdcc->next;
    }
    return NULL;
}

/* remove a dcc struct entry */

int removepdcc(int usern)
{
    struct dcct *apdcc,*pdcc,*epdcc=NULL;
    struct socketnodes *ps;
    apdcc=getpsocketpdcc(usern,currentsocket);
    if(apdcc==NULL) return 0x0;
    pdcc=user(usern)->pdcc;
    while(pdcc!=NULL)
    {
	if(pdcc==apdcc)
	{
	    if(epdcc==NULL)
	    {
		user(usern)->pdcc=pdcc->next;
	    } else {
		epdcc->next=pdcc->next;
	    }
	    if(pdcc->sock>0) 
	    {
		ps=getpsocketbysock(pdcc->sock);
		if(ps!=NULL)
		{
		    ps->sock->destructor=NULL;
		    ps->sock->errorhandler=NULL;
#ifdef SCRIPTING
		    if(pdcc->pid!=0)
		    {
			if(getsubtaskbypid(pdcc->pid)!=NULL)
			    terminatetask(pdcc->pid,0);
		    }
#endif
		    killsocket(pdcc->sock);
		}
	    }
	    free(pdcc);
	    return 0x0;
	}
	epdcc=pdcc;
	pdcc=pdcc->next;
    }
    return 0x0;
}

/* event routines for dcc chats */

int pdccconnected(int usern)
{
    struct dcct *pdcc;
#ifdef SCRIPTING
    struct subtask *stsk;
#endif
    char netc[20];
    netc[0]=0;
    if(user(usern)->parent!=0)
    {
	ap_snprintf(netc,sizeof(netc),"%s'",user(usern)->network);
    }
    pdcc=getpsocketpdcc(usern,currentsocket);
    if(pdcc==NULL) return 0x0;
    pdcc->delay=0;
    if(user(usern)->instate==STD_CONN)
    {
	ssnprintf(user(usern)->insock,lngtxt(314),netc,pdcc->nick,pdcc->host,user(usern)->nick,currentsocket->sock->syssock);
    }    
#ifdef SCRIPTING
    if(pdcc->pid!=0)
    {
	stsk=getsubtaskbypid(pdcc->pid);
	if(stsk!=NULL)
	{
	    ssnprintf(stsk->fdout,lngtxt(315));
	}
    }
#endif
    return 0x0;
}

/* remapping a pdcc socket */

int pdccremap(int usern, int newsocket)
{
    struct dcct *pdcc;
    pdcc=getpsocketpdcc(usern,currentsocket);
    if(pdcc==NULL) return 0x0;
    pdcc->sock=newsocket;
    return 0x0;
}

/* error in socket */

int pdccerror(int usern, int r)
{
    struct dcct *pdcc;
    char netc[20];
    netc[0]=0;
    if(user(usern)->parent!=0)
    {
	ap_snprintf(netc,sizeof(netc),"%s'",user(usern)->network);
    }
    pdcc=getpsocketpdcc(usern,currentsocket);
    if(pdcc==NULL) return 0x0;
    if(user(usern)->instate==STD_CONN)
    {
	ssnprintf(user(usern)->insock,lngtxt(316),netc,pdcc->nick,pdcc->host,user(usern)->nick,currentsocket->sock->syssock);
    }    
    removepdcc(usern);
}

int pdccquery(int usern)
{
#ifdef SCRIPTING
    struct subtask *stsk;
#endif
    struct dcct *pdcc;
    char netc[20];
    char *pt;
    pcontext;
    netc[0]=0;
    if(user(usern)->parent!=0)
    {
	ap_snprintf(netc,sizeof(netc),"%s'",user(usern)->network);
    }
    pdcc=getpsocketpdcc(usern,currentsocket);
    if(pdcc!=NULL)
    {
	if(user(usern)->instate==STD_CONN)
	    ssnprintf(user(usern)->insock,lngtxt(317),netc,pdcc->nick,pdcc->host,user(usern)->nick,ircbuf);
    
#ifdef SCRIPTING
	if(pdcc->pid!=0)
	{
	    stsk=getsubtaskbypid(pdcc->pid);
	    if(stsk!=NULL)
	    {
		pt=strchr(ircbuf,'\r');
		if(pt!=NULL)
		{
		    *pt='\n';
		    pt++;
		    *pt=0;
		}
		writesock_STREAM(stsk->fdout,ircbuf,strlen(ircbuf));
	    }
	}
#endif
    } else {
	p_log(LOG_ERROR,-1,"Unknown DCC for socket %d",currentsocket->sock->syssock);
    }
    return 0x0;
}

int pdccclosed(int usern)
{
    pdccerror(usern,0);
    return 0x0;
}

/* event for the incoming chat */

int acceptpdccchat(int usern)
{
    struct dcct *pdcc;
    int nsock;
    int userp=usern;
    int rc;
    struct socketnodes *ps,*eps;
    char netc[20];
    netc[0]=0;
    if(user(usern)->parent!=0)
    {
	ap_snprintf(netc,sizeof(netc),"%s'",user(usern)->network);
    }
    pdcc=getpsocketpdcc(usern,mastersocket);
    if(pdcc==NULL) return 0x0;
    nsock=currentsocket->sock->syssock;
    if(nsock<=0) return 0x0;
    ps=getpsocketbysock(nsock);
    if(ps==NULL) return 0x0;
    pdcc->delay=0;
    pdcc->type=PDC_CHATTO;
    pdcc->sock=nsock;
    killsocket(mastersocket->sock->syssock);
    strmncpy(pdcc->host,acceptip,sizeof(pdcc->host));
    if(user(usern)->instate==STD_CONN)
    {
	ssnprintf(user(usern)->insock,lngtxt(318),netc,pdcc->nick,pdcc->host,user(usern)->nick,nsock);
    }    
    return 0x0;
}

/* events for the incoming file transfers */

int pdccfileerror(int usern, int r)
{
    struct dcct *pdcc;
    char netc[20];
    netc[0]=0;
    if(user(usern)->parent!=0)
    {
	ap_snprintf(netc,sizeof(netc),"%s'",user(usern)->network);
    }
    pdcc=getpsocketpdcc(usern,currentsocket);
    if(pdcc==NULL) return 0x0;
    if(user(usern)->instate==STD_CONN)
    {
	ssnprintf(user(usern)->insock,lngtxt(319),user(usern)->nick,pdcc->file,netc,pdcc->nick);
    }    
    fclose(pdcc->fhandle);
    removepdcc(usern);
}

int pdccfileclosed(int usern)
{
    struct dcct *pdcc;
    char netc[20];
    netc[0]=0;
    if(user(usern)->parent!=0)
    {
	ap_snprintf(netc,sizeof(netc),"%s'",user(usern)->network);
    }
    pdcc=getpsocketpdcc(usern,currentsocket);
    if(pdcc==NULL) return 0x0;
    if(user(usern)->instate==STD_CONN)
    {
	ssnprintf(user(usern)->insock,lngtxt(320),user(usern)->nick,pdcc->file,netc,pdcc->nick);
    }    
    fclose(pdcc->fhandle);
    removepdcc(usern);
}

int pdccfilesendack(int usern)
{
    struct dcct *pdcc;
    char *pt=ircbuf;
    unsigned long *addrp;
    unsigned long ackdcc;
    int rc;
    char netc[20];
    netc[0]=0;
    if(user(usern)->parent!=0)
    {
	ap_snprintf(netc,sizeof(netc),"%s'",user(usern)->network);
    }
    addrp=(unsigned long *)pt;
    ackdcc=ntohl(*addrp);
    pdcc=getpsocketpdcc(usern,currentsocket);
    if(pdcc==NULL) return 0x0;
    if(pdcc->transferred!=ackdcc)
    {
	return 0x0;
    }        
    if(feof(pdcc->fhandle)!=0)
    {
	if(user(usern)->instate==STD_CONN)
	{
	    p_log(LOG_INFO,usern,lngtxt(321),pdcc->file,netc,pdcc->nick);
	}
	fclose(pdcc->fhandle);
	removepdcc(usern);
    } else {
	rc=fread(stdcc,1,2048,pdcc->fhandle);    
	pdcc->lasttransferred=rc;
	pdcc->transferred+=rc;
	writesock_STREAM(pdcc->sock,stdcc,rc);
    }    
    return 0x0;
}

int acceptpdccfile(int usern)
{
    struct dcct *pdcc;
    int nsock,rc;
    int userp=usern;
    struct socketnodes *ps;
    char netc[20];
    netc[0]=0;
    if(user(usern)->parent!=0)
    {
	ap_snprintf(netc,sizeof(netc),"%s'",user(usern)->network);
    }
    pdcc=getpsocketpdcc(usern,mastersocket);
    if(pdcc==NULL) return 0x0;
    nsock=currentsocket->sock->syssock;
    if(nsock<=0) return -1;
    pdcc->fhandle=fopen(pdcc->file,"r");
    if(pdcc->fhandle==NULL) 
    {
	killsocket(nsock);
	return 0x0;
    }
    ps=getpsocketbysock(nsock);
    if(ps==NULL) return 0x0;
    ps->sock->dataflow=SD_STREAM;
    pdcc->delay=0;
    pdcc->sock=nsock;
    pdcc->type=PDC_SENDTO;
    mastersocket->sock->destructor=NULL;
    mastersocket->sock->errorhandler=NULL;
    killsocket(mastersocket->sock->syssock);
    strmncpy(pdcc->host,acceptip,sizeof(pdcc->host));
    if(user(usern)->instate==STD_CONN)
    {
	ssnprintf(user(usern)->insock,lngtxt(322),user(usern)->nick,netc,pdcc->nick,pdcc->host,pdcc->file);
    }    
    rc=fread(stdcc,1,2048,pdcc->fhandle);
    pdcc->lasttransferred=rc;
    pdcc->transferred=rc;
    writesock_STREAM(nsock,stdcc,rc);
    return 0x0;
}

/* file receive */

int pdccfconnected(int usern)
{
    struct dcct *pdcc;
    char netc[20];
    char mypath[100];
    mode_t oldum;
    netc[0]=0;
    if(user(usern)->parent!=0)
    {
	ap_snprintf(netc,sizeof(netc),"%s'",user(usern)->network);
    }
    pdcc=getpsocketpdcc(usern,currentsocket);
    if(pdcc==NULL) return 0x0;
    oldum=umask(0000);
    umask(0000);    
    mkdir(lngtxt(323), 0777 );
    ap_snprintf(mypath,sizeof(mypath),lngtxt(324),usern);
    mkdir(mypath, 0777 );
    umask(oldum);
    unlink(pdcc->file);
    pdcc->fhandle=fopen(pdcc->file,"w");
    if(pdcc->fhandle==NULL)
    {
	p_log(LOG_ERROR,usern,lngtxt(325),pdcc->file);
	killsocket(currentsocket->sock->syssock);
	return 0x0;
    }
    if(user(usern)->instate==STD_CONN)
    {
	ssnprintf(user(usern)->insock,lngtxt(326),user(usern)->nick,netc,pdcc->nick,pdcc->host,pdcc->file);
    }    
    return 0x0;
}

int pdccferror(int usern,int r)
{
    struct dcct *pdcc;
    char netc[20];
    netc[0]=0;
    if(user(usern)->parent!=0)
    {
	ap_snprintf(netc,sizeof(netc),"%s'",user(usern)->network);
    }
    pdcc=getpsocketpdcc(usern,currentsocket);
    if(pdcc==NULL) return 0x0;
    if(pdcc->fhandle!=NULL)
	fclose(pdcc->fhandle);
    if(user(usern)->instate==STD_CONN)
    {
	ssnprintf(user(usern)->insock,lngtxt(327),user(usern)->nick,netc,pdcc->nick,pdcc->host,pdcc->file);
    }    
    removepdcc(usern);
}

int pdccfget(int usern)
{
    unsigned long acks;
    unsigned long *addrptr;
    char *pt;
    int rc;
    struct dcct *pdcc;
    char netc[20];
    netc[0]=0;
    if(user(usern)->parent!=0)
    {
	ap_snprintf(netc,sizeof(netc),"%s'",user(usern)->network);
    }
    pdcc=getpsocketpdcc(usern,currentsocket);
    if(pdcc==NULL) return 0x0;
    pdcc->lasttransferred=currentsocket->sock->datalen;
    pdcc->transferred+=pdcc->lasttransferred;
    /* store data */
    rc=fwrite(ircbuf,1,pdcc->lasttransferred,pdcc->fhandle);
    if(rc!=pdcc->lasttransferred)
    {
	if(user(usern)->instate==STD_CONN)
	{
	    ssnprintf(user(usern)->insock,lngtxt(328),user(usern)->nick,netc,pdcc->nick,pdcc->host,pdcc->file);
	}
	fclose(pdcc->fhandle);
	removepdcc(usern);
	return 0x0;
    }
    /* hm. problem: data still could wait on the socket (due to a maximum of 8K buffersize)
       be sure to not send an ack until all data was processed */
    if(socketdatawaiting(currentsocket->sock->syssock)==1)
	return 0x0; /* dont send an ack while data still has to be processed */
    /* send back ack */
    acks=htonl(pdcc->transferred);
    addrptr=&acks;
    pt=(char *)addrptr;
    writesock_STREAM(currentsocket->sock->syssock,pt,4);
    return 0x0;
}

int pdccfclosed(int usern)
{
    struct dcct *pdcc;
    char netc[20];
    netc[0]=0;
    if(user(usern)->parent!=0)
    {
	ap_snprintf(netc,sizeof(netc),"%s'",user(usern)->network);
    }
    pdcc=getpsocketpdcc(usern,currentsocket);
    if(pdcc==NULL) return 0x0;
    {
	if(pdcc->filesize!=pdcc->transferred)
	    ssnprintf(user(usern)->insock,lngtxt(329),user(usern)->nick,netc,pdcc->nick,pdcc->host,pdcc->file);
	else
	    p_log(LOG_INFO,usern,lngtxt(330),pdcc->file,netc,pdcc->nick,pdcc->host);
    }    
    if(pdcc->fhandle!=NULL)
	fclose(pdcc->fhandle);    
    removepdcc(usern);
}

int pdccfremap(int usern, int newsocket)
{
    struct dcct *pdcc;
    pdcc=getpsocketpdcc(usern,currentsocket);
    if(pdcc)
	pdcc->sock=newsocket;
    return 0x0;
}

/* add a pending dcc connection */

int addpendingdcc(int usern, int type, char *host, int port, char *nick, char *file, char *newfile, unsigned long fsize, int eproto)
{
    struct dcct *pd;
    struct socketnodes *ps;
    int prt;
    char c=1;
    int sck;
    int proto=AF_INET;
    int rc,cnt=0;
#ifdef IPV6
    struct hostent *he;
    struct sockaddr_in6 sin6;
#endif
    struct sockaddr_in sin;
    unsigned long flen;
    unsigned long dip;
    FILE *check;
    char myfile[800];
    char myhost[200];
    char ihost[60];
    char en[64],an[64];
    char netc[20];
    char *nck;
    char *fil;
#ifdef HAVE_SSL
    int issl=SSL_OFF;
#else
    int issl=0;
#endif
    int mf=0;
    unsigned long dccreq;
    int userp=usern;
    pcontext;
    netc[0]=0;
    if(eproto!=0) proto=eproto;
    if(user(usern)->parent!=0)
    {
	userp=user(usern)->parent;
	ap_snprintf(netc,sizeof(netc),"%s'",user(usern)->network);
    }
    switch(type)
    {
	case PDC_CHATTORQ:
	    if(user(usern)->outstate!=STD_CONN)
	    {
		ssnprintf(user(usern)->insock,lngtxt(331),user(usern)->nick);
		return -1;
	    }
	    if(dcchost[0]==0)
	    {
		p_log(LOG_ERROR,usern,lngtxt(332));
		return -1;
	    }
	    sck=0;cnt=0;
	    while(sck<=0 && cnt<20)
	    {
		prt=randport();
		sck=createlistener(dcchost,prt,getprotocol(dcchost),1,acceptpdccchat,pdccerror,pdccquery,pdccclosed);
		cnt++;
	    }
	    if(sck<=0)
	    {
		p_log(LOG_ERROR,usern,lngtxt(333));
		return -1;
	    }
	    ps=getpsocketbysock(sck);
	    if(ps==NULL)
	    {
		p_log(LOG_ERROR,usern,lngtxt(334));
		return -1;
	    }
	    ps->sock->param=usern;
	    strmncpy(ps->sock->dest,"DCC",sizeof(ps->sock->dest));
	    pd=createpdcc(usern);
#ifdef HAVE_SSL
	    if(strstr(nick,"S=")==nick)
	    {
		nck=nick+2;
		ps->sock->ssl=SSL_ON;
		pd->ssl=SSL_ON;	    
	    } else
#endif
		nck=nick;
	    strmncpy(pd->nick,nck,sizeof(pd->nick));
	    strmncpy(pd->host,ps->sock->source,sizeof(pd->host));
	    pd->type=PDC_CHATTORQ;
	    pd->port=prt;
	    pd->sock=sck;
	    pd->uid=usern;
	    pd->delay=50;
	    if(user(usern)->instate==STD_CONN)
		ssnprintf(user(usern)->insock,lngtxt(335),user(usern)->nick,netc,pd->nick,pd->host,pd->port);
	    /* telling the user we want to dcc chat him */
#ifdef HAVE_SSL
	    if(pd->ssl==SSL_ON)
		ssnprintf(user(usern)->outsock,lngtxt(336),pd->nick,pd->host);
	    else
#endif
		ssnprintf(user(usern)->outsock,lngtxt(337),pd->nick,pd->host);
	    /* sending the request */
#ifdef IPV6
	    if(inet_pton(AF_INET6,dcchost,&sin6.sin6_addr)==1)
	    {
		sin6.sin6_family=AF_INET6;
		inet_ntop(AF_INET6,&sin6,myhost,sizeof(myhost));
		ssnprintf(user(usern)->outsock,lngtxt(338),pd->nick,c,myhost,pd->port,c);
	    } else
#endif
		if(dcchost[0])
		    ssnprintf(user(usern)->outsock,lngtxt(339),pd->nick,c,htonl(inet_addr(dcchost)),pd->port,c);
		else
		    ssnprintf(user(usern)->outsock,lngtxt(339),pd->nick,c,htonl(inet_addr(pd->host)),pd->port,c);
	    return 0x0; 	    
	case PDC_CHATFROMRQ:
	    dccreq=user(usern)->lastdccchat;
	    time(&user(usern)->lastdccchat);
	    if((user(usern)->lastdccchat-dccreq)<5) return -1; /* 5 seconds before next chat */
	    if(port<1024 || port>65535) return -1; /* silent ignore */
	    pd=createpdcc(usern);
	    if(pd!=NULL)
	    {
		pd->delay=50;
		pd->uid=usern;
		pd->type=PDC_CHATFROMRQ;
		pd->port=port;
		if(strchr(host,':')==NULL) /* Not an IPV6 Host */
		{
		    sscanf(host,"%lu",&dip);
		    sin.sin_addr.s_addr=htonl(dip);
		    strmncpy(pd->host,inet_ntoa(sin.sin_addr),sizeof(pd->host));
		} else
		    strmncpy(pd->host,host,sizeof(pd->host));
		strmncpy(pd->nick,nick,sizeof(pd->nick));
#ifdef HAVE_SSL
    		ssnprintf(user(usern)->insock,lngtxt(340),user(usern)->nick,netc,pd->nick,pd->nick,pd->nick,pd->host,pd->port);
#else
    		ssnprintf(user(usern)->insock,lngtxt(341),user(usern)->nick,netc,pd->nick,pd->nick,pd->host,pd->port);
#endif		
#ifdef SCRIPTING
		pd->pid=dccchatscript(usern,ircfrom);
#endif
		return 0x0;
	    }
	    break;
	case PDC_CHATFROM:
	    pd=user(usern)->pdcc;
#ifdef HAVE_SSL
	    issl=SSL_OFF;
	    if(strstr(nick,"S=")==nick)
	    {
		nck=nick+2;
		strmncpy(en,nck,sizeof(en));
		issl=SSL_ON;
	    } else
#endif	    
		strmncpy(en,nick,sizeof(en));
	    ucase(en);
	    while(pd!=NULL)
	    {
		strmncpy(an,pd->nick,sizeof(an));
		ucase(an);
		if(pd->type==PDC_CHATFROMRQ && strmcmp(en,an)==1)
		{
#ifdef HAVE_SSL
		    sck=createsocket(0,ST_CONNECT,usern,SGR_NONE,NULL,pdccconnected,pdccerror,pdccquery,pdccclosed,pdccremap,proto,SSL_OFF);
#else
		    sck=createsocket(0,ST_CONNECT,usern,SGR_NONE,NULL,pdccconnected,pdccerror,pdccquery,pdccclosed,pdccremap,proto,0);
#endif
		    if(sck!=0) /* silently ignore errors */
		    {
#ifdef HAVE_SSL
			ps=getpsocketbysock(sck);
			if(ps!=NULL)
			    ps->sock->ssl=issl;
			pd->ssl=issl;
#endif
			pd->sock=sck;
			pd->delay=0;
			pd->type=PDC_CHATFROM;
		    }	
		    sck=connectto(sck,pd->host,pd->port,user(usern)->vhost);
		    if(sck!=0) return 0x0;
		}
		pd=pd->next;
	    }
	    if(user(usern)->instate==STD_CONN)
		ssnprintf(user(usern)->insock,lngtxt(342),user(usern)->nick,nick);
	    break;
	case PDC_SENDTORQ:
	    if(user(usern)->outstate!=STD_CONN)
	    {
		ssnprintf(user(usern)->insock,lngtxt(343),user(usern)->nick);
		return -1;
	    }
	    if(dcchost[0]==0)
	    {
		p_log(LOG_ERROR,usern,lngtxt(344));
		return -1;
	    }
	    fil=stripdccfile(file,0);
	    if(fil==NULL)
	    {
		p_log(LOG_ERROR,usern,lngtxt(345),file);
		return -1;
	    }
	    check=fopen(file,"r");
	    if(check==NULL)
	    {
		ap_snprintf(myfile,sizeof(myfile),lngtxt(346),usern,fil);
		check=fopen(myfile,"r");
		if(check==NULL)
		{		
		    ssnprintf(user(usern)->insock,lngtxt(347),user(usern)->nick,file);
		    return -1;
		}
		mf=1;
	    }
	    fseek(check,0,SEEK_END);
	    flen=ftell(check);
	    fclose(check);
	    sck=0;cnt=0;
	    while(sck<=0 && cnt<20)
	    {
		prt=randport();
		sck=createlistener(dcchost,prt,getprotocol(dcchost),1,acceptpdccfile,pdccfileerror,pdccfilesendack,pdccfileclosed);
		cnt++;
	    }
	    if(sck<=0)
	    {
		p_log(LOG_ERROR,usern,lngtxt(348));
		return -1;
	    }
	    ps=getpsocketbysock(sck);
	    if(ps==NULL)
	    {
		p_log(LOG_ERROR,usern,lngtxt(349));
		return -1;
	    }
	    ps->sock->param=usern;
	    strmncpy(ps->sock->dest,"DCC",sizeof(ps->sock->dest));
	    pd=createpdcc(usern);
#ifdef HAVE_SSL
	    issl=SSL_OFF;
#else
	    issl=0;
#endif
#ifdef HAVE_SSL
	    if(strstr(nick,"S=")==nick)
	    {
		nck=nick+2;
		issl=SSL_ON;
	    } else
#endif
		nck=nick;
	    strmncpy(pd->nick,nck,sizeof(pd->nick));
	    strmncpy(pd->host,ps->sock->source,sizeof(pd->host));
	    if(mf==1)
		strmncpy(pd->file,myfile,sizeof(pd->file));
	    else
		strmncpy(pd->file,file,sizeof(pd->file));
    	    pd->type=PDC_SENDTORQ;
#ifdef HAVE_SSL
	    pd->ssl=issl;
	    ps->sock->ssl=issl;
#endif
    	    pd->port=prt;
	    pd->sock=sck;
	    pd->uid=usern;
	    pd->filesize=flen;
	    pd->delay=50;
	    if(user(usern)->instate==STD_CONN)
	    {
#ifdef HAVE_SSL
		if(issl==SSL_ON)
		    ssnprintf(user(usern)->insock,lngtxt(350),user(usern)->nick,fil,netc,pd->nick,pd->host,pd->port);
		else
#endif
		    ssnprintf(user(usern)->insock,lngtxt(351),user(usern)->nick,fil,netc,pd->nick,pd->host,pd->port);
	    }
	    /* telling the user we want to dcc send him a file */
#ifdef HAVE_SSL
	    if(issl==SSL_ON)
		ssnprintf(user(usern)->outsock,lngtxt(352),pd->nick,fil,pd->host);
	    else
#endif
		ssnprintf(user(usern)->outsock,lngtxt(353),pd->nick,fil,pd->host);
	    /* sending the request */
#ifdef IPV6
	    if(inet_pton(AF_INET6,dcchost,&sin6.sin6_addr)==1)
	    {
		sin6.sin6_family=AF_INET6;
		inet_ntop(AF_INET6,&sin6,myhost,sizeof(myhost));
		ssnprintf(user(usern)->outsock,lngtxt(354),pd->nick,c,fil,myhost,pd->port,flen,c);         
	    } else
#endif
		if(dcchost[0])
		    ssnprintf(user(usern)->outsock,lngtxt(355),pd->nick,c,fil,htonl(inet_addr(dcchost)),pd->port,flen,c);
		else
		    ssnprintf(user(usern)->outsock,lngtxt(355),pd->nick,c,fil,htonl(inet_addr(pd->host)),pd->port,flen,c);
	    return 0x0;
	case PDC_RECVFROMRQ:
	    if(port<1024 || port>65535) return -1; /* silent ignore of lame from ports */
	    fil=stripdccfile(file,1);
	    if(fil==NULL) return -1; /* silent ignore of bogus file sends */
	    /* file should be the file itself now, without /,.. or ~ */
	    ap_snprintf(myfile,sizeof(myfile),lngtxt(356),usern,fil);
	    pd=createpdcc(usern);
	    if(pd!=NULL)
	    {
		pd->delay=50;
		pd->uid=usern;
		pd->type=PDC_RECVFROMRQ;
		pd->port=port;
		strmncpy(pd->file,myfile,sizeof(pd->file));
		pd->filesize=fsize;
		if(strchr(host,':')==NULL) /* Its an ipv4 adress */
		{
		    sscanf(host,"%lu",&dip);
		    sin.sin_addr.s_addr=htonl(dip);
		    strmncpy(pd->host,inet_ntoa(sin.sin_addr),sizeof(pd->host));
		} else
		    strmncpy(pd->host,host,sizeof(pd->host));
		strmncpy(pd->nick,nick,sizeof(pd->nick));
		if(user(usern)->autogetdcc==1)
		{
		    ssnprintf(user(usern)->insock,lngtxt(357),user(usern)->nick,netc,pd->nick,fil,netc,pd->host,pd->port);
		    /* recursive */
		    addpendingdcc(usern, PDC_RECVFROM, pd->host, pd->port, pd->nick, fil, NULL, pd->filesize, getprotocol(pd->host));
		} else {
#ifdef HAVE_SSL
		    ssnprintf(user(usern)->insock,lngtxt(358),user(usern)->nick,netc,pd->nick,fil,netc,pd->nick,fil,netc,pd->nick,fil,pd->host,pd->port);
#else
		    ssnprintf(user(usern)->insock,lngtxt(359),user(usern)->nick,netc,pd->nick,fil,netc,pd->nick,fil,pd->host,pd->port);
#endif
		}
#ifdef SCRIPTING
		strmncpy(irccontent,pd->file,sizeof(irccontent));
		pd->pid=dccfilescript(usern,ircfrom);
#endif
		return 0x0;
	    }
	    break;
	case PDC_RECVFROM:
	    pd=user(usern)->pdcc;
#ifdef HAVE_SSL
	    issl=SSL_OFF;
	    if(strstr(nick,"S=")==nick)
	    {
		nck=nick+2;
		issl=SSL_ON;
	    } else
#endif
		nck=nick;
	    while(pd!=NULL)
	    {
		if(pd->type==PDC_RECVFROMRQ && strmncasecmp(nck,pd->nick)==1)
		{
		    fil=stripdccfile(pd->file,0);
		    if(fil!=NULL)
		    {
		        if (strmncasecmp(file,fil)!=0 || *file == '*')
			{ 
			    proto=getprotocol(pd->host);
			    sck=createsocket(0,ST_CONNECT,usern,SGR_NONE,NULL,pdccfconnected,pdccferror,pdccfget,pdccfclosed,pdccfremap,proto,issl);
			    /* offering a possibility to change the filename (RFC..) */
			    if(newfile!=NULL && *file != '*')
			    {
				strmncpy(pd->file,newfile,sizeof(pd->file));
			    }
			    if(sck!=0) /* silently ignore errors */
			    {
				ps=getpsocketbysock(sck);
				if(ps!=NULL)
				{
				    ps->sock->dataflow=SD_STREAM;
#ifdef HAVE_SSL
				    ps->sock->ssl=issl;
#endif
				    pd->sock=sck;
				    pd->delay=0;
				    pd->type=PDC_RECVFROM;
#ifdef HAVE_SSL
				    pd->ssl=issl;
#endif
				}
			    }	
			    sck=connectto(sck,pd->host,pd->port,user(usern)->vhost);
			    if(sck!=0)
				return 0x0;
			}
		    }
		}
		pd=pd->next;
	    }
	    if(user(usern)->instate==STD_CONN)
		ssnprintf(user(usern)->insock,lngtxt(360),user(usern)->nick,nick,file);
	    break;
	
    }
    return -1;    
}

/* querying to a dcc chat */

int querydccchat(int usern, char *nick)
{
    struct dcct *pdcc;
    pdcc=user(usern)->pdcc;
    while(pdcc!=NULL)
    {
	if(pdcc->type==PDC_CHATFROM || pdcc->type==PDC_CHATTO)
	{
	    if(strmncasecmp(nick,pdcc->nick)==1)
	    {
		writesock(pdcc->sock,irccontent);
		return 0x1;
	    }
	}
	pdcc=pdcc->next;
    }
    return 0x0;
}

/* checking timeouts for userdccs */

int checkdcctimeouts()
{
    struct usernodes *un;
    struct socketnodes *ps;
    struct dcct *pdcc,*epdcc=NULL,*prevdcc=NULL,*ipdcc;
    pcontext;
    un=usernode;
    while(un!=NULL)
    {
	if(un->user!=NULL)
	{
	    if(un->user->pdcc!=NULL)
	    {
		pdcc=un->user->pdcc;
		epdcc=NULL;prevdcc=NULL;
		while(pdcc!=NULL)
		{
		    epdcc=pdcc->next;
		    ipdcc=pdcc;
		    if((pdcc->type==PDC_SENDTORQ || pdcc->type==PDC_RECVFROMRQ || 
		       pdcc->type==PDC_CHATTORQ || pdcc->type==PDC_CHATFROMRQ) && 
		       pdcc->delay>0)
		    {
			pdcc->delay--;
			if(pdcc->delay==0)
			{
			    if(prevdcc==NULL)
			    {
				un->user->pdcc=epdcc;
				ipdcc=NULL;
			    } else {
				prevdcc->next=epdcc;
				ipdcc=prevdcc; /* bug -> just a case of prevdcc */
			    }
#ifdef SCRIPTING
			    if(pdcc->pid!=0)
			    {
				if(getsubtaskbypid(pdcc->pid)!=NULL)
				    terminatetask(pdcc->pid,0);
			    }
#endif
			    ps=getpsocketbysock(pdcc->sock);
			    if(ps!=NULL)
			    {
				killsocket(pdcc->sock);
			    }
			    free(pdcc);
			}
		    }
		    prevdcc=ipdcc;
		    pdcc=epdcc;
		}
	    }
	}
	un=un->next;
    }
    return 0x0;
}

/* list the pending dccs  */

int listpdccs(int usern)
{
    char *dcckind[]={
    "Unknown",
    "DCC Chat Request To",
    "DCC Chat To",
    "DCC Chat Request From",
    "DCC Chat From",
    "DCC Send Request To",
    "DCC Send To ",
    "DCC Send Request From",
    "DCC Send From",
    "Unknown"
    };
    int idx,pnd=0;
    struct dcct *pdcc;
    pdcc=user(usern)->pdcc;
    while(pdcc!=NULL)
    {
	idx=pdcc->type;
	if(idx>0 && idx <9)
	{
	    if(pnd==0)
	    {
		ssnprintf(user(usern)->insock,lngtxt(361),user(usern)->nick);
		pnd=1;
	    }
	    if(idx>4)
		ssnprintf(user(usern)->insock,lngtxt(362),user(usern)->nick,pdcc->sock,dcckind[idx],pdcc->nick,pdcc->file);
	    else
		ssnprintf(user(usern)->insock,lngtxt(363),user(usern)->nick,pdcc->sock,dcckind[idx],pdcc->nick);
	}
	pdcc=pdcc->next;
    }
    if(pnd==1)
    {
	ssnprintf(user(usern)->insock,lngtxt(364),user(usern)->nick);
    }
    return 0x0;
}

/* cancel a dcc */

int canceldcc(int usern, char *nick, char *file)
{
    struct dcct *pdcc,*epdcc;
    struct socketnodes *sckn;
    char *fil;
    sckn=currentsocket;
    pdcc=user(usern)->pdcc;
    while(pdcc!=NULL)
    {
	epdcc=pdcc->next;
	if(file==NULL && (pdcc->type==PDC_CHATTO || pdcc->type==PDC_CHATFROM))
	{
	    if(strmncasecmp(nick,pdcc->nick)==1)
	    {
		currentsocket=getpsocketbysock(pdcc->sock);
		if(currentsocket!=NULL)
		{
		    pdccerror(usern,0);
		}
		currentsocket=sckn;
		return 0x0;
	    }
	} else 
	if (file!=NULL) 
	{
	    if(pdcc->type==PDC_RECVFROM || pdcc->type==PDC_SENDTO)
	    {
		fil=stripdccfile(pdcc->file,0);
		if(fil==NULL) return 0x0;
		if(strmncasecmp(nick,pdcc->nick)==1 && (strmncasecmp(file,fil)==1 || *file=='*'))
		{
		    currentsocket=getpsocketbysock(pdcc->sock);
		    if(currentsocket!=NULL)
		    {
			pdccferror(usern,0);
		    }
		    currentsocket=sckn;
		    return 0x0;
		}
	    }	
	}
	pdcc=epdcc;
    }
    ssnprintf(user(usern)->insock,lngtxt(365),user(usern)->nick);
    return 0x0;
}

/* answering a ctcp request */

int answerctcp(int usern,char *nick, char *ctcp)
{
    char c=1;
    char buf[300];
#ifdef SCRIPTING
    if(ctcpscript(usern,ctcp)==0)
    {
#endif 
#ifdef CTCPVERSION
	if(strmncasecmp(ctcp,lngtxt(366)))
	{
	    ap_snprintf(buf,sizeof(buf),lngtxt(367),nick,c);
	    ssnprintf(user(usern)->outsock,"%s%s%c",buf,CTCPVERSION,c);
	}
#endif
#ifdef SCRIPTING
    }
#endif   
    return 0x0;
}

/* parsing a sent request from a user */

int parsectcps(int usern)
{
    static char host[200],file[200];
    int port;
    unsigned long filelen;
    char *pt,*ept,*pt2;
    if(*irccontent==0x1 && strmncasecmp(ircto,user(usern)->nick)==1)
    {
	if(strchr(irccontent,' ')==NULL)
	{
	    pt=irccontent+1;
	    pt2=strchr(pt,1);
	    if(pt2!=NULL)
	    {
		*pt2=0;
		return answerctcp(usern,ircnick,pt);
	    }
	    /* a request for a ctcp */
	} else {
	    /* an answer or a dcc */
#ifdef DCCCHAT
	    ept=irccontent+1;
	    if(strstr(ept,lngtxt(368))==ept && user(usern)->dccenabled==1)
	    {
		pt=irccontent+10;
		pt=strchr(pt,' ');
		if(pt!=NULL)
		{
		    pt++;
		    pt2=strchr(pt,' ');
		    if(pt2!=NULL)
		    {
			*pt2=0;
			strmncpy(host,pt,sizeof(host));
			pt2++;
			pt=strchr(pt2,1);
			if(pt!=NULL)
			{
			    *pt=0;
			    port=atoi(pt2);
			    addpendingdcc(usern, PDC_CHATFROMRQ, host, port, ircnick, NULL, NULL, 0L, getprotocol(host));
			    return 0x1;
			}
		    }
		}
	    }
#endif	
#ifdef DCCFILES
	    ept=irccontent+1;
	    if(strstr(ept,lngtxt(369))==ept && strmncasecmp(ircnick,user(usern)->nick)==0 && user(usern)->dccenabled==1)
	    {
		pt=irccontent+10;
		pt=strchr(pt,' ');
		if(pt!=NULL)
		{
		    *pt=0;
		    ept=irccontent+10;
		    strmncpy(file,ept,sizeof(file));
		    pt++;
		    pt2=strchr(pt,' ');
		    if(pt2!=NULL)
		    {
			*pt2=0;
			strmncpy(host,pt,sizeof(host));
			pt2++;
			pt=strchr(pt2,' ');
			if(pt!=NULL)
			{
			    *pt=0;
			    port=atoi(pt2);
			    pt++;
			    pt2=strchr(pt,1);
			    if(pt2!=NULL)
			    {
				*pt2=0;			
				filelen=atol(pt);
				addpendingdcc(usern, PDC_RECVFROMRQ, host, port, ircnick, file, NULL, filelen, getprotocol(host));
				return 0x1;
			    }
			}
		    }
		}
	    
	    }
#endif	    
	}
    }
    return 0x0;
}
