/* $Id: p_link.c,v 1.4 2005/06/04 18:00:14 hisi Exp $ */
/************************************************************************
 *   psybnc2.3.2, src/p_link.c
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
static char rcsid[] = "@(#)$Id: p_link.c,v 1.4 2005/06/04 18:00:14 hisi Exp $";
#endif

#define P_LINK

#include <p_global.h>

#ifdef LINKAGE

/* look for a link by hostname and port */

int getlink(int peern)
{
    int i=1;
    char *ho;
    pcontext;
    while (i<=MAXCONN)
    {
	ho=datalink(i)->host;
	if(strstr(datalink(i)->host,"S=")==datalink(i)->host)
	    ho+=2;
	if (strmcmp(ho,newpeer(peern)->host)) 
	{
	    if (datalink(i)->port==newpeer(peern)->lnkport)
		return i;
	}
	i++;
    }
    return 0x0;
}

/* look for a link by name */

int getlinkbyname(char *namelink)
{
    struct linknodes *th;
    int i;
    pcontext;
    th=linknode;
    while (th!=NULL)
    {
	i=th->uid;
	if (strlen(datalink(i)->name)==strlen(namelink)) {
	    if (strstr(datalink(i)->name,namelink)!=NULL) {
		return i;
	    }
	}
	th=th->next;
    }
    return 0x0;
}
/* return a new link */

int getnewlink()
{
    int i=1;
    pcontext;
    D_CREATE=1;
    while (i<=MAXCONN)
    {
	if (datalink(i)->type==0) return i;
	i++;
    }
    return 0x0;
}

/* linking a relay */

int linkrelayremap(int nlink, int newsocket)
{
    datalink(nlink)->outsock=newsocket;
    return 0x0;
}
    
int linkrelayconnected(int nlink)
{
    int tmpsock;
    pcontext;
    tmpsock=datalink(nlink)->outsock;
    ssnprintf(tmpsock,lngtxt(507),datalink(nlink)->iam,datalink(nlink)->iam,datalink(nlink)->iam);
    ssnprintf(tmpsock,lngtxt(508),datalink(nlink)->iam);
    p_log(LOG_INFO,-1,lngtxt(509),datalink(nlink)->iam);
    return 0x0;
}

int errorrelaylink(int nlink, int errn)
{
    struct socketnodes *lkm;
    pcontext;
    p_log(LOG_ERROR,-1,lngtxt(510),datalink(nlink)->iam);
    lkm=getpsocketbysock(datalink(nlink)->outsock);
    if(lkm!=NULL)
    {
	lkm->sock->errorhandler=NULL;
	killsocket(datalink(nlink)->outsock);
    }
    lkm=getpsocketbysock(datalink(nlink)->insock);
    if(lkm!=NULL)
    {
	lkm->sock->errorhandler=NULL;
	killsocket(datalink(nlink)->insock);
    }
    datalink(nlink)->instate=STD_NOCON;
    datalink(nlink)->outstate=STD_NOCON;
    datalink(nlink)->insock=0;
    datalink(nlink)->outsock=0;
    return -1;
}

int killrelaylink(int nlink)
{
    struct socketnodes *lkm;
    pcontext;
    p_log(LOG_WARNING,-1,lngtxt(511),datalink(nlink)->iam);
    lkm=getpsocketbysock(datalink(nlink)->outsock);
    if(lkm!=NULL)
    {
	lkm->sock->destructor=NULL;
	killsocket(datalink(nlink)->outsock);
    }
    lkm=getpsocketbysock(datalink(nlink)->insock);
    if(lkm!=NULL)
    {
	lkm->sock->destructor=NULL;
	killsocket(datalink(nlink)->insock);
    }
    datalink(nlink)->instate=STD_NOCON;
    datalink(nlink)->outstate=STD_NOCON;
    datalink(nlink)->insock=0;
    datalink(nlink)->outsock=0;
    return -1;
}


/* link connected */

