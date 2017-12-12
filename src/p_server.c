/* $Id: p_server.c,v 1.7 2005/06/04 18:00:14 hisi Exp $ */
/************************************************************************
 *   psybnc2.3.2, src/p_server.c
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
static char rcsid[] = "@(#)$Id: p_server.c,v 1.7 2005/06/04 18:00:14 hisi Exp $";
#endif

#define P_SERVER

#include <p_global.h>

jmp_buf serveralarmret;

#ifndef BLOCKDNS
int userproxyconnectresolved(struct resolve *rp);
int userproxyconnectnotresolved(struct resolve *rp);
#endif

int serverinit(int usern,int msgn)
{
    FILE *userb;
    char fname[40];
    char sic[4096];
    char *zer;
    int uusern;
    pcontext;
    uusern=usern;
    if(user(usern)->parent!=0) uusern=user(usern)->parent;
    if (user(usern)->srinit == 0) return 0x0;
    ap_snprintf(fname,sizeof(fname),lngtxt(725),usern);
    if (msgn == 1) 
    {
       resetallchannels(usern);
       zer=strstr(ircbuf,lngtxt(726));
       if(zer!=NULL)
       {
           zer=strchr(zer,'@');
	   if(zer!=NULL)
	   {
	       zer++;
	       *zer=0;
	       if(strlen(ircbuf)+strlen(user(uusern)->host)<sizeof(ircbuf))
	           strcat(ircbuf,user(uusern)->host);
	   }
       }
       user(usern)->welcome=1;
       if (user(usern)->channels!=NULL && user(usern)->leavequit==0) {
          rejoinchannels(usern);
       }
       strmncpy(user(usern)->server,ircfrom,sizeof(user(usern)->server));
#ifdef INTNET
       if(user(usern)->parent==0)
       {
           strmncpy(irccontent,ircto,sizeof(irccontent));
           strmncpy(sic,ircbuf,sizeof(sic));
           cmdintnick(usern,0);
           strmncpy(ircbuf,sic,sizeof(ircbuf));
           parse();
       }
#endif
       /* new since version 2.3.2, show nick change, if msg 001 did show another nick */
       if (user(usern)->instate == STD_CONN && user(usern)->parent==0 && user(usern)->nick[0]!=0) 
       {
	    if(!strmcmp(ircto,user(usern)->nick))
		ssnprintf(user(usern)->insock,lngtxt(53),user(usern)->nick,user(usern)->login,user(usern)->host,ircto);
       }
       strmncpy(user(usern)->nick,ircto,sizeof(user(usern)->nick));
       strmncpy(user(usern)->firstnick,ircto,sizeof(user(usern)->firstnick));
       ssnprintf(user(usern)->outsock,lngtxt(727),ircto);
       userb=fopen(fname,"w");
       user(usern)->gotop=0;
    } else {
       userb=fopen(fname,"a");
    }
    if(userb)
    {
	fprintf(userb,"%s",ircbuf);
	fclose(userb);
    }
    if(user(usern)->parent!=0) ircbuf[0]=0;
    return 1;
}

int pong(int usern)
{
    char buf[400];
    struct uchannelt *chan;
    pcontext;
    if (user(usern)->outstate != STD_CONN) return 0x0;
    /* we try to regain nick on ping, antiidle, rejoin channels, reop us and save actual channel stats */
    if (strlen(irccontent)>0) { /* some servers need ponged content of ping */
	ap_snprintf(buf,sizeof(buf),lngtxt(728),irccontent);
    } else {
	ap_snprintf(buf,sizeof(buf),lngtxt(729),user(usern)->server);
    }
    writesock_URGENT(user(usern)->outsock,buf);
    user(usern)->autopongreply=0;
    if (user(usern)->instate != STD_CONN) {
       if (user(usern)->channels!=NULL && user(usern)->leavequit==0 && user(usern)->autorejoin==1) {
          rejoinchannels(usern);
       }
       if(user(usern)->antiidle==1)
           ssnprintf(user(usern)->outsock,lngtxt(730),user(usern)->nick);
       if(*user(usern)->awaynick!=0)
           ssnprintf(user(usern)->outsock,lngtxt(731),user(usern)->awaynick);
       else	
           ssnprintf(user(usern)->outsock,lngtxt(732),user(usern)->wantnick);
       chan=user(usern)->channels;
       while(chan!=NULL)
       {
	    askforop(usern,chan->name);
	    chan=chan->next;
       }
    }
}

char nnick[64];

char *numnick(char *original)
{
    int maxlen=((strlen(original)>9)?64:9);
    char numb[6];
    ap_snprintf(numb,sizeof(numb),"%lu",rand()%999);
    strmncpy(nnick,original,sizeof(nnick));
    nnick[maxlen-strlen(numb)]=0;
    strcat(nnick,numb);
    return nnick;
}

int nickused(int usern)
{
    char buf[400];
    int cnt;
    char *pt;
    char *pt2;
    pcontext;
    /* This was removed. Also if a client is possibly handling this, 
       lets takeover this task. suggested by warlock */
/*    if (user(usern)->instate == STD_CONN && user(usern)->parent==0) {
	writesock(user(usern)->insock,ircbuf);
	return -1;
    }*/
    if (user(usern)->welcome == 1) {
	return -1; /* we fuck the jupe, it happened after we tried to change the nick */
    }
    ap_snprintf(buf,sizeof(buf),"%s",ircto);
    /* figher, infect and p say: random-nicks are evil */
    pt=strchr(ircbuf,' ');
    if(pt) 
    {
	pt++;
	pt=strchr(pt,' ');
	if(pt)
	{
	    pt++;
	    pt=strchr(pt,' ');
	}
    }
    if(pt)
    {
	pt++;
	pt2=strchr(pt,' ');
	if(pt2)
	{
	    *pt2=0;
	    if(strmcmp(user(usern)->wantnick,pt))
	    {
		if(user(usern)->altnick[0]==0)
		{
		    ap_snprintf(buf,sizeof(buf),"_%s",user(usern)->wantnick);
		}
		ssnprintf(user(usern)->outsock,lngtxt(733),buf);
		return 0x0;
	    }
	}
    }
    ssnprintf(user(usern)->outsock,lngtxt(733),numnick(user(usern)->wantnick));
    return 0x0;
}

