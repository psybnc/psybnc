/* $Id: p_intnet.c,v 1.6 2005/06/04 18:00:14 hisi Exp $ */
/************************************************************************
 *   psybnc2.3.2, src/p_intnet.c
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
static char rcsid[] = "@(#)$Id: p_intnet.c,v 1.6 2005/06/04 18:00:14 hisi Exp $";
#endif

#define P_INTNET

#include <p_global.h>

/*
 * This snippet of code was created to emulate an internal ircd
 * on psybnc. Channels and modes are shared thruout the linked
 * bouncers.
 */

#ifdef INTNET

struct clientt {
    int intuser;
    char nick[64];
    char ident[30];
    char host[200];
    char server[200];
    struct clientt *next;
};

struct clientt *clients=NULL;

/* helper to add a client */

struct clientt *createclient(int usern, char *nick, char *ident, char *host, char *server)
{
    struct clientt *cl,*cl1=NULL;
    cl=clients;
    while(cl!=NULL)
    {
	if(cl->intuser==usern && usern!=0) return cl;
	cl1=cl;
	cl=cl->next;
    }
    if(cl1==NULL)
    {
	clients=(struct clientt *)pmalloc(sizeof(struct clientt));
	cl1=clients;
    } else {
	cl1->next=(struct clientt *)pmalloc(sizeof(struct clientt));
	cl1=cl1->next;
    }
    cl1->intuser=usern;
    strmncpy(cl1->nick,nick,sizeof(cl1->nick));
    strmncpy(cl1->ident,ident,sizeof(cl1->ident));
    strmncpy(cl1->host,host,sizeof(cl1->host));
    strmncpy(cl1->server,server,sizeof(cl1->server));
    return cl1;    
}

/* helper to remove a client */

int removeclient(char *nick)
{
    struct clientt *client,*eclient=NULL;
    client=clients;
    while(client!=NULL)
    {
	if(strmcmp(client->nick,nick)==1)
	{
	    if(eclient==NULL)
	    {
		clients=client->next;
	    } else {
		eclient->next=client->next;
	    }
	    free(client);
	    return 0x1;
	}
	eclient=client;
	client=client->next;
    }
    return 0x0;
}

/* helper for getting a client from a nickname */

struct clientt *getclientbynick(char *nick)
{
    struct clientt *cl;
    char uc1[64],uc2[64];
    cl=clients;
    strmncpy(uc1,nick,sizeof(uc1));
    ucase(uc1);
    while(cl!=NULL)
    {
	strmncpy(uc2,cl->nick,sizeof(uc2));
	ucase(uc2);
	if(strmcmp(uc1,uc2)==1)
	    return cl;
	cl=cl->next;
    }
    return NULL;
}


/* helper for getting a client from a usernumber */

struct clientt *getclientbynumber(int usern)
{
    struct clientt *cl;
    cl=clients;
    while(cl!=NULL)
    {
	if(usern==cl->intuser)
	    return cl;
	cl=cl->next;
    }
    return NULL;
}

/* helper to add a ban */

int addban(struct uchannelt *chan, char *banmask)
{
    struct stringarray *banlist,*oban=NULL;
    char *q,*p;
    int cnt=0;
    banlist=chan->bans;
    while(banlist!=NULL)
    {
	if(strmcmp(banmask,banlist->entry)==1) return 0x0;
	banlist=banlist->next;
    }
    banlist=chan->bans;
    q=strchr(banmask,'!');
    p=strchr(banmask,'@');
    if(p==NULL || q==NULL) return 0x0;
    if(q>p) return 0x0;
    while(banlist!=NULL && cnt<20)
    {
	oban=banlist;
	banlist=banlist->next;
	cnt++;
    }
    if(cnt>=20) return 0x0;
    if(oban==NULL)
    {
	oban=(struct stringarray *)pmalloc(sizeof(struct stringarray));
	chan->bans=oban;
    } else {
	oban->next=(struct stringarray *)pmalloc(sizeof(struct stringarray));
	oban=oban->next;
    }
    oban->entry=(char *)pmalloc(strlen(banmask)+2);
    strmncpy(oban->entry,banmask,strlen(banmask)+1);
    return 0x1;
}

/* helper to remove a ban */

int removeban(struct uchannelt *chan, char *banmask)
{
    struct stringarray *banlist,*oban=NULL;
    char *q,*p;
    int cnt=0;
    banlist=chan->bans;
    q=strchr(banmask,'!');
    p=strchr(banmask,'@');
    if(p==NULL || q==NULL) return 0x0;
    if(q>p) return 0x0;
    while(banlist!=NULL && cnt<20)
    {
	if(strmcmp(banlist->entry,banmask)==1)
	{
	    if(oban==NULL)
	    {
		chan->bans=banlist->next;
	    } else {
		oban->next=banlist->next;
	    }    
	    free(banlist->entry);
	    free(banlist);
	    return 0x1;
	}
	oban=banlist;
	banlist=banlist->next;
	cnt++;
    }
    return 0x0;
}

/* helper to add an invite */

int addinvite(struct uchannelt *chan, char *invite)
{
    struct stringarray *invlist,*oinv=NULL;
    char *q,*p;
    int cnt=0;
    invlist=chan->invites;
    p=strchr(invite,' ');
    if(p!=NULL) *p=0x0;
    while(invlist!=NULL && cnt<20)
    {
	oinv=invlist;
	invlist=invlist->next;
	cnt++;
    }
    if(cnt>=20) return 0x0;
    if(oinv==NULL)
    {
	oinv=(struct stringarray *)pmalloc(sizeof(struct stringarray));
	chan->invites=oinv;
    } else {
	oinv->next=(struct stringarray *)pmalloc(sizeof(struct stringarray));
	oinv=oinv->next;
    }
    oinv->entry=(char *)pmalloc(strlen(invite)+2);
    strmncpy(oinv->entry,invite,strlen(invite)+1);
    return 0x1;
}

/* helper to remove an invite */

int removeinvite(struct uchannelt *chan, char *invite)
{
    struct stringarray *invlist,*oinv=NULL;
    char *q,*p;
    int cnt=0;
    invlist=chan->invites;
    p=strchr(invite,' ');
    if(p!=NULL) *p=0;
    while(invlist!=NULL && cnt<20)
    {
	if(strmcmp(invlist->entry,invite)==1)
	{
	    if(oinv==NULL)
	    {
		chan->invites=invlist->next;
	    } else {
		oinv->next=invlist->next;
	    }    
	    free(invlist->entry);
	    free(invlist);
	    return 0x1;
	}
	oinv=invlist;
	invlist=invlist->next;
	cnt++;
    }
    return 0x1;
}

/* helper to add an intop */

int addintop(struct uchannelt *chan, char *intop)
{
    struct stringarray *intoplist,*ointo=NULL;
    char *q,*p;
    int cnt=0;
    intoplist=chan->intops;
    p=strchr(intop,' ');
    if(p!=NULL) *p=0x0;
    while(intoplist!=NULL && cnt <100)
    {
	ointo=intoplist;
	intoplist=intoplist->next;
	cnt++;
    }
    if(cnt>=100) return 0x0;
    if(ointo==NULL)
    {
	ointo=(struct stringarray *)pmalloc(sizeof(struct stringarray));
	chan->intops=ointo;
    } else {
	ointo->next=(struct stringarray *)pmalloc(sizeof(struct stringarray));
	ointo=ointo->next;
    }
    ointo->entry=(char *)pmalloc(strlen(intop)+2);
    strmncpy(ointo->entry,intop,strlen(intop)+1);
    return 0x1;
}

/* helper to remove an intop */

int removeintop(struct uchannelt *chan, char *intop)
{
    struct stringarray *intoplist,*ointop=NULL;
    char *q,*p;
    int cnt=0;
    intoplist=chan->intops;
    p=strchr(intop,' ');
    if(p!=NULL) *p=0;
    while(intoplist!=NULL && cnt<100)
    {
	if(strmcmp(intoplist->entry,intop)==1)
	{
	    if(ointop==NULL)
	    {
		chan->intops=intoplist->next;
	    } else {
		ointop->next=intoplist->next;
	    }    
	    free(intoplist->entry);
	    free(intoplist);
	    return 0x1;
	}
	ointop=intoplist;
	intoplist=intoplist->next;
	cnt++;
    }
    return 0x1;
}

/* helper to check if the user may join */

