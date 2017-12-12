/* $Id: p_script.c,v 1.7 2005/06/04 18:00:14 hisi Exp $ */
/************************************************************************
 *   psybnc2.3.2, src/p_script.c
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
static char rcsid[] = "@(#)$Id: p_script.c,v 1.7 2005/06/04 18:00:14 hisi Exp $";
#endif

#define P_SCRIPT

#include <p_global.h>

#ifdef SCRIPTING

#define ERR_SUB "Cannot execute program '%s' invoked by %s"

static int forks=0;

#define MAXFORKS 50

/*
 * Scripting for psyBNC
 * the two following routines are the 'core' for scripting usage.
 * addsubtask will be called by startpipe, which will be called
 * from the high level event and alias routines
 *
 * We are using socketpair to create sockets for the in&output
 * to allow the usage of those in the socketdriver.
 * This is 4.3BSDish, so it should work even on proprietry
 * Operating Systems as like SunOs :P
 * STDIN will be used also to read data from. As file that
 * wont be possible, as socketpair it is possible.
 */

struct subtask *previoustask;

/* getting a subtask structure by pid */

struct subtask *getsubtaskbypid(int pid)
{
    struct subtask *sts;
    sts=subtasks;
    previoustask=NULL;
    while(sts!=NULL)
    {
	if(sts->pid==pid) return sts;
	previoustask=sts;
	sts=sts->next;
    }
    return NULL;
}

/* handling the internal structure */

int addsubtask(int uid, int pid, int fdin, int fdout, int fderr, char *program, struct scriptt *script)
{
    struct subtask *st,*pst;
    pcontext;
    st=subtasks;
    pst=st;
    if(st==NULL)
    {
	st=(struct subtask *)pmalloc(sizeof(struct subtask));
	subtasks=st;
    } else {
	while (st!=NULL)
	{
	    pst=st;
	    st=st->next;
	}
	pst->next=(struct subtask *)pmalloc(sizeof(struct subtask));
	st=pst->next;
    }
    st->uid=uid;
    st->pid=pid;
    st->fdin=fdin;
    st->fdout=fdout;
    st->fderr=fderr;            
    st->script=script;
    st->desc=(char *)pmalloc(strlen(program)+1);
    strmncpy(st->desc,program,strlen(program)+1);
    return 0x1;
}

/* the forking pipe for the subtask */