int gotnick(int usern)
{
    char sic[4096];
    pcontext;
    if(strmcmp(ircnick,user(usern)->nick))
    {
	strmncpy(user(usern)->nick,ircto,sizeof(user(usern)->nick));
	ap_snprintf(sic,sizeof(sic),lngtxt(734),usern);
	writeini("USER","NICK",sic,ircto);
#ifdef INTNET
	if(user(usern)->parent==0)
	{
	    strmncpy(irccontent,ircto,sizeof(irccontent));
	    strmncpy(sic,ircbuf,sizeof(sic));
	    cmdintnick(usern,0);
	    strmncpy(ircbuf,sic,sizeof(ircbuf));
	}
#endif
    }
    nickchange(usern,ircnick,ircto);
    return 0x0;    
}

int rejoinchannels(int usern)
{
    struct uchannelt *chan;
    if(user(usern)->outstate!=STD_CONN) return 0x0; /* not connected */
    chan=user(usern)->channels;
    while(chan!=NULL)
    {
	if(chan->ison==0)
	{
	    if(strchr(chan->modes,'k')!=NULL)
		ssnprintf(user(usern)->outsock,lngtxt(735),chan->name,chan->key);
	    else
		ssnprintf(user(usern)->outsock,lngtxt(736),chan->name);
	}
	chan=chan->next;
    }
    return 0x0;
}

/* leaving all channels */

int partleavemsg(int usern)
{
    struct uchannelt *chan;
    chan=user(usern)->channels;
    while(chan!=NULL)
    {
	if(chan->ison==1)
	{
	    if(strmcmp(user(usern)->leavemsg,lngtxt(737))==1)
		ssnprintf(user(usern)->outsock,lngtxt(738),chan->name,irccontent);
	    else 
		ssnprintf(user(usern)->outsock,lngtxt(739),chan->name,user(usern)->leavemsg);
	}
	chan=chan->next;
    }
    return 0x0;
}

/* sending a leavemsg to all channels we are on */

int sendleavemsg(int usern)
{
    struct uchannelt *chan;
    chan=user(usern)->channels;
    while(chan!=NULL)
    {
	if(chan->ison==1)
	{
	    if(strmcmp(user(usern)->leavemsg,lngtxt(740))==1)
		ssnprintf(user(usern)->outsock,lngtxt(741),chan->name,1,irccontent,1);
	    else
		ssnprintf(user(usern)->outsock,lngtxt(742),chan->name,1,user(usern)->leavemsg,1);
	}
	chan=chan->next;
    }
    return 0x0;
}

int rejoinclient(int usern)
{
    struct uchannelt *chan;
    struct uchannelusert *chanuser;
    char netc[15];
    char mynick[64];
    char md[2];
    char *pt;
    pcontext;
    md[1]=0;
    user(usern)->afterquit=0;
    chan=user(usern)->channels;
    if (user(usern)->parent != 0)
    {
	ap_snprintf(netc,sizeof(netc),"#%s'",user(usern)->network);
	strmncpy(mynick,user(user(usern)->parent)->nick,sizeof(mynick));
    } else {
	netc[0]=0;
	strmncpy(mynick,user(usern)->nick,sizeof(mynick));
    }
    while(chan!=NULL)
    {
	if(chan->ison==1)
	{
	    ssnprintf(user(usern)->insock,lngtxt(743),mynick,user(usern)->login,user(usern)->host,netc,chan->name);
	    if(chan->topic[0]!=0)
		ssnprintf(user(usern)->insock,lngtxt(744),user(usern)->server,user(usern)->nick,netc,chan->name,chan->topic);
	    /* for bitchx and other clients - the names :) */
	    sendnames(usern,chan);
	    chanuser=getchannelnick(chan,user(usern)->nick);
	    if(chanuser!=NULL) /* should not happen */
	    {
		pt=strchr(chan->prefixchars,chanuser->mode[0]);
		if(pt)
		{
		    pt=pt-chan->prefixchars+chan->prefixmodes;
		    md[0]=*pt;
		}	
		if (strchr(chan->prefixchars,chanuser->mode[0])!=NULL && chanuser->mode[0]!=0)
		    ssnprintf(user(usern)->insock,lngtxt(745),netc,chan->name,md[0],mynick);
	    }
	}
	chan=chan->next;
    }
    user(usern)->instate = STD_CONN;
    return 0x0;
}

int checkbans(int usern)
{
    char bc=')';
    int userp;
    pcontext;
    if (user(usern)->parent!=0) userp=user(usern)->parent; else userp=usern;
    if (checkstrings(user(usern)->bans)) 
    {
	if(*ehost!=bc && hasmode(usern,user(usern)->nick,ircto,'@')==0x1)
	{
	    ssnprintf(user(usern)->outsock,lngtxt(746),ircto,ehost);
	    ssnprintf(user(usern)->outsock,lngtxt(747),ircto,ircnick,eparm);
	    if (user(usern)->instate == STD_CONN) {
   	       ssnprintf(user(usern)->insock,lngtxt(748),user(userp)->nick,ircnick,ircfrom,ehost,ircto);
	    }
	}
    }
}

int checkautoop(int usern)
{
    char bc=')';
    int userp;
    pcontext;
    if (user(usern)->parent!=0) userp=user(usern)->parent; else userp=usern;
    if (checkstrings(user(usern)->aops)) 
    {
	if(*ehost!=bc && hasmode(usern,user(usern)->nick,ircto,'@')==0x1)
	{
	    ssnprintf(user(usern)->outsock,lngtxt(749),ircto,ircnick);
	    if (user(usern)->instate == STD_CONN) {
   	       ssnprintf(user(usern)->insock,lngtxt(750),user(userp)->nick,ircnick,ircfrom,ehost,ircto);
	    }
	}
    }
}