int mayjoin(struct uchannelt *chan, struct clientt *client, char *key)
{
    struct stringarray *lst;
    char ehost[300];
    ap_snprintf(ehost,sizeof(ehost),lngtxt(385),client->nick,client->ident,client->host);
    if(strchr(chan->modes,'i')!=NULL)
    {
	lst=chan->invites;
	while(lst!=NULL)
	{
	    if(strmcmp(client->nick,lst->entry)==1) return 0x0;
	    lst=lst->next;
	}
	return -3;
    }
    lst=chan->bans;
    while(lst!=NULL)
    {
	if(wild_match(lst->entry,ehost))
	    return -1;
	lst=lst->next;
    }
    if(strchr(chan->modes,'k')!=NULL)
    {
	if(strmcmp(chan->key,key)==0) return -2;
    }
    return 0x0;
}

/* write from internal network to a client 
 * syntax: sendtoclient(char *nick,int notlink, char *format, ...)
 */

int 
sendtoclient(char *nick, int notlink, char *format,...)
{
    va_list va;
    int sock;
    char buf[2048];
    struct clientt *client;
    va_start(va,format);
    client=getclientbynick(nick);
    ap_vsnprintf(buf,sizeof(buf),format,va);
    strmncpy(ircbuf,buf,sizeof(ircbuf));
    parse();
    if(client==NULL)
    {
	ap_snprintf(ircbuf,sizeof(ircbuf),lngtxt(386),me,ircbuf);
    	broadcast(0);
    } else {
	if(client->intuser>0)
	{
	    if(user(client->intuser)->instate==STD_CONN)
	    {
		strmncpy(ircbuf,buf,sizeof(ircbuf));
		addtoken(client->intuser+10000);
		ssnprintf(user(client->intuser)->insock,"%s",ircbuf);
	    }	
	} else {
	    ap_snprintf(ircbuf,sizeof(ircbuf),lngtxt(387),me,buf);
    	    broadcast(notlink);
	}	
    }
    va_end(va);
    return strlen(buf);
}

/* write from internal network to a link
 * syntax: sendtolink(int link, char *format, ...)
 */

int 
sendtolink(int link, char *format,...)
{
    va_list va;
    int sock;
    char buf[2048];
    va_start(va,format);
    ap_vsnprintf(buf,sizeof(buf),format,va);
    ap_snprintf(ircbuf,sizeof(ircbuf),lngtxt(388),me,buf);
    broadcasttolink(link);
    va_end(va);
    return strlen(buf);
}

/* write from internal network to a channel 
 * syntax: sendtochannel(char *channel, int notlink, char *format, ...)
 */

int sendtochannel(char *channel, int notlink, char *format, ...)
{
    va_list va;
    int sock;
    char buf[2048];
    int isextern=0;
    struct uchannelt *chan;    
    struct uchannelusert *chanu;
    struct clientt *client;
    va_start(va,format);
    ap_vsnprintf(buf,sizeof(buf),format,va);
    chan=getuserchannel(-1,channel);
    strmncpy(ircbuf,buf,sizeof(ircbuf));
    parse();
    if(chan!=NULL)
    {
	chanu=chan->users;
	while(chanu!=NULL)
	{
	    client=getclientbynick(chanu->nick);
	    if(client!=NULL)
	    {
		if(client->intuser>0)
		{
		    if(user(client->intuser)->instate==STD_CONN)
		    {
			strmncpy(ircbuf,buf,sizeof(ircbuf));
			addtoken(client->intuser+10000);
			ssnprintf(user(client->intuser)->insock,"%s",ircbuf);
		    }
		}
	    }
	    chanu=chanu->next;
	}
    }
    ap_snprintf(ircbuf,sizeof(ircbuf),lngtxt(389),me,buf);
    broadcast(notlink);
    va_end(va);    
    return 0x0;    
}

/* write from internal network to a channel expect to one user
 * syntax: sendtochannelbutone(char *channel,char *nick, int notlink, char *format, ...)
 */

int sendtochannelbutone(char *channel, char *nick, int notlink, char *format,...)
{
    va_list va;
    int sock;
    char buf[2048];
    int isextern=0;
    struct uchannelt *chan;    
    struct uchannelusert *chanu;
    struct clientt *client;
    va_start(va,format);
    client=getclientbynick(nick);
    ap_vsnprintf(buf,sizeof(buf),format,va);
    chan=getuserchannel(-1,channel);
    strmncpy(ircbuf,buf,sizeof(ircbuf));
    parse();
    if(chan!=NULL)
    {
	chanu=chan->users;
	while(chanu!=NULL)
	{
	    client=getclientbynick(chanu->nick);
	    if(client!=NULL)
	    {
		if(strmcmp(nick,client->nick)==0)
		{
		    if(client->intuser==0)
			isextern=1;
		    else if(user(client->intuser)->instate==STD_CONN)
		    {
			strmncpy(ircbuf,buf,sizeof(ircbuf));
			addtoken(client->intuser+10000);
			ssnprintf(user(client->intuser)->insock,"%s",ircbuf);
		    }
		}
	    } else { 
		isextern=1;
	    }
	    chanu=chanu->next;
	}
    }
    ap_snprintf(ircbuf,sizeof(ircbuf),lngtxt(390),me,buf);
    broadcast(notlink);
    va_end(va);    
    return 0x0;    
}

/* write from internal network to all channels of one nick
 * syntax: sendtochannelsofnick(char *nick, int notlink, char *format, ...)
 */

int sendtochannelsofnick(char *nick, int notlink, char *format, ...)
{
    va_list va;
    int sock;
    char buf[2048];
    int isextern=0;
    int didsend=0;
    struct uchannelt *chan;    
    struct uchannelusert *chanu,*chanu2;
    struct clientt *client,*cyc;
    va_start(va,format);
    client=getclientbynick(nick);
    ap_vsnprintf(buf,sizeof(buf),format,va);
    cyc=clients;
    strmncpy(ircbuf,buf,sizeof(ircbuf));
    parse();
    while(cyc!=NULL)
    {
	chan=intchan;
	didsend=0;
	while(chan!=NULL && didsend==0)
	{
	    chanu=getchannelnick(chan,cyc->nick);
	    if(chanu!=NULL)
	    {
		chanu2=getchannelnick(chan,nick);
		if(chanu2!=NULL)
		{
		    if(cyc->intuser>0)
		    {
			if(user(cyc->intuser)->instate==STD_CONN)
			{
			    strmncpy(ircbuf,buf,sizeof(ircbuf));
			    addtoken(cyc->intuser+10000);
			    ssnprintf(user(cyc->intuser)->insock,"%s",ircbuf);
			}
		    }
		    didsend=1;
		}
	    }
	    chan=chan->next;
	}
	cyc=cyc->next;
    }
    ap_snprintf(ircbuf,sizeof(ircbuf),lngtxt(391),me,buf);
    broadcast(notlink);
    va_end(va);    
    return 0x0;    
}

/* helper to add a client and to tell this to the network */

struct clientt *enteruser(int usern, char *nick, char *ident, char *host, char *server)
{
    struct clientt *client;
    if(usern>0)
    {
	client=getclientbynick(nick);
	if(client!=NULL)
	{
	    if(client->intuser==usern)
	    {
		sendtochannelsofnick(nick,0,lngtxt(1321),client->nick,client->ident,client->host);
		removenickfromallchannels(-1,nick);
		removeclient(nick);
	    } else {
		ssnprintf(user(usern)->insock,lngtxt(383),user(usern)->nick,client->nick,client->ident,client->host);
		return NULL;
	    }
	}
    }
    client=createclient(usern,nick,ident,host,server);
    ap_snprintf(ircbuf,sizeof(ircbuf),lngtxt(384),me,nick,ident,host,server);
    broadcast(0);
    return client;
}

char intname[100];

/* handle single events */