int startpipe(	int usern, 
		char *program,
		struct scriptt *script,
		int(*setenvironment)(int,struct scriptt *),
		int(*inboundhandler)(int),
		int(*outboundhandler)(int),
		int(*errorlog)(int),
		int(*terminated)(int,int),
		int(*destroyed)(int))
{
    int in_fds[2],out_fds[2],err_fds[2];
    int pid,ss1=0,ss2=0,ss3=0,rc;
    int temp_in,temp_out,temp_err;
    int mypid;
    struct socketnodes *sdes;
    pcontext;
    if(forks>MAXFORKS)
    {
	p_log(LOG_ERROR,usern,lngtxt(714),program,user(usern)->login);
	return -1;
    }
    if(mainlog!=NULL)
    {
	fclose(mainlog);
	mainlog=NULL;
    }
    if(socketpair(AF_UNIX,SOCK_STREAM,0,in_fds)<0)
    {
	p_log(LOG_ERROR,usern,ERR_SUB,program,user(usern)->login);
	return -1;    
    }
    if(socketpair(AF_UNIX,SOCK_STREAM,0,out_fds)<0)
    {
	shutdown(in_fds[0],2);
	close(in_fds[0]);
	shutdown(in_fds[1],2);
	close(in_fds[1]);
	p_log(LOG_ERROR,usern,ERR_SUB,program,user(usern)->login);
	return -1;    
    }
    if(socketpair(AF_UNIX,SOCK_STREAM,0,err_fds)<0)
    {
	shutdown(in_fds[0],2);
	close(in_fds[0]);
	shutdown(in_fds[1],2);
	close(in_fds[1]);
	shutdown(out_fds[0],2);
	close(out_fds[0]);
	shutdown(out_fds[1],2);
	close(out_fds[1]);
	p_log(LOG_ERROR,usern,ERR_SUB,program,user(usern)->login);
	return -1;    
    }
    signal(SIGCHLD,SIG_IGN); /* zombie.. */
    if ((pid=fork())<0)
    {
	shutdown(in_fds[0],2);
	close(in_fds[0]);
	shutdown(in_fds[1],2);
	close(in_fds[1]);
	shutdown(out_fds[0],2);
	close(out_fds[0]);
	shutdown(out_fds[1],2);
	close(out_fds[1]);
	shutdown(err_fds[0],2);
	close(err_fds[0]);
	shutdown(err_fds[1],2);
	close(err_fds[1]);
	p_log(LOG_ERROR,usern,ERR_SUB,program,user(usern)->login);
	return -1;
    }    
    if (!pid)
    {
	close(out_fds[0]);
	dup2(out_fds[1],STD_OUT);
	close(out_fds[1]);
	close(in_fds[1]);
	dup2(in_fds[0],STD_IN);
	close(in_fds[0]);
	close(err_fds[0]);
	dup2(err_fds[1],STD_ERR);
	close(err_fds[1]);
	if(setenvironment!=NULL)
	    rc=(*setenvironment)(usern,script);
	sleep(1);	/* one second delay for mothers actions */
	/*
    	 * we call "system", which is normally not called by
	 * demon applications. But in this case the call will
	 * only get the parameters being taken from the user
	 * defined script. So no "bogus" data can be passed
	 * on. Also we only define the environment, there
	 * are no "usergiven" env-parameters.
	 */
	system(program);
/*	write(STD_ERR,lngtxt(715),4);  sending us an EOF  */
	sleep(1); /* wait some time, until mother gets we died. */
	exit(0x0);    
    } else {
	/* creating internal structures */
	close(out_fds[1]);
	close(in_fds[0]);
	close(err_fds[1]);
	ss1=createsocket(in_fds[1],ST_CONNECT,0,SGR_NONE,NULL,NULL,terminated,outboundhandler,destroyed,NULL,AF_INET,SSL_OFF);
	if(ss1>0)
	{
	    ss2=createsocket(out_fds[0],ST_CONNECT,0,SGR_NONE,NULL,NULL,terminated,inboundhandler,destroyed,NULL,AF_INET,SSL_OFF);
	    if(ss2>0)
	    {
		ss3=createsocket(err_fds[0],ST_CONNECT,0,SGR_NONE,NULL,NULL,terminated,errorlog,destroyed,NULL,AF_INET,SSL_OFF);
	    }
	}
	if(ss3<=0)
	{
	    if(ss1>0) killsocket(ss1);
	    if(ss2>0) killsocket(ss2);
	    p_log(LOG_ERROR,usern,lngtxt(716),program,user(usern)->login);
	    return -1;
	}
	forks++;
	/* post fill of the psockets */
	sdes=getpsocketbysock(in_fds[1]);
	if(sdes!=NULL)
	{
	    ap_snprintf(sdes->sock->source,sizeof(sdes->sock->source),lngtxt(717),pid);
	    strcpy(sdes->sock->dest,"0:0");
	    sdes->sock->sport=pid;
	    sdes->sock->dport=usern;
	    sdes->sock->param=pid;
	    sdes->sock->flag=SOC_CONN;
	}
	sdes=getpsocketbysock(out_fds[0]);
	if(sdes!=NULL)
	{
	    ap_snprintf(sdes->sock->source,sizeof(sdes->sock->source),lngtxt(718),pid);
	    strcpy(sdes->sock->dest,"0:0");
	    sdes->sock->sport=pid;
	    sdes->sock->dport=usern;
	    sdes->sock->param=pid;
	    sdes->sock->flag=SOC_CONN;
	}
	sdes=getpsocketbysock(err_fds[0]);
	if(sdes!=NULL)
	{
	    ap_snprintf(sdes->sock->source,sizeof(sdes->sock->source),lngtxt(719),pid);
	    strcpy(sdes->sock->dest,"0:0");
	    sdes->sock->sport=pid;
	    sdes->sock->dport=usern;
	    sdes->sock->param=pid;
	    sdes->sock->flag=SOC_CONN;
	}
	addsubtask(usern,pid,in_fds[1],out_fds[0],err_fds[0],program,script);
    }
    return pid;
}

/* standard routine to kill a task */

