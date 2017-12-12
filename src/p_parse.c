/* $Id: p_parse.c,v 1.3 2005/06/04 18:00:14 hisi Exp $ */
/************************************************************************
 *   psybnc2.3.2, src/p_parse.c
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
static char rcsid[] = "@(#)$Id: p_parse.c,v 1.3 2005/06/04 18:00:14 hisi Exp $";
#endif

#define P_PARSE

#include <p_global.h>

/* parsing irc to different buffers */

int generalparse ()
{
   char *p1;
   char *p2;
   int stleng;
   static char secbuf[8191];
   ircserver = 0;
   strmncpy(secbuf,ircbuf,sizeof(secbuf));
   p1=strchr(secbuf,'\r');
   if(p1==NULL) p1=strchr(secbuf,'\n');
   if(p1!=NULL) *p1=0;
   irchost[0]=0;ircident[0]=0;ircnick[0]=0;ircfrom[0]=0;ircto[0]=0;irccommand[0]=0;irccontent[0]=0;
   p1 = secbuf;
   if (p1 == NULL) { return; }
   if (secbuf[0] == ':') { /* from server */
      ircserver = 1;
      p1++;
      p2=strchr(secbuf, ' ');
      if (p2 == NULL) {return;}
      stleng = p2 - p1;
      secbuf[stleng+1] = 0;
      strmncpy(ircfrom,p1,sizeof(ircfrom));
      p2++;
      strmncpy(secbuf,p2,sizeof(secbuf));
      p1 = secbuf;
      p2=strchr(p1,' ');
      if (p2 == NULL)
         return;
      stleng = p2 - p1;
      secbuf[stleng]=0;
      strmncpy(irccommand,p1,sizeof(irccommand));
      p2++;
      strmncpy(secbuf,p2,sizeof(secbuf));
      p1 = secbuf;
      if (*p1 == ':') p1++;
      p2=strchr(secbuf,' ');
      if (p2 == NULL) 
      { 
	 strmncpy(ircto,rtrim(p1),sizeof(ircto));
         return;
      }
      stleng = p2 - p1;
      secbuf[stleng] = 0;
      strmncpy(ircto,rtrim(p1),sizeof(ircto));
      p2++;
      if (*p2 == ':') { 
         p2++;
      } else {
         p1 = strchr(p2,':');
         if (p1 != NULL) { p1++; p2=p1; }	
      }
      strmncpy(irccontent,p2,sizeof(irccontent));
   } else { /* from client */
      p2=strchr(secbuf,' ');
      if (p2 == NULL) 
      {
	 strmncpy(irccommand,secbuf,sizeof(irccommand));
	 return;
      }
      stleng = p2 - p1;
      secbuf[stleng] = 0;
      strmncpy(irccommand,p1,sizeof(irccommand));
      p2++;
      strmncpy(secbuf,p2,sizeof(secbuf));
      p1 = secbuf;
      if(*p1=='"')
      {
          p1++;
	  p2=strchr(p1,'"');
	  if(p2==NULL)
	  {
	      strmncpy(irccontent,p1,sizeof(irccontent));
	  } else {
	      *p2=0;
	      p2++;
	      strmncpy(ircto,rtrim(p1),sizeof(ircto));
	      p1=strchr(p2,':');
	      if(p1!=NULL)
	      {
	          p1++;
		  strmncpy(irccontent,p1,sizeof(irccontent));
	      } else {
	          strmncpy(irccontent,p2,sizeof(irccontent));
	      }
	  }
      } else {
          p2=strchr(p1,':');
          if (p2 == NULL) 
          {
	      strmncpy(irccontent,p1,sizeof(irccontent));
	      return; 
          }
          stleng = p2 - p1;
          secbuf[stleng]=0;
          strmncpy(ircto,rtrim(p1),sizeof(ircto));
          if (*p2 = ':') p2++;
          strmncpy(irccontent,p2,sizeof(irccontent));
      }
   }
   return 0x0;
}

/* final parse of from user/host/nick 
 * - there was a bug up to 2.1.1, crashing on a long nickname from
 *   server a-like inputs.
 *   no compromise possible, but it could crash due to the fact
 *   the nick was 64 bytes big in buffer, but stlen wasnt checked
 *   for boundarys, and could overwrite a string pointer.
 *                             - psychoid 07/19/2000
 */


int parse ()
{
    char *p1;
    char *p2;
    int stlen;
    generalparse();
    if (strlen(ircfrom) > 0) {
       p1 = ircfrom;
       p2 = strchr(ircfrom,'!');
       if (p2 != NULL) {
	  stlen = p2 - p1;
	  stlen++;
	  if(stlen>sizeof(ircnick)) stlen=sizeof(ircnick); /* this one */
	  strmncpy(ircnick,p1,stlen);
	  p2++;
	  p1=strchr(p2,'@');
	  if(p1!=NULL)
	  {
	      *p1=0;
	      strmncpy(ircident,p2,sizeof(ircident));
	      *p1='@';
	  }    
       }
       p1 = ircfrom;
       p2 = strchr(ircfrom,'@');
       if (p2 != NULL) {
          stlen = p2 - p1;
	  stlen = strlen(ircfrom) - stlen;
	  p2++;
	  if(stlen>sizeof(irchost)) stlen=sizeof(irchost); /* and this one */
	  strmncpy(irchost,p2,stlen);
       }
    }
    ucase(irccommand);
    return 0x0;
}

/* boolean result, if ircommand is equal to given param, will be removed */

int ifcommand(char *cmd)
{
    if (*irccommand==0) return 0x0;
    if (cmd==NULL) return 0x0;
    if (strmcmp(cmd,irccommand)) return 0x1;
    return 0x0;
}