int checkop(int usern)
{
    char *pt;
    char *pt1;
    char bc='-';
    char *pt2;
    int userp;
    pcontext;
    if (user(usern)->parent!=0) userp=user(usern)->parent; else userp=usern;
    pt1 = irccontent +3;
    while(*pt1==' ') pt1++;
    if (*pt1 == 0) return -1;
    pt = strchr(pt1,' '); /* the pass */
    if (pt == NULL) return -1;
    *pt=0;
    pt++;
    while(*pt==' ') pt++;
    pt2=pt;
    pt=pt1;
    strmncpy(ircto,pt2,sizeof(ircto));
    if (checkstrings(user(usern)->ops)) 
    {
	if(*ehost!=bc && hasmode(usern,user(usern)->nick,ircto,'@')==0x1)
	{
	    if (strmcmp(pt,eparm)) {
 		ssnprintf(user(usern)->outsock,lngtxt(751),pt2,ircnick);
		if (user(usern)->instate == STD_CONN) {
   	           ssnprintf(user(usern)->insock,lngtxt(752),user(userp)->nick,ircnick,ircfrom,ehost,pt2);
  	           return 0x0;
		}
	    }
	}
    }
    return -1;
}

/* check, if an ignore is triggerd. returns 1, if match */

int checkignores(int usern)
{
    int rc=0;
    char ighost[500];
    char *content;
    struct stringarray *ts;    
    for(ts=user(usern)->ignores;ts;ts=ts->next)
    {	
	strmncpy(ighost,ts->entry,sizeof(ighost));
	content=strchr(ighost,';');
	if(content!=NULL)
	{
	    *content=0;
	    content++;
	    if(wild_match(ighost,ircfrom) || (strlen(ighost)==1 && *ighost=='*'))
	    {
		if(strstr(irccontent,content)!=NULL || (strlen(content)==1 && *content=='*'))
		{
		    rc=1;
		    break;
		}
	    }
	}
    }
    return rc;
}

/* check a host if it will be asked */

int getops(int usern)
{
    struct datalinkt *dccbot;
    char buf[1024];
    char bc='-',bc1=')';
    int userp;
    pcontext;
    if (user(usern)->parent!=0) userp=user(usern)->parent; else userp=usern;
    if (strstr(irccontent,"@") == NULL) return 0x0;
    if (checkstrings(user(usern)->askops)) {
        if (*ehost==bc || *ehost==bc1) {
	    dccbot=checkdcc(usern,ehost+1);
	    if (dccbot !=NULL)
	    {
		if (dccbot->outstate==STD_CONN)
		{
		    ap_snprintf(buf,sizeof(buf),lngtxt(753),user(usern)->nick,echan);
		    writesock_DELAY(dccbot->outsock,buf,3);
		    user(usern)->gotop=1; /* no multiple asks */
		}
	    }
	    return 0x0;
	}
	user(usern)->gotop=1; /* no multiple asks */
	ap_snprintf(buf,sizeof(buf),lngtxt(754),ircnick,eparm,ircto);
	writesock_DELAY(user(usern)->outsock,buf,3);
	if (user(usern)->instate == STD_CONN) {
   	   ssnprintf(user(usern)->insock,lngtxt(755),user(userp)->nick,ircnick,ircfrom,ircto);
	}
    }
    return 0x0;
}

/* ask for op for a single channel */

int askforop(int usern, char *channel)
{
    struct uchannelt *chan;
    struct uchannelusert *usr;

    chan=getuserchannel(usern,channel);
    if(chan!=NULL)
    {
	if(chan->ison!=0)
	{
	    usr=getchannelnick(chan,user(usern)->nick);
	    if(usr!=NULL)
	    {
		if(strchr(usr->mode,'@')==NULL)
		{
		    user(usern)->gotop=0;
		    usr=chan->users;
		    while(user(usern)->gotop==0 && usr!=NULL)
		    {
			if(strchr(usr->mode,'@')!=NULL && strlen(usr->host)>1)
			{
			    ap_snprintf(ircfrom,sizeof(ircfrom),lngtxt(756),usr->nick,usr->ident,usr->host);
			    strmncpy(ircto,channel,sizeof(ircto));
			    strmncpy(ircnick,usr->nick,sizeof(ircnick));
			    strmncpy(irchost,usr->host,sizeof(irchost));
			    strmncpy(ircident,usr->ident,sizeof(ircident));
			    strmncpy(irccontent,usr->mode,sizeof(irccontent));
			    getops(usern);
			}   
			usr=usr->next;
		    }
		}
	    }
	}
    }
    return 0x0;
}

/* send who to the channel on join */

int sendwho(int usern)
{
    struct uchannelt *chan;
    pcontext;
    chan=getuserchannel(usern,ircto);
    if(chan!=NULL) removeallusersfromchannel(chan);
    ssnprintf(user(usern)->outsock,lngtxt(757),ircto);
    return 0x0;
}

/* who entry - add to the channel structure */

int performwho(int usern)
{
    char hnick[64];
    char hident[64];
    char hhost[1024];
    char hchan[1024];
    char hbuf[8192];
    char flags[10];
    char *p1;
    char *p2;
    char *pt;
    struct uchannelt *chan;
    struct uchannelusert *chanuser;
    pcontext;
    strmncpy(hbuf,ircbuf,sizeof(hbuf));
    p1 = strchr(hbuf,' ');
    if (p1 == NULL) return 0x0;
    p1++;
    p1 = strchr(p1,' ');
    if (p1 == NULL) return 0x0;
    p1++;
    p1 = strchr(p1,' ');
    if (p1 == NULL) return 0x0;
    p1++;
    p2=p1;
    p1 = strchr(p1,' ');
    if (p1 == NULL) return 0x0;
    *p1=0;
    strmncpy(hchan,p2,sizeof(hchan));
    p2=p1;
    p2++;
    p1 = strchr(p2,' ');
    if (p1 == NULL) return 0x0;
    *p1=0;
    strmncpy(hident,p2,sizeof(hident));
    p1++;
    p2 = strchr(p1,' ');
    if (p2 == NULL) return 0x0;
    *p2=0;
    strmncpy(hhost,p1,sizeof(hhost));
    p2++;
    p1 = strchr(p2,' ');
    if (p1 == NULL) return 0x0;
    p1++;
    p2 = strchr(p1,' ');
    if (p2 == NULL) return 0x0;
    *p2 = 0;
    strmncpy(hnick,p1,sizeof(hnick));
    p2++;
    p1 = strchr(p2,' ');
    if (p1 == NULL) return 0x0;
    *p1 = 0;
    strmncpy(flags,p2,sizeof(flags));
    chan=getuserchannel(usern,hchan);
    /* update host, ident, but dont set modes here. buggy up to 2.3 */
    if(chan!=NULL)
	chanuser=addnicktochannel(chan,hnick,hident,hhost);
    return 0x0;
}

