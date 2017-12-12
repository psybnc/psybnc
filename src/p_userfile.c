/* $Id: p_userfile.c,v 1.4 2005/06/04 18:00:14 hisi Exp $ */
/************************************************************************
 *   psybnc2.3.2, src/p_userfile.c
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
static char rcsid[] = "@(#)$Id: p_userfile.c,v 1.4 2005/06/04 18:00:14 hisi Exp $";
#endif

#define P_USERFILE

#include <p_global.h>

/* loading a specific user */

int loaduser (int usernum) {
   struct socketnodes *sno=socketnode;
   char fnmuser[100];
   char buf[100];
   char *pt;
   int rc;
   ap_snprintf(fnmuser,sizeof(fnmuser),lngtxt(904),usernum);
   rc = getini("USER",lngtxt(907),fnmuser);
   if (rc != 0) {
       if (user(usernum)->outstate > 1) {
          writesock(user(usernum)->outsock,lngtxt(905));
          killsocket(user(usernum)->outsock);
          user(usernum)->server[0]=0;
          user(usernum)->outstate=0;
          user(usernum)->outsock=0;
       }
       if (user(usernum)->instate > 1 && user(usernum)->parent==0) {
           ssnprintf(user(usernum)->insock,lngtxt(906),user(usernum)->nick);
	   while(sno=getpsocketbygroup(sno,SGR_USERINBOUND+usernum,-1))
	   {
	       if(sno->sock)
                   killsocket(sno->sock->syssock);
	       sno=socketnode;
	   }
           user(usernum)->host[0]=0;
           user(usernum)->instate=0;
           user(usernum)->insock=0;
       }
       clearuser(usernum);
       return -1;
   }
//   clearuser(usernum);
   U_CREATE=1; /* if not existing, make new */
   nousers=0;
   strmncpy(user(usernum)->login,value,sizeof(user(usernum)->login));
   rc = getini("USER",lngtxt(908),fnmuser);
   if (rc==0)    
      user(usernum)->parent=atoi(value);
   else
      user(usernum)->parent=0;
   rc = getini("USER","USER",fnmuser);
   strmncpy(user(usernum)->user,value,sizeof(user(usernum)->user));    
   rc = getini("USER","NICK",fnmuser);
   if (rc != 0) {
      ap_snprintf(value,sizeof(value),lngtxt(909),usernum);
   }
   strmncpy(user(usernum)->nick,value,sizeof(user(usernum)->nick));
   strmncpy(user(usernum)->wantnick,value,sizeof(user(usernum)->wantnick));
   rc = getini("USER","PASS",fnmuser);
   if (rc != 0) {
      p_log(LOG_ERROR,-1,lngtxt(910),user(usernum)->login);
      strmncpy(value,randstring(16),sizeof(value));
   }
   strcpy(buf,slt1);
   strcat(buf,slt2);
   if (*value!='=')
   {
       if(*value=='+')
       {
	   strmncpy(value,decryptit(value),sizeof(value));
       }
       pt=BLOW_stringencrypt(buf,value);
       ap_snprintf(value,sizeof(value),"=%s",pt);
       free(pt);
       writeini("USER","PASS",fnmuser,value);
       flushconfig();	
   }
   strmncpy(user(usernum)->pass,value,sizeof(user(usernum)->pass));
   rc = getini("USER",lngtxt(911),fnmuser);
   strmncpy(user(usernum)->vhost,value,sizeof(user(usernum)->vhost));
   rc = getini("USER",lngtxt(912),fnmuser);
   strmncpy(user(usernum)->proxy,value,sizeof(user(usernum)->proxy));    
   rc = getini("USER","AWAY",fnmuser);
   strmncpy(user(usernum)->away,value,sizeof(user(usernum)->away));
   rc = getini("USER",lngtxt(913),fnmuser);
   strmncpy(user(usernum)->awaynick,value,sizeof(user(usernum)->awaynick));
   rc = getini("USER",lngtxt(914),fnmuser);
   strmncpy(user(usernum)->leavemsg,value,sizeof(user(usernum)->leavemsg));
   rc = getini("USER",lngtxt(915),fnmuser);
   user(usernum)->channels=NULL;
   if(rc==0)
       convertlastwhois(usernum,value);
   else
       loadchannels(usernum);
   rc = getini("USER",lngtxt(916),fnmuser);
   strmncpy(user(usernum)->network,value,sizeof(user(usernum)->network));
   rc = getini("USER",lngtxt(917),fnmuser);
   if (rc != 0) {
      user(usernum)->rights=RI_USER;
   } else {
      user(usernum)->rights = atoi(value);
   }
#ifdef DYNAMIC
   user(usernum)->quitted=1;
#else
   rc = getini("USER",lngtxt(918),fnmuser);
   if (rc != 0) {
      user(usernum)->quitted=0;
   } else {
      user(usernum)->quitted = atoi(value);
   }
#endif
   rc = getini("USER",lngtxt(919),fnmuser);
   if (rc != 0) {
      user(usernum)->vlink=0;
   } else {
      user(usernum)->vlink = atoi(value);
   }
   rc = getini("USER",lngtxt(920),fnmuser);
   if (rc != 0) {
      user(usernum)->pport=0;
   } else {
      user(usernum)->pport = atoi(value);
   }
   rc = getini("USER",lngtxt(921),fnmuser);
   if (rc != 0) {
      user(usernum)->autogetdcc=0; /* default is off. turn on, if needed */
   } else {
      user(usernum)->autogetdcc = atoi(value);
   }
   rc = getini("USER",lngtxt(922),fnmuser);
   if (rc != 0) {
      user(usernum)->dccenabled=1;
   } else {
      user(usernum)->dccenabled = atoi(value);
   }
   rc = getini("USER",lngtxt(923),fnmuser);
   if (rc != 0) {
      user(usernum)->antiidle=1;
   } else {
      user(usernum)->antiidle = atoi(value);
   }
   rc = getini("USER",lngtxt(924),fnmuser);
   if (rc != 0) {
      user(usernum)->leavequit=0;
   } else {
      user(usernum)->leavequit = atoi(value);
   }
   rc = getini("USER",lngtxt(925),fnmuser);
   if (rc != 0) {
      user(usernum)->autorejoin=0; /* default is 0 now. Set it, if needed */
   } else {
      user(usernum)->autorejoin = atoi(value);
   }
   rc = getini("USER",lngtxt(926),fnmuser);
   if (rc != 0) {
      user(usernum)->sysmsg=1;
   } else {
      user(usernum)->sysmsg = atoi(value);
   }
   rc = getini("USER",lngtxt(927),fnmuser);
   pcontext;
   if (rc != 0) {
      user(usernum)->lastlog=0;
   } else {
      user(usernum)->lastlog = atol(value);
   }
#ifdef HAVE_SSL
   rc = getini("USER","CERT",fnmuser);
   pcontext;
   if (rc == 0) {
      strmncpy(user(usernum)->cert,decryptit(value),sizeof(user(usernum)->cert));
   }
#endif
   pcontext;
   if(user(usernum)->instate==STD_NOUSE) user(usernum)->instate=STD_NOCON;
   if(user(usernum)->outstate==STD_NOUSE) user(usernum)->outstate=STD_NOCON;
   if(user(usernum)->dcc==NULL)
   {
       pcontext;
       user(usernum)->dcc=(struct linknodes *)pmalloc(sizeof(struct linknodes));
       pcontext;
       loaddccs(usernum);
   }
   pcontext;
   if(user(usernum)->bans==NULL)
   {
       ap_snprintf(fnmuser,sizeof(fnmuser),lngtxt(929),usernum);
       user(usernum)->bans=loadlist(fnmuser,user(usernum)->bans);
   }
   pcontext;
   if(user(usernum)->ops==NULL)
   {
       ap_snprintf(fnmuser,sizeof(fnmuser),lngtxt(930),usernum);
       user(usernum)->ops=loadlist(fnmuser,user(usernum)->ops);
   }
   pcontext;
   if(user(usernum)->aops==NULL)
   {
       ap_snprintf(fnmuser,sizeof(fnmuser),lngtxt(931),usernum);
       user(usernum)->aops=loadlist(fnmuser,user(usernum)->aops);
   }
   pcontext;
   if(user(usernum)->askops==NULL)
   {
       ap_snprintf(fnmuser,sizeof(fnmuser),lngtxt(932),usernum);
       user(usernum)->askops=loadlist(fnmuser,user(usernum)->askops);
   }
   pcontext;
   if(user(usernum)->logs==NULL)
   {
       ap_snprintf(fnmuser,sizeof(fnmuser),lngtxt(933),usernum);
       user(usernum)->logs=loadlist(fnmuser,user(usernum)->logs);
   }
   pcontext;
   if(user(usernum)->ignores==NULL)
   {
       ap_snprintf(fnmuser,sizeof(fnmuser),lngtxt(1331),usernum);
       user(usernum)->ignores=loadlist(fnmuser,user(usernum)->ignores);
   }
   pcontext;
#ifdef CRYPT
   if(user(usernum)->encrypt==NULL)
   {
       ap_snprintf(fnmuser,sizeof(fnmuser),lngtxt(934),usernum);
       user(usernum)->encrypt=loadlist(fnmuser,user(usernum)->encrypt);
   }
#endif
#ifdef TRANSLATE
   pcontext;
   if(user(usernum)->translates==NULL)
   {
       ap_snprintf(fnmuser,sizeof(fnmuser),lngtxt(935),usernum);
       user(usernum)->translates=loadlist(fnmuser,user(usernum)->translates);
   }
#endif
#ifdef TRAFFICLOG
   pcontext;
   if(user(usernum)->trafficlog!=NULL)
   {
       fclose(user(usernum)->trafficlog);
   }
   ap_snprintf(buf,sizeof(buf),lngtxt(936),usernum);
   user(usernum)->trafficlog=fopen(buf,"a");
#endif
#ifdef SCRIPTING
   loadscript(usernum);
#endif
   return;
}


