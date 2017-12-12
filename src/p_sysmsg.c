/* $Id: p_sysmsg.c,v 1.4 2005/06/04 18:00:14 hisi Exp $ */
/************************************************************************
 *   psybnc2.3.2, src/p_sysmsg.c
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
static char rcsid[] = "@(#)$Id: p_sysmsg.c,v 1.4 2005/06/04 18:00:14 hisi Exp $";
#endif

#define P_SYSMSG

#include <p_global.h>

/* querying text to the linked network */

int issys=0;

int 
sysparty(char *format,...)
{
    va_list va;
    va_start(va,format);
    ap_vsnprintf(irccontent,sizeof(irccontent),format,va);
    issys=1;
    strcpy(ircto,"$$");
    strmncpy(nulluser->login,lngtxt(841),sizeof(nulluser->login));
    strcpy(nulluser->nick,"SYS");
    strmncpy(irccommand,lngtxt(842),sizeof(irccommand));
    querybounce(0);
    memset(nulluser,0x0,sizeof(struct usert));
    issys=0;
    va_end(va);
    return 0x0;
}

#ifdef PARTYCHANNEL

/* partychannel specifics */

int joinparty(int usern)
{
    char buf1[400];
    struct stringarray *lkm;
    ap_snprintf(buf1,sizeof(buf1),"%s%s\r\n",lngtxt(832),PARTYCHAN);
    ssnprintf(user(usern)->insock,buf1,user(usern)->nick,
				       user(usern)->login,
				       user(usern)->host);
    ap_snprintf(buf1,sizeof(buf1),"%s%s%s",lngtxt(833),PARTYCHAN,lngtxt(834));
    ssnprintf(user(usern)->insock,buf1,user(usern)->nick,partytopic);
    ap_snprintf(buf1,sizeof(buf1),"%s%s :",lngtxt(835),PARTYCHAN);
    ap_snprintf(ircbuf,sizeof(ircbuf),buf1,user(usern)->nick);
    lkm=partyusers;
    while(lkm!=NULL)
    {
	if(strlen(ircbuf)>500)
	{
	    strcat(ircbuf,"\r\n");
	    writesock(user(usern)->insock,ircbuf);
	    ap_snprintf(buf1,sizeof(buf1),"%s%s :",lngtxt(836),PARTYCHAN);
	    ap_snprintf(ircbuf,sizeof(ircbuf),buf1,user(usern)->nick);
	}
	strcat(ircbuf,lkm->entry);
	strcat(ircbuf," ");
	lkm=lkm->next;
    }    							    
    if(strlen(ircbuf)<sizeof(ircbuf-2))
	strcat(ircbuf,"\r\n");
    writesock(user(usern)->insock,ircbuf);
    ap_snprintf(buf1,sizeof(buf1),"%s%s%s",lngtxt(837),PARTYCHAN,lngtxt(838));
    ssnprintf(user(usern)->insock,buf1,user(usern)->nick);
    sysparty(lngtxt(839),user(usern)->login);
    return 0x0;
}

int partyadd(char *xuser)
{
    char buf[200];
    char *pt;
    int rc;
    pt=strchr(xuser,'@');
    if(pt!=NULL) *pt='*';
    rc=partyremove(xuser);
    ap_snprintf(buf,sizeof(buf),"$%s",xuser);
    partyusers=addstring(buf,partyusers);
    return rc;
}

int partyremove(char *xuser)
{
    struct stringarray *lkm,*pre;
    char *pt;
    char buf[200];
    pt=strchr(xuser,'@');
    if(pt!=NULL) *pt='*';
    ap_snprintf(buf,sizeof(buf),"$%s",xuser);
    lkm=partyusers;
    pre=NULL;
    while(lkm!=NULL)
    {
	if (strmcmp(lkm->entry,buf))
	{
	    if(pre!=NULL)
	    {
		pre->next=lkm->next;
	    } else {
		partyusers=lkm->next;
	    }
	    free(lkm->entry);
	    free(lkm);
	    return 0; /* old */
	}
	pre=lkm;
	lkm=lkm->next;
    }
    return 1; /* new */
}

