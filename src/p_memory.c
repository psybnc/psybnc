/* $Id: p_memory.c,v 1.5 2005/06/04 18:00:14 hisi Exp $ */
/************************************************************************
 *   psybnc2.3.2, src/p_memory.c
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
static char rcsid[] = "@(#)$Id: p_memory.c,v 1.5 2005/06/04 18:00:14 hisi Exp $";
#endif

#define P_MEMORY

#include <p_global.h>

FILE *logm=NULL;

/* malloc-wrapper. No memory will log an error entry and kill the bouncer */

unsigned long *__pmalloc(unsigned long size,char *module,char *function, int line)
{
    unsigned long *rc;
    if (!(rc=(unsigned long *)malloc(size)))
    {
	p_log(LOG_ERROR,-1,lngtxt(602),module,function,line);
	exit(0x0);
    }	
    memset(rc,0x0,size);
#ifdef LOGALLOC
    if(logm==NULL)
	logm=fopen("log/alloc.log","w");
    if(logm)
    {
	fprintf(logm,"%s/%s/%d: alloc(%d)=0x%08x\n",module,function,line,size,rc);
	fflush(logm);
    }
#endif
    return rc;
}

void _pfree(unsigned long *pointer, char *module, char *function, int line)
{
#ifdef LOGALLOC
    if(logm==NULL)
	logm=fopen("log/alloc.log","w");
    if(logm)
    {
	fprintf(logm,"%s/%s/%d: free(0x%08x)\n",module,function,line,pointer);
	fflush(logm);
    }
#endif
    free(pointer);
}

#define free(a) _pfree((void *)a,__FILE__,__FUNCTION__,__LINE__)

/* struct wrappers. Those alloc, delete and return the needed structures */

/* user structure */

struct usert *_lastu=NULL;
int _lastuser=-1;

struct usert *user(int usern)
{
    static struct usernodes *th,*thold;
    if(usern>10000) usern-=10000;
    if(usern==_lastuser && U_CREATE!=1) return _lastu;
    _lastuser=usern;
    if (dummyuser==NULL) {
        dummyuser=(struct usert *)pmalloc(sizeof(struct usert));
    }
    if (nulluser==NULL) {
	nulluser=(struct usert *)pmalloc(sizeof(struct usert));
    }
    if (usernode==NULL) {
	usernode=(struct usernodes *)pmalloc(sizeof(struct usert));
    }
    th=usernode;
    if (usern==0 || usern >MAX_USER) {
       thisuser=NULL;
       _lastu=nulluser;	
       return nulluser;
    }
    while(th!=NULL)
    {
	if (th->uid==usern)
	{
	    if (th->user==NULL) {
		th->user=(struct usert *) pmalloc(sizeof(struct usert));
	    }
	    thisuser=th;
	    _lastu=th->user;
	    return th->user;		
	}
	thold=th;
	th=th->next;
    }
    if (U_CREATE==1)
    {
	U_CREATE=0; /* resetting this */
	thold->next=(struct usernodes *) pmalloc(sizeof(struct usernodes));
	th=thold->next;
	th->user=(struct usert *) pmalloc(sizeof(struct usert));
	th->uid=usern;
	th->next=NULL;
	thisuser=th;
	_lastu=th->user;
	return th->user;
    } else {
	thisuser=NULL;
	_lastu=dummyuser;
	return dummyuser; /* yes, sure. Get it */
    }        
}

struct newpeert *_npeer;
int _lastpeer=-1;

/* this function returns the desired peerstructure */

struct newpeert *newpeer(int usern)
{
    static struct peernodes *th,*thold;
    if(_lastpeer==usern && P_CREATE!=1) return _npeer;
    _lastpeer=usern;
    if (dummypeer==NULL) {
        dummypeer=(struct newpeert *)pmalloc(sizeof(struct newpeert));
    }
    th=peernode;
    while(th!=NULL)
    {
	if (th->uid==usern)
	{
	    if (th->peer==NULL) {
		th->peer=(struct newpeert *) pmalloc(sizeof(struct newpeert));
	    }
	    thispeer=th;
	    _npeer=th->peer;
	    return th->peer;		
	}
	thold=th;
	th=th->next;
    }
    if (P_CREATE==1)
    {
	P_CREATE=0; /* resetting this */
	thold->next=(struct peernodes *) pmalloc(sizeof(struct peernodes));
	th=thold->next;
	th->peer=(struct newpeert *) pmalloc(sizeof(struct newpeert));
	th->uid=usern;
	th->next=NULL;
	thispeer=th;
	_npeer=th->peer;
	return th->peer;
    } else {
	thispeer=NULL;
	_npeer=dummypeer;
	return dummypeer; /* yes, sure. Get it */
    }        
}

struct datalinkt *_nlink;
int _lastlink=-1;