int terminatetask(int pid,int err)
{
    struct subtask *st;
    struct scriptt *scr;
    struct socketnodes *ps;
    pcontext;
    if(pid<=0) return -1;
    if(forks>0) forks--;
    st=getsubtaskbypid(pid);
    if(st==NULL) return 0x0;
    if(previoustask==NULL)
	subtasks=st->next;
    else
	previoustask->next=st->next;
    ps=getpsocketbysock(st->fdin);
    if(ps!=NULL)
    {
	ps->sock->destructor=NULL;
	ps->sock->errorhandler=NULL;
	killsocket(st->fdin);
    }
    ps=getpsocketbysock(st->fdout);
    if(ps!=NULL)
    {
	ps->sock->destructor=NULL;
	ps->sock->errorhandler=NULL;
	killsocket(st->fdout);
    }
    ps=getpsocketbysock(st->fderr);
    if(ps!=NULL)
    {
	ps->sock->destructor=NULL;
	ps->sock->errorhandler=NULL;
	killsocket(st->fderr);
    }
    scr=st->script;
    if(scr!=NULL)
    {
	scr->pid=0;
    }
kmd:
    free(st->desc);
    free(st);
    return 0x0;
}

int terminatedest(int pid)
{
    return terminatetask(pid,0);
}


/* clearing the script from a user */

int clearuserscript(int usern)
{
    struct scriptt *sm,*lsm;
    struct subtask *stsk,*ntsk;
    pcontext;
    sm=user(usern)->script;
    while(sm!=NULL)
    {
	lsm=sm->next;
	if(sm->event!=NULL) free(sm->event);
	if(sm->from!=NULL) free(sm->from);
	if(sm->to!=NULL) free(sm->to);
	if(sm->content!=NULL) free(sm->content);
	if(sm->command!=NULL) free(sm->command);
	/* we need to kill all bound tasks */
	stsk=subtasks;
	while(stsk!=NULL)
	{
	    ntsk=stsk->next;
	    if(stsk->script==sm && stsk->pid!=0)
	    {
		terminatetask(stsk->pid,0);
	    }
	    stsk=ntsk;
	}
	free(sm);
	sm=lsm;    
    }
    user(usern)->script=NULL;
    return 0x0;
}

/* get the function from the string */

int getscriptfunction(char *func)
{
    char ffunc[strlen(func)+1];
    strmncpy(ffunc,func,strlen(func)+1);
    ucase(ffunc);
    if(strmcmp(ffunc,lngtxt(720))) return SCR_DCCCHAT;
    if(strmcmp(ffunc,lngtxt(721))) return SCR_DCCSEND;
    if(strmcmp(ffunc,lngtxt(722))) return SCR_SERVER;
    if(strmcmp(ffunc,lngtxt(723))) return SCR_DIALOGUE;
    if(strmcmp(ffunc,"CTCP")) return SCR_CTCP;
    if(strmcmp(ffunc,lngtxt(724))) return SCR_ALIAS;
    return SCR_NONE;
}


/* parsing a single line of a user script */