int connectedlink(int nlink)
{
    char buf[400];
    struct socketnodes *lkm;
    int tmpsock;
    pcontext;
    tmpsock=datalink(nlink)->outsock;
    ssnprintf(tmpsock,lngtxt(512),datalink(nlink)->name,socketnode->sock->sport);
    if (*datalink(nlink)->pass==0)
    {
	strmncpy(datalink(nlink)->pass,randstring(15),sizeof(datalink(nlink)->pass));
	writelink(nlink);
    }
    ssnprintf(tmpsock,lngtxt(513),datalink(nlink)->pass);
    ssnprintf(tmpsock,lngtxt(514),me,me);
#ifdef PARTYCHANNEL
    ssnprintf(tmpsock,lngtxt(515));
#endif
#ifdef INTNET
    joinintnettolink(nlink);
#endif
    ssnprintf(tmpsock,lngtxt(516));
    p_log(LOG_INFO,-1,lngtxt(517),
	nlink,datalink(nlink)->host,datalink(nlink)->port);
    return 0x0;
}

int linkremap(int nlink,int newsocket)
{
    pcontext;
    datalink(nlink)->outsock=newsocket;
    return -1;
}

int errorlink(int nlink,int errn)
{
    pcontext;
    p_log(LOG_ERROR,-1,lngtxt(518),
	nlink,datalink(nlink)->host,datalink(nlink)->port);
    currentsocket->sock->destructor=NULL;
    killsocket(datalink(nlink)->outsock);
    datalink(nlink)->outstate=STD_NOCON;
    return -1;
}

int killedlink(int nlink)
{
    char buf[400];
    pcontext;
    p_log(LOG_WARNING,-1,lngtxt(519),
	nlink,datalink(nlink)->host,datalink(nlink)->port);
    sysparty(lngtxt(520),datalink(nlink)->iam);
    killsocket(datalink(nlink)->outsock);
    datalink(nlink)->outstate=STD_NOCON;
    removetopology(me,datalink(nlink)->iam,lostlink);
    return -1;
}

/* the simplest way of the proxy - the relay handling */

int relay(int nlink)
{
    pcontext;
    if(currentsocket->sock->syssock==datalink(nlink)->outsock)
	writesock(datalink(nlink)->insock,ircbuf);
    if(currentsocket->sock->syssock==datalink(nlink)->insock)
	writesock(datalink(nlink)->outsock,ircbuf);
}

/* process a single link */

