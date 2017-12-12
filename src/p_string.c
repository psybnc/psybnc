/* $Id: p_string.c,v 1.3 2005/06/04 18:00:14 hisi Exp $ */
/************************************************************************
 *   psybnc2.3.2, src/p_string.c
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
static char rcsid[] = "@(#)$Id: p_string.c,v 1.3 2005/06/04 18:00:14 hisi Exp $";
#endif

#define P_STRING

#include <p_global.h>

char langname[100];

char **language;
int maxindex=0;

/* get a line from the language file */

char *lngtxt(unsigned int msgnum)
{
    static char gbuf[200];
    char *rcs;
    if(msgnum<=maxindex && language!=NULL)
    {
	rcs=language[msgnum];    
    } else {
	ap_snprintf(gbuf,sizeof(gbuf),lngtxt(831),msgnum);
	rcs=gbuf;
    }
    return rcs;;
}

/* string copy with len and zero delimit */

char *strmncpy(char *dest, char *source, unsigned int len)
{
    char bf[strlen(source)+2];
    char *pt;
    if(dest==NULL | source==NULL) return NULL;
    memcpy(&bf[0],source,strlen(source)+1);
    pt=strncpy(dest,bf,len-1);
    if(strlen(source)+1>=len)
	dest[len-1]=0;
    return pt;
}

/* string compare not case sensitive */

int strmncasecmp(char *one,char *two)
{
    if(one==NULL || two==NULL) return 0x0;
    if(strlen(one)==strlen(two))
    {
	char u1[strlen(one)+1];
	char u2[strlen(two)+1];
	strcpy(u1,one);
	strcpy(u2,two);
	ucase(u1);
	ucase(u2);
	if(strstr(u1,u2)==u1) return 0x1;
    }
    return 0x0;
}

/* string compare */

int strmcmp(char *one, char *two)
{
    if(one==NULL || two==NULL) return 0x0;
    if(strlen(one)==strlen(two))
    {
	if (strstr(one,two)==one) return 0x1;
    }
    return 0x0;
}

/* ucase */

int ucase (char *inc)
{
   char *po;
   for (po = inc;*po;po++) *po = toupper( *po );
   return 0x0;
}

/* string compare with a wildcard (case insensitive)
 *
 * this compares a fixed string with a "wildcard" string.
 * this wildcard string can be:
 *
 * *something*
 * *something
 * something*
 * *some?hing*
 * some??ing*
 * *some??ing
 * some*thing
 * *some*thing*
 * some*thing*
 * *some*thing
 */

int strmwildcmp(char *one, char *pattern)
{
    char *pat;
    char *pat2,*pat3,*pat4;
    char uone[2048];
    char upat[1024];
    char *eone;
    char a,e;
    int ln;
    strmncpy(uone,one,sizeof(uone));
    strmncpy(upat,pattern,sizeof(upat));
    ucase(uone);
    ucase(upat);
    pat=upat;eone=uone;
    while(*pat!=0 && *eone!=0)
    {
	if(*pat=='*')
	{
	    pat++;
	    if(*pat==0x0) return 0x1;
	    pat2=strchr(pat,'*');
	    pat3=strchr(pat,'?');
	    if(pat2==NULL || (pat3!=NULL && pat3<pat2)) pat2=pat3;
	    if(pat2==NULL)
	    {
		pat2=strstr(eone,pat);
		pat3=NULL;
		while(pat2!=NULL)
		{
		    pat3=pat2;
		    pat2++;
		    pat2=strstr(pat2,pat);
		}
		if(pat3!=NULL)
		    return strmcmp(pat3,pat);
		else
		    return 0x0;
	    }
	    else 
	    {
		a=*pat2;
		*pat2=0;
		pat3=strstr(eone,pat);
		ln=strlen(pat);
		*pat2=a;
		if(pat3==NULL) return 0x0;
		eone=pat3+ln;
		pat=pat2;
	    }
	} else
	if(*pat=='?')
	{
	    pat++;
	    eone++;
	} 
	else
	{
	    a=*pat;e=*eone;
	    a=toupper(a);e=toupper(e);
	    if(a!=e) return 0x0;
	    pat++;
	    eone++;
	}
    }
    /* if a substring pattern still has '*'  as last entry, ending phrases at the key of *phrase* wont be processed, if the
       content would end with the word 'phrase' */
    if((*pat==0 || (*pat=='*' && *(pat+1)==0)) && *eone==0) return 0x1;
    return 0x0;
}