int cmdintprivmsg(int usern, int link)
{
    char currentto[200];
    struct uchannelt *chan;
    struct clientt *client;
    struct uchannelusert *chanuser;
    char *pt,*pt2;
    if(link>=0 && usern==0)
    {
	client=getclientbynick(ircnick);
	if(client==NULL && *ircnick!=0) return 0x0; /* ignore fakes */
	if(*ircto=='#')
	    sendtochannel(ircto,link,"%s",ircbuf);
	else
	    sendtoclient(ircto,link,"%s",ircbuf);
    } else {
	pt=ircto;
	while(pt!=NULL)
	{
	    pt2=strchr(pt,',');
	    if(pt2!=NULL)
	    {
		*pt2=0;pt2++;
	    }
	    if(strlen(pt)==0) return 0x0;
	    strmncpy(currentto,pt,sizeof(currentto));
	    if(*currentto=='#')
	    {
		chan=getuserchannel(-1,currentto);
		if(chan==NULL)
		{
		    client=getclientbynumber(usern);
		    if(client==NULL) 
    		    {
			ap_snprintf(intname,sizeof(intname),lngtxt(392),me);
			client=enteruser(usern,user(usern)->nick,user(usern)->login,intname,me);
		    }		    		    
		    if(client==NULL) return 0x0;
		    sendtoclient(client->nick,0,lngtxt(393),me,user(usern)->nick,currentto);
		} else {
		    client=getclientbynumber(usern);
		    if(client==NULL) 
    		    {
			ap_snprintf(intname,sizeof(intname),lngtxt(394),me);
			client=enteruser(usern,user(usern)->nick,user(usern)->login,intname,me);
		    }		    		    
		    if(client==NULL) return 0x0;
		    chanuser=getchannelnick(chan,client->nick);
		    if(strchr(chan->modes,'n')!=NULL && chanuser==NULL)
		    {
			sendtoclient(client->nick,0,lngtxt(395),me,client->nick,currentto);
		    } else {
			if(strchr(chan->modes,'m')!=NULL && strchr(chanuser->mode,'@')==NULL && strchr(chanuser->mode,'+')==NULL)
			    sendtoclient(client->nick,0,lngtxt(396),me,client->nick,currentto);
			else
			    sendtochannelbutone(currentto,client->nick,0,lngtxt(397),client->nick,client->ident,client->host,irccommand,currentto,irccontent);
		    }
		}
	    } else {
		client=getclientbynick(currentto);
		if(client==NULL)
		{
		    client=getclientbynumber(usern);
		    if(client==NULL) 
    		    {
			ap_snprintf(intname,sizeof(intname),lngtxt(398),me);
			client=enteruser(usern,user(usern)->nick,user(usern)->login,intname,me);
		    }		    		    
		    if(client==NULL) return 0x0;
		    sendtoclient(client->nick,0,lngtxt(399),me,client->nick,currentto);
		} else {
		    client=getclientbynumber(usern);
		    if(client==NULL) 
    		    {
			ap_snprintf(intname,sizeof(intname),lngtxt(400),me);
			client=enteruser(usern,user(usern)->nick,user(usern)->login,intname,me);
		    }		    		    
		    if(client==NULL) return 0x0;
		    sendtoclient(currentto,0,lngtxt(401),client->nick,client->ident,client->host,irccommand,currentto,irccontent);
		}				
	    }
	    pt=pt2;
	    
	}    	
    }
}

/* notice is same as like privmsg, so this is just a recall */

int cmdintnotice(int usern, int link)
{
    cmdintprivmsg(usern,link);
}

/* adding a joined intnet channel to an internal user */

int addconfigchannel(int usern, char *channel)
{
    char buf[200];
    char buf2[200];
    int i,rc;
    int cntchan;
    ap_snprintf(buf,sizeof(buf),lngtxt(402),usern);
    for(i=0;i<20;i++)
    {
	ap_snprintf(buf2,sizeof(buf2),lngtxt(403),i);
	rc=getini(lngtxt(404),buf2,buf);
	if(rc==0)
	{
	    if(strmcmp(value,channel)==1) return 0x0;
	}
    }
    cntchan=countconfentries(lngtxt(405),lngtxt(406),buf);
    ap_snprintf(buf2,sizeof(buf2),lngtxt(407),lastfree);
    writeini(lngtxt(408),buf2,buf,channel);
    flushconfig();
    return 0x0;
}

/* remove a parted channel from the internal user */

int removeconfigchannel(int usern, char *channel)
{
    int i;
    int rc;
    char buf[200];
    char buf2[200];
    ap_snprintf(buf,sizeof(buf),lngtxt(409),usern);
    for(i=0;i<20;i++)
    {
	ap_snprintf(buf2,sizeof(buf2),lngtxt(410),i);
	rc=getini(lngtxt(411),buf2,buf);
	if(rc==0)
	{
	    if(strmcmp(channel,value)==1)
	    {
		writeini(lngtxt(412),buf2,buf,NULL);
		flushconfig();
		return 0x0;
	    }			
	}
    }
}

/* rejoin the client to the saved channels */

int dontsave=0;

int rejoinintchannels(int usern)
{
    int i;
    int rc;
    struct uchannelt *chan;
    struct clientt *client;
    char buf[200];
    char buf2[200];
    int ingroup=(getpsocketbygroup(socketnode,SGR_USERINBOUND+usern,user(usern)->insock)!=NULL);
    ap_snprintf(buf,sizeof(buf),lngtxt(413),usern);
    dontsave=1;
    for(i=0;i<20;i++)
    {
	ap_snprintf(buf2,sizeof(buf2),lngtxt(414),i);
	rc=getini(lngtxt(415),buf2,buf);
	if(rc==0)
	{
	    strmncpy(irccontent,value,sizeof(irccontent));
	    cmdintjoin(usern);
	    if(ingroup)
	    {
		chan=getuserchannel(-1,irccontent);
		if(chan)
		{
		    client=getclientbynumber(usern);
		    if(client)
		    {
			if(getchannelnick(chan,client->nick)!=NULL)
			{
			    ssnprintf(user(usern)->insock,":%s!%s@%s JOIN :#int'%s",client->nick,client->ident,client->host,chan->name);
			    sendnames(usern+10000,chan);								
			    if(chan->topic[0]!=0)
			    {
				ssnprintf(user(usern)->insock,":%s 332 %s #int'%s :%s",me,client->nick,chan->name,chan->topic);
			    }
			    if(chan->modes[0]!=0)
			    {
				sendmode(usern,chan->name);
			    }
			}
		    }		    
		}		
	    }
	}
    }
    dontsave=0;
    return 0x0;
}

/* the join command */

int cmdintjoin(int usern, int link)
{
    char currentto[200];
    char bop[200];
    struct uchannelt *chan;
    struct stringarray *intops;
    struct clientt *client;
    int rc;
    struct uchannelusert *chanuser;
    char *pt,*pt2;
    char key[200];
    int newchan=0;
    if(link>=0 && usern==0)
    {
	chan=getuserchannel(-1,ircto);
	client=getclientbynick(ircnick);
	if(client==NULL) return 0x0; /* ignore fakes */
	if(chan==NULL)
	{
	    chan=addchanneltouser(-1,ircto,1);
	}
	removeinvite(chan,client->nick);
	chanuser=addnicktochannel(chan,client->nick,client->ident,client->host);
	sendtochannel(ircto,link,"%s",ircbuf);
    } else {
	pt=irccontent;
	pt2=strchr(pt,' ');
	if(pt2!=NULL)
	{
	    *pt2=0;
	    pt2++;
	    strmncpy(key,pt2,sizeof(key));
	}
	while(pt!=NULL)
	{
	    newchan=0;
	    pt2=strchr(pt,',');
	    if(pt2!=NULL)
	    {
		*pt2=0;pt2++;
	    }
	    if(strlen(pt)==0) return 0x0;
	    strmncpy(currentto,pt,sizeof(currentto));
	    if(*currentto=='#')
	    {
		client=getclientbynumber(usern);
		if(client==NULL) 
    		{
		    ap_snprintf(intname,sizeof(intname),lngtxt(416),me);
		    client=enteruser(usern,user(usern)->nick,user(usern)->login,intname,me);
		}		    		    
		if(client==NULL) return 0x0;
		chan=getuserchannel(-1,currentto);
		if(chan==NULL)
		{
		    chan=addchanneltouser(-1,currentto,0);
		    newchan=1;
		}
		pcontext;
		if(getchannelnick(chan,client->nick)!=NULL)
		    rc=-5;
		else
		    rc=mayjoin(chan,client,key);
		pcontext;
		if(rc<0)
		{
		    switch(rc)
		    {
			case -1:
			    pcontext;
			    sendtoclient(client->nick,0,lngtxt(417),me,client->nick,currentto);
			    break;
			case -2:
			    pcontext;
			    sendtoclient(client->nick,0,lngtxt(418),me,client->nick,currentto);
			    break;
			case -3:
			    pcontext;
			    sendtoclient(client->nick,0,lngtxt(419),me,client->nick,currentto);
			    break;
		    }
		} else {
		    pcontext;
		    removeinvite(chan,client->nick);
		    pcontext;
		    chanuser=addnicktochannel(chan,client->nick,client->ident,client->host);
		    pcontext;
		    sendtochannel(chan->name,0,lngtxt(420),client->nick,client->ident,client->host,currentto);
		    pcontext;
		    if(newchan==1)
		    {		
			sendtochannel(chan->name,0,lngtxt(421),client->nick,client->ident,client->host,currentto,client->nick);
			strmncpy(chanuser->mode,"@",sizeof(chanuser->mode));
			ap_snprintf(bop,sizeof(bop),"*!%s@%s",client->ident,client->host);
			addintop(chan,bop);
		    } else {
			ap_snprintf(bop,sizeof(bop),"%s!%s@%s",client->nick,client->ident,client->host);
			intops=chan->intops;
			while(intops)
			{
			    if(wild_match(intops->entry,bop))
			    {
				sendtochannel(chan->name,0,lngtxt(421),client->nick,client->ident,client->host,currentto,client->nick);
				strmncpy(chanuser->mode,"@",sizeof(chanuser->mode));
				intops=NULL;
			    }			        
			    if(intops) intops=intops->next;
			}
		    }
		    sendnames(usern+10000,chan);								
		    if(chan->topic[0]!=0)
		    {
			sendtoclient(client->nick,0,lngtxt(422),me,client->nick,chan->name,chan->topic);
		    }
		    if(chan->modes[0]!=0)
		    {
			sendmode(usern,chan->name);
		    }
		    if(dontsave==0) addconfigchannel(usern,chan->name);
		}
	    } else {
		client=getclientbynumber(usern);
		if(client==NULL) 
    		{
		    ap_snprintf(intname,sizeof(intname),lngtxt(423),me);
		    client=enteruser(usern,user(usern)->nick,user(usern)->login,intname,me);
		}		    		    
		if(client==NULL) return 0x0;
		sendtoclient(client->nick,0,lngtxt(424),me,client->nick,currentto);
	    }
	    pt=pt2;
	}    	
    }    
}