int processscriptline(int usern, char *line)
{
    char *nothing=lngtxt(1315);
    char *function=nothing,*event=nothing,*from=nothing,*to=nothing,*content=nothing,*command=nothing;
    char *pt,*ept;
    char *ln;
    struct scriptt *uscript,*ouscript=NULL;
    char nline[strlen(line)+1];
    int zncount=0;
    pcontext;
    strmncpy(nline,line,strlen(line)+1);
    ln=nline;
    while(ln!=NULL && zncount!=6)
    {
	while(*ln!=0 && (*ln==' ' || *ln=='\t')) ln++; /* stripping */
	if(*ln=='"')
	{
	    ln++;
	    pt=strchr(ln,'"');
	    if(pt==NULL) return -1;
	    *pt=0;pt++;
	} else {
	    pt=strchr(ln,0x9); /* tabs */
	    if(pt==NULL) pt=strchr(ln,' ');
	    ept=strchr(ln,' ');
	    if(ept!=NULL && ept<pt) pt=ept;
	    if(pt!=NULL && zncount!=5)
		{*pt=0;pt++;}
	}
	switch(zncount)
	{
	    case 0:
		function=ln;
		break;
	    case 1:
		event=ln;
		break;
	    case 2:
		from=ln;
		break;
	    case 3:
		to=ln;
		break;
	    case 4:
		content=ln;
		break;
	    case 5:
		command=ln;
		break;
	}
	zncount++;
	ln=pt;
    }
    /* when we are here, we did not get enough parameters */
    if(zncount!=6) return -1;
    pcontext;
    if(getscriptfunction(function)==SCR_NONE) return -1;
    uscript=user(usern)->script;
    while(uscript!=NULL)
    {
	ouscript=uscript;
	uscript=uscript->next;
    }
    if(ouscript==NULL)
    {
	user(usern)->script=(struct scriptt *)pmalloc(sizeof(struct scriptt));
	uscript=user(usern)->script;
    } else {
	ouscript->next=(struct scriptt *)pmalloc(sizeof(struct scriptt));
	uscript=ouscript->next;
    }
    uscript->event=(char *)pmalloc(strlen(event)+1);
    uscript->from=(char *)pmalloc(strlen(from)+1);
    uscript->to=(char *)pmalloc(strlen(to)+1);
    uscript->content=(char *)pmalloc(strlen(content)+1);
    uscript->command=(char *)pmalloc(strlen(command)+1);
    strmncpy(uscript->event,event,strlen(event)+1);
    strmncpy(uscript->from,from,strlen(from)+1);
    strmncpy(uscript->to,to,strlen(to)+1);
    strmncpy(uscript->content,content,strlen(content)+1);
    strmncpy(uscript->command,command,strlen(command)+1);
    uscript->function=getscriptfunction(function);
    uscript->pid=0;
    return 0x0;
}

/* loading the script of a single user */

int loadscript(int usern)
{
    FILE *script;
    char bf[60];
    char *pt;
    char scline[4096];
    int line=1;
    pcontext;
    if(user(usern)->script!=NULL)
	clearuserscript(usern);
    ap_snprintf(bf,sizeof(bf),"scripts/USER%d.SCRIPT",usern);
    script=fopen(bf,"r");
    if(script==NULL) 
    {
	strcpy(bf,"scripts/DEFAULT.SCRIPT");
	script=fopen(bf,"r");
    }
    if(script==NULL)
	return -1;
    while(fgets(scline,sizeof(scline),script))
    {	
	pt=strchr(scline,'\r');
	if(pt==NULL) pt=strchr(scline,'\n');
	if(pt!=NULL) *pt=0;
	if(strlen(scline)>1 && *scline!=';' && processscriptline(usern, scline)==-1)
	{
	    p_log(LOG_ERROR,usern,"Bogus scripting Line in %s Line %d: %s",bf,line,scline);
	}
	line++;
    }
    fclose(script);
    return 0x0;        	
}

/* get next script entry from script identifier */

struct scriptt *getnextfunctionentry(struct scriptt *msc,int func)
{
    if(msc==NULL) return NULL;
    msc=msc->next;
    while(msc!=NULL)
    {
	if(msc->function==func) return 0x0;
	msc=msc->next;
    }
    return NULL;
}

/* setup standard environments */

int setupstandardenv(int usern,struct scriptt *script)
{
    char fn[20];
    char *pt,*pt2,*pt3;
    char c;
    int icnt=1;
    setenv("NICK",ircnick,1);
    setenv("IDENT",ircident,1);
    setenv("HOST",irchost,1);
    setenv("USER",ircfrom,1);
    setenv("CMD",irccommand,1);
    setenv("TO",ircto,1);
    setenv("CONTENT",irccontent,1);
    ap_snprintf(fn,sizeof(fn),"%d",usern);
    setenv("USERNUMBER",fn,1);
    setenv("USERLOGIN",user(usern)->login,1);
    setenv("USERNICK",user(usern)->nick,1);
    setenv("USERHOST",user(usern)->host,1);
    if(user(usern)->instate==STD_CONN)
	setenv("USERON","1",1);
    else
	setenv("USERON","0",1);
    /* setup P parameters (from ircinput) */
    pt=ircbuf;
    pt2=pt;
    while(pt2!=NULL)
    {
	pt2=strchr(pt,' ');
	if(pt2!=NULL)
	{
	    *pt2=0;
	}
	ap_snprintf(fn,sizeof(fn),"P%d",icnt);
	pt3=strchr(pt,'\r');
	if(pt3==NULL) pt3=strchr(pt,'\n');
	if(pt3!=NULL) 
	{	
	    c=*pt3;
	    *pt3=0;
	}
	setenv(fn,pt,1);
	if(pt3!=NULL) *pt3=c;
	if(pt2!=NULL)
	{
	    *pt2=' ';
	    pt2++;
	    pt=pt2;
	}
	icnt++;
    }
    return 0x0;
}