/* someone joined a channel */

int gotjoin(int usern)
{
    struct uchannelt *chan;
    pcontext;
    if (strmcmp(user(usern)->nick,ircnick)==1)
    {
	    chan=getuserchannel(usern,ircto);
	    if(chan==NULL)
	    {
		chan=addchanneltouser(usern,ircto,0);
	    }
	    if(chan!=NULL)
	    {
		addnicktochannel(chan,ircnick,ircident,irchost);
		chan->ison=1;
	    }
	    strmncpy(chan->prefixmodes,user(usern)->prefixmodes,sizeof(chan->prefixmodes));
	    strmncpy(chan->prefixchars,user(usern)->prefixchars,sizeof(chan->prefixchars));
	    strmncpy(chan->chanmodes,user(usern)->prefixmodes,sizeof(chan->chanmodes));
	    sendwho(usern);
	    return 0x0;
    }
    chan=getuserchannel(usern,ircto);
    if(chan!=NULL)
	addnicktochannel(chan,ircnick,ircident,irchost);
    checkbans(usern);
    checkautoop(usern);
    return 0x0;
}

/* someone parted from a channel */

int gotpart(int usern)
{
    struct uchannelt *chan;
    pcontext;
    if (strmcmp(user(usern)->nick,ircnick)==1)
    {
	    if(user(usern)->instate==STD_CONN)
	    {
	        removechannelfromuser(usern,ircto);
	    }
	    else
	    {
		chan=getuserchannel(usern,ircto);
		if(chan==NULL) return 0x0;
		removeallusersfromchannel(chan);
		chan->topic[0]=0;
		chan->ison=0;
	    }
	    return 0x0;
    }
    chan=getuserchannel(usern,ircto);
    if(chan!=NULL)
	removenickfromchannel(chan,ircnick);
    return 0x0;
}

/* someone was kicked */

int gotkick(int usern)
{
    struct uchannelt *chan;
    char *pt,*pt1,*pt2;
    char knick[64],kchan[200];
    char kreason[511];
    pcontext;
    pt=strstr(ircbuf,lngtxt(758));
    if(pt!=NULL)
    {
	pt+=6;
	pt2=strchr(pt,' ');
	if(pt2!=NULL)
	{
	    *pt2=0;
	    strmncpy(kchan,pt,sizeof(kchan));
	    *pt2=' ';
	    pt2++;
	    pt=strchr(pt2,' ');
	    if(pt!=NULL)
	    {
		*pt=0;
		strmncpy(knick,pt2,sizeof(knick));
		*pt=' ';
		chan=getuserchannel(usern,kchan);
		if(chan!=NULL)
		    removenickfromchannel(chan,knick);
		if(strmcmp(knick,user(usern)->nick)==1)
		{
		    if(chan!=NULL)
		    {
			removeallusersfromchannel(chan);
			chan->topic[0]=0;
			chan->ison=0;
			if(user(usern)->instate!=STD_CONN)
			{
			    strmncpy(kreason,irccontent,sizeof(kreason));
			    ap_snprintf(irccontent,sizeof(irccontent),lngtxt(759),kchan,kreason);
			    privatelog(usern);
			}
		    }
		}
	    }
	}
    }
    return 0x0;
}

/* someone quitted */

int gotquit(int usern)
{
    struct uchannelt *chan;
    pcontext;
    
    if(strmcmp(ircnick,user(usern)->nick)==1)
    {
	resetallchannels(usern);
    } else {
	removenickfromallchannels(usern,ircnick);
    }
    return 0x0;
}

/* someone set a mode  */

int gotmode(int usern)
{
    struct uchannelt *chan;
    char *param;
    char nbuf[2];
    pcontext;
    nbuf[0]=0;
    chan=getuserchannel(usern,ircto);
    if(chan!=NULL)
    {
	param=strchr(irccontent,' ');
	if(param==NULL)
	    param=nbuf;
	else
	{
	    *param=0;
	    param++;
	}
	addchannelmode(chan,irccontent,param);
    }
    return 0x0;
}

/* got topic change */

int gottopic(int usern)
{
    struct uchannelt *chan;
    chan=getuserchannel(usern,ircto);
    if(chan!=NULL)
	setchanneltopic(chan,irccontent);	
    return 0x0;
}

/* server sent an error to us */

int msgERROR(int usern)
{
    char fi[40];
    struct socketnodes *lkm;
    pcontext;
    p_log(LOG_ERROR,usern,lngtxt(760),user(usern)->login,user(usern)->network,user(usern)->server,irccontent);
    lkm=getpsocketbysock(user(usern)->outsock);
    if(lkm!=NULL)
	lkm->sock->destructor=NULL;
    killsocket(user(usern)->outsock);
    memset(user(usern)->server,0x0,sizeof(user(usern)->server));
    user(usern)->outstate=STD_NOCON;
    user(usern)->outsock = 0;
    resetallchannels(usern);
    ap_snprintf(fi,sizeof(fi),lngtxt(761),usern);
    oldfile(fi);
    return -1;
}

int msg001(int usern)
{
    pcontext;
    strcpy(user(usern)->prefixmodes,"ov");
    strcpy(user(usern)->prefixchars,"@+");
    strcpy(user(usern)->chantypes,"#!&+");
    strcpy(user(usern)->chanmodes,"beI,k,l,imnpst");
    user(usern)->srinit = 1;
    serverinit(usern,1);
    return -1;
}