/* this function returns the desired datalink-structure */

struct datalinkt *datalink(int usern)
{
    static struct linknodes *th,*thold;
    if(_lastlink==usern && D_CREATE!=1) return _nlink;
    _lastlink=usern;
    if (dummylink==NULL) {
        dummylink=(struct datalinkt *) pmalloc(sizeof(struct datalinkt));
        memset(dummylink,0x0,sizeof(struct datalinkt));
    }
    th=linknode;
    while(th!=NULL)
    {
	if (th->uid==usern)
	{
	    if (th->link==NULL)
		th->link=(struct datalinkt *) pmalloc(sizeof(struct datalinkt));
	    thislink=th;
	    _nlink=th->link;
	    return th->link;		
	}
	thold=th;
	th=th->next;
    }
    if (D_CREATE==1)
    {
	D_CREATE=0; /* resetting this */
	thold->next=(struct linknodes *) pmalloc(sizeof(struct linknodes));
	th=thold->next;
	th->link=(struct datalinkt *) pmalloc(sizeof(struct datalinkt));
	th->uid=usern;
	th->next=NULL;
	thislink=th;
	_nlink=th->link;
	return th->link;
    } else {
	thislink=NULL;
	_nlink=dummylink;
	return dummylink; /* yes, sure. Get it */
    }        
}

/* clearing structs */

int clearuser(int usern)
{
    struct linknodes *nextdcc;
    struct linknodes *thisdcc;
    _lastuser=-1;
    while (user(usern)->ignores!=NULL)
        user(usern)->ignores=removestring(0,user(usern)->ignores);
    while (user(usern)->bans!=NULL)
        user(usern)->bans=removestring(0,user(usern)->bans);
    while (user(usern)->ops!=NULL)
        user(usern)->ops=removestring(0,user(usern)->ops);
    while (user(usern)->aops!=NULL)
        user(usern)->aops=removestring(0,user(usern)->aops);
    while (user(usern)->askops!=NULL)
        user(usern)->askops=removestring(0,user(usern)->askops);
    while (user(usern)->logs!=NULL)
        user(usern)->logs=removestring(0,user(usern)->logs);
    while (user(usern)->keys!=NULL)
        user(usern)->keys=removestring(0,user(usern)->keys);
#ifdef CRYPT
    while (user(usern)->encrypt!=NULL)
        user(usern)->encrypt=removestring(0,user(usern)->encrypt);
#endif
#ifdef TRANSLATE
    while (user(usern)->translates!=NULL)
        user(usern)->translates=removestring(0,user(usern)->translates);
#endif
#ifdef SCRIPTING
    clearuserscript(usern);
#endif
    removeallchannelsfromuser(usern);
    nextdcc=user(usern)->dcc;
    if (nextdcc !=NULL)
    {	
	thisdcc=nextdcc;
	if (thisdcc->link !=NULL)
	{
	    if (thisdcc->link->outstate==STD_CONN)
		killsocket(thisdcc->link->outsock);
	    if (thisdcc->link!=NULL)
		free(thisdcc->link);
	}
	nextdcc=nextdcc->next;
	free(thisdcc);
    }
#ifdef TRAFFICLOG
    if(user(usern)->trafficlog!=NULL) fclose(user(usern)->trafficlog);
    user(usern)->trafficlog=NULL;
#endif
    memset(user(usern),0x0,sizeof(struct usert));
    if (thisuser!=NULL)
    {
	/* collecting the garbage */
	if (thisuser->user != NULL)
	{
	    free(thisuser->user);
	    if(thisuser)
		thisuser->user=NULL;
	}
        thisuser=NULL;
    }
}

int clearpeer(int peern)
{
    memset(newpeer(peern),0x0,sizeof(struct newpeert));
    _lastpeer=-1;
    if (thispeer!=NULL)
    {
	if (thispeer->peer != NULL)
	{
	    free(thispeer->peer);
	    thispeer->peer=NULL;
	}
    }
}

int clearlink(int peern)
{
    memset(datalink(peern),0x0,sizeof(struct datalinkt));
    _lastlink=-1;
    if (thislink!=NULL)
    {
	if (thislink->link != NULL)
	{
	    free(thislink->link);
	    thislink->link=NULL;
	}
    }
}

/* logging last context */

int p_debug()
{
    p_log(LOG_ERROR,-1,lngtxt(603),ctxt,cfunc,cline);
    return 0x0;
}

int nosignals=0;

/* error handling */

void bus_error(int r)
{
    if(nosignals == 0)
    {
	nosignals=1;
	p_debug();
	p_log(LOG_ERROR,-1,lngtxt(604));
    }
    exit(0x0);
}

void segv_error(int r)
{
    if(nosignals == 0)
    {
	nosignals=1;
	p_debug();
	p_log(LOG_ERROR,-1,lngtxt(605));
    }
    exit(0x0);
}