/* mode helpers */

int addop(struct uchannelt *chan, char *nick, struct clientt *client)
{
    struct uchannelusert *chanuser;
    struct clientt *opclient;
    char bop[200];
    chanuser=getchannelnick(chan,nick);
    if(chanuser!=NULL)
    {
	opclient=getclientbynick(nick);
	if(opclient)
	{
	    if(opclient->intuser>0)
	    {
		/* add a local user to remember as op */
		ap_snprintf(bop,sizeof(bop),"*!%s@%s",opclient->ident,opclient->host);
		addintop(chan,bop);
	    }
	}
	strmncpy(chanuser->mode,"@",sizeof(chanuser->mode));
	return 0x0;
    } else {
	if(client==NULL) return 0x0;
	if(client->intuser>0)
	    sendtoclient(client->nick,0,lngtxt(425),me,client->nick,chan->name,nick);
    }
    return 0x1;
}

int delop(struct uchannelt *chan, char *nick, struct clientt *client)
{
    struct uchannelusert *chanuser;
    char bop[200];
    struct clientt *opclient;
    chanuser=getchannelnick(chan,nick);
    if(chanuser!=NULL)
    {
	if(strchr(chanuser->mode,'@')!=NULL)
	{
	    if(strchr(chanuser->mode,'+')!=NULL)
		strmncpy(chanuser->mode,"+",sizeof(chanuser->mode));
	    else
		chanuser->mode[0]=0;
	    opclient=getclientbynick(nick);
	    if(opclient)
	    {
		if(opclient->intuser>0)
		{
		    /* remove a user from remember as op */
		    ap_snprintf(bop,sizeof(bop),"*!%s@%s",opclient->ident,opclient->host);
		    removeintop(chan,bop);
		}
	    }
	    return 0x0;
	}
    } else {
	if(client==NULL) return 0x0;
	if(client->intuser>0)
	    sendtoclient(client->nick,0,lngtxt(426),me,client->nick,chan->name,nick);
    }
    return 0x1;
}

int addvoice(struct uchannelt *chan, char *nick, struct clientt *client)
{
    struct uchannelusert *chanuser;
    chanuser=getchannelnick(chan,nick);
    if(chanuser!=NULL)
    {
	if(strchr(chanuser->mode,'@')==NULL)
	{
	    strmncpy(chanuser->mode,"+",sizeof(chanuser->mode));
	    return 0x0;
	}
    } else {
	if(client==NULL) return 0x0;
	if(client->intuser>0)
	    sendtoclient(client->nick,0,lngtxt(427),me,client->nick,chan->name,nick);
    }
    return 0x0;
}

int delvoice(struct uchannelt *chan, char *nick, struct clientt *client)
{
    struct uchannelusert *chanuser;
    chanuser=getchannelnick(chan,nick);
    if(chanuser!=NULL)
    {
	if(strchr(chanuser->mode,'+')!=NULL)
	{
	    if(strchr(chanuser->mode,'@')!=NULL)
		strmncpy(chanuser->mode,"@",sizeof(chanuser->mode));
	    else
		chanuser->mode[0]=0;
	    return 0x0;
	}
    } else {
	if(client==NULL) return 0x0;
	if(client->intuser>0)
	    sendtoclient(client->nick,0,lngtxt(428),me,client->nick,chan->name,nick);
    }
    return 0x1;
}

/* modes */

int intmode(char *to, char *modes, char *params,int link, int usern)
{
    char *m,*p,*pt,*pt2,*p2;
    char prefix='+';
    struct uchannelt *chan;
    struct clientt *client,*oc;
    struct uchannelusert *chanuser;
    struct stringarray *bans;
    char newmode[300];
    char newparam[400];
    int prefixchange=0;
    int rc;
    char mm[2];
    int aktbans=0;
pcontext;
    newmode[0]=0;
    newparam[0]=0;
    mm[1]=0;
    client=NULL;
    if(usern>0)
    {
	client=getclientbynumber(usern);
	if(client==NULL) 
	{
	    ap_snprintf(intname,sizeof(intname),lngtxt(429),me);
	    client=enteruser(usern,user(usern)->nick,user(usern)->login,intname,me);
	}		    		    
	if(client==NULL) return 0x0;
    }
    chan=getuserchannel(-1,to);
pcontext;
    if(chan==NULL)
    {
	if(usern>0 && client!=NULL)
	{
	    if(*to=='#')
		sendtoclient(client->nick,0,lngtxt(430),me,client->nick,to);
	    /* ignore user flags.. */
	}
	return 0x0;
    }
    m=modes;
    if(params[0]==0) 
	p=NULL;
    else
	p=params;
    if(p!=NULL)
	if(strlen(rtrim(p))<=1) p=NULL;
    while(*m)
    {
	    if(strchr("+-",*m)!=NULL) 
	    {
		if(prefix!=*m && prefixchange==0)
		{
		    prefix=*m;
		    prefixchange=1;
		    mm[0]=prefix;
		    strcat(newmode,mm);
		}
	    }
	    else 
	    if(strchr(lngtxt(431),*m)!=NULL)
	    {
		if(strchr(newmode,*m)==NULL) 
		{
		    mm[0]=*m;
		    if(prefix=='+' && strchr(chan->modes,*m)==NULL)
		    {	
			strcat(newmode,mm);
			prefixchange=0;
		    }
		    if(prefix=='-' && strchr(chan->modes,*m)!=NULL)
		    {	
			strcat(newmode,mm);
			prefixchange=0;
		    }
		}
	    }
	    else 
	    if(*m=='b')
	    {
		if(p==NULL)
		{
		    if(usern>0 && client!=NULL)
		    {
			bans=chan->bans;
			while(bans!=NULL)
			{
			    sendtoclient(client->nick,0,lngtxt(432),me,client->nick,chan->name,bans->entry);	    
			    bans=bans->next;
			}
			sendtoclient(client->nick,0,lngtxt(433),me,client->nick,chan->name);
		    }
		} else {
		    p2=strchr(p,' ');
		    if(p2!=NULL)
		    {
			*p2=0;p2++;
			if(*p2==0) p2=NULL;
		    }
		    if(strlen(p)>100)
		    {
			p[99]=0;
		    }
		    if(aktbans<3)
		    {
			if(prefix=='+')
			    rc=addban(chan,p);
			else
			    rc=removeban(chan,p);		
		    } else {
			rc=-1;
		    }
		    if(rc==0x1)
		    {
			mm[0]=*m;
			strcat(newmode,mm);
			if(strlen(newparam)+strlen(p)+1<sizeof(newparam)-1)
			{
			    strcat(newparam,p);
			    strcat(newparam," ");
			}
			aktbans++;
			prefixchange=0;
		    }
		}
		p=p2;
	    }
	    else 
	    if(*m=='o' || *m=='v')
	    {
		if(p!=NULL)
		{
		    p2=strchr(p,' ');
		    if(p2!=NULL)
		    {
			*p2=0;p2++;
			if(*p2==0) p2=NULL;
		    }
		    if(aktbans<3)
		    {
			if(prefix=='+')
			    if(*m=='o')
				rc=addop(chan,p,client);
			    else
				rc=addvoice(chan,p,client);
			else
			    if(*m=='o')
				rc=delop(chan,p,client);
			    else
				rc=delvoice(chan,p,client);
		    } else {
			rc=-1;
		    }
		    if(rc==0x0)
		    {
			mm[0]=*m;
			strcat(newmode,mm);
			if(strlen(newparam)+strlen(p)+1<sizeof(newparam)-1)
			{
			    oc=client;
			    client=getclientbynick(p);
			    if(client!=NULL)
				strcat(newparam,client->nick); /* cases */
			    else
				strcat(newparam,p);
			    client=oc;
			    strcat(newparam," ");
			}
			aktbans++;
			prefixchange=0;
		    }
		}
		p=p2;
	    }
	    else 
	    if(*m=='k' && p!=NULL)
	    {
		p2=strchr(p,' ');
		if(p2!=NULL)
		{
		    *p2=0;p2++;
		    if(*p2==0) p2=NULL;
		}
		mm[0]=*m;
		strcat(newmode,mm);
		if(strlen(newparam)+strlen(p)+1<sizeof(newparam)-1)
		{
		    strcat(newparam,p);
		    strcat(newparam," ");
		    prefixchange=0;
		}
		p=p2;		
	    }
	m++;
    }
    if((strchr("+-",*newmode)!=NULL && strlen(newmode)>1) || (strchr("+-",*newmode)==NULL && strlen(newmode)>0))
    {
	if(*newmode!='+' && *newmode!='-')
	    mm[0]='+';
	else
	    mm[0]=0;
	mm[1]=0;
	if(usern>0)
	    sendtochannel(chan->name,0,lngtxt(434),client->nick,client->ident,client->host,to,mm,newmode,rtrim(newparam));
	else
	    sendtochannel(chan->name,link,lngtxt(435),ircfrom,to,mm,newmode,rtrim(newparam));
	addchannelmode(chan,newmode,newparam);
    }
}