int processlink(int nlink, int sock, int state)
{
    struct usernodes *th,*th2;
    struct linknodes *lh;
    struct stringarray *lkm,*pre;
    int rc,rr=0;
    char *pt,*pt2;
    char buf[600];
    char buf1[400];
    char l,k;
    char o[]="->";
    char i[]="<-";
    char r[]="R ";
    char sic[500];
    int last;
    pcontext;
    if (state!=STD_CONN) return 0x0;
    parse();
    pcontext;
    if (!ifcommand("IAM") && !ifcommand(lngtxt(521))) broadcast(nlink); /* if its the IAM message, it does NOT
						get broadcasted */
    pcontext;
    pt=strchr(ircto,'@');
#ifdef PARTYCHANNEL
    if(pt==NULL) pt=strchr(ircto,'*');
#endif
    pcontext;
    if (pt!=NULL) 
    {
	*pt=0;
	pt++;
	if (strmcmp(pt,me)!=0 || (*ircto=='*' && strlen(pt)==1))
	{
	    pcontext;
	    if (ifcommand(lngtxt(522)))
	    {
		rc=checkuser(ircto);
		if (rc==0)
		{
		    ap_snprintf(ircbuf,sizeof(ircbuf),lngtxt(523),me,ircnick,irchost,ircto);
		    broadcast(0);	    
		    return 0x0;
		}
#ifdef PARTYCHANNEL
		ssnprintf(user(rc)->insock,lngtxt(524),ircnick,irchost,user(rc)->nick,irccontent);
#else
		ssnprintf(user(rc)->insock,lngtxt(525),ircnick,irchost,user(rc)->nick,irccontent);
#endif
		return 0x0;
	    }
	    pcontext;
	    if (ifcommand(lngtxt(526)))
	    {
		if(strstr(irccontent,o)!=NULL || strstr(irccontent,i)!=NULL) /* this is a listlink for the topology */
		{
		    pt=strchr(irccontent,'[');
		    if(pt!=NULL)
		    {
			pt++;
			pt2=strstr(pt,"]*");
			if(pt2!=NULL)
			{
			    *pt2=0;
			    strmncpy(sic,pt,sizeof(sic)); /* in sic we got the first linker */
			    pt2++;
			    pt=strstr(pt2,o);
			    if(pt==NULL) pt=strstr(pt2,i);
			    if(pt==NULL) return 0x0;		    
			    pt2=strchr(pt+3,' ');
			    if(pt2!=NULL)
			    {
				*pt2=0;
				if(strstr(pt,o)==pt)
				{
				    pt+=3;
				    addtopology(sic,pt);
				} else {
				    pt+=3;
				    addtopology(pt,sic);
				}
				return 0x0;
			    }
			}
		    }
		}
		pcontext;
#ifdef PARTYCHANNEL
		if(strstr(ircbuf,lngtxt(527))==ircbuf)
		{
		    if(*ircto=='*') /* systemrequest */
		    {
			pt=strchr(irccontent,']');
			if(pt!=NULL)
			{
			    pt+=2;
			    if(*pt=='*')
			    {
				pt2=strchr(pt,'(');
				if (pt2!=NULL)
				{
				    pt+=2;
				    *pt2=0;
				    ap_snprintf(buf,sizeof(buf),lngtxt(528),pt,irchost);
				    if(partyadd(buf)==1)
				    {
					ap_snprintf(buf1,sizeof(buf1),"%s%s\r\n",lngtxt(529),PARTYCHAN);
					ap_snprintf(buf,sizeof(buf),buf1,pt,irchost,pt,irchost);
					th=usernode;
					while(th!=NULL)
					{
					    rc=th->uid;
					    if(user(rc)->instate==STD_CONN && user(rc)->sysmsg==1 && user(rc)->parent==0)
					    {
						writesock(user(rc)->insock,buf);
					    }
					    th=th->next;
					}
					return 0x0;
				    }
				}
			    }
			}
		    }
		}
#endif
		rc=checkuser(ircto);
		if (rc!=0)
		{
		    ssnprintf(user(rc)->insock,lngtxt(530),ircnick,user(rc)->nick,irccontent);
		    return 0x0;
		}
	    }
	}
    }
    pcontext;
    if (ifcommand("IAM"))
    {
	if(*ircnick==0 || strchr(ircnick,' ')!=NULL || strchr(ircnick,'@')!=NULL || strchr(ircnick,'*')!=NULL)
	{
	    sysparty(lngtxt(531));
	    killsocket(sock);
	    datalink(nlink)->instate=STD_NOCON;
	    datalink(nlink)->outstate=STD_NOCON;
	    datalink(nlink)->insock=0;
	    datalink(nlink)->outsock=0;
	    clearlink(nlink);
	    eraselinkini(nlink);
	    return 0x0;
	}
	strmncpy(datalink(nlink)->iam,ircnick,sizeof(datalink(nlink)->iam));
	addtopology(me,datalink(nlink)->iam);
	writelink(nlink);
    }
    pcontext;
    if (ifcommand(lngtxt(532)))
    {
	if (strmcmp(me,ircto)!=0)
	{
	    p_log(LOG_ERROR,-1,lngtxt(533),
	      nlink,datalink(nlink)->host,datalink(nlink)->port);
	    killsocket(sock);
	    removetopology(me,datalink(nlink)->iam,lostlink);
	    datalink(nlink)->instate=STD_NOCON;
	    datalink(nlink)->outstate=STD_NOCON;
	    datalink(nlink)->insock=0;
	    datalink(nlink)->outsock=0;
	    clearlink(nlink);
	    eraselinkini(nlink);
	    return 0x0;	
	}
    }
    pcontext;
    if (ifcommand(lngtxt(534)))
    {
	th=usernode;
	while (th!=NULL) {
	    rc=th->uid;
	    if ((user(rc)->instate==STD_CONN && user(rc)->parent==0 && user(rc)->sysmsg==1) || rc==1)
	    {
		strmncpy(sic,irccontent,sizeof(sic));
#ifdef PARTYCHANNEL
		strmncpy(irccommand,lngtxt(535),sizeof(irccommand));
		strmncpy(ircto,PARTYCHAN,sizeof(ircto));
		if(strlen(ircto+1)<sizeof(ircto))
		    strcat(ircto," ");
#endif
		/* keeping being compatible with earlier versions requires this */
		if(strlen(ircnick)==6 && strstr(ircnick,lngtxt(536))==ircnick)
		{
#ifdef PARTYCHANNEL
		    if(strstr(irccontent,lngtxt(537))!=NULL)
		    {
			pt=strstr(irccontent,lngtxt(538));
			if(pt!=NULL)
			{
			    pt+=5;
			    pt2=strchr(pt,' ');
			    if (pt2!=NULL) *pt2=0;
			    strmncpy(ircnick,pt,sizeof(ircnick));
			    ap_snprintf(buf,sizeof(buf),lngtxt(539),pt,irchost);
			    strcpy(irccommand,"JOIN");
			    strmncpy(irccontent,PARTYCHAN,sizeof(irccontent));
			    *ircto=0;
			    if(rr==0) { partyadd(buf); rr=1; }
			}
		    }
		    if(strstr(irccontent,lngtxt(540))!=NULL)    
		    {
			pt=strstr(irccontent,lngtxt(541));
			if(pt!=NULL)
			{
			    pt+=5;
			    pt2=strchr(pt,' ');
			    if (pt2!=NULL) *pt2=0;
			    strmncpy(ircnick,pt,sizeof(ircnick));
			    ap_snprintf(buf,sizeof(buf),lngtxt(542),pt,irchost);
			    strcpy(irccommand,"PART");
			    strmncpy(irccontent,lngtxt(543),sizeof(irccontent));
			    if(rr==0) { partyremove(buf); rr=1; }
			}
		    }
#endif
		    if(strstr(irccontent,lngtxt(544))==irccontent)
		    {
			ap_snprintf(ircbuf,sizeof(ircbuf),lngtxt(545),me);
			broadcast(0);
			return 0x0;
		    }
		    if(strstr(irccontent,lngtxt(546))==irccontent)    
		    {
			pt=irccontent+11;
			pt2=strchr(pt,')');
			if(pt2!=NULL) *pt2=0;
			removetopology(irchost,pt,lostlink);
			if(pt2!=NULL) *pt2=')';
		    }
		}
#ifdef PARTYCHANNEL
		ap_snprintf(buf,sizeof(buf),lngtxt(547),
						    ircnick,irchost,ircnick,irchost,
						    irccommand,ircto,irccontent);
		if(user(rc)->instate==STD_CONN && user(rc)->parent==0 && user(rc)->sysmsg==1)
		    writesock(user(rc)->insock,buf);						    
#endif	
		ap_snprintf(irccontent,sizeof(irccontent),"%s",sic);
	    }
	    th=th->next;
	}	
	return 0x0;
    }
    pcontext;
    if (ifcommand("BWHO"))
    {
	th=usernode;
	while (th!=NULL) {
	    rc=th->uid;last=0;
	    if (user(rc)->instate!=STD_NOUSE)
	    {
	       if (user(rc)->parent != 0) l='^'; else { l='*';last=1; }
	       if (user(rc)->sysmsg == 0) l='+';
	       if (*user(rc)->host==0) l=' '; else last=0;
	       if(last==1)
      	           ap_snprintf(ircbuf,sizeof(ircbuf),lngtxt(548),me,ircnick,irchost,me,l,user(rc)->login,user(rc)->nick,user(rc)->network,user(rc)->server,user(rc)->user,user(rc)->last);
	       else	 
      	           ap_snprintf(ircbuf,sizeof(ircbuf),lngtxt(549),me,ircnick,irchost,me,l,user(rc)->login,user(rc)->nick,user(rc)->network,user(rc)->server,user(rc)->user);
	       broadcast(0);						    
	    }
	    th=th->next;
	}
	return 0x0;	
    }
    pcontext;
    if (ifcommand(lngtxt(550)))
    {
	lh=linknode;	
	while (lh!=NULL)
	{	
	    rc=lh->uid;
	    l=' ';
	    if (datalink(rc)->type!=0)
	    {
		if (datalink(rc)->type==LI_LINK) 
		{
		    pt=o;
		    if (datalink(rc)->outstate==STD_CONN) l='*';
		}
		if (datalink(rc)->type==LI_ALLOW) 
		{
		    if (datalink(rc)->instate==STD_CONN) l='*';
		    pt=i;
		}
		if (datalink(rc)->type==LI_RELAY) { pt=r; l='*';}
		ap_snprintf(ircbuf,sizeof(ircbuf),lngtxt(551),me,ircnick,irchost,me,l,rc,pt,datalink(rc)->iam,datalink(rc)->host,datalink(rc)->port); 
		broadcast(0);
	    }    
	    lh=lh->next;
	}
	return 0x0;
    }
    pcontext;
#ifdef PARTYCHANNEL
    if (ifcommand(lngtxt(552)))
    {
	strmncpy(partytopic,irccontent,sizeof(partytopic));
	th=usernode;
	while (th!=NULL) {
	    rc=th->uid;
	    if (user(rc)->instate==STD_CONN && user(rc)->parent==0)
	    {
	       ap_snprintf(buf1,sizeof(buf1),"%s%s%s",lngtxt(553),PARTYCHAN,lngtxt(554));
  	       ssnprintf(user(rc)->insock,buf1,
	                       ircnick,irchost,ircnick,irchost,irccontent);
	    }
	    th=th->next;
	}
    }
#endif
#ifdef INTNET
    if (ifcommand(lngtxt(555)))
    {
	pt=strchr(ircbuf,'\r');
	if(pt==NULL) pt=strchr(ircbuf,'\n');
	if(pt!=NULL) *pt=0;
	strmncpy(ircbuf,rtrim(irccontent),sizeof(ircbuf));
	internalinbound(0,nlink);
    }
#endif
    pcontext;
}