void fpe_error(int r)
{
    if(nosignals == 0)
    {
	nosignals=1;
	p_debug();
	p_log(LOG_ERROR,-1,lngtxt(606));
    }
    exit(0x0);
}

void term_error(int r)
{
    struct usernodes *th;
    if(nosignals == 1)
	return;
    nosignals=1;
    p_debug();
    p_log(LOG_ERROR,-1,lngtxt(610));
    flushconfig();
    nosignals=0;
    return;
}

void hup_error(int r)
{
    if(nosignals == 0)
    {
	nosignals=1;
	p_debug();
	p_log(LOG_WARNING,-1,lngtxt(611));
	if(*user(1)->login!=0)
	    cmdrehash(1);
	nosignals=0;
    }
    return;
}

void quit_error(int r)
{
    if(nosignals == 0)
    {
	nosignals=1;
	p_debug();
	p_log(LOG_WARNING,-1,lngtxt(612));
	nosignals=0;
    }
    return;
}

#ifdef HAVE_SSL

void logsslstats(SSL *reference)
{
    char sglobal[1024];
    p_log(LOG_INFO,-1, lngtxt(613));
    if(reference==NULL)
	p_log(LOG_INFO,-1,lngtxt(614));
    else
    {
	if(SSL_get_shared_ciphers(reference,sglobal,1023)==NULL)
	    strmncpy(sglobal,lngtxt(615),sizeof(sglobal));
	p_log(LOG_INFO,-1,lngtxt(616),sglobal);
    }
    p_log(LOG_INFO,-1, lngtxt(617),
        SSL_CTX_sess_number(srvctx)+SSL_CTX_sess_number(clnctx));
    p_log(LOG_INFO,-1, lngtxt(618),
        SSL_CTX_sess_connect(srvctx)+SSL_CTX_sess_connect(clnctx));
    p_log(LOG_INFO,-1, lngtxt(619),
        SSL_CTX_sess_connect_good(srvctx)+SSL_CTX_sess_connect_good(clnctx));
#if SSLEAY_VERSION_NUMBER >= 0x0922
    p_log(LOG_INFO,-1, lngtxt(620),
        SSL_CTX_sess_connect_renegotiate(srvctx)+SSL_CTX_sess_connect_renegotiate(clnctx));
#endif
    p_log(LOG_INFO,-1, lngtxt(621),
        SSL_CTX_sess_accept(srvctx)+SSL_CTX_sess_accept(clnctx));
    p_log(LOG_INFO,-1, lngtxt(622),
        SSL_CTX_sess_accept_good(srvctx)+SSL_CTX_sess_accept_good(clnctx));
#if SSLEAY_VERSION_NUMBER >= 0x0922
    p_log(LOG_INFO,-1, lngtxt(623),
        SSL_CTX_sess_accept_renegotiate(srvctx)+SSL_CTX_sess_accept_renegotiate(clnctx));
#endif
    p_log(LOG_INFO,-1, lngtxt(624), SSL_CTX_sess_hits(srvctx)+SSL_CTX_sess_hits(clnctx));
    p_log(LOG_INFO,-1, lngtxt(625), SSL_CTX_sess_misses(srvctx)+SSL_CTX_sess_misses(clnctx));
    p_log(LOG_INFO,-1, lngtxt(626), SSL_CTX_sess_timeouts(srvctx)+SSL_CTX_sess_timeouts(clnctx));
    p_log(LOG_INFO,-1, lngtxt(627));
    return;
}

#endif