/* this writes a user info to the bounce inifile */

int writeuser(int usern)
{
    char iset[8];
    char fname[20];
    char buf[100];
    ap_snprintf(fname,sizeof(fname),lngtxt(937),usern);
    writeini("USER","NICK",fname,user(usern)->nick);
    writeini("USER",lngtxt(938),fname,user(usern)->login);
    writeini("USER","USER",fname,user(usern)->user);
    if(*user(usern)->pass!='=')
    {
	strcpy(buf,slt1);
	strcat(buf,slt2);
	ap_snprintf(user(usern)->pass,sizeof(user(usern)->pass),"=%s",BLOW_stringencrypt(buf,user(usern)->pass));
    }
    writeini("USER","PASS",fname,user(usern)->pass);
    writeini("USER",lngtxt(939),fname,user(usern)->vhost);
    writeini("USER",lngtxt(940),fname,user(usern)->proxy);
    writeini("USER","AWAY",fname,user(usern)->away);
    writeini("USER",lngtxt(941),fname,user(usern)->leavemsg);
    writeini("USER",lngtxt(942),fname,user(usern)->awaynick);
    writeini("USER",lngtxt(943),fname,user(usern)->network);
    ap_snprintf(iset,sizeof(iset),"%d",user(usern)->rights);
    writeini("USER",lngtxt(944),fname,iset);
    ap_snprintf(iset,sizeof(iset),"%d",user(usern)->vlink);
    writeini("USER",lngtxt(945),fname,iset);
    ap_snprintf(iset,sizeof(iset),"%d",user(usern)->pport);
    writeini("USER",lngtxt(946),fname,iset);
    ap_snprintf(iset,sizeof(iset),"%d",user(usern)->parent);
    writeini("USER",lngtxt(947),fname,iset);
    ap_snprintf(iset,sizeof(iset),"%d",user(usern)->quitted);
    writeini("USER",lngtxt(948),fname,iset);
    ap_snprintf(iset,sizeof(iset),"%d",user(usern)->dccenabled);
    writeini("USER",lngtxt(949),fname,iset);
    ap_snprintf(iset,sizeof(iset),"%d",user(usern)->autogetdcc);
    writeini("USER",lngtxt(950),fname,iset);
    ap_snprintf(iset,sizeof(iset),"%d",user(usern)->antiidle);
    writeini("USER",lngtxt(951),fname,iset);
    ap_snprintf(iset,sizeof(iset),"%d",user(usern)->leavequit);
    writeini("USER",lngtxt(952),fname,iset);
    ap_snprintf(iset,sizeof(iset),"%d",user(usern)->autorejoin);
    writeini("USER",lngtxt(953),fname,iset);
    ap_snprintf(iset,sizeof(iset),"%d",user(usern)->sysmsg);
    writeini("USER",lngtxt(954),fname,iset);
    ap_snprintf(iset,sizeof(iset),"%d",user(usern)->lastlog);
    writeini("USER",lngtxt(955),fname,iset);
    writeini("USER",lngtxt(956),fname,NULL); /* erasing lastwhois */
#ifdef HAVE_SSL
    writeini("USER","CERT",fname,cryptit(user(usern)->cert));
#endif
    flushconfig();
}