int mapprefixed(int usern)
{
    char *pt;
    char *ept;
    pt=strstr(ircbuf," PREFIX=(");
    if(pt)
    {
	pt+=9;
	ept=strchr(pt,')');
	if(ept)
	{
	    *ept=0;
	    strmncpy(user(usern)->prefixmodes,pt,sizeof(user(usern)->prefixmodes));
	    *ept=')';
	    ept++;
	    pt=strchr(pt,' ');
	    if(pt)
	    {
		*pt=0;
		strmncpy(user(usern)->prefixchars,ept,sizeof(user(usern)->prefixchars));
		*pt=' ';
	    }
	}
    }
    pt=strstr(ircbuf," CHANTYPES=");
    if(pt)
    {
	pt+=11;
	ept=strchr(pt,' ');
	if(ept)
	{
	    *ept=0;
	    strmncpy(user(usern)->chantypes,pt,sizeof(user(usern)->chantypes));
	    *ept=' ';
	}
    }
    pt=strstr(ircbuf," CHANMODES=");
    if(pt)
    {
	pt+=11;
	ept=strchr(pt,' ');
	if(ept)
	{
	    *ept=0;
	    strmncpy(user(usern)->chanmodes,pt,sizeof(user(usern)->chanmodes));
	    *ept=' ';
	}
    }
    return 0x0;
}

int msg002to005(int usern)
{
    pcontext;
    if(ifcommand("005"))
	mapprefixed(usern);
    if (serverinit(usern,2)) return 0;
    return 1;
}

int msg251to255265to266(int usern)
{
    pcontext;
    if (serverinit(usern,3)) return 0;
    return 1;
}

int msg372to375(int usern)
{
    pcontext;
    if (serverinit(usern,4)) return 0;
    return 1;
}

int msg376(int usern)
{
    pcontext;
    serverinit(usern,4);
    user(usern)->srinit = 0;
    return -1;
}

int msg352(int usern)
{
    pcontext;
    if (user(usern)->triggered == 0) 
    {
	performwho(usern);
	return 0;
    }
    return 1;
}

int msg315(int usern)
{
    char *pt,*ept;
    struct uchannelt *chan;
    pcontext;
    user(usern)->gotop=0;
    if (user(usern)->triggered == 0) {
	pt=strstr(ircbuf,lngtxt(762));
	if(pt!=NULL)
	{
	    pt+=5;
	    pt=strchr(pt,' ');
	    if(pt!=NULL)
	    {
		pt++;
		ept=strchr(pt,' ');
		if(ept!=NULL)
		{
		    *ept=0;
		    chan=getuserchannel(usern,pt);
		    if(chan!=NULL)
			askforop(usern,pt);
		    
		    *ept=' ';
		}
	    }
	}
	return 0;
    }
    user(usern)->triggered--;
    return 1;
}

int msg432to437(int usern)
{
    pcontext;
    if (nickused(usern)==0) return 0;
    return 1;
}

/* channelmode */

int msg324(int usern)
{
    struct uchannelt *channel;
    char *pt,*pt2,*pt3;
    char chan[200],mode[40],param[200];
    char ebuf[2048];
    strmncpy(ebuf,ircbuf,sizeof(ebuf));
    pt=strchr(ebuf,'\r');
    if(pt==NULL) pt=strchr(ebuf,'\n');
    if(pt!=NULL) *pt=0;
    param[0]=0;
    pt=strstr(ebuf,lngtxt(763));
    if(pt!=NULL)
    {
	pt+=5;
	pt=strchr(pt,' ');
	if(pt!=NULL)
	{
	    pt++;
	    pt2=strchr(pt,' ');
	    if(pt2!=NULL)
	    {
		*pt2=0;
		strmncpy(chan,pt,sizeof(chan));
		*pt2=' ';
		pt2++;
		if(*pt2=='+') pt2++;
		pt3=strchr(pt2,' ');
		if(pt3!=NULL)
		{
		    *pt3=0;
		    strmncpy(mode,pt2,sizeof(mode));
		    *pt3=' ';
		    pt3++;
		    strmncpy(param,pt3,sizeof(param));
		} else {
		    strmncpy(mode,pt2,sizeof(mode));
		}
		channel=getuserchannel(usern,chan);
		if(channel!=NULL)
		{
		    setchannelmode(channel,mode,param);
		}
	    }
	}
    }
}

/* a names line */

int msg332(int usern)
{
    struct uchannelt *channel;
    char *pt,*pt2,*pt3;
    char chan[200],topic[600];
    topic[0]=0;
    pt=strstr(ircbuf,lngtxt(764));
    if(pt!=NULL)
    {
	pt+=5;
	pt=strchr(pt,' ');
	if(pt!=NULL)
	{
	    pt++;
	    pt2=strchr(pt,' ');
	    if(pt2!=NULL)
	    {
		*pt2=0;
		strmncpy(chan,pt,sizeof(chan));
		*pt2=' ';
		pt2++;
		if(*pt2=='+') pt2++;
		pt3=strchr(pt2,':');
		if(pt3!=NULL)
		{
		    pt3++;
		    strmncpy(topic,pt3,sizeof(topic));
		    pt=strchr(topic,'\r');
		    if(pt==NULL) pt=strchr(topic,'\n');
		    if(pt!=NULL) *pt=0;
		}
		channel=getuserchannel(usern,chan);
		if(channel!=NULL)
		{
		    setchanneltopic(channel,topic);
		}
	    }
	}
    }
}

/* a single names line, needs to be saved */