/* right trimming (no spaces on the right side) */

char *rtrim(char *totrim)
{
    int smlen;
    char *pnt;
    pnt = totrim;
    smlen = strlen(totrim);
    if (smlen>4095) smlen = 4095;
    pnt=pnt+smlen;
    while (*pnt == ' ' || *pnt== 0x0) 
    { 
	*pnt=0x0; 
	if(pnt==totrim)
	    break;
	else
	    pnt--; 
    }
    return totrim;
}

/* replaces given character with another character */

int replace(char *rps,char whatc, char toc)
{
   char *p1;    
   p1=strchr(rps,whatc);
   while (p1) {
      *p1=toc;
      p1++;
      p1=strchr(p1,whatc);
   }
}

char nbr[8192];

/* this routine filters breaks out of a ircstring */

char *nobreak(char *tobreak)
{
    int smlen;
    char *pnt;
    pnt=strchr(tobreak,'\r');
    if(pnt==NULL) pnt=strchr(tobreak,'\n');
    if (pnt != NULL) {
       smlen = pnt-tobreak;
       smlen++;
       if (smlen > 8191) smlen=8191;
       strmncpy(nbr,tobreak,smlen);
       pnt = nbr;
    } else {
      pnt = tobreak;
    }
    return rtrim(pnt);
}


/* random string */

char *randstring(int length)
{
    char *po;
    int i;
    srand(time(NULL)); /* no, this was also done in the demon startup. but still.. */
    po=rbuf;
    if (length >100) length = 100;
    for(i=0;i<length;i++) { *po=(char)(0x61+(rand()&15)); po++; }
    *po=0x0;
    po=rbuf;
    return po;
}

/* string add with memory mapping */

char *strmcat(char *first,char *second)
{
    char *n;
    pcontext;
    n=(char *)pmalloc(strlen(first)+strlen(second)+2);
    strcpy(n,first);
    strcat(n,second);
    pcontext;
    free(first);
    pcontext;
    return n;
}


/* stringarray - support 
 * added for saving the lists to chained user memory */

struct stringarray *addstring(char *toadd, struct stringarray *ltm)
{
    struct stringarray *th;
    if (ltm==NULL) {
	ltm=(struct stringarray *) pmalloc(sizeof(struct stringarray));
	ltm->next=NULL;
	ltm->entry=NULL;
    }
    th=ltm;
    first=th;
    while (th->entry!=NULL) 
    {
	if (th->next==NULL) {
	    th->next=(struct stringarray *) pmalloc(sizeof(struct stringarray));
	    th->next->entry=NULL;
	    th->next->next=NULL;
	}
        th=th->next;
    }
    th->entry=(char *) pmalloc(strlen(toadd)+3);
    strmncpy(th->entry,toadd,strlen(toadd)+2);
    return first; /* returning parent node */
}

char vbuf[650];

char *getstring(int entry, struct stringarray *ltm)
{
    int cnt;
    struct stringarray *th;
    char *tt;
    th=ltm;
    cnt=0;
    while (1)
    {
	if (th==NULL) return NULL;
	if (cnt==entry) {
	    if (th->entry==NULL) return NULL;
	    strmncpy(vbuf,th->entry,sizeof(vbuf));
	    tt=vbuf;
	    return tt;
	}
	th=th->next;
	cnt++;	
    }    
}

struct stringarray *removestring(int entry, struct stringarray *ltm)
{
    int cnt;
    struct stringarray *th;
    struct stringarray *lastth;
    th=ltm;
    lastth=NULL;
    cnt=0;
    while (th!=NULL)
    {
	if (cnt==entry) {
	    if (th==ltm) {
		first=th->next;
		if (th->entry!=NULL) free(th->entry);
		th->entry=NULL;
		free(th);
		return first;
	    } else {
		lastth->next=th->next;
		if(th->entry!=NULL) free(th->entry);
		th->entry=NULL;
		free(th);
		first=ltm;
		return first;
	    }
	} 
	lastth=th;
	th=th->next;
	cnt++;	
    }
    first=ltm;
    return first;
}