/* send a channels mode */

int sendmode(int usern, char *to)
{
    struct uchannelt *chan;
    struct clientt *client;
    struct uchannelusert *chanuser;
    chan=getuserchannel(-1,to);
    client=getclientbynumber(usern);
    if(client==NULL) 
    {
        ap_snprintf(intname,sizeof(intname),lngtxt(436),me);
        client=enteruser(usern,user(usern)->nick,user(usern)->login,intname,me);
    }		    		    
    if(client==NULL) return 0x0;
    if(chan==NULL)
	sendtoclient(client->nick,0,lngtxt(437),me,client->nick,to);
    else
    {
	chanuser=getchannelnick(chan,client->nick);
	if(chanuser!=NULL)
	{
	    if(strchr(chan->modes,'k')!=NULL)
		sendtoclient(client->nick,0,lngtxt(438),me,client->nick,to,chan->modes,chan->key);
	    else
		sendtoclient(client->nick,0,lngtxt(439),me,client->nick,to,chan->modes);
	}
	else
	    sendtoclient(client->nick,0,lngtxt(440),me,client->nick,to,chan->modes);
    }
}

/* the caller of the mode */

int cmdintmode(int usern, int link)
{
    char currentto[200];
    char tolist[400];
    char modelist[200];
    char param[400];
    struct uchannelt *chan;
    struct clientt *client;
    int rc;
    struct uchannelusert *chanuser;
    char *pt,*pt2;
    if(link>=0 && usern==0)
    {
	strmncpy(currentto,ircto,sizeof(currentto));
	if(*currentto!=0)
	{
	    pt=irccontent;
	    pt2=strchr(pt,' ');
	    if(pt2!=NULL)
	    {
	        *pt2=0;
	        pt2++;
	    }
	    strmncpy(modelist,pt,sizeof(modelist));
	    if(pt2==NULL)
		param[0]=0;
	    else
		strmncpy(param,pt2,sizeof(param));
	    intmode(currentto,modelist,param,link,usern);
	}	
    } else {
	pt=irccontent;
	if(*pt!=0)
	{
	    pt2=strchr(pt,' ');
	    if(pt2!=NULL)
	    {
		*pt2=0;
		strmncpy(tolist,pt,sizeof(tolist));
		pt2++;
		pt=strchr(pt2,' ');
		if(1)
		{
		    if(pt!=NULL)
		    {
			*pt=0;pt++;
		    }
		    strmncpy(modelist,pt2,sizeof(modelist));
		    if(pt!=NULL)
			strmncpy(param,pt,sizeof(param));
		    else
			param[0]=0;
		    pt=tolist;
		    while(pt!=NULL)
		    {
			pt2=strchr(pt,',');
			if(pt2!=NULL)
			{
			    *pt2=0;pt2++;
			}
			client=getclientbynumber(usern);
			if(client==NULL) 
    			{
			    ap_snprintf(intname,sizeof(intname),lngtxt(441),me);
			    client=enteruser(usern,user(usern)->nick,user(usern)->login,intname,me);
			    if(client==NULL) return 0x0;
			}		    		    
			strmncpy(currentto,pt,sizeof(currentto));
			chan=getuserchannel(-1,currentto);
			if(chan==NULL)
			    sendtoclient(client->nick,0,lngtxt(442),me,client->nick,currentto);
			else
			{
			    chanuser=getchannelnick(chan,client->nick);
			    if(chanuser==NULL)
				sendtoclient(client->nick,0,lngtxt(443),me,client->nick,currentto);
			    else
			    {
				if(strchr(chanuser->mode,'@')!=NULL)
				{
				    intmode(currentto,modelist,param,link,usern);
				} else {
				    sendtoclient(client->nick,0,lngtxt(444),me,client->nick,currentto);
				}
			    }
			}
		    	pt=pt2;
		    }		    		
		}	    
	    } else {
		strmncpy(tolist,pt,sizeof(tolist));
		pt=tolist;
		while(pt!=NULL)
		{
		    pt2=strchr(pt,',');
		    if(pt2!=NULL)
		    {
			*pt2=0;pt2++;
		    }
		    strmncpy(currentto,pt,sizeof(currentto));
		    sendmode(usern,currentto);
		    pt=pt2;		    
		}		    		
	    }
	}	
    }
}

int cmdintpart(int usern, int link)
{
    char currentto[200];
    struct uchannelt *chan;
    struct clientt *client;
    int rc;
    struct uchannelusert *chanuser;
    char *pt,*pt2;
    char key[200];
    int newchan=0;
    if(link>=0 && usern==0)
    {
	chan=getuserchannel(-1,ircto);
	client=getclientbynick(ircnick);
	if(client==NULL) return 0x0; /* ignore fakes */
	if(chan==NULL)
	{
	    return 0x0; /* no such channel */
	}
	chanuser=getchannelnick(chan,ircnick);
	if(chanuser==NULL) return 0x0;
	removenickfromchannel(chan,ircnick);
	if(chan->users==NULL)
	    removechannelfromuser(-1,chan->name);
	sendtochannel(ircto,link,"%s",ircbuf);
    } else {
	if(*ircto==0)
	{
	    strmncpy(ircto,irccontent,sizeof(ircto));
	    strmncpy(irccontent,user(usern)->nick,sizeof(irccontent));
	}
	pt=ircto;
	while(pt!=NULL)
	{
	    pt2=strchr(pt,',');
	    if(pt2!=NULL)
	    {
		*pt2=0;pt2++;
	    }
	    if(strlen(pt)==0) return 0x0;
	    strmncpy(currentto,pt,sizeof(currentto));
	    if(*currentto=='#')
	    {
		client=getclientbynumber(usern);
		if(client==NULL) 
    		{
		    ap_snprintf(intname,sizeof(intname),lngtxt(445),me);
		    client=enteruser(usern,user(usern)->nick,user(usern)->login,intname,me);
		}		    		    
		if(client==NULL) return 0x0;
		chan=getuserchannel(-1,currentto);
		if(chan==NULL)
		{
		    sendtoclient(client->nick,0,lngtxt(446),me,client->nick,currentto);
		} else {
		    chanuser=getchannelnick(chan,client->nick);
		    if(chanuser==NULL)
		    {
			sendtoclient(client->nick,0,lngtxt(447),me,client->nick,currentto);
		    } else {
			sendtochannel(chan->name,0,lngtxt(448),client->nick,client->ident,client->host,currentto,irccontent);
			removenickfromchannel(chan,client->nick);
			removeconfigchannel(usern,chan->name);
			if(chan->users==NULL)
			{
			    removechannelfromuser(-1,chan->name);
			}
		    }
		}
	    } else {
		client=getclientbynumber(usern);
		if(client==NULL) 
    		{
		    ap_snprintf(intname,sizeof(intname),lngtxt(449),me);
		    client=enteruser(usern,user(usern)->nick,user(usern)->login,intname,me);
		}		    		    
		if(client==NULL) return 0x0;
		sendtoclient(client->nick,0,lngtxt(450),me,client->nick,currentto);
	    }
	    pt=pt2;
	}    	
    }    
}