/* error handler for the link checking */

int checklinkerror(int nlink,int errn)
{
    checklinkkill(nlink);
    currentsocket->sock->destructor=NULL;
    return 0x0;
}

/* Destructor for the link checking */

int checklinkkill(int nlink,int errn)
{
    int sock;
    pcontext;
    if(datalink(nlink)->type==LI_ALLOW)
	sock=datalink(nlink)->insock;
    else
	sock=datalink(nlink)->outsock;
    sysparty(lngtxt(556),datalink(nlink)->iam);
    p_log(LOG_WARNING,-1,lngtxt(557),nlink);
    killsocket(sock);
    datalink(nlink)->instate=STD_NOCON;
    datalink(nlink)->outstate=STD_NOCON;
    datalink(nlink)->insock=0;
    datalink(nlink)->outsock=0;
    removetopology(me,datalink(nlink)->iam,lostlink);
    return 0x0;
}

/* checking a single linktraffic */

int checklinkdata (int nlink)
{
    pcontext;
    if (datalink(nlink)->type==0) return 0x0;
    if (datalink(nlink)->instate!=STD_CONN && datalink(nlink)->outstate!=STD_CONN) return 0x0;
    if (datalink(nlink)->type==LI_RELAY)
	relay(nlink);
    if (datalink(nlink)->type==LI_LINK)
	processlink(nlink,datalink(nlink)->outsock,datalink(nlink)->outstate);
    if (datalink(nlink)->type==LI_ALLOW)
	processlink(nlink,datalink(nlink)->insock,datalink(nlink)->instate);
}