/* generic script-error routine */

int scripterror(int pid)
{
    struct subtask *stsk;
    char *pt;
    stsk=getsubtaskbypid(pid);
    if(stsk!=NULL)
    {
	pt=strchr(ircbuf,'\r');
	if(pt==NULL) pt=strchr(ircbuf,'\n');
	if(pt!=NULL) *pt=0;
	p_log(LOG_WARNING,stsk->uid,"[%d/%s]: %s",pid,user(stsk->uid)->login,ircbuf);
    }
    return 0x0;
}

/*
 * Now the application specific routines
 * dialogues, aliases etc.
 */

/* dialogue events */

/* setting the environment for the dialogue */

int setdialogueenv(int usern,struct scriptt *script)
{
    setupstandardenv(usern,script);
    setenv("MYNICK",script->to,1);
    return 0x0;
}

/* dialogue input handler */

int dialogueevent(int pid)
{
    struct subtask *stsk;
    pcontext;
    stsk=getsubtaskbypid(pid);
    if(stsk!=NULL)
    {
	if(user(stsk->uid)->instate==STD_CONN)
	{
	    if(stsk->script!=NULL)
	    {
		ssnprintf(user(stsk->uid)->insock,"(%s)!psyBNC@lam3rz.de PRIVMSG %s :%s\r\n",stsk->script->to,user(stsk->uid)->nick,ircbuf);
	    }
	}
    }
    return 0x0;
}

/* start all dialogues when the user sings on */

int startdialogues(int usern)
{
    struct scriptt *msc;
    pcontext;
    msc=user(usern)->script;
    while(msc!=NULL)
    {
	if(msc!=NULL)
	{
	    if(msc->function==SCR_DIALOGUE)
		msc->pid=startpipe(usern,msc->command,msc,setdialogueenv,dialogueevent,dialogueevent,scripterror,terminatetask,terminatedest);
	}
	msc=msc->next;
    }
}

/* stop the dialogues if the user signs off */

int stopdialogues(int usern)
{
    struct scriptt *msc,*pmsc;
    pcontext;
    msc=user(usern)->script;
    while(msc!=NULL)
    {
	pmsc=msc->next;
	if(msc->function==SCR_DIALOGUE && msc->pid!=0)
	{
	    terminatetask(msc->pid,0);
	}
	msc=pmsc;
    }    
    return 0x0;
}

/* send a users input to the dialogue */

int senddialoguequery(int usern)
{
    char *tnick;
    struct subtask *tsk;
    char sndcmd[2048];
    pcontext;
    tnick=strchr(ircto,'(');
    if(tnick!=ircto) return 0x0;
    tnick=strchr(ircto,')');
    if(tnick==NULL) 
    {
	return querydccchat(usern,ircto+1);
    }
    *tnick=0;
    tnick=ircto+1;
    tsk=subtasks;
    while(tsk!=NULL)
    {
	if(tsk->script!=NULL)
	{
	    if(tsk->script->function==SCR_DIALOGUE && strmcmp(tsk->script->to,tnick)==1)
	    {
		ap_snprintf(sndcmd,sizeof(sndcmd),"%s\n",irccontent); /* \n only */
		writesock_STREAM(tsk->fdout,sndcmd,strlen(sndcmd));
		return 0x1;	    
	    }
	}
	tsk=tsk->next;
    }
    return 0x0; /* no match found */
}

/* 
 * server events
 * run normally not backgrounded, and dont get any user input.
 * STD_OUT output goes to the Server, using the USERINBOUND, so that
 * a script could even call psybnc commands.
 * everything else than a command of psybnc goes directly to the
 * server without user notification.
 * STD_IN is being abused as output to the clientside, if it is
 * connected. The script user has to "simulate" valid server
 * messages for the USEROUTBOUND.
 * STD_ERR always goes into the log of psybnc and is logged with
 * pid and usernumber.
 */

/* check a single server event for plausibility */

