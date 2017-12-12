/* $Id: p_log.c,v 1.9 2005/06/04 18:00:14 hisi Exp $ */
/************************************************************************
 *   psybnc2.3.2, src/p_log.c
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
static char rcsid[] = "@(#)$Id: p_log.c,v 1.9 2005/06/04 18:00:14 hisi Exp $";
#endif

#define P_LOG

#include <p_global.h>

/* write to log */

int __p_log (char *what, int usern,int level) {
  char tx [20];
  time_t tm;
  time ( &tm );
  strmncpy(tx,ctime( &tm ),sizeof(tx));
#ifndef NOLOG
  if(mainlog==NULL)
     mainlog = fopen(logfile,"a");
  if(level>=LOGLEVEL && mainlog!=NULL)
  {
      fprintf(mainlog,lngtxt(563),tx,what);
      fflush(mainlog);
  }
  if(usern==-1)
    noticeall(RI_ADMIN,lngtxt(564),tx,what);
  else
    systemnotice(usern,lngtxt(565),tx,what);
#endif
  return 0x0;
}

/* log with format */

int 
p_log(int level, int usern, char *format,...)
{
    static char buf[1024];
    va_list va;
    if(inwrite==1) return 0x0; /* if we bug here, we wont iterate */
    inwrite=1;
    va_start(va,format);
    ap_vsnprintf(buf,sizeof(buf),format,va);
    __p_log(buf,usern,level);
    va_end(va);
    inwrite=0;
    return strlen(buf);
}

/* check for privatelog and the existance of salt.h */

int checkforlog ( int usern ) {
  char fname[40];
  FILE *salt;
  FILE *log;
  pcontext;
  strmncpy(fname,lngtxt(566),sizeof(fname));
  if ((log = fopen(fname,"r")) != NULL) {
     fclose(log);
     /* if founder logs on, tell him to move salt.h from the bouncer to a safe place */
     if (usern==1) {
        ssnprintf(user(usern)->insock,lngtxt(567),user(usern)->nick);
     }
  }
  ap_snprintf(fname,sizeof(fname),lngtxt(568),usern);
  if ((log = fopen(fname,"r")) == NULL) {
     pcontext;
     return 0x0;
  }
  fclose(log);
  pcontext;
  return 1;
}

/* write to privatelog */

int privatelog ( int usern ) {
  char tx [20];
  char fname[40];
  char noact='\x01';
  int uid;
  FILE *log;
  time_t tm;
  pcontext;
#ifdef NOLOG
  return 0x0;
#endif
  if(strchr(irccontent,noact)!=NULL) return 0x0; /* dont log actions, ctcps */
  if(strchr(ircfrom,'@')==NULL) return 0x0; /* dont log servermsgs/notices */
  uid=usern;
  if (user(uid)->parent !=0) uid=user(usern)->parent;
  if (strlen(ircnick) == strlen(user(usern)->nick)) {
     if (strstr(ircnick,user(usern)->nick)) return -1;
  }     
  time ( &tm );
  ap_snprintf(fname,sizeof(fname),lngtxt(573),uid);
  strmncpy(tx, ctime( &tm ), sizeof(tx));
  log = fopen(fname,"a");
  if(log!=NULL)
  {
      fprintf(log,lngtxt(574),user(usern)->network,tx,ircfrom,irccontent);
      fclose(log);
  }
  return 0x0;
}

/* play private log */

int playprivatelog ( int usern ) {
  char buf [4200];
  char *pt;
  char fname[40];
  FILE *log;
  int userp;
  pcontext;
  if (user(usern)->parent!=0) userp=user(usern)->parent; else userp=usern;
  if (checkforlog(usern) == 0) return 0x0;
  ap_snprintf(fname,sizeof(fname),lngtxt(575),usern);
  if ((log = fopen(fname,"r"))==NULL) return 0x0;
  ssnprintf(user(usern)->insock,lngtxt(576),user(usern)->nick);
  while(fgets(buf,sizeof(buf),log)) {
      pt=buf;
      if(*pt=='\'') pt++; /* stripping none-network msgs */
      ssnprintf(user(usern)->insock,lngtxt(577),user(userp)->nick,pt);
  }      
  ssnprintf(user(usern)->insock,lngtxt(578),user(userp)->nick);
  fclose(log);
  return 0x0;
}

/* play main log */

int playmainlog ( int usern ) {
  char buf [4200];
  FILE *log;
  int userp;
  pcontext;
  if (user(usern)->parent!=0) userp=user(usern)->parent; else userp=usern;
  if (user(userp)->rights != RI_ADMIN) {
      ssnprintf(user(usern)->insock,lngtxt(579),user(userp)->nick);
      return 0x0;
  }
  if(mainlog!=NULL) fclose(mainlog);
  if ((log = fopen(logfile,"r")) == NULL) {
     mainlog=NULL;
     return 0x0;
  }
  ssnprintf(user(usern)->insock,lngtxt(580),user(userp)->nick);
  while(fgets(buf,sizeof(buf),log)) {
      ssnprintf(user(usern)->insock,lngtxt(581),user(userp)->nick,buf);
  }      
  ssnprintf(user(usern)->insock,lngtxt(582),user(userp)->nick);
  fclose(log);
  mainlog=NULL;
  return 0x0;
}