/* this writes a link info to the link inifile */

int writelink(int linkn)
{
    char iset[8];
    char fname[20];
    char lname[20];
    ap_snprintf(fname,sizeof(fname),"%s",lngtxt(958));
    ap_snprintf(lname,sizeof(lname),lngtxt(959),linkn);
    ap_snprintf(iset,sizeof(iset),"%d",datalink(linkn)->type);
    writeini(lname,"TYPE",fname,iset);
    ap_snprintf(iset,sizeof(iset),"%d",datalink(linkn)->port);
    writeini(lname,"PORT",fname,iset);
    writeini(lname,"NAME",fname,datalink(linkn)->name);
    writeini(lname,"IAM",fname,datalink(linkn)->iam);
    writeini(lname,"HOST",fname,datalink(linkn)->host);
    writeini(lname,"PASS",fname,cryptit(datalink(linkn)->pass));
    ap_snprintf(iset,sizeof(iset),"%d",datalink(linkn)->allowrelay);
    writeini(lname,lngtxt(960),fname,iset);
    flushconfig();
}

/* erase a link entry */

int eraselinkini(int linkn)
{
    char iset[8];
    char fname[20];
    char lname[20];
    ap_snprintf(fname,sizeof(fname),"%s",lngtxt(961));
    ap_snprintf(lname,sizeof(lname),lngtxt(962),linkn);
    writeini(lname,"TYPE",fname,NULL);
    writeini(lname,"PORT",fname,NULL);
    writeini(lname,"NAME",fname,NULL);
    writeini(lname,"IAM",fname,NULL);
    writeini(lname,"HOST",fname,NULL);
    writeini(lname,"PASS",fname,NULL);
    writeini(lname,lngtxt(963),fname,NULL);
    flushconfig();
}