void usr1_error(int r)
{
    int i;
    struct socketnodes *lkm,*pre;
    char buf[800];
    char *types[4];
    char *flags[5];
    char *enc[5];
    char *ssle[2];
    int noadv;
#ifdef HAVE_SSL
    char ssha[]="HANDSHAKE";
    SSL_CIPHER *c;
    int bits;
    SSL *reference=NULL;
    char *cname;
#endif
    types[0]="CONNECT";
    types[1]="LISTEN ";
    types[2]="RESOLVE";
    types[3]="UNKNOWN";
    flags[0]="NOUSE  ";
    flags[1]="SYN    ";
    flags[2]="CONN   ";
    flags[3]="ERROR  ";
    ssle[0] ="NONE";
    ssle[1] =" SSL";
    if(nosignals == 1) return;
    nosignals=1;
    if(r!=31337)
	p_log(LOG_INFO,-1,lngtxt(628));
    lkm=socketnode;
    pre=lkm;
    while(lkm!=NULL)
    {
	noadv=0;
	
	if(lkm->sock!=NULL)
	{
#ifdef HAVE_SSL
	    if(lkm->sock->ssl==SSL_ON && lkm->sock->sslfd!=NULL)
	    {
		c=SSL_get_current_cipher(lkm->sock->sslfd);
		SSL_CIPHER_get_bits(c,&bits);
		reference=lkm->sock->sslfd;
		if(bits==0)
		    cname=ssha;
		else
		    cname=(char *)SSL_CIPHER_get_name(c);
		p_log(LOG_INFO,-1,lngtxt(629),
	             lkm->sock->syssock,types[lkm->sock->type],lkm->sock->source,lkm->sock->sport,lkm->sock->dest,lkm->sock->dport,
		     flags[lkm->sock->flag],lkm->sock->bytesin,lkm->sock->bytesout,ssle[lkm->sock->ssl],cname,bits,lkm->sock->since);
	    }
	    else
		p_log(LOG_INFO,-1,lngtxt(630),
	             lkm->sock->syssock,types[lkm->sock->type],lkm->sock->source,lkm->sock->sport,lkm->sock->dest,lkm->sock->dport,
		     flags[lkm->sock->flag],lkm->sock->bytesin,lkm->sock->bytesout,ssle[lkm->sock->ssl],lkm->sock->since);
#else
	    p_log(LOG_INFO,-1,lngtxt(631),
	             lkm->sock->syssock,types[lkm->sock->type],lkm->sock->source,lkm->sock->sport,lkm->sock->dest,lkm->sock->dport,
		     flags[lkm->sock->flag],lkm->sock->bytesin,lkm->sock->bytesout,lkm->sock->since);
#endif		
	    if(lkm->sock->type==SOC_CONN || lkm->sock->flag==SOC_SYN)
	    {
		if(fcntl(lkm->sock->syssock, F_GETFD,0) <0) {
		    p_log(LOG_WARNING,-1,lngtxt(632),lkm->sock->syssock);
		    killsocket(lkm->sock->syssock);
		    lkm=pre->next;
		    noadv=1;
		}
	    }
	}
	if (noadv==0) {pre=lkm; lkm=lkm->next;}
    }
#ifdef HAVE_SSL
    logsslstats(reference);
#endif
#ifndef BLOCKDNS
    dns_stat(-1);
#endif
    if(r!=31337)
	p_log(LOG_INFO,-1,"Done");
    nosignals=0;
    return;
}

void usr2_error(int r)
{
    if(nosignals == 0)
    {
	nosignals=1;
	p_debug();
	p_log(LOG_INFO,-1,lngtxt(633));
	resetconfig();
	readconfig();
	loadusers();
	nosignals=0;
    }
    return;
}

void int_error(int r)
{
    if(nosignals == 0)
    {
	nosignals=1;
	p_debug();
	p_log(LOG_ERROR,-1,lngtxt(634));
    }
    exit(0x0);
}

void ill_error(int r)
{
    if(nosignals == 0)
    {
	nosignals=1;
	p_debug();
	p_log(LOG_WARNING,-1,lngtxt(635));
	nosignals=0;
    }
    return;
}

void kill_error(int r)
{
    if(nosignals == 0)
    {
	nosignals=1;
	p_debug();
	p_log(LOG_ERROR,-1,lngtxt(636));
    }
    exit(0x0);
}

void alrm_error(int r)
{
    return;
}

/* setting the handlers */

int errorhandling()
{
  struct sigaction sv;
  sigemptyset(&sv.sa_mask);
  sv.sa_flags=0;  
  sv.sa_handler = killed;
  sv.sa_handler=bus_error;
  sigaction( SIGBUS, &sv, NULL);
#ifndef LOGALLOC
  sv.sa_handler=segv_error;
  sigaction( SIGSEGV, &sv, NULL);
#endif
  sv.sa_handler=fpe_error;
  sigaction( SIGFPE, &sv, NULL);
  sv.sa_handler=term_error;
  sigaction( SIGTERM, &sv, NULL);
  sv.sa_handler=hup_error;
  sigaction( SIGHUP, &sv, NULL);
  sv.sa_handler=quit_error;
  sigaction( SIGQUIT, &sv, NULL);
  sv.sa_handler=SIG_IGN; /* broken pipes ignoring */
  sigaction( SIGPIPE, &sv, NULL);
  sv.sa_handler=usr1_error;
  sigaction( SIGUSR1, &sv, NULL);
  sv.sa_handler=usr2_error;
  sigaction( SIGUSR2, &sv, NULL);
  sv.sa_handler=int_error;
  sigaction( SIGINT, &sv, NULL);
  sv.sa_handler=ill_error;
  sigaction( SIGILL, &sv, NULL);
  sv.sa_handler=kill_error;
  sigaction( SIGKILL, &sv, NULL);
  sv.sa_handler=alrm_error;
  sigaction( SIGALRM, &sv, NULL);
  umask( ~S_IRUSR & ~S_IWUSR );
  srand( time( NULL) );
  return 0x0;
}