#ifdef TRAFFICLOG

/* play traffic log */

int playtrafficlog ( int usern ) 
{
  char source[8192];
  char src2[8192];
  char filter[8192];
  char buf [4200];
  char fname[40];
  int last=0;
  char *apt;
  int rc;
  char *pt,*ept;
  int fyy=0,fmm=0,fdd=0,tyy=0,tmm=0,tdd=0,fh=0,fs=0,fm=0,th=0,ts=0,tm=0;
  int xyy,xmm,xdd,xh,xm,xs;
  FILE *log;
  int userp;
  pcontext;
  if (user(usern)->parent!=0) userp=user(usern)->parent; else userp=usern;
  ap_snprintf(fname,sizeof(fname),lngtxt(583),usern);
  if(*irccontent==0) 
  {
      ap_snprintf(buf,sizeof(buf),lngtxt(584),user(userp)->nick);
      writesock(user(usern)->insock,buf);
      return 0x0;
  }
  if(strstr(irccontent,"last")==irccontent)
  {
      last=1;
  }
  pt=strchr(ircbuf,' ');
  if (pt!=NULL)
  {
      pt++;
      ept=pt;
      if(last!=1)
          rc=sscanf(pt,lngtxt(585),
             source,&fmm,&fdd,&fyy,&fh,&fm,&fs,&tmm,&tdd,&tyy,&th,&tm,&ts,filter);
      if (rc==14 || last==1)
      {
	  if(last==1)
	  {
	      filter[0]='*';
	      filter[1]=0;
	  } else {
	      pt=strchr(filter,'\r');
	      if(pt==NULL) pt=strchr(filter,'\n');
	      if(pt!=NULL) *pt=0;
	      if(fyy<80) fyy+=2000; else fyy+=1900;
	      if(tyy<80) tyy+=2000; else tyy+=1900;
	      ucase(source);
	  }
          if(user(usern)->trafficlog!=NULL)
              fclose(user(usern)->trafficlog);
	  if ((log=fopen(fname,"r"))!=NULL) 
	  {
	      if(last==1)
	      {
	          fseek(log,user(usern)->lastlog,SEEK_SET);
	      }
	      if(last==1)
	      {
	         ap_snprintf(buf,sizeof(buf),lngtxt(586));
	      } else {
	         ap_snprintf(buf,sizeof(buf),lngtxt(587),
	               source,source,fyy,fmm,fdd,fh,fm,fs,tyy,tmm,tdd,th,tm,ts,filter);
	      }
              writesock(user(usern)->insock,buf);
              while(fgets(buf,sizeof(buf),log)) {
	          if(last!=1)
	              rc=sscanf(buf,lngtxt(588),&xyy,&xmm,&xdd,&xh,&xm,&xs,src2);
	          if (rc==7 || last==1)	
	          {
		    if(last!=1)
		    {
		      if(xyy<80) xyy+=2000; else xyy+=1900;
		      pt=strchr(src2,':');
		      if(pt!=NULL) *pt=0;
		      ucase(src2);
		      /* is it in range ? */
		      if(xyy>tyy) break;
		      if(xyy==tyy && xmm>tmm) break;
		      if(xyy==tyy && xmm==tmm && xdd>tdd) break;
		      if(xyy==tyy && xmm==tmm && xdd==tdd && xh>th) break;
		      if(xyy==tyy && xmm==tmm && xdd==tdd && xh==th && xm>tm) break;
		      if(xyy==tyy && xmm==tmm && xdd==tdd && xh==th && xm==tm && xs>ts) break;
		    }        
		    if(((xyy>fyy || (xyy==fyy && xmm>fmm) || (xyy==fyy && xmm==fmm && xdd>fdd) ||
		          (xyy==fyy && xmm==fmm && xdd==fdd && xh>fh) ||
		          (xyy==fyy && xmm==fmm && xdd==fdd && xh==fh && xm>fm) ||
		          (xyy==fyy && xmm==fmm && xdd==fdd && xh==fh && xm==fm && xs>=fs)) && 
		          strmncasecmp(source,src2)) || last==1)
		      {
		          pt=strchr(buf,':');
		          if(pt!=NULL)
		          {
		              pt++;
		              pt=strchr(pt,':');
		              if (pt!=NULL)
		              {
		                  pt++;
			          if(*filter=='*' || strstr(pt,filter)!=NULL)
                                      writesock(user(usern)->insock,pt);
		              }
		          }
		      }
	          }
              }
	      if(last==1)
	      {
	          user(usern)->lastlog=ftell(log);
	          writeuser(usern);
	      }      
	      if(last==1)
	          ssnprintf(user(usern)->insock,lngtxt(589));
	      else
	          ssnprintf(user(usern)->insock,lngtxt(590),source,filter);
	      fclose(log);
	      user(usern)->trafficlog=fopen(fname,"a");
	      return 0x0;
	  }
      }
  }
  ssnprintf(user(usern)->insock,lngtxt(591),user(userp)->nick);
  return 0x0;
}