/* read all lines from a config listsection to a stringarray */

struct stringarray *loadlist(char *afile,struct stringarray *th)
{
    struct stringarray *first;
    int cnt;
    int i,rc;
    char *pt;
    int fn;
    char buf[100];
    char section[100];
    char fname[100];
    char entry[100];
    pcontext;
    first=th;
    strmncpy(buf,afile,sizeof(buf));
    pt=strchr(buf,'.');
    if(pt!=NULL)
    {
	pcontext;
	*pt=0;
	pt++;
	strmncpy(fname,buf,sizeof(fname));
	strmncpy(section,pt,sizeof(section));
	pt--;
	*pt='.';
	pcontext;
    } else {
	strmncpy(fname,lngtxt(818),sizeof(fname));
	strmncpy(section,afile,sizeof(section));
    }
    pcontext;
    cnt=0;
    for(i=0;i<100;i++)
    {
	ap_snprintf(entry,sizeof(entry),lngtxt(819),i);
	rc=getini(section,entry,fname);
	if(rc==0)
	    first=addstring(value,first);
    }
    pcontext;
    return first;
}


/* write a line, maybe also dupes to a filelist */

struct stringarray *writelist(char *host, char *param, char *afile,struct stringarray *th)
{
    char *pt;
    int fn;
    char buf[2048];
    char section[100];
    char fname[100];
    char entry[100];
    struct stringarray *first;
    strmncpy(buf,afile,sizeof(buf));
    pt=strchr(buf,'.');
    if(pt!=NULL)
    {
	*pt=0;
	pt++;
	strmncpy(fname,buf,sizeof(fname));
	strmncpy(section,pt,sizeof(section));
	pt--;
	*pt='.';
    } else {
	strmncpy(fname,lngtxt(820),sizeof(fname));
	strmncpy(section,afile,sizeof(section));
    }
    strmncpy(entry,lngtxt(821),sizeof(entry));
    fn=countconfentries(section,entry,fname);
    ap_snprintf(entry,sizeof(entry),lngtxt(822),lastfree);
    ap_snprintf(buf,sizeof(buf),lngtxt(823),host,param);
    writeini(section,entry,fname,buf);
    first=addstring(buf,th);
    return first;
}

/* erase a matching entry from a list section with reorganisation */

struct stringarray *eraselist(int entryn, char *afile,struct stringarray *th)
{
    char *pt;
    int fn;
    char fbuf[200];
    char section[100];
    char fname[100];
    char entry[100];
    struct stringarray *ith;
    int cnt,nwcnt;
    cnt=0;
    strmncpy(fbuf,afile,sizeof(fbuf));
    pt=strchr(fbuf,'.');
    if(pt!=NULL)
    {
	*pt=0;
	pt++;
	strmncpy(fname,fbuf,sizeof(fname));
	strmncpy(section,pt,sizeof(section));
	pt--;
	*pt='.';
    } else {
	strmncpy(fname,lngtxt(824),sizeof(fname));
	strmncpy(section,afile,sizeof(section));
    }
    first=removestring(entryn,th);
    ap_snprintf(fbuf,sizeof(fbuf),lngtxt(825),fname,section);
    clearsectionconfig(fbuf); /* clearing the whole configsection */
    ith=first; 
    cnt=0;
    while(ith!=NULL) /* reorganisation */
    {
	if(ith->entry!=NULL)
	{
	    ap_snprintf(entry,sizeof(entry),lngtxt(826),cnt);
	    writeini(section,entry,fname,ith->entry);
	    cnt++;
	}
	ith=ith->next;
    }
    return first;
}

/* list the file */