/* load all user structures */

int loadusers(void)
{
   struct usernodes *th;    
   int curuser;
   pcontext;
   curuser = 1;
   p_log(LOG_INFO,-1,lngtxt(964));
   while (curuser <= MAX_USER) 
   {
       loaduser(curuser);
       curuser++;
   }
   if (nousers) p_log(LOG_WARNING,-1,lngtxt(965));
   th=usernode;
   while (th!=NULL) 
   {
       curuser=th->uid;
       checkparents(curuser);
       th=th->next;
   }
   return 0x0;
}

/* load a special link */

int loadlink(int linkn)
{
    int rc;
    char lname[20];
    char buf[400];
    char lfile[]="LINKS";
    if (datalink(linkn)->type==LI_RELAY) return 0x0; /* not resetting dynamic links */
    if (datalink(linkn)->outstate==STD_CONN) {
        p_log(LOG_WARNING,-1,lngtxt(966),datalink(linkn)->host,datalink(linkn)->port);
	killsocket(datalink(linkn)->outsock);
    }
    if (datalink(linkn)->instate==STD_CONN) {
        p_log(LOG_WARNING,-1,lngtxt(967),datalink(linkn)->host,datalink(linkn)->port);
	killsocket(datalink(linkn)->insock);
    }
    ap_snprintf(lname,sizeof(lname),lngtxt(968),linkn);
    rc=getini(lname,"TYPE",lfile);
    clearlink(linkn);
    if (rc!=0) 
	return 0x0;
    D_CREATE=1;
    datalink(linkn)->type=atoi(value);
    rc=getini(lname,"HOST",lfile);
    if (rc!=0) memset(value,0x0,sizeof(value));
    strmncpy(datalink(linkn)->host,value,sizeof(datalink(linkn)->host));
    rc=getini(lname,"PORT",lfile);
    if (rc!=0)
	datalink(linkn)->port=0;
    else
	datalink(linkn)->port=atoi(value);
    rc=getini(lname,lngtxt(969),lfile);
    if (rc!=0)
	datalink(linkn)->allowrelay=0;
    else
	datalink(linkn)->allowrelay=atoi(value);
    rc=getini(lname,"PASS",lfile);
    strmncpy(datalink(linkn)->pass,decryptit(value),sizeof(datalink(linkn)->pass));
    rc=getini(lname,"NAME",lfile);
    if (rc!=0) memset(value,0x0,sizeof(value));
    strmncpy(datalink(linkn)->name,value,sizeof(datalink(linkn)->name));
    rc=getini(lname,"IAM",lfile);
    if (rc!=0) value[0]=0;
    strmncpy(datalink(linkn)->iam,value,sizeof(datalink(linkn)->iam));
    datalink(linkn)->instate=STD_NOCON;
    datalink(linkn)->outstate=STD_NOCON;
    datalink(linkn)->delayed=1;
    return 0x0;
}

/* load all links */

int loadlinks()
{
    int i=1;
    pcontext;
    while (i<=MAXCONN)
    {
	loadlink(i);
	i++;
    }
    return 0x0;
}

/* check if user exists, returns number of userrecord or 0 if not existant */

int checkuser(char *nick)
{
    struct usernodes *th;
    int uind;
    char *pt;
    th=usernode;
    while (th!=NULL)
    {
	uind=th->uid;
	if (strlen(user(uind)->login) == strlen(nick)) {
	    pt=strstr(user(uind)->login,nick);
	    if (pt != NULL) {
	       if (user(uind)->parent!=0) uind=user(uind)->parent;
	       return uind;
	    }
	}
	th=th->next;
    }
    return 0x0;
}