/* got the kick ? */

int cmdintkick(int usern, int link)
{
    char currentto[200];
    char nicks[400];
    char currentnick[64];
    struct uchannelt *chan;
    struct clientt *client;
    struct clientt *kclient;
    int rc;
    struct uchannelusert *chanuser;
    char *pt,*pt2;
    char *ot,*ot2;
    char *onicks;
    char key[200];
    char bop[200];
    int newchan=0;
    if(link>=0 && usern==0)
    {
	pt=strstr(ircbuf,lngtxt(451));
	if(pt==NULL) return 0x0;
	pt+=6;
	pt2=strchr(pt,' ');
	if(pt2==NULL) return 0x0;
	*pt2=0;
	pt2++;
	ot=strstr(pt2," :");
	if(ot==NULL) return 0x0;
	*ot=0;
	ot+=2;
	strmncpy(ircto,pt,sizeof(ircto));
	strmncpy(irccontent,pt2,sizeof(irccontent));
	pt=ot;
	chan=getuserchannel(-1,ircto);
	client=getclientbynick(ircnick);
	if(chan==NULL || client==NULL)
	{
	    return 0x0; /* no such channel */
	}
	chanuser=getchannelnick(chan,irccontent);
	if(chanuser==NULL) return 0x0;
	strmncpy(currentnick,chanuser->nick,sizeof(currentnick));
	sendtochannel(ircto,link,lngtxt(452),client->nick,client->ident,client->host,ircto,currentnick,pt);
	removenickfromchannel(chan,currentnick);
	kclient=getclientbynick(currentnick);
	if(kclient)
	{
	    if(kclient->intuser>0)
	    {
		ap_snprintf(bop,sizeof(bop),"*!%s@%s",kclient->ident,kclient->host);
		removeintop(chan,bop);
	    }
	}
	if(chan->users==NULL)
	    removechannelfromuser(-1,chan->name);
    } else {
	client=getclientbynumber(usern);
	if(client==NULL) 
    	{	
	    ap_snprintf(intname,sizeof(intname),lngtxt(453),me);
	    client=enteruser(usern,user(usern)->nick,user(usern)->login,intname,me);
	}		    		    
	if(client==NULL) return 0x0;
	if(*ircto==0)
	{
	    strmncpy(ircto,irccontent,sizeof(ircto));
	    strmncpy(irccontent,client->nick,sizeof(irccontent));
	}
	/* now we got the channels and the clients to kick in ircto, the reason in irccontent */
	pt=strchr(ircto,' ');
	if(pt==NULL) return 0x0;
	*pt=0;
	pt++;
	onicks=pt;
	pt=ircto;
	while(pt!=NULL)
	{
	    pt2=strchr(pt,',');
	    if(pt2!=NULL)
	    {
		*pt2=0;
		pt2++;
		if(*pt2==0) pt2=NULL;
	    }
	    strmncpy(currentto,pt,sizeof(currentto));
	    chan=getuserchannel(-1,currentto);
	    if (chan==NULL)
		sendtoclient(client->nick,0,lngtxt(454),me,client->nick,currentto);
	    else
	    {
		chanuser=getchannelnick(chan,client->nick);
		if(chanuser==NULL)
		    sendtoclient(client->nick,0,lngtxt(455),me,client->nick,currentto);
		else
		{
		    if(strchr(chanuser->mode,'@')==NULL)
			sendtoclient(client->nick,0,lngtxt(456),me,client->nick,currentto);
		    else
		    {
			strmncpy(nicks,onicks,sizeof(nicks));
			ot=nicks;
			while(ot!=NULL)
			{
			    ot2=strchr(ot,',');
			    if(ot2!=NULL)
			    {
				*ot2=0;
				ot2++;
				if(*ot2==0) ot2=NULL;				
			    }
			    chanuser=getchannelnick(chan,ot);
			    if(chanuser!=NULL)
			    {
				sendtochannel(currentto,0,lngtxt(457),client->nick,client->ident,client->host,currentto,chanuser->nick,irccontent);
				removenickfromchannel(chan,ot);
				kclient=getclientbynick(ot);
				if(kclient)
				{
				    if(kclient->intuser>0)
				    {
					ap_snprintf(bop,sizeof(bop),"*!%s@%s",kclient->ident,kclient->host);
					removeintop(chan,bop);
				    }
				}
				if(chan->users==NULL)
				{
				    removechannelfromuser(-1,currentto);
				}
			    } else {
				sendtoclient(client->nick,0,lngtxt(458),me,client->nick,ot,currentto);
			    }
			    ot=ot2;
			}
		    }
		}
	    }
	    pt=pt2;
	}
    }
}

/* make a nickchange */

int cmdintnick(int usern, int link)
{
    struct clientt *client,*client2;
    char *pt;
    char ocoll[200];
    if(link>=0 && usern==0)
    {
	client=getclientbynick(ircnick);
	if(client==NULL) return 0x0;
	client2=getclientbynick(ircto);
	if(client2!=NULL) /* ohoh, two equal nicks.. refuse that link */
	{
	    ap_snprintf(irccontent,sizeof(irccontent),"%d",link);
#ifdef LINKAGE
	    cmdrelink(1);
#endif
	    sendtoclient(client2->nick,link,lngtxt(459),ircnick,ircident,irchost,client2->nick,datalink(link)->iam);
	} else {
	    strmncpy(ocoll,ircto,sizeof(ocoll));
	    sendtochannelsofnick(ircnick,link,"%s",ircbuf);
	    nickchange(-1,client->nick,ocoll);
	    strmncpy(client->nick,ocoll,sizeof(client->nick));
	}
    } else {
	client=getclientbynick(ircto);
	if(client!=NULL) return 0x0;
	client=getclientbynumber(usern);
	if(client==NULL) 
	{
	    ap_snprintf(intname,sizeof(intname),lngtxt(460),me);
	    client=enteruser(usern,user(usern)->nick,user(usern)->login,intname,me);
	}		    		    
	if(client==NULL) return 0x0;
	pt=ircto;
	if(strmcmp(pt,client->nick)==0)
	{
	    strmncpy(ocoll,ircto,sizeof(ocoll));
	    sendtochannelsofnick(client->nick,0,lngtxt(461),client->nick,client->ident,client->host,ircto);
	    nickchange(-1,client->nick,ircto);
	    strmncpy(client->nick,ircto,sizeof(client->nick));
	}
    }
}

/* userhost command */

int cmdintuserhost(int usern, int link)
{
    struct clientt *client,*client2;
    char *pt;
    char ocoll[200];
    if(link>=0 && usern==0)
    {
	return 0x0;
    } else {
	client=getclientbynick(irccontent);
	if(client==NULL) return 0x0;
	client2=getclientbynumber(usern);
	if(client2==NULL) 
	{
	    ap_snprintf(intname,sizeof(intname),lngtxt(460),me);
	    client2=enteruser(usern,user(usern)->nick,user(usern)->login,intname,me);
	}		    		    
	if(client2==NULL) return 0x0;
        if(client==client2)
    	    ssnprintf(user(usern)->insock,":%s-psybnc.net 302 %s :%s=+%s!%s",me,user(usern)->nick,client->nick,client->ident,client->host);
	else
	    ssnprintf(user(usern)->insock,":%s-psybnc.net 302 %s :int'%s=+%s!%s",me,user(usern)->nick,client->nick,client->ident,client->host);
    }
}

/* ison command */