/* linking a relay */

int linkrelay(int npeer, int rootlink)
{
    int tmpsock;
    struct socketnodes *lkm;
    char *ho;
    char vsl[10];
    int issl=SSL_OFF;
    int nlink;
    int proto=AF_INET;
    pcontext;
    vsl[0]=0;
#ifdef HAVE_SSL
    if(strstr(newpeer(npeer)->server,"S=")==newpeer(npeer)->server)
    {
	issl=SSL_ON;
	ho=newpeer(npeer)->server+2;
	strcpy(vsl,"SSL-");
    } else
#endif
	ho=newpeer(npeer)->server;
    p_log(LOG_INFO,-1,lngtxt(558),
	 vsl,
         newpeer(npeer)->login,
	 datalink(rootlink)->name,
	 newpeer(npeer)->host,
	 newpeer(npeer)->lnkport,
	 ho,
	 newpeer(npeer)->port,
	 newpeer(npeer)->vhost);
    lkm=getpsocketbysock(newpeer(npeer)->insock);
    if(lkm!=NULL)
    {
	lkm->sock->flag=SOC_CONN;
	lkm->sock->param=npeer;
	lkm->sock->constructor=NULL;
	lkm->sock->constructed=NULL;
	lkm->sock->handler=checklinkdata;
	lkm->sock->errorhandler=errorrelaylink;
	lkm->sock->destructor=killrelaylink;
    }
    tmpsock=createsocket(0,ST_CONNECT,0,SGR_NONE,NULL,linkrelayconnected,errorrelaylink,checklinkdata,killrelaylink,linkrelayremap,proto,issl);
    tmpsock=connectto(tmpsock,ho,newpeer(npeer)->port,newpeer(npeer)->vhost);
    if (tmpsock!=0)
    {
	nlink=getnewlink();
	if (nlink==0) {
	    killsocket(tmpsock);
	    p_log(LOG_ERROR,-1,lngtxt(559));
	    return -1;
        }
        datalink(nlink)->type=LI_RELAY;
        datalink(nlink)->instate=STD_CONN;
        datalink(nlink)->insock=newpeer(npeer)->insock;
        strmncpy(datalink(nlink)->iam,newpeer(npeer)->nick,sizeof(datalink(nlink)->iam));
        strmncpy(datalink(nlink)->host,newpeer(npeer)->host,sizeof(datalink(nlink)->host));
        strmncpy(datalink(nlink)->pass,newpeer(npeer)->pass,sizeof(datalink(nlink)->pass));
        strmncpy(datalink(nlink)->name,newpeer(npeer)->name,sizeof(datalink(nlink)->name));
        datalink(nlink)->port=datalink(nlink)->port;
        datalink(nlink)->outstate=STD_CONN;
        datalink(nlink)->outsock=tmpsock;
	if(lkm) lkm->sock->param=nlink;
        lkm=getpsocketbysock(tmpsock);
        if(lkm!=NULL)
        {
    	    lkm->sock->flag=SOC_SYN;
    	    lkm->sock->param=nlink;
        }
	clearpeer(npeer);
	return 0x0;
    }
    p_log(LOG_ERROR,-1,lngtxt(560),newpeer(npeer)->server,newpeer(npeer)->port);
    killsocket(newpeer(npeer)->insock);
    return -1;
}