int checkserverevent(int usern,struct scriptt *scr)
{
    static char pm[200];
    static char pm1[200];
    if(strmcmp(scr->event,irccommand))
    {
	strmncpy(pm,scr->to,sizeof(pm));
	ucase(pm);
	strmncpy(pm1,ircto,sizeof(pm1));
	ucase(pm1);
	if(strmcmp(pm,pm1) || *scr->to=='*')
	{
	    if(strmwildcmp(irccontent,scr->content) || strmcmp(scr->content,"*")==1)
	    {
		if(wild_match(scr->from,ircfrom) || strmcmp(scr->from,"*!*@*"))
		{
		    return 1;
		}
	    }
	}
    }    
    return 0x0;
}

/* setting the environment for the server event */

int setserverenv(int usern,struct scriptt *script)
{
    struct uchannelt *chan;
    struct uchannelusert *chanuser;
    char us[100];
    char ebuf[600];
    int nr=1;
    setupstandardenv(usern,script);
    chan=getuserchannel(usern,rtrim(script->to));
    if(chan!=NULL)
    {
	setenv("CHANNEL",chan->name,1);
	setenv("CHANTOPIC",chan->topic,1);
	setenv("CHANMODE",chan->modes,1);
	if(chan->key[0]!=0)
	    setenv("CHANKEY",chan->key,1);
	if(chan->limit!=0)
	{
	    ap_snprintf(us,sizeof(us),"%d",chan->limit);
	    setenv("CHANLIMIT",us,1);
	}
	chanuser=chan->users;
	while(chanuser!=NULL)
	{
	    ap_snprintf(us,sizeof(us),"CHANUSER%d",nr);
	    ap_snprintf(ebuf,sizeof(ebuf),"%s!%s@%s|%c",chanuser->nick,chanuser->ident,chanuser->host,chanuser->mode[0]);
	    setenv(us,ebuf,1);
	    nr++;
	    chanuser=chanuser->next;
	}
    } else {
	if(strchr("#&+!",*script->to)!=NULL)
	{
	    setenv("CHANNEL",script->to,1);
	}
    }
    return 0x0;
}

/* the running server event or alias, sending data to server */

int servertoserverevent(int pid)
{
    struct subtask *stsk;
    struct socketnodes *ssec;
    ssec=currentsocket;
    stsk=getsubtaskbypid(pid);
    if(stsk!=NULL)
    {
	if(user(stsk->uid)->outstate==STD_CONN)
	{
	    SCRSOCK=1; /* dont inherit the current socket to the users inbound socket */
	    userinbound(stsk->uid); /* simulate as if it came from the client */
	    SCRSOCK=0; /* reset this */
	}    
    }
    return 0x0;
}

/* the running server event or alias, sending data to the client */

int servertoclientevent(int pid)
{
    struct subtask *stsk;
    stsk=getsubtaskbypid(pid);
    if(stsk!=NULL)
    {
	if(user(stsk->uid)->instate==STD_CONN)
	{
	    useroutbound(stsk->uid); /* simulate as if it came from the server */
	}    
    }
    return 0x0;
}

/* the event loop for a user */

int serverevent(int usern)
{
    struct scriptt *scr;
    static char sicbuf[8192];
    strmncpy(sicbuf,ircbuf,sizeof(sicbuf));
    scr=user(usern)->script;
    while(scr!=NULL)
    {
	if(scr->function==SCR_SERVER)
	{
	    if(checkserverevent(usern,scr)==1)
	    {
		scr->pid=startpipe(usern,scr->command,scr,setserverenv,servertoclientevent,servertoserverevent,scripterror,terminatetask,terminatedest);
	    }
	}
	scr=scr->next;
    }
    strmncpy(ircbuf,sicbuf,sizeof(ircbuf));
    parse();
    return 0x0;
}

/*
 * aliases
 * This are commands a user may execute as like he executes psyBNC
 * commands. If an alias ist existent which is similiar to a fixed
 * command used by psyBNC, the alias will replace the internal
 * command.
 * An alias may not stay in the background, if the alias task is
 * still active, when the alias will be triggered by the user,
 * then another alias background task will be raised. If as "TO"
 * parameter a channel is defined, then the channel will be handed
 * over as environment. If BHELP should show proper Infos for the
 * specific command, a COMMAND.TXT file has to be added to the help
 * directory of psybnc.
 * STDIN output will be sent to the clients input (see server events)
 * STDOUT will be sent to the server similiar to server events.
 * STDERR goes to the error log.
 */