int cmdintison(int usern, int link)
{
    struct clientt *client,*client2;
    char *pt;
    char ocoll[200];
    if(link>=0 && usern==0)
    {
	return 0x0;
    } else {
	client=getclientbynick(irccontent);
	if(client==NULL) return 0x0;
	client2=getclientbynumber(usern);
	if(client2==NULL) 
	{
	    ap_snprintf(intname,sizeof(intname),lngtxt(460),me);
	    client2=enteruser(usern,user(usern)->nick,user(usern)->login,intname,me);
	}		    		    
	if(client2==NULL) return 0x0;
	if(client==client2)
	    ssnprintf(user(usern)->insock,":%s-psybnc.net 303 %s :%s",me,user(usern)->nick,client->nick);
	else
	    ssnprintf(user(usern)->insock,":%s-psybnc.net 303 %s :int'%s",me,user(usern)->nick,client->nick);
    }
}

/* names */

int cmdintnames(int usern, int link)
{
    struct clientt *client;
    struct uchannelt *chan;
    struct uchannelusert *chanuser;
    if(link>=0 && usern==0)
	return 0x0; /* names from a link !? yes.. right. */
    else
    {
	client=getclientbynumber(usern);
	if(client==NULL) 
	{
	    ap_snprintf(intname,sizeof(intname),lngtxt(462),me);
	    client=enteruser(usern,user(usern)->nick,user(usern)->login,intname,me);
	}		    		    
	if(client==NULL) return 0x0;
	chan=getuserchannel(-1,irccontent);
	if(chan==NULL)
	{
	    sendtoclient(client->nick,0,lngtxt(463),me,client->nick,irccontent);
	} else {
	    chanuser=getchannelnick(chan,client->nick);
	    if(chanuser==NULL)
	    {
		sendtoclient(client->nick,0,lngtxt(464),me,client->nick,irccontent);
	    } else {
		sendnames(usern+10000,chan);
	    }
	}
    }
}

/* quit */

int cmdintquit(int usern, int link)
{
    char tmpnick[64];
    struct clientt *client;
    if(link>=0 && usern==0)
    {
	client=getclientbynick(ircnick);
	if(client!=NULL)
	{
	    if(client->intuser==0)
	    {
		strmncpy(tmpnick,ircnick,sizeof(tmpnick));
		sendtochannelsofnick(tmpnick,link,"%s",ircbuf);
		removenickfromallchannels(-1,tmpnick);
		removeclient(tmpnick);
	    }
	} 
    } else {
	client=getclientbynumber(usern);
	if(client==NULL) 
	{
	    return 0x0;
	}		    		    
	sendtochannelsofnick(client->nick,0,lngtxt(465),client->nick,client->ident,client->host,irccontent);			
	removenickfromallchannels(-1,client->nick);
	removeclient(client->nick);
    }
    return 0x0;
}

/* who query */

int cmdintwho(int usern, int link)
{
    struct uchannelt *chan;
    struct uchannelusert *chanuser;
    struct clientt *client,*listclient;
    if(link>=0 && usern==0)
	return 0x0; /* who from a link !? yes.. right. */
    else
    {
	client=getclientbynumber(usern);
	if(client==NULL) 
	{
	    ap_snprintf(intname,sizeof(intname),lngtxt(466),me);
	    client=enteruser(usern,user(usern)->nick,user(usern)->login,intname,me);
	}		    		    
        if(client==NULL) return 0x0;
	if(*irccontent==0)
	{
	    listclient=clients;
	    while(listclient!=NULL)
	    {
		sendtoclient(client->nick,0,lngtxt(467),me,client->nick,listclient->ident,listclient->host,listclient->server,listclient->nick);
		listclient=listclient->next;
	    }	
	    sendtoclient(client->nick,0,lngtxt(468),me,client->nick);
	} else {
	    chan=getuserchannel(-1,irccontent);
	    if(chan==NULL)
	    {
		sendtoclient(client->nick,0,lngtxt(469),me,client->nick,irccontent);
	    } else {
		chanuser=getchannelnick(chan,client->nick);
		if(chanuser==NULL)
		{
		    sendtoclient(client->nick,0,lngtxt(470),me,client->nick,irccontent);
		} else {
		    chanuser=chan->users;
		    while(chanuser!=NULL)
		    {
			sendtoclient(client->nick,0,lngtxt(471),me,client->nick,chan->name,chanuser->ident,chanuser->host,chanuser->nick);
			chanuser=chanuser->next;
		    }
		    sendtoclient(client->nick,0,lngtxt(472),me,client->nick,chan->name);
		}
	    }
	}
    }
}

/* whois */

int cmdintwhois(int usern, int link)
{
    struct uchannelt *chan;
    struct uchannelusert *chanuser;
    struct clientt *client,*listclient;
    int gotchan=0;
    char chanbuf[4096];
    if(link>=0 && usern==0)
	return 0x0; /* who from a link !? yes.. right. */
    else
    {
	client=getclientbynumber(usern);
	if(client==NULL) 
	{
	    ap_snprintf(intname,sizeof(intname),lngtxt(473),me);
	    client=enteruser(usern,user(usern)->nick,user(usern)->login,intname,me);
	}		    		    
	if(client==NULL) return 0x0;
	listclient=getclientbynick(irccontent);
	if(listclient==NULL)
	{
	    sendtoclient(client->nick,0,lngtxt(474),me,client->nick,irccontent);
	} else {
	    sendtoclient(client->nick,0,lngtxt(475),me,client->nick,listclient->nick,listclient->ident,listclient->host);	
	    ap_snprintf(chanbuf,sizeof(chanbuf),lngtxt(476),me,client->nick,listclient->nick);
	    chan=intchan;
	    while(chan!=NULL)
	    {
		chanuser=getchannelnick(chan,client->nick);
		if(chanuser==NULL)
		{
		    if(strchr(chan->modes,'s')==NULL && strchr(chan->modes,'p')==NULL) 
		    {
			if(getchannelnick(chan,listclient->nick)!=NULL)
			{
			    if(strlen(chanbuf)+strlen(chan->name)+1<sizeof(chanbuf))
			    {
				strcat(chanbuf,chan->name);
				strcat(chanbuf," ");
				gotchan=1;
			    }
			}
		    }
		}
		chan=chan->next;
	    }
	    if(gotchan==1)
		sendtoclient(client->nick,0,"%s",chanbuf);
	    sendtoclient(client->nick,0,lngtxt(477),me,client->nick,listclient->nick,listclient->server);
	    if(listclient->intuser>0)
	    {
		if(user(listclient->intuser)->rights==RI_ADMIN)
		    sendtoclient(client->nick,0,lngtxt(478),me,client->nick,listclient->nick);
	    }
	    sendtoclient(client->nick,0,lngtxt(479),me,client->nick,listclient->nick);
	}
    }
}

/* user received */

int cmdintuser(int usern, int link)
{
    
    struct uchannelt *chan;
    struct uchannelusert *chanuser;
    struct clientt *client;
    char *pt,*ept;
    char nick[64],ident[16],host[200],server[200];
    char renam[64];
    if(link>=0 && usern==0)
    {
	strmncpy(server,irccontent,sizeof(server));
	pt=strchr(ircto,' ');
	if(pt!=NULL)
	{
	    *pt=0;
	    pt++;
	    strmncpy(nick,ircto,sizeof(nick));
	    if(strstr(nick,lngtxt(480))==nick)
	    {
		ept=nick+5;
		if(atoi(ept)!=0)
		{
		    return 0x0; /* silently ignore Guest Nicks */
		}
	    }
	    ept=strchr(pt,' ');
	    if(ept!=NULL)
	    {
		*ept=0;
		ept++;
		strmncpy(ident,pt,sizeof(ident));
		strmncpy(host,ept,sizeof(host));
		pt=strchr(host,' ');
		if(pt!=NULL) *pt=0;
		client=getclientbynick(nick);
		if(client!=NULL)
		{
		    ap_snprintf(irccontent,sizeof(irccontent),"%d",link);
#ifdef LINKAGE
		    cmdrelink(1);
#endif
		    sendtoclient(client->nick,link,lngtxt(481),nick,ident,host,client->nick,datalink(link)->iam);
		} else {
		    createclient(0,nick,ident,host,server);
		    client=getclientbynick(nick);
		    ap_snprintf(ircbuf,sizeof(ircbuf),lngtxt(482),me,nick,ident,host,server);		    
		    broadcast(link);
		}
	    }
	}	
    }
    else
    {
	enteruser(usern,user(usern)->nick,user(usern)->login,user(usern)->nick,me);
    }
    return 0x0;
}

/* set the topic */