/* connect a single link */

int connectlink(int nlink)
{
    int tmpsock;
    int proto=AF_INET;
    int issl=SSL_OFF;
    char vsl[10];
    char *ho;
    pcontext;
    vsl[0]=0;
    if (datalink(nlink)->type!=LI_LINK)
	return 0x0;
    if (datalink(nlink)->outstate==STD_CONN)
	return 0x0;
    if (datalink(nlink)->delayed>0)
    {
	datalink(nlink)->delayed-=1;
	return 0x0;
    }
#ifdef HAVE_SSL
    if(strstr(datalink(nlink)->host,"S=")==datalink(nlink)->host)
    {
	ho=datalink(nlink)->host+2;
	issl=SSL_ON;
	strcpy(vsl,"SSL-");
    } else
#endif
	ho=datalink(nlink)->host;
    datalink(nlink)->delayed=0;
    p_log(LOG_INFO,-1,lngtxt(561),vsl,nlink,ho,datalink(nlink)->port);
    /* we got a server and a port */
    tmpsock=createsocket(0,ST_CONNECT,nlink,SGR_NONE,NULL,connectedlink,errorlink,checklinkdata,killedlink,linkremap,proto,issl);
    if (tmpsock!=0)
    {
	datalink(nlink)->outstate=STD_CONN;
	datalink(nlink)->outsock=tmpsock;
    }
    tmpsock=connectto(tmpsock,ho,datalink(nlink)->port,NULL);
    if(tmpsock!=0)
	return 0x1;
    p_log(LOG_ERROR,-1,lngtxt(562),nlink,datalink(nlink)->host,datalink(nlink)->port);
    datalink(nlink)->outstate=STD_NOCON;
    return -1;
}

#endif

/* checking unconnected links */

int checklinks()
{
#ifdef LINKAGE
    struct linknodes *th;
    static int lastlink;
    int i;
    pcontext;
    th=linknode;
    while (th!=NULL)
    {
	i=th->uid;
	if (i != lastlink && connectlink(i)==1) {
	    lastlink=i;
	    return 0x0;
	}
	th=th->next;
    }
    lastlink=-1;
#endif
    return 0x0;
}