int liststrings(struct stringarray *th, int usern)
{
    char buf[650];
    char ebuf[650];
    char sbuf[650];
    char gbuf[650];
    char *pt;
    char *pt2;
    int counter;
    int userp;
    if (user(usern)->parent!=0) userp=user(usern)->parent; else userp=usern;
    counter=0;
    while (1)
    {
	pt=getstring(counter,th);
	if (pt==NULL) return 0x0;
	pt2=strchr(pt,';');
	if (pt2!=NULL)
	{
	    *pt2=0;
	    pt2++;
	    if (*pt2=='+')
		pt2=decryptit(pt2);
	    ap_snprintf(buf,sizeof(buf),lngtxt(827),pt,pt2);
	} else
	    strmncpy(buf,pt,sizeof(buf));
	ssnprintf(user(usern)->insock,lngtxt(828),user(userp)->nick,counter,buf);
	counter++;
    }
}

char ibuf[650];

/* check an entry */

int checkstrings(struct stringarray *th)
{
    char *po;
    char *pt;
    char *pc;
    int sze;
    char bc='-';
    char bca=')';
    int counter;
    int match;
    ehost[0]=0;eparm[0]=0;echan[0]=0;
    counter=0;
    while (1)
    {
	pt=getstring(counter,th);
	if (pt==NULL) return 0;
	po=strchr(pt,';');
	if (po != NULL) {
	    *po=0;
	    po++;pc=NULL;
	    if (*po=='+') {
		po=decryptit(po);
	    }
	    if (strchr("#&+!",*po)!=NULL) {
	       pc=strchr(po,' ');
	       if (pc != NULL) {
		  *pc=0;
		  ucase(po);
		  strmncpy(echan,nobreak(po),sizeof(echan));
	          po=pc;
		  po++;
	       } else {
		  ucase(po);
		  pc=po;
		  strmncpy(echan,nobreak(po),sizeof(echan));
		  po=NULL;
	       }
	    }
	    if (pc==NULL) echan[0]=0;
	    if (po==NULL) {
	       eparm[0]=0;
	    } else {
	       strmncpy(eparm,nobreak(po),sizeof(eparm));	
	    }
	    strmncpy(ehost,nobreak(pt),sizeof(ehost));
	    match=1;
	    if (pc) {
	       ucase(ircto);
	       if (strlen(ircto)!=strlen(echan)) {
	           match =0;
	       } else
	       if (strstr(ircto,echan)==NULL) {
	           match =0;
	       }	
	    }
	    if (match==1) {
	       if (*ehost==bc || *ehost==bca) return 1; /* bot */
 	       if (wild_match(ehost,ircfrom)) {
		   return 1;
	       }
	    }
	}
	counter++;
    }
    return 0;
}

/* clear the language structure */

void clearlanguage()
{
    int ln;
    if(language!=NULL)
    {
	for(ln=0;language[ln];ln++)
	    free(language[ln]);
	free(language);
    }
    language=NULL;
    return;
}

/* load a language file */

int loadlanguage(char *langf)
{
    FILE *lfile;
    char lfname[200];
    char ln[1024];
    int rc=0;
    char *pt;
    int msg;
    pcontext;
    clearlanguage();
    ap_snprintf(lfname,sizeof(lfname),"lang/%s.lng",langf);
    maxindex=0;
    langname[0]=0;
    lfile=fopen(lfname,"r");
    if(lfile)
    {
	/* removed dynamic counting of language file records.. processorusage at startup */
	maxindex=1500;
	language=(char **)pmalloc((maxindex+1)*sizeof(char *)); /* i know. */
	while(fgets(ln,sizeof(ln),lfile))
	{
	    pt=strchr(ln,'\r');
	    if(pt==NULL) pt=strchr(ln,'\n');
	    if(pt) *pt=0;
	    if(langname[0]==0)
	    {
		pt=strstr(ln,"desc ");
		if(pt==ln)
		{
		    pt+=5;
		    strmncpy(langname,pt,sizeof(langname));
		}
	    }
	    pt=strstr(ln,"msg");
	    if(pt==ln)
	    {
		pt+=7;
		*pt=0;
		pt-=4;
		msg=atoi(pt);
		pt+=5;
		if(msg<=maxindex)
		{
		    replace(pt,254,'\r');
		    replace(pt,255,'\n');
		    if(language[msg]!=NULL)
			free(language[msg]);
		    language[msg]=(char *)pmalloc(strlen(pt)+3);
		    memcpy(language[msg],pt,strlen(pt)+1);
		}		
	    }
	}		
	fclose(lfile);
	rc=0;
    } else
	rc=-1;
    return rc;
}