/* setting the environment for the alias event */

int setaliasenv(int usern,struct scriptt *script)
{
    struct uchannelt *chan;
    struct uchannelusert *chanuser;
    char us[100];
    char ebuf[600];
    int nr=1;
    setupstandardenv(usern,script);
    chan=getuserchannel(usern,rtrim(ircto));
    if(chan!=NULL)
    {
	setenv("CHANNEL",chan->name,1);
	setenv("CHANTOPIC",chan->topic,1);
	setenv("CHANMODE",chan->modes,1);
	if(chan->key[0]!=0)
	    setenv("CHANKEY",chan->key,1);
	if(chan->limit!=0)
	{
	    ap_snprintf(us,sizeof(us),"%d",chan->limit);
	    setenv("CHANLIMIT",us,1);
	}
	chanuser=chan->users;
	while(chanuser!=NULL)
	{
	    ap_snprintf(us,sizeof(us),"CHANUSER%d",nr);
	    ap_snprintf(ebuf,sizeof(ebuf),"%s!%s@%s|%c",chanuser->nick,chanuser->ident,chanuser->host,chanuser->mode[0]);
	    setenv(us,ebuf,1);
	    nr++;
	    chanuser=chanuser->next;
	}
    } else {
	if(strchr("#&+!",*ircto)!=NULL)
	{
	    setenv("CHANNEL",ircto,1);
	}
    }
    return 0x0;
}

/* check if a command is an alias */

int checkalias(int usern, struct scriptt *scr)
{
    char ebuf[40];
    strmncpy(ebuf,scr->event,sizeof(ebuf));
    ucase(ebuf);
    return strmcmp(ebuf,irccommand);
}

/* the bhelp-extension to aliases */

int aliasbhelp(int usern)
{
    int headout=0;
    struct scriptt *scr;
    char *event,*from;
    char defev[]="Undef.";
    char deffrom[]="*";
    scr=user(usern)->script;
    while(scr!=NULL)
    {
	if(scr->function==SCR_ALIAS)
	{
	    if(headout==0)
	    {
		headout=1;
		ssnprintf(user(usern)->insock,":-psyBNC!psyBNC@lam3rz.de PRIVMSG %s :BHELP User defined Aliases:\n",user(usern)->nick);
	    }
	    event=defev;
	    from=deffrom;
	    if(scr->event!=NULL)
		event=scr->event;
	    if(scr->from!=NULL)
		from=scr->from;
	    ssnprintf(user(usern)->insock,":-psyBNC!psyBNC@lam3rz.de PRIVMSG %s :BHELP   %-15s - %s\n",user(usern)->nick,event,from);
	}
	scr=scr->next;
    }
    return 0x0;
}

/* execute an alias */

int executealias(int usern)
{
    struct scriptt *scr;
    struct subtask *sub;
    scr=user(usern)->script;
    while(scr!=NULL)
    {
	if(scr->function==SCR_ALIAS)
	{
	    if(checkalias(usern,scr)==1)
	    {
		scr->pid=startpipe(usern,scr->command,scr,setaliasenv,servertoclientevent,servertoserverevent,scripterror,terminatetask,terminatedest);
		return 0x1; /* success, alias executed */
	    }
	}
	scr=scr->next;
    }
    return 0x0; /* no alias found */
}

/*
 * DCC Chats and Files
 * DCC Chat requests will be sent to the event, which may
 * background itself. The creation of DCC Chat scriptingevents
 * happens from the DCC Chat Control in p_dcc.c and wants a 
 * pid in return. That pid will, as long as it exists, be
 * fed with the input of the DCC Chat and in return the
 * DCC Chat will be fed with the output of the task while
 * the User on the bouncer will be able to monitor both the
 * in and output.
 * DCC Filesends will be sent to the event, which possibly
 * could automatically answer the Send Request via an answer
 * to the User-Inbound (Triggering the DCCGET command).
 */

/* setting the environment for the dccchat event */

int setdccchatenv(int usern,struct scriptt *script)
{
    setupstandardenv(usern,script);
    setenv("IP",irchost,1);
    return 0x0;
}

/* carring data from the task to the dcc chat */