int msg353(int usern)
{
    struct uchannelt *chan;
    struct uchannelusert *chanuser;
    char *pt,*pt2;
    char channel[300];
    static char m3[2048];
    char cmpbuf[100];
    char mm[2];
    mm[1]=0;
    mm[0]=0;
    strmncpy(m3,ircbuf,sizeof(m3));
    pt=strchr(m3,'\r');
    if(pt==NULL) pt=strchr(m3,'\n');
    if(pt!=NULL) *pt=0;
    pt=strstr(m3,lngtxt(765));
    if(pt!=NULL)
    {
	pt+=5;
	pt2=strstr(pt," = ");
	if(pt2==NULL) pt2=strstr(pt," @ ");
	if(pt2==NULL) pt2=strstr(pt," * ");
	if(pt2!=NULL)
	{
	    pt2+=3;
	    pt=pt2;
	    pt2=strchr(pt2,' ');
	    if(pt2!=NULL)
	    {
		*pt2=0;
		strmncpy(channel,pt,sizeof(channel));
		if(user(usern)->innames==0)
		{
		    strmncpy(user(usern)->inchan,channel,sizeof(user(usern)->inchan));
		    user(usern)->innames=1;
		}
		*pt2=' ';
		pt=strchr(pt2,':');
		if(pt!=NULL)
		{
		    chan=getuserchannel(usern,channel);
		    if(chan==NULL) return 0x0;
		    pt++;
		    while(pt!=NULL)
		    {
			mm[0]=0;
			pt2=strchr(pt,' ');
			if(pt2!=NULL)
			{
			    *pt2=0;
			}
			
			ap_snprintf(cmpbuf,sizeof(cmpbuf),"%s ",user(usern)->prefixchars);
			while(strchr(cmpbuf,*pt)!=NULL && *pt!=0) 
			{ 
			      if(*pt!=' ' && mm[0]==0)
			          mm[0]=*pt;
			      pt++;
			}
			if(strlen(pt)>0 && *pt!=' ')
			{
			    chanuser=addnicktochannel(chan,pt,"","");
			    if(chanuser!=NULL)
				strmncpy(chanuser->mode,mm,sizeof(chanuser->mode));
			}
			if(pt2!=NULL)
			{
			    *pt2=' ';
			    pt2++;
			}
			pt=pt2;
		    }
		}
	    }
	}
    } 
    return 0x0;
}

int msg366(int usern)
{
    struct uchannelt *chan;
    if(user(usern)->innames==2)
    {
	chan=getuserchannel(usern,rtrim(user(usern)->inchan));
	if(chan!=NULL && user(usern)->instate==STD_CONN)
	    sendnames(usern,chan);
    }
    if(user(usern)->innames!=0)
    {
	user(usern)->innames=0;
	askforop(usern,user(usern)->inchan);
	user(usern)->inchan[0]=0;
    }
    return 0x0;
}

/* privmsg */

int msgprivmsg(int usern)
{
    char buf[400];
    char *po;
    pcontext;
    if(checkignores(usern)==1) return 0x0;
    if(parsectcps(usern)==1) return 0x0;
#ifdef CRYPT
    checkcrypt(usern);
#endif
#ifdef TRANSLATE
    checktranslate(usern);
#endif
    strcpy(buf,"op ");
    po = strstr(irccontent,buf);
    if (po == irccontent) {
	if (checkop(usern) == 0x0) return 0;
    }
    if (strmncasecmp(ircto,user(usern)->nick) && user(usern)->instate!=STD_CONN) {
	privatelog(usern);
    }
    return 1;
}

void gotalrm(int sig)
{
    longjmp(serveralarmret,0x0);
}

int userproxyconnected(int nuser)
{
#ifdef BLOCKDNS
    struct hostent *he;
    int proto;
    char buf[40];
    char gip[5];
    pcontext;
    signal(SIGALRM,gotalrm);
    if(setjmp(serveralarmret)==0x0) 
    {
        alarm(10);
        he=gethostbyname(user(nuser)->server);
        signal(SIGALRM,SIG_IGN);
        alarm(0);
    } else
        he=NULL;
    if(!he)
    {
        killsocket(user(nuser)->outsock);
        return 0x0;
    }
    signal(SIGALRM,SIG_IGN);
    memcpy(&gip[0],&he->h_addr[0],4);
#else
    char data[8];
    struct socketnodes *snode;
    pcontext;
    snode=getpsocketbysock(user(nuser)->outsock);
    if(snode!=NULL)
    {
	ap_snprintf(data,sizeof(data),"%d",nuser);
	if(snode->sock)
	    snode->sock->flag=SOC_RESOLVE;
	if(dns_forward(user(nuser)->server,userproxyconnectresolved,userproxyconnectnotresolved,data)==0x2)
	{
	    killsocket(user(nuser)->outsock);
	}	
    }
    return 0x0;
}


int userproxyconnectnotresolved(struct resolve *rp)
{
    int nuser;
    if(rp)
    {
	if(rp->data)
	{
	    nuser=atoi(rp->data);
	    killsocket(user(nuser)->outsock);
	}
    }
    return 0x0;
}

int userproxyconnectresolved(struct resolve *rp)
{
    int nuser;
    char gip[5];
    char buf[40];
    struct socketnodes *snode;
    if(rp)
    {
	if(rp->data)
	{
	    nuser=atoi(rp->data);
	    if(rp->state==STATE_FINISHED && rp->protocol==AF_INET)
		memcpy(&gip[0],&rp->ip,4);
	    else
		memset(&gip[0],0x0,4);
	} else {
	    return 0x0;
	}
	snode=getpsocketbysock(user(nuser)->outsock);
	if(snode)
	{
	    snode->sock->flag=SOC_CONN;
	}
    } else {
	return 0x0;
    }
#endif
    if(user(nuser)->pport==1080) /* socks 4 */
    {
	ap_snprintf(buf,sizeof(buf),lngtxt(767),
	    (user(nuser)->port>>8) & 0xff,user(nuser)->port & 0xff,
	    (char) gip[0],
	    (char) gip[1],
	    (char) gip[2],
	    (char) gip[3]);
	send(user(nuser)->outsock,buf,9,0);
	currentsocket->sock->nowfds=1;
        writesock_DELAY(user(nuser)->outsock,"\n",3);
    } else {
        if(user(nuser)->pport==23) /* wingate */
        {
    	    ssnprintf(user(nuser)->outsock,lngtxt(768),user(nuser)->server,user(nuser)->port);
	    currentsocket->sock->nowfds=1;
	    writesock_DELAY(user(nuser)->outsock,"\n",3);
	} else { /* any other port, we assume a web proxy */
    	    ap_snprintf(ircbuf,sizeof(ircbuf),lngtxt(769),user(nuser)->server,user(nuser)->port);
	    send(user(nuser)->outsock,ircbuf,strlen(ircbuf)+1,0);
	    currentsocket->sock->nowfds=1;
	    writesock_DELAY(user(nuser)->outsock,"\n",3);
	}
    } 
    userconnected(nuser);    
    return 0x0;
}