int cmdinttopic(int usern, int link)
{
    struct uchannelt *chan;
    struct uchannelusert *chanuser;
    struct clientt *client,*listclient;
    int gotchan=0;
    char chanbuf[4096];
    if(link>=0 && usern==0)
    {
	chan=getuserchannel(-1,ircto);
	if(chan!=NULL)
	{
	    strmncpy(chan->topic,irccontent,sizeof(chan->topic));
	    sendtochannel(chan->name,link,"%s",ircbuf);
	}
    }
    else
    {
	client=getclientbynumber(usern);
	if(client==NULL) 
	{
	    ap_snprintf(intname,sizeof(intname),lngtxt(483),me);
	    client=enteruser(usern,user(usern)->nick,user(usern)->login,intname,me);
	}		    		    
	if(client==NULL) return 0x0;
	chan=getuserchannel(-1,ircto);
	if(chan==NULL)
	{
	    sendtoclient(client->nick,0,lngtxt(484),me,client->nick,ircto);
	} else {
	    chanuser=getchannelnick(chan,client->nick);
	    if(chanuser==NULL)
	    {
		sendtoclient(client->nick,0,lngtxt(485),me,client->nick,ircto);
	    } else {
		if(strchr(chanuser->mode,'@')==NULL)
		{
		    sendtoclient(client->nick,0,lngtxt(486),me,client->nick,chan->name);
		} else {
		    if(strlen(irccontent)>80)
			irccontent[80]=0;
		    strmncpy(chan->topic,irccontent,sizeof(chan->topic));
		    sendtochannel(chan->name,0,lngtxt(487),client->nick,client->ident,client->host,chan->name,chan->topic);			
		}
	    }
	}
    }
}

/* add an invite */

int cmdintinvite(int usern, int link)
{
    char currentto[200];
    char nicks[400];
    char currentnick[64];
    struct uchannelt *chan;
    struct clientt *client;
    int rc;
    struct uchannelusert *chanuser;
    char *pt,*pt2;
    char *ot,*ot2;
    char *onicks;
    char key[200];
    int newchan=0;
    if(link>=0 && usern==0)
    {
	chan=getuserchannel(-1,irccontent);
	client=getclientbynick(ircnick);
	if(client==NULL) return 0x0; /* ignore fakes */
	if(chan==NULL) return 0x0; /* no such channel */
	chanuser=getchannelnick(chan,ircto);
	if(chanuser!=NULL) return 0x0;
	addinvite(chan,ircto);
	sendtoclient(ircto,link,"%s",ircbuf);
    } else {
	client=getclientbynumber(usern);
	if(client==NULL) 
    	{	
	    ap_snprintf(intname,sizeof(intname),lngtxt(488),me);
	    client=enteruser(usern,user(usern)->nick,user(usern)->login,intname,me);
	}		    		    
	if(client==NULL) return 0x0;
	if(*ircto==0)
	{
	    strmncpy(ircto,irccontent,sizeof(ircto));
	    strmncpy(irccontent,client->nick,sizeof(irccontent));
	}
	/* now we got the channels and the clients to kick in ircto, the reason in irccontent */
	pt=strchr(ircto,' ');
	if(pt==NULL) return 0x0;
	*pt=0;
	pt++;
	onicks=pt;
	pt=ircto;
	while(pt!=NULL)
	{
	    pt2=strchr(pt,',');
	    if(pt2!=NULL)
	    {
		*pt2=0;
		pt2++;
		if(*pt2==0) pt2=NULL;
	    }
	    strmncpy(currentto,pt,sizeof(currentto));
	    chan=getuserchannel(-1,currentto);
	    if (chan==NULL)
		sendtoclient(client->nick,0,lngtxt(489),me,client->nick,currentto);
	    else
	    {
		chanuser=getchannelnick(chan,client->nick);
		if(chanuser==NULL)
		    sendtoclient(client->nick,0,lngtxt(490),me,client->nick,currentto);
		else
		{
		    if(strchr(chanuser->mode,'@')==NULL)
			sendtoclient(client->nick,0,lngtxt(491),me,client->nick,currentto);
		    else
		    {
			strmncpy(nicks,onicks,sizeof(nicks));
			ot=nicks;
			while(ot!=NULL)
			{
			    ot2=strchr(ot,' ');
			    if(ot2!=NULL)
			    {
				*ot2=0;
				ot2++;
				if(*ot2==0) ot2=NULL;				
			    }
			    chanuser=getchannelnick(chan,ot);
			    if(chanuser==NULL)
			    {
				sendtoclient(ot,0,lngtxt(492),client->nick,client->ident,client->host,ot,currentto);
				addinvite(chan,ot);
			    } else {
				sendtoclient(client->nick,0,lngtxt(493),me,client->nick,ot,currentto);
			    }
			    ot=ot2;
			}
		    }
		}
	    }
	    pt=pt2;
	}
    }
}

/* join the network to a link */

int joinintnettolink(int link)
{
    struct clientt *client;
    struct uchannelt *chan;
    struct uchannelusert *chanuser;
    struct stringarray *bans;
    char modeline[1024];
    char paramline[1024];
    int modes;
    pcontext;
    client=clients;
    while(client!=NULL)
    {
	sendtolink(link,lngtxt(494),client->nick,client->ident,client->host,client->server);
	client=client->next;
    }        
    chan=intchan;
    while(chan!=NULL)
    {
	chanuser=chan->users;
	ap_snprintf(modeline,sizeof(modeline),lngtxt(495),me,chan->name);
	paramline[0]=0;
	modes=0;
	while(chanuser!=NULL)
	{
	    sendtolink(link,lngtxt(496),chanuser->nick,chanuser->ident,chanuser->host,chan->name);
	    if(chanuser->mode[0]=='@' || chanuser->mode[0]=='+')
	    {
		if(modes==3)
		{
		    sendtolink(link,lngtxt(497),modeline,paramline);		
		    ap_snprintf(modeline,sizeof(modeline),lngtxt(498),me,chan->name);
		    paramline[0]=0;
		    modes=0;
		}
		if(chanuser->mode[0]=='@')
		    strcat(modeline,"o");
		else    
		    strcat(modeline,"v");
		if(strlen(paramline)+strlen(chanuser->nick)+1<sizeof(paramline))
		{
		    strcat(paramline,chanuser->nick);
		    modes++;
		    if(modes!=3) strcat(paramline," ");
		}
	    }
	    chanuser=chanuser->next;
	}
	if(modes!=0)
	{
	    sendtolink(link,lngtxt(499),modeline,paramline);		
	}
	ap_snprintf(modeline,sizeof(modeline),lngtxt(500),me,chan->name);
	paramline[0]=0;
	modes=0;
	bans=chan->bans;
	while(bans!=NULL)
	{
	    if(modes==3)
	    {
		sendtolink(link,lngtxt(501),modeline,paramline);		
		ap_snprintf(modeline,sizeof(modeline),lngtxt(502),me,chan->name);
		paramline[0]=0;
		modes=0;	    
	    }
	    if(strlen(modeline)+1<sizeof(modeline))
		strcat(modeline,"b");
	    if(strlen(paramline)+strlen(bans->entry)+1<sizeof(paramline))
	    {
		strcat(paramline,bans->entry);
		strcat(paramline," ");
	    }
	    modes++;
	    bans=bans->next;
	}
	if(modes!=0)
	{
	    sendtolink(link,lngtxt(503),modeline,paramline);		
	    modes=0;	    
	}
	sendtolink(link,lngtxt(504),me,chan->name,chan->modes);
	chan=chan->next;
    }
    pcontext;
    return 0x0;
}

/* remove all specified users of a lost link from the network */

int removeinternal(char *server)
{
    char tmpnick[64];
    struct topologyt *topo;
    struct clientt *client,*eclient;
    client=clients;
    if(strmcmp(server,me)==1) return 0x0; /* dont remove ourselves */
    while(client!=NULL)
    {
	eclient=client->next;
	if(strmcmp(client->server,server)==1)
	{
	    strmncpy(tmpnick,client->nick,sizeof(tmpnick));
	    sendtochannelsofnick(tmpnick,0,lngtxt(505),client->nick,client->ident,client->host,me,server);
	    removeclient(tmpnick);	
	    removenickfromallchannels(-1,tmpnick);
	} else {
	    /* collect garbage, extremely needed */
	    topo=gettopology(client->server);
	    if(topo==NULL)
	    {
		strmncpy(tmpnick,client->nick,sizeof(tmpnick));
		sendtochannelsofnick(tmpnick,0,lngtxt(506),client->nick,client->ident,client->host,me,server);
		removeclient(tmpnick);	
		removenickfromallchannels(-1,tmpnick);
	    }
	}
	client=eclient;
    }
    return 0x0;
}


#endif