int partylinklost(char *linkname)
{
    struct stringarray *lkm,*pre;
    struct usernodes *th;
    char *pt;
    char buf[200];
    char eb[100];
    pcontext;
    lkm=partyusers;
    pre=NULL;
    if(strmcmp(me,linkname)==1) return 0x0;
    ap_snprintf(eb,sizeof(eb),"*%s",linkname);
    while(lkm!=NULL)
    {
	pt=strstr(lkm->entry,eb);
	if(pt!=NULL)
	{
	    if(strlen(pt)!=strlen(eb)) pt=NULL;
	}
	if(pt!=NULL)
	{
	    if(pre==NULL)
	    { 
		partyusers=lkm->next;
	    } else {
		pre->next=lkm->next;
	    }
	    th=usernode;
	    ap_snprintf(buf,sizeof(buf),lngtxt(840),lkm->entry,lkm->entry+1,linkname);
	    pt=strchr(buf,'*');
	    if(pt!=NULL)
	    {
		pt++;
		pt=strchr(pt,'*');
		if(pt!=NULL) *pt='@';
	    }
	    while(th!=NULL)
	    {
		if(user(th->uid)->instate==STD_CONN && user(th->uid)->parent==0 && user(th->uid)->sysmsg==1)
		    writesock(user(th->uid)->insock,buf);
		th=th->next;
	    }
	    free(lkm->entry);
	    free(lkm);
	    if(pre==NULL)
		lkm=partyusers;
	    else
		lkm=pre->next;
	} else {
	    pre=lkm;
	    lkm=lkm->next;
	}
    }
    pcontext;
    return 0; /* new */
}

#endif

int lostlink(char *namelink)
{
#ifdef PARTYCHANNEL
    partylinklost(namelink);
#endif
#ifdef INTNET
    removeinternal(namelink);
#endif
}

/* sending system notice to a user */

int 
systemnotice(int usernum, char *format, ...)
{
    char buf[1024];
    va_list va;
    va_start(va,format);
    ap_vsnprintf(buf,sizeof(buf),format,va);
    /* removed check for parent. Problem was: User got no Infos on networks */
    if (user(usernum)->instate == STD_CONN) {
       ssnprintf(user(usernum)->insock,lngtxt(843),user(usernum)->nick,buf);
    }
    va_end(va);
    return 0x0;
}

/* noticeing all users */

int 
noticeall (int adminonly, char *format,...)
{
  char buf[1024];
  struct usernodes *th;    
  int cuuser;
  va_list va;
  va_start(va,format);
  ap_vsnprintf(buf,sizeof(buf),format,va);
  th=usernode;
  while (th!=NULL) {
     cuuser=th->uid;
     if((adminonly==RI_ADMIN && user(cuuser)->rights==RI_ADMIN) || adminonly!=RI_ADMIN)
     {
	if (user(cuuser)->instate == STD_CONN && user(cuuser)->parent==0) {
           ssnprintf(user(cuuser)->insock,lngtxt(844),user(cuuser)->nick,buf);
	}
     }     
     th=th->next;
  }   
  va_end(va);
  return 0x0;
}

/* broadcasting a link message to all linked bouncers except #nlink */

int broadcast(int nlink)
{
    struct linknodes *th;    
    int i;
    th=linknode;
    while (th!=NULL)
    {
	i=th->uid;
	if (i!=nlink)
	{
	    if (datalink(i)->type==LI_LINK)
	    {
		if (datalink(i)->outstate==STD_CONN)
		    writesock(datalink(i)->outsock,ircbuf);
	    } else
	    if (datalink(i)->type==LI_ALLOW) {
		if (datalink(i)->instate==STD_CONN)
		    writesock(datalink(i)->insock,ircbuf);
	    }
	}
	th=th->next;
    }
}

/* broadcasting a link message to #nlink */

int broadcasttolink(int nlink)
{
    struct linknodes *th;    
    int i;
    th=linknode;
    while (th!=NULL)
    {
	i=th->uid;
	if (i==nlink)
	{
	    if (datalink(i)->type==LI_LINK)
	    {
		if (datalink(i)->outstate==STD_CONN)
		    writesock(datalink(i)->outsock,ircbuf);
	    } else
	    if (datalink(i)->type==LI_ALLOW) {
		if (datalink(i)->instate==STD_CONN)
		    writesock(datalink(i)->insock,ircbuf);
	    }
	    return 0x0;
	}
	th=th->next;
    }
}

/* query to a user on the bounce */