int userconnected(int nuser)
{
    int nlink;
    pcontext;
    nlink=user(nuser)->vlink;
    if(user(nuser)->pport==0)
    {
	if (nlink!=0)
	{
    	    if (datalink(nlink)->type !=1 && datalink(nlink)->type!=2)
    		nlink=0;
    	    if (datalink(nlink)->outstate!=STD_CONN && datalink(nlink)->instate!=STD_CONN)
    		nlink=0;
	}
    }
    if (nlink!=0)
    {
        ssnprintf(user(nuser)->outsock,lngtxt(770),datalink(nlink)->name,socketnode->sock->sport);
        ssnprintf(user(nuser)->outsock,lngtxt(771),user(nuser)->vhost);
        ssnprintf(user(nuser)->outsock,lngtxt(772),user(nuser)->server,user(nuser)->port);
	writesock_DELAY(user(nuser)->outsock,"\n",3);
    }
    if(*user(nuser)->spass!=0)
    {
	ssnprintf(user(nuser)->outsock,lngtxt(773),user(nuser)->spass);
    }
    ssnprintf(user(nuser)->outsock,lngtxt(774),user(nuser)->login,user(nuser)->login,user(nuser)->user);
    if(user(nuser)->instate!=STD_CONN && *user(nuser)->awaynick!=0)
        ssnprintf(user(nuser)->outsock,lngtxt(775),user(nuser)->awaynick);
    else
        ssnprintf(user(nuser)->outsock,lngtxt(776),user(nuser)->wantnick);
    if (nlink!=0)
    {
        ssnprintf(user(nuser)->outsock,lngtxt(777),datalink(nlink)->pass);
    }
    p_log(LOG_INFO,nuser,lngtxt(778),user(nuser)->login,user(nuser)->network,user(nuser)->server,user(nuser)->port,user(nuser)->vhost);
    user(nuser)->autopongreply=500;
    user(nuser)->pinged=0;
    user(nuser)->pingtime=0;
    user(nuser)->nickgain=time(NULL);
    return 0x0;
}

int userremap(int nuser,int newsocket)
{
    user(nuser)->outsock=newsocket;
    return 0x0;
}

int usererror(int nuser,int errn)
{
    struct socketnodes *th;
    char buf[400];
    pcontext;
    if(*user(nuser)->server)
	p_log(LOG_ERROR,nuser,lngtxt(779),user(nuser)->login,user(nuser)->server,user(nuser)->port);
    th=getpsocketbysock(user(nuser)->outsock);
    if(th!=NULL)
	th->sock->destructor=NULL;
    killsocket(user(nuser)->outsock);
    user(nuser)->outstate=STD_NOCON;
    user(nuser)->pinged=0;
    user(nuser)->pingtime=0;
    user(nuser)->nickgain=time(NULL);
    ap_snprintf(buf,sizeof(buf),lngtxt(780),nuser);
    resetallchannels(nuser);
    oldfile(buf);
    memset(user(nuser)->server,0x0,sizeof(user(nuser)->server));
    return -1;
}

int userclosed(int nuser)
{
    struct socketnodes *th;
    char buf[400];
    pcontext;
    p_log(LOG_WARNING,nuser,lngtxt(781),user(nuser)->login);
    th=getpsocketbysock(user(nuser)->outsock);
    if(th!=NULL)
    {
	th->sock->destructor=NULL;
    }
    killsocket(user(nuser)->outsock);
    user(nuser)->outstate=STD_NOCON;
    user(nuser)->pinged=0;
    user(nuser)->pingtime=0;
    user(nuser)->nickgain=time(NULL);
    ap_snprintf(buf,sizeof(buf),lngtxt(782),nuser);
    resetallchannels(nuser);
    oldfile(buf);
    memset(user(nuser)->server,0x0,sizeof(user(nuser)->server));
    return -1;
}
/* check the clients for server connections */

int checkclients()
{
    struct usernodes *th;
    static int lastusern=0;
    int usern;
    th=usernode;
    while (th!=NULL) {
	usern=th->uid;
        if (usern!=lastusern && connectuser(usern)==1) 
	{
	    lastusern=usern;
	    return 0x0;
	}
	th=th->next;
    }
    lastusern=-1;
    return 0x0;
}


/* connect User #nuser */