#endif

/* creating backup file */

int oldfile(char *fname)
{
  char oldf[100];
  pcontext;
  ap_snprintf(oldf,sizeof(oldf),lngtxt(592),fname);
  rename (fname,oldf);
  return 0x0;
}

/* backup private log */

int eraseprivatelog ( int usern ) {
  char fname[40];
  int userp;
  pcontext;
  if (user(usern)->parent!=0) userp=user(usern)->parent; else userp=usern;
  ap_snprintf(fname,sizeof(fname),lngtxt(593),usern);
  oldfile(fname);
  ssnprintf(user(usern)->insock,lngtxt(594),user(userp)->nick);
  return 0x0;
}

/* backup main log */

int erasemainlog ( int usern ) {
  char fname[40];
  int userp;
  pcontext;
  if (user(usern)->parent!=0) userp=user(usern)->parent; else userp=usern;
  if (user(userp)->rights != RI_ADMIN) {
      ssnprintf(user(usern)->insock,lngtxt(595),user(userp)->nick);
      return 0x0;
  }
  if(mainlog!=NULL) fclose(mainlog);
  mainlog=NULL;
  oldfile(logfile);
  ssnprintf(user(usern)->insock,lngtxt(596),user(usern)->nick);
  return 0x0;
}

#ifdef TRAFFICLOG

/* erase traffic log */

int erasetrafficlog ( int usern ) {
  char fname[40];
  int userp;
  pcontext;
  if (user(usern)->parent!=0) userp=user(usern)->parent; else userp=usern;
  if(user(usern)->trafficlog!=NULL)
  {
      fclose(user(usern)->trafficlog);
      ap_snprintf(fname,sizeof(fname),lngtxt(597),usern);
      unlink(fname);
      user(usern)->lastlog=0;
      user(usern)->trafficlog=fopen(fname,"a");
      ssnprintf(user(usern)->insock,lngtxt(598),user(usern)->nick);
  } else {
      ssnprintf(user(usern)->insock,lngtxt(599),user(usern)->nick);
  }    
  return 0x0;
}

/* checks, if an entry has to be logged and does, if */

int checklogging(int usern)
{
    static char buf[400];
    char *dest;
    char *filter;
    char *somp;
    int entry=0;
    int tried=0;
    static char fname[40];
    struct tm *xtm;
    time_t tm;
    int iyear;
    struct stringarray *th;
    pcontext;
    time( &tm );
    xtm=localtime(&tm);
    while(1)
    {
	th=user(usern)->logs;
	if(th==NULL) return 0x0;
	somp=getstring(entry,th);
	if(somp==NULL) return 0x0;
	strmncpy(buf,somp,sizeof(buf));
	dest=strchr(buf,';');
	if(dest!=NULL)
	{
	    *dest++=0;
	    filter=buf;
	    ucase(dest);
	    ucase(ircto);
	    if(strmcmp(dest,ircto) || strmcmp(irccommand,LOG_QUIT)) /* dest doesn't match ircto in case of a QUIT event */
	    {
		    if (*filter=='*' || strstr(ircbuf,filter)!=NULL)
		    {
			iyear=xtm->tm_year;
			if(iyear<80) iyear=iyear+2000; /* from 00-79 = 2000-2079 */
			if(iyear<1900) iyear=iyear+1900; /* 100 = 2000, 99 = 1999 */
			if(user(usern)->trafficlog==NULL && tried!=1)
			{
			    ap_snprintf(fname,sizeof(fname),lngtxt(600),usern);
			    user(usern)->trafficlog=fopen(fname,"a");
			    tried=1;
			}
			if(user(usern)->trafficlog!=NULL)
			{
			    if(*ircbuf) /* be sure only to log a filled line.. tokening may clear a irc line */
			    {
				fprintf(user(usern)->trafficlog,lngtxt(601),iyear,xtm->tm_mon,xtm->tm_mday,xtm->tm_hour,xtm->tm_min,xtm->tm_sec,ircto,ircbuf);
				fflush(user(usern)->trafficlog); /* added for 2.3.2.. hopefully not too much processor usage created */
			    }
			}
			return 0x0;
		    }
	    }
	}
	entry++;
    }
}

#endif