int querybounce(int usern) 
{
    char *po;
    int rc;
    int all;
    int ext;
    char *pt,*pt1,*pt2;
    struct stringarray *lkm,*par;
    char buf[4096];
    char sic[500];
    char sic2[500];
    int rr=0;
    po = ircto;
    po++;
    ext=1;
    if (*po == '$') {
       all=1;
       rc=1;
       ap_snprintf(ircbuf,sizeof(ircbuf),lngtxt(845),user(usern)->login,me,irccontent);
       broadcast(0);
    } else {
       rc=checkuser(po);
       all=0;
    }   
#ifdef PARTYCHANNEL
    pt=strchr(ircto,'*');
    if (pt!=NULL && *ircto=='$') {
	pt++;
	if(strmcmp(pt,me))
	{
	    pt--;
	    *pt=0;
	    rc=checkuser(po);
	} else {
	    pt--;*pt='@'; /* old way compatible */
	    ap_snprintf(ircbuf,sizeof(ircbuf),lngtxt(846),user(usern)->login,me,ircto+1,irccontent);
	    broadcast(0);
	    return 0x0;
	}
    }
#endif
    while (ext)
    {
      if (rc) {
	if(user(rc)->instate != STD_CONN) {
	  if (all==0) {
	    ap_snprintf(buf,sizeof(buf),lngtxt(847),user(usern)->nick,po);
	    writesock(user(usern)->insock,buf);
	    return 0x0;
	  }
	} else {
	   if (all==1) {
	     if (user(rc)->parent==0) {
#ifdef PARTYCHANNEL
	         /* parsing the systemmessages to JOIN, PART */
		 strmncpy(ircto,PARTYCHAN,sizeof(ircto));
		 strcat(ircto," ");
		 strmncpy(sic,irccontent,sizeof(sic));
		 strmncpy(sic2,user(usern)->login,sizeof(sic2));
	         if(strmcmp(user(usern)->login,lngtxt(848)))
		 {
		     if(strstr(irccontent,lngtxt(849))!=NULL)
		     {
			pt=irccontent+5;
			pt2=strchr(pt,' ');
			if(pt2!=NULL)
			{
			     *pt2=0;
			     strcpy(irccommand,"JOIN");
			     strmncpy(user(usern)->login,pt,sizeof(user(usern)->login));
			     strmncpy(buf,pt,sizeof(buf));
			     if (rr==0) { partyadd(buf);rr=1; }
			}
			strmncpy(irccontent,PARTYCHAN,sizeof(irccontent));
			*ircto=0;
		     }
		     if(strstr(irccontent,lngtxt(850))!=NULL)
		     {
			    pt=irccontent+5;
			    pt2=strchr(pt,' ');
			    if(pt2!=NULL)
			    {
			         *pt2=0;
			         strcpy(irccommand,"PART");
				 strmncpy(user(usern)->login,pt,sizeof(user(usern)->login));
				 strmncpy(buf,pt,sizeof(buf));
				 if (rr==0) { partyremove(buf);rr=1; }
			    }
			    strmncpy(irccontent,lngtxt(851),sizeof(irccontent));
		     }
		     if(strstr(irccontent,lngtxt(852))==irccontent)
		     {
			  lkm=partyusers;
			  pt=irccontent+11;
			  ap_snprintf(buf,sizeof(buf),"*%s",pt);
			  pt=strchr(buf,')');
			  if(pt!=NULL)*pt=0;
			  par=NULL;
			  while(lkm!=NULL)
			  {
				 pt=strstr(lkm->entry,buf);
				 if(pt==NULL) pt=buf;
			         if(pt!=buf && strlen(pt)==strlen(buf))
				 {
		                         if(user(rc)->sysmsg==1)
				         ssnprintf(user(rc)->insock,lngtxt(853),lkm->entry,lkm->entry+1);
					 if(par==NULL)
					 {
					     partyusers=lkm->next;
					 } else {
					     par->next=lkm->next;
					 }
					 free(lkm->entry);
					 free(lkm);
					 if(par==NULL)
					     lkm=partyusers;
					 else
					     lkm=par->next;
				 } else {
				         par=lkm;
				         lkm=lkm->next;
				 }
			   }
		     }
		 }
		 if(usern!=rc)
		 {
		     if(user(rc)->sysmsg==1)
		         ssnprintf(user(rc)->insock,lngtxt(854),user(usern)->login,
		                                             user(usern)->login,me,
		                                             irccommand,ircto,
							     irccontent);
		 }
		 strmncpy(user(usern)->login,sic2,sizeof(user(usern)->login));
		 strmncpy(irccontent,sic,sizeof(irccontent));
#endif
	     }
	   } else {    
   	      ssnprintf(user(rc)->insock,lngtxt(855),user(usern)->login,
	 						   user(usern)->login,
							   me,
							   irccommand,
							   user(rc)->nick,
							   irccontent);
	   }						   
	}
      } else {
	ssnprintf(user(usern)->insock,lngtxt(856),user(usern)->nick,po);
      }  
      if (all==0) ext=0;
      rc++;
      if(rc>=MAXUSER) ext=0;
    }
}

/* query to a dcc bot */

int querybot(int usern) 
{
    struct datalinkt *th;
    char *po;
    int rc;
    int userp;
    if (user(usern)->parent!=0) userp=user(usern)->parent; else userp=usern;
    po = ircto;
    po++;

    th=checkdcc(usern,po);
    if (th==NULL || th->outstate!=STD_CONN)
    {
	ssnprintf(user(usern)->insock,lngtxt(857),user(userp)->nick,po);
	return 0x0;
    }
    ssnprintf(th->outsock,lngtxt(858),irccontent);    
    return 0x0;
}