int connectuser(int nuser)
{
   char buf[400];
   char *ho;
   int issl=SSL_OFF;
   char vsl[10];
   char *ep;
   static unsigned long lastconnect=0;
   unsigned long thistime=time(NULL);
   struct socketnodes *lkm;
#ifdef OIDENTD
   mode_t oldum;
   FILE *oid;
#endif
   int nlink=0; 
   if((thistime-lastconnect)<5) return 0x0;
   vsl[0]=0;   
   if (user(nuser)->outstate == STD_NOCON)
   {
	if (user(nuser)->quitted ==1) return 0x0;
	if (user(nuser)->delayed) {
	    user(nuser)->delayed-=delayinc;
	    return 0x0;
	}
	if (getnextserver(nuser) != 0) {
	    user(nuser)->outstate = STD_NOCON;
	    p_log(LOG_INFO,nuser,lngtxt(783),user(nuser)->login,user(nuser)->network);
	    memset(user(nuser)->server,0x0,sizeof(user(nuser)->server));
	    user(nuser)->delayed = 5;
	    return 0x0;
	}
	if (user(nuser)->port == 0) {
	    user(nuser)->outstate = STD_NOCON;
	    p_log(LOG_INFO,nuser,lngtxt(784),user(nuser)->login,user(nuser)->network);
	    memset(user(nuser)->server,0x0,sizeof(user(nuser)->server));
	    user(nuser)->delayed = 5;
	    return 0x0;
	}
	user(nuser)->delayed = 0;
	nlink=user(nuser)->vlink;
	if (nlink!=0)
	{
	    if (datalink(nlink)->type !=1 && datalink(nlink)->type!=2)
	    {
		nlink=0;
	    }
	    if (datalink(nlink)->outstate!=STD_CONN && datalink(nlink)->instate!=STD_CONN)
		nlink=0;
	}
	user(nuser)->triggered=0;
	if (nlink!=0)
	{
#ifdef HAVE_SSL
	    if(strstr(datalink(nlink)->host,"S=")==datalink(nlink)->host)
	    {
		ho=datalink(nlink)->host+2;
		issl=SSL_ON;
		strcpy(vsl,"SSL-");
	    } else
#endif	
		ho=datalink(nlink)->host;
	}
	else
	{
#ifdef HAVE_SSL
	    if(strstr(user(nuser)->server,"S=")==user(nuser)->server)
	    {
		ho=user(nuser)->server+2;
		issl=SSL_ON;
		strcpy(vsl,"SSL-");
	    } else
#endif	
		ho=user(nuser)->server;
	}
#ifdef OIDENTD
	ep=getenv("HOME");
	if(ep!=NULL)
	{
	    ap_snprintf(buf,sizeof(buf),lngtxt(785),ep);
	    oldum=umask(0000);
	    umask(022);
	    oid=fopen(buf,"w");
	    if(oid!=NULL)
	    {
		fprintf(oid,"%s\n",user(nuser)->login);
		fclose(oid);
	    }
	    umask(oldum);
	    ap_snprintf(buf,sizeof(buf),"%s/.oidentd.conf",ep);
	    oldum=umask(0000);
	    umask(022);
	    oid=fopen(buf,"w");
	    if(oid!=NULL)
	    {
		fprintf(oid,"global { reply \"%s\" }\n",user(nuser)->login);
		fclose(oid);
	    }
	    umask(oldum);
	}
#endif
	if(user(nuser)->pport!=0)
	    user(nuser)->outsock=createsocket(0,ST_CONNECT,nuser,SGR_NONE,NULL,userproxyconnected,usererror,useroutbound,userclosed,userremap,AF_INET,issl);
	else
	    user(nuser)->outsock=createsocket(0,ST_CONNECT,nuser,SGR_NONE,NULL,userconnected,usererror,useroutbound,userclosed,userremap,AF_INET,issl);
	lkm=getpsocketbysock(user(nuser)->outsock);
	if (user(nuser)->pport!=0)
	{
	   p_log(LOG_INFO,nuser,lngtxt(786),user(nuser)->login,user(nuser)->network,user(nuser)->proxy,user(nuser)->pport,user(nuser)->server,user(nuser)->port);
	   user(nuser)->outsock=connectto(user(nuser)->outsock,user(nuser)->proxy,user(nuser)->pport,NULL);
	} else
	if (nlink!=0)
	{
  	   p_log(LOG_INFO,nuser,lngtxt(787),user(nuser)->login,user(nuser)->network,user(nuser)->server,user(nuser)->port,datalink(nlink)->name,ho,datalink(nlink)->port,user(nuser)->vhost);
	   user(nuser)->outsock=connectto(user(nuser)->outsock,ho,datalink(nlink)->port,NULL);
	} else {
  	   p_log(LOG_INFO,nuser,lngtxt(788),user(nuser)->login,user(nuser)->network,ho,user(nuser)->port,user(nuser)->vhost);
	   user(nuser)->outsock=connectto(user(nuser)->outsock,ho,user(nuser)->port,user(nuser)->vhost);
	}
	lkm=getpsocketbysock(user(nuser)->outsock);
	if (lkm!=NULL)
	    lkm->sock->serversocket=1;
	if (user(nuser)->outsock == 0) 
	{
	    if(*user(nuser)->server)
		p_log(LOG_ERROR,nuser,lngtxt(789),user(nuser)->login,user(nuser)->server,user(nuser)->port);
	    return 0x0;
	}
        user(nuser)->outstate=STD_CONN;
        user(nuser)->welcome=0;
	lastconnect=thistime;
	return 0x1;
    }
    return 0x0;
}

#ifndef STONETIME
#define STONETIME 240
#endif

#ifndef STONECHECKDELAY
#define STONECHECKDELAY 120
#endif

/* stone check added, faster nick gain. requested by fighter */

int checkstonednick()
{
    struct usernodes *th;
    int usern;
    th=usernode;
    while (th!=NULL) 
    {
	usern=th->uid;
        if(user(usern)->outstate==STD_CONN && user(usern)->welcome==1)
	{
	    if(user(usern)->instate!=STD_CONN)
	    {
		/* did not use awaynick as mask, changed for -2 */
		if(!strmcmp(user(usern)->nick,user(usern)->wantnick) && !strmcmp(user(usern)->nick,user(usern)->awaynick))
		{
		    if(user(usern)->nickgain==0 || (user(usern)->nickgain+10 < time(NULL)))
		    {
			user(usern)->nickgain=time(NULL);
			ssnprintf(user(usern)->outsock,lngtxt(277),user(usern)->wantnick);
		    }
		}
	    }
	    if(user(usern)->pinged==1)
	    {
		if(user(usern)->pingtime+STONETIME<=time(NULL))
		{
		    user(usern)->pinged=0;
		    user(usern)->pingtime=0;
		    p_log(LOG_INFO,usern,lngtxt(1357),user(usern)->login,user(usern)->nick);
		    ssnprintf(user(usern)->outsock,lngtxt(61));
		    killsocket(user(usern)->outsock);
		}
	    } else {
		if(user(usern)->pingtime==0 || (user(usern)->pingtime>0 && user(usern)->pingtime+STONECHECKDELAY<=time(NULL)))
		{
		    user(usern)->pinged=1;
		    user(usern)->pingtime=time(NULL);
		    ssnprintf(user(usern)->outsock,lngtxt(800));
		}
	    }
	}
	th=th->next;
    }
    return 0x0;
}