int tasktochatevent(int pid)
{
    struct dcct *pdcc;
    struct subtask *stsk;
    char netc[20];
    netc[0]=0;
    stsk=getsubtaskbypid(pid);
    if(stsk==NULL) return 0x0;
    pdcc=user(stsk->uid)->pdcc;
    if(user(stsk->uid)->parent!=0)
    {
	ap_snprintf(netc,sizeof(netc),"%s'",user(stsk->uid)->network);
    }
    while(pdcc!=NULL)
    {
	if(pdcc->pid==pid)
	{
	    if(pdcc->type==PDC_CHATFROM)
		writesock(pdcc->sock,ircbuf);
	    if(user(stsk->uid)->instate==STD_CONN)
	    {
		ssnprintf(user(stsk->uid)->insock,":%s(%s!DCCChat@%s PRIVMSG %s :[%d] %s",netc,pdcc->nick,pdcc->host,user(stsk->uid)->nick,pid,ircbuf);  
	    }
	    return 0x0;
	}
	pdcc=pdcc->next;
    }
    return 0x0;
}

/*  the event called by the dcc chat receive */

int dccchatscript(int usern, char *from)
{
    struct scriptt *scr;
    struct subtask *sub;
    int pid=0;
    static char sicbuf[8192];
    strmncpy(sicbuf,ircbuf,sizeof(sicbuf));
    scr=user(usern)->script;
    while(scr!=NULL)
    {
	if(scr->function==SCR_DCCCHAT)
	{
	    if(wild_match(scr->from,from))
	    {
		scr->pid=startpipe(usern,scr->command,scr,setdccchatenv,servertoserverevent,tasktochatevent,scripterror,terminatetask,terminatedest);
		pid=scr->pid; /* success, chat executed */
		break;
	    }
	}
	scr=scr->next;
    }
    strmncpy(ircbuf,sicbuf,sizeof(ircbuf));
    parse();
    return pid; /* no dcc chat found */
}

/* setting the environment for the dccfile event */

int setdccfileenv(int usern,struct scriptt *script)
{
    setupstandardenv(usern,script);
    setenv("IP",irchost,1);
    setenv("FILE",irccontent,1);
    return 0x0;
}

/* the event called by the dcc file receive */

int dccfilescript(int usern, char *from)
{
    struct scriptt *scr;
    struct subtask *sub;
    int pid=0;
    static char sicbuf[8192];
    strmncpy(sicbuf,ircbuf,sizeof(sicbuf));
    scr=user(usern)->script;
    while(scr!=NULL)
    {
	if(scr->function==SCR_DCCSEND)
	{
	    if(wild_match(scr->from,from))
	    {
		scr->pid=startpipe(usern,scr->command,scr,setdccfileenv,servertoserverevent,servertoclientevent,scripterror,terminatetask,terminatedest);
		pid=scr->pid; /* success, chat executed */
		break;
	    }
	}
	scr=scr->next;
    }
    strmncpy(ircbuf,sicbuf,sizeof(ircbuf));
    parse();
    return pid; /* no dcc chat found */
}

/*
 * the ctcp event
 * simple. print all output to the userinbound.
 */

/* set the ctcp environment */

int setctcpenv(int usern, struct scriptt *script)
{
    char x01[]="\x01";
    setupstandardenv(usern,script);
    setenv("X01",x01,1);
    return 0x0;
    
}

/* the event */

int ctcpscript(int usern, char *ctcp)
{
    struct scriptt *scr;
    struct subtask *sub;
    static char sicbuf[8192];
    int pid=0;
    strmncpy(sicbuf,ircbuf,sizeof(sicbuf));
    scr=user(usern)->script;
    while(scr!=NULL)
    {
	if(scr->function==SCR_CTCP)
	{
	    if(strmncasecmp(scr->event,ctcp)==1 && wild_match(scr->from,ircfrom))
	    {
		scr->pid=startpipe(usern,scr->command,scr,setctcpenv,servertoclientevent,servertoserverevent,scripterror,terminatetask,terminatedest);
		pid=scr->pid; /* success, chat executed */
		break;
	    }
	}
	scr=scr->next;
    }
    strmncpy(ircbuf,sicbuf,sizeof(ircbuf));
    parse();
    return pid; /* no dcc chat found */
}

#endif
