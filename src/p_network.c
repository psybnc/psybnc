/* $Id: p_network.c,v 1.6 2005/06/04 18:00:14 hisi Exp $ */
/************************************************************************
 *   psybnc2.3.2, src/p_network.c
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
static char rcsid[] = "@(#)$Id: p_network.c,v 1.6 2005/06/04 18:00:14 hisi Exp $";
#endif

#define P_NETWORK

#include <p_global.h>

/* check parent users */

int checkparents(int usernum)
{
   int uparent;    
   pcontext;
   if (user(usernum)->parent != 0)
   {
       uparent=user(usernum)->parent;
       if (user(uparent)->instate == STD_NOUSE)
       {
           deluser(usernum);
	   loaduser(usernum);
       } else {
	   strmncpy(user(usernum)->login,user(uparent)->login,sizeof(user(usernum)->login));
	   strmncpy(user(usernum)->user,user(uparent)->user,sizeof(user(usernum)->user));
	   strmncpy(user(usernum)->pass,user(uparent)->pass,sizeof(user(usernum)->pass));
	   user(usernum)->insock=user(uparent)->insock;
	   user(usernum)->instate=user(uparent)->instate;    
	   user(usernum)->rights=user(uparent)->rights;
       }
   }    
}

/* converting a outbounded input string to the nick of the parent */

int parentnick(int usern)
{
    char *pt;
    char *pt1;
    char *pt2;
    int uparent;
    char tmpbuf[8192];
    int intnet=0;
    char nicktmp[70];
#ifdef INTNET
    if(usern>10000)
    {
	/* no change. bugged */
	return 0x0;
    }
    else
#endif
	if (user(usern)->parent==0) return 0x0;
    pcontext;
    uparent=user(usern)->parent;
    if (strmncasecmp(user(usern)->nick,user(uparent)->nick)==1) return 0x0;

    if (ircbuf[0]==':')
    {
	/* new: if we hadnt got a nick yet, set it to the wanted */
	if(user(uparent)->nick[0]==0)
	{
	    if(user(uparent)->wantnick[0]!=0)
		strmncpy(user(uparent)->nick,user(uparent)->wantnick,sizeof(user(uparent)->nick));
	}
	ap_snprintf(nicktmp,sizeof(nicktmp),":%s!",user(usern)->nick);
	pt=strstr(ircbuf,nicktmp);
	if (pt==ircbuf) {
	    pt=ircbuf+strlen(nicktmp);
    	    ap_snprintf(tmpbuf,sizeof(tmpbuf),lngtxt(638),user(uparent)->nick,pt);
	    strmncpy(ircbuf,tmpbuf,sizeof(ircbuf));
	}
    }

    ap_snprintf(nicktmp,sizeof(nicktmp)," %s ",user(usern)->nick);
    if (strlen(ircbuf)<1) return 0x0;
    pt=ircbuf;
    pt2=strchr(pt+1,':');
    while (1)
    {
	pt=strstr(pt+1,nicktmp);
	if (pt==NULL) break;
	if (pt2 && (pt>pt2)) break;
	/* its the nick */
	pt1=pt+strlen(nicktmp);
	pt1--;
	*pt1=0;
	pt1++;
	*pt=0;
        ap_snprintf(tmpbuf,sizeof(tmpbuf),lngtxt(640),ircbuf,user(uparent)->nick,pt1);
	strmncpy(ircbuf,tmpbuf,sizeof(ircbuf));
    }
}

/* this routine checks for a network token - phew */

int checknetwork(int usern)
{
    struct usernodes *th;
    char tmpircbuf[8192];
    int i;
    char *pt;
    char *pt1;
    char *pt2;
    char *pt3;
    pcontext;
    th=usernode;
    strmncpy(tmpircbuf,ircbuf,sizeof(tmpircbuf));
    pt3=strstr(tmpircbuf,"NICK ");
    pt=strchr(tmpircbuf,'\'');
    if (pt!=NULL) 
    {
	pt1=pt;
	pt1--;
	if (*pt1=='*' || *pt1=='!') { /* if its a userflag, try next item */
	    pt1=pt;pt1++;
	    pt=strchr(pt1,'\'');
	    if (pt==NULL) return usern;
	}
	pt1=tmpircbuf+1;
	pt1=strchr(pt1,':');
	if (pt1 != NULL && pt>pt1 && pt3!=tmpircbuf) /* if network specified in content, bye */
	    return usern;
	pt1=pt;
	while (pt1!=tmpircbuf && *pt1!=' ' && *pt1!=':') pt1--;
	if (pt1==tmpircbuf) return usern; /* at start of the buffer ? nah */
	/* in pt1 = start of networktoken, in pt = end of network token */
	*pt1=0; /* buffer part 1 */
	*pt=0; /* buffer part 2 */
	pt2=pt;pt2++;pt1++;
	if (*pt1=='#') pt1++; /* filtering the channels */
#ifdef INTNET
	if(strmncasecmp(pt1,"int")==1)
	{
	    i=usern+10000;
            ap_snprintf(ircbuf,sizeof(ircbuf),lngtxt(641),tmpircbuf,pt2);    
	    return i;
	} else {
#endif
	    while (th!=NULL)
	    {
		i=th->uid;
		if (user(i)->parent==usern)
		{
		    if (strmcmp(user(i)->network,pt1)==1) 
		    {
			/* ok, we got it. its a valid network. Network user is in i
		           pt2 has the second part of the buffer, tmpircbuf is terminated
		           before start of network token. Lets craft the untokened ircbuf now */
		        ap_snprintf(ircbuf,sizeof(ircbuf),lngtxt(642),tmpircbuf,pt2);    
		        /* setting parents inbound flags to child */
		        if(i<10000)
		        {
			    user(i)->insock=user(usern)->insock;
		    	    user(i)->instate=user(usern)->instate;
		        }
		        /* returning the network uid */
		        return i;
		    }
		}
		th=th->next;
	    }
	    /* interesting network token, but not existent, telling the user its unknown */
	    ssnprintf(user(usern)->insock,lngtxt(643),pt1);
	    /* clearing ircbuf */
	    ircbuf[0]=0;
#ifdef INTNET
	}
#endif
    }
    return usern;
}


/* new addtoken-routine for version 2.3.1
   is no bitch anymore, works generally on server input, adds tokens
   relying on the current command. some code parts had been derived from
   version 2.3.
   */

/* first: the hash */

struct thasht {
    char *command;
    int(*handler)(int,char *, char *);
};

char netircfrom[200];
int nuid;
/* the single handler routines */

int netnick(int usern, char *netname, char *myparentnick)
{
    pcontext;
    if(strmncasecmp(user(usern)->nick,ircto)==0 && strmncasecmp(myparentnick,ircto)==0)
	ap_snprintf(ircbuf,sizeof(ircbuf),":%s NICK :%s'%s",netircfrom,netname,ircto);
    else
	ircbuf[0]=0;
    return 0x0;
}

int netjoin(int usern, char *netname, char *myparentnick)
{
    pcontext;
    ap_snprintf(ircbuf,sizeof(ircbuf),":%s JOIN :#%s'%s",netircfrom,netname,ircto);
    return 0x0;
}

int netpart(int usern, char *netname, char *myparentnick)
{
    pcontext;
    ap_snprintf(ircbuf,sizeof(ircbuf),":%s PART #%s'%s :%s",netircfrom,netname,ircto,irccontent);
    return 0x0;
}

int nettopic(int usern, char *netname, char *myparentnick)
{
    pcontext;
    ap_snprintf(ircbuf,sizeof(ircbuf),":%s TOPIC #%s'%s :%s",netircfrom,netname,ircto,irccontent);
    return 0x0;
}

int netquit(int usern, char *netname, char *myparentnick)
{
    pcontext;
    if(strmncasecmp(user(usern)->nick,ircnick)==1)
	ircbuf[0]=0;
    return 0x0;
}

int netprivmsg(int usern, char *netname, char *myparentnick)
{
    pcontext;
    if(strchr("#!+&",ircto[0])!=NULL)
	ap_snprintf(ircbuf,sizeof(ircbuf),":%s %s #%s'%s :%s",netircfrom,irccommand,netname,ircto,irccontent);
    /* hand the ircbuf unchanged, if we query ourself */
    return 0x0;
}

int netkick(int usern, char *netname, char *myparentnick)
{
    char kck[]=" KICK ";
    char tmpircbuf[8192];
    char *knick,*kchan,*eo;
    pcontext;
    kchan=strstr(ircbuf,kck);
    if(kchan)
    {
	kchan+=6;
	knick=strchr(kchan,' ');
	if(knick)
	{
	    *knick=0;
	    knick++;
	    eo=strchr(knick,' ');
	    if(eo)
	    {
		*eo=0;
		if(strmncasecmp(user(usern)->nick,knick)==0)
		    ap_snprintf(tmpircbuf,sizeof(tmpircbuf),":%s KICK #%s'%s %s'%s :%s",netircfrom,netname,kchan,netname,knick,irccontent);
		else
		    ap_snprintf(tmpircbuf,sizeof(tmpircbuf),":%s KICK #%s'%s %s :%s",netircfrom,netname,kchan,knick,irccontent);
		strmncpy(ircbuf,tmpircbuf,sizeof(ircbuf));
	    }
	}
    }     
    return 0x0;
}

int netnames(int usern, char *netname, char *myparentnick)
{
    char *pt,*pt1,*pt2;
    int eflg=0;
    int flg=0;
    char l;
    char tmpircbuf[8192];
    char tmpircbuf2[8192];
    char nameline[8192];
    char *prefixchars;
    pcontext;
    if(strmcmp(netname,"int"))
	prefixchars="@+";
    else
	prefixchars=user(usern)->prefixchars;
    pt1=ircbuf;
    for(l=0;l<4;l++)
    {
	if(pt1)
	{
	     pt1++;
	     pt1=strchr(pt1,' ');
	}
    }
    if(pt1)
    {
	*pt1=0;
	pt1++;
	/* the channel */
	ap_snprintf(tmpircbuf,sizeof(tmpircbuf),"%s #%s'%s",ircbuf,netname,pt1);
	strmncpy(ircbuf,tmpircbuf,sizeof(ircbuf));
    }
    pt=ircbuf;
    pt++;
    pt=strstr(pt," :");
    if (pt!=NULL) 
    {
        pt+=2;
        l=*pt;
        *pt=0;
        strmncpy(tmpircbuf,ircbuf,sizeof(tmpircbuf));
        strmncpy(nameline,ircbuf,sizeof(nameline));
        *pt=l;
        /* now the hard stuff.. we change every Nick */
        while(eflg==0)
        {
    	    pt1=strchr(pt,' ');
	    if (pt1!=NULL) {
    		*pt1=0;l=32;
    	    } else {
    	        eflg=1;l=0;
    	    }	
    	    flg=0;
	    /* the modes */
    	    if (strchr(prefixchars,*pt)!=NULL) {
    		flg=*pt;
    		pt++;
    	    }
    	    /* if its us, we dont parse */
    	    if (strmncasecmp(pt,user(usern)->nick)==1)
    	    {
    		if(flg!=0)
    		    ap_snprintf(tmpircbuf2,sizeof(tmpircbuf2),lngtxt(647),tmpircbuf,flg,myparentnick,l);
    		else
    		    ap_snprintf(tmpircbuf2,sizeof(tmpircbuf2),lngtxt(648),tmpircbuf,myparentnick,l);
	    } else {
    		if (l==0)
     		    ap_snprintf(tmpircbuf2,sizeof(tmpircbuf2),lngtxt(649),tmpircbuf,pt,l);
    		else
    		{
    		    if(flg!=0)
     			ap_snprintf(tmpircbuf2,sizeof(tmpircbuf2),lngtxt(650),tmpircbuf,flg,netname,pt,l);
    		    else
     			ap_snprintf(tmpircbuf2,sizeof(tmpircbuf2),lngtxt(651),tmpircbuf,netname,pt,l);
    		}
    	    }
    	    strmncpy(tmpircbuf,tmpircbuf2,sizeof(tmpircbuf));
    	    /* avoiding the limit exceed */
    	    if (strlen(tmpircbuf)>500)
    	    {
    		/* sending the result up to here */
    		writesock(user(usern)->insock,tmpircbuf);
    		/* building new names line */
    		strmncpy(tmpircbuf,nameline,sizeof(tmpircbuf));
    	    }
    	    if(eflg==0) pt=pt1+1;
	}
        strmncpy(ircbuf,tmpircbuf,sizeof(ircbuf));
    }
    return 0x0;
}

int netwhoischannels(int usern, char *netname, char *myparentnick)
{
    char *pt,*pt2;
    int flg=0;
    char mm[3];
    char tmpircbuf[8192];
    char tmpircbuf2[8192];
    char *prefixchars;
    pcontext;
    if(strmcmp(netname,"int"))
	prefixchars="@+";
    else
	prefixchars=user(usern)->prefixchars;
    ap_snprintf(tmpircbuf,sizeof(tmpircbuf)," :%s",irccontent);
    pt=strstr(ircbuf,tmpircbuf);
    if(pt!=NULL)
    {
        *pt=0; /* cutting */
        /* and now: the channels: */
        pt=irccontent;
        tmpircbuf2[0]=0;
        mm[0]=0;mm[1]=0;mm[2]=0;
        while(pt!=NULL)
        {
	    pt2=strchr(pt,' ');
    	    if(pt2!=NULL)
    	    {
    		*pt2=0;pt2++;
	    }
	    flg=0;
	    if(strchr(prefixchars,*pt)!=NULL)
	    {
	         flg=*pt;
    	         pt++;
	    }
	    if(strlen(tmpircbuf2)+strlen(netname)+6+strlen(pt)<sizeof(tmpircbuf2) && *pt!=' ' && *pt!=0)
	    {
	        if(flg!=0)
	        {
	    	    mm[0]=flg;
		    strcat(tmpircbuf2,mm);
		}
	        strcat(tmpircbuf2,"#");
	        strcat(tmpircbuf2,netname);
	        strcat(tmpircbuf2,"'");
	        strcat(tmpircbuf2,pt);
	        strcat(tmpircbuf2," ");
	    }
	    pt=pt2;
	}
	ap_snprintf(tmpircbuf,sizeof(tmpircbuf),lngtxt(653),ircbuf,tmpircbuf2);
	strmncpy(ircbuf,tmpircbuf,sizeof(ircbuf));
    }
    netdefault(usern,netname,myparentnick);
    return 0x0;
}

int netwho(int usern, char *netname, char *myparentnick)
{
    int eflg;
    char *pt,*pt2;
    char tmpircbuf[8192];
    pcontext;
    pt=ircbuf;
    for (eflg=0;eflg<7;eflg++)
    {
       pt=strchr(pt+1,' ');if (pt==NULL) return 0x0;
    }
    *pt=0;
    pt++;
    pt2=strchr(pt,' ');
    if (pt2==NULL) return 0x0;
    *pt2=0;
    pt2++;
    /* here also, we skip us */
    if (strmncasecmp(user(usern)->nick,pt)==1) {
       ap_snprintf(tmpircbuf,sizeof(tmpircbuf),lngtxt(654),ircbuf,myparentnick,pt2);
    } else {
       ap_snprintf(tmpircbuf,sizeof(tmpircbuf),lngtxt(655),ircbuf,netname,pt,pt2);
    }
    strmncpy(ircbuf,tmpircbuf,sizeof(ircbuf));
    netdefault(usern,netname,myparentnick);
    return 0x0;
}

int netinvite(int usern, char *netname, char *myparentnick)
{
    char tmpircbuf[8192];
    char tmpircbuf2[8192];
    char *pt;
    pcontext;
    ap_snprintf(tmpircbuf,sizeof(tmpircbuf)," :%s",irccontent);
    pt=strstr(ircbuf,tmpircbuf);
    if(pt!=NULL)
    {
        *pt=0;
        ap_snprintf(tmpircbuf2,sizeof(tmpircbuf2),lngtxt(661),ircbuf,netname,irccontent);
        strmncpy(ircbuf,tmpircbuf2,sizeof(ircbuf));
    }
    return 0x0;
}

int netisonuserhost(int usern, char *netname, char *myparentnick)
{
    char ne[200];
    ap_snprintf(ne,sizeof(ne),"%s=",user(usern)->nick);
    if(*irccontent!=0)
    {
	if(strstr(irccontent,ne)==irccontent)
	{
	    ap_snprintf(ircbuf,sizeof(ircbuf),":%s %s %s :%s",netircfrom,irccommand,myparentnick,irccontent);
	} else {
	    ap_snprintf(ircbuf,sizeof(ircbuf),":%s %s %s :%s'%s",netircfrom,irccommand,myparentnick,netname,irccontent);
	}
    }
}

int netdefault(int usern, char *netname, char *myparentnick)
{
    char *pt;
    char checkcmd[200];
    char checknick[64];
    char tmpircbuf[8192];
    int nlen=0;
    char *chantypes;
    pcontext;
    if(strmcmp(netname,"int"))
	chantypes="#";
    else
	chantypes=user(usern)->chantypes;
    if(atoi(irccommand)!=0)
    {
	nlen=strlen(user(usern)->nick);
	strmncpy(checknick,user(usern)->nick,sizeof(checknick));
	ap_snprintf(checkcmd,sizeof(checkcmd)," %s %s",irccommand,user(usern)->nick);
	pt=strstr(ircbuf,checkcmd);
	if(pt==NULL)
	{
	    nlen=strlen(myparentnick);
	    strmncpy(checknick,myparentnick,sizeof(checknick));
	    ap_snprintf(checkcmd,sizeof(checkcmd)," %s %s",irccommand,myparentnick);
	    pt=strstr(ircbuf,checkcmd);
	}
	if(pt)
	{
	    pt+=strlen(irccommand);
	    pt++;
	    *pt=0;
	    pt++;
	    pt+=nlen;
	    pt++;
	    if(strchr(chantypes,*pt)!=NULL) /* channel */
		ap_snprintf(tmpircbuf,sizeof(tmpircbuf),"%s %s #%s'%s",ircbuf,myparentnick,netname,pt);
	    else  /* nick */
	    {
		if(*pt==':')
		{
		    ap_snprintf(tmpircbuf,sizeof(tmpircbuf),"%s %s %s",ircbuf,myparentnick,pt);
		} else {
		    ap_snprintf(checkcmd,sizeof(checkcmd),"%s ",checknick);
		    if(strstr(pt,checkcmd)==pt)
		    {
			pt+=strlen(checkcmd);
			ap_snprintf(tmpircbuf,sizeof(tmpircbuf),"%s %s %s %s",ircbuf,myparentnick,myparentnick,pt);
		    } else
			ap_snprintf(tmpircbuf,sizeof(tmpircbuf),"%s %s %s'%s",ircbuf,myparentnick,netname,pt);
		}
	    }
	    strmncpy(ircbuf,tmpircbuf,sizeof(ircbuf));
	}
    }
    return 0x0;
}

int netmode(int usern, char *netname, char *myparentnick)
{
    char tmpircbuf[8192];
    char *pt;
    char *inmodes;
    char *inparams,*nextparam;
    char mde[]=" MODE ";
    char dest[512];
    char modes[128];
    char params[8192];
    char *pmode;
    int hasparam;
    int nmode;
    char *chanmodes;
    char *prefixmodes;
    char *chantypes;
    char *ept;
    int aftercomma=0;
    int minus=0;
    char mk[2];
    modes[0]=0;
    params[0]=0;
    pt=strstr(ircbuf,mde);
    mk[1]=0;
    if(pt)
    {
	if(strmcmp(netname,"int"))
	{
	    chanmodes="b,k,l,imnps";
	    prefixmodes="ov";
	    chantypes="#";
	} else 	{
	    chanmodes=user(usern)->chanmodes;
	    prefixmodes=user(usern)->prefixmodes;	    
	    chantypes=user(usern)->chantypes;
	}
	pt+=strlen(mde);
	if(strchr(chantypes,*pt))
	{
	    inmodes=strchr(pt,' ');
	    if(inmodes)
	    {
		*inmodes=0;
		inmodes++;
		pmode=inmodes;
		ap_snprintf(dest,sizeof(dest),"#%s'%s",netname,pt);
		inparams=strchr(inmodes,' ');
		if(inparams)
		{
		    *inparams=0;
		    inparams++;
		    while(*inmodes)
		    {
			nextparam=inparams;
			hasparam=0;
			nmode=0;
			if(*inmodes=='-') minus=1;
			if(*inmodes=='+') minus=0;
			if(strchr(prefixmodes,*inmodes)!=NULL) 
			{
			    hasparam=1;
			    nmode=1;
			}
			else 
			{
			    ept=strchr(chanmodes,*inmodes);
			    if(ept)
			    {
				aftercomma=0;
				while(ept!=chanmodes)
				{
				    if(*ept==',') aftercomma++;
				    ept--;
				}
				if(aftercomma<2)
				{
				    hasparam=1;
				} else {
				    if(aftercomma==2 && minus==0) hasparam=1;
				}
				if(hasparam)
				{
				    if(*ept!='l' && *ept!='k')
					nmode=1; /* everything else should be a tokened mode */
				}
			    }		    
			}
			if(hasparam && inparams!=NULL)
			{
			    if(*inparams!=0)
			    {
				pt=strchr(inparams,' ');
				if(pt)
				{
				    *pt=0;
				    pt++;				    
				}
				nextparam=pt;
				if(strlen(inparams)>0)
				{
				    if(nmode)
				    {
					if(strlen(params)+strlen(netname)+1+strlen(inparams)+2<sizeof(params))
					{
					    pt=strchr(inparams,'\r');
					    if(pt==NULL) pt=strchr(inparams,'\n');
					    if(pt) *pt=0;
					    if(strmncasecmp(inparams,user(usern)->nick)==0 && strmncasecmp(inparams,myparentnick)==0)
					    {
						strcat(params,netname);
						strcat(params,"'");
						strcat(params,inparams);
						strcat(params," ");	
					    } else {
						if(strlen(params)+strlen(myparentnick)+1<sizeof(params))
						{
						    strcat(params,myparentnick);
						    strcat(params," ");
						}
					    }
					}
				    } else  {
					if(strlen(params)+1+strlen(inparams)+1<sizeof(params))
					{
					    strcat(params,inparams);
					    strcat(params," ");
					}				    
				    }
				}
			    }
			}
			inparams=nextparam;
			inmodes++;
		    }
		    ap_snprintf(tmpircbuf,sizeof(tmpircbuf),":%s MODE %s %s %s",netircfrom,dest,pmode,params);
		} else {
		    ap_snprintf(tmpircbuf,sizeof(tmpircbuf),":%s MODE %s %s",netircfrom,dest,pmode);
		}	
		strmncpy(ircbuf,tmpircbuf,sizeof(ircbuf));
	    }
	}
    }
    return 0x0;
}

/* the hash, can be enhanced, if needed */

struct thasht thash[] = {
    {"MODE",	netmode},
    {"KICK",	netkick},
    {"PRIVMSG", netprivmsg},
    {"NICK",	netnick},
    {"JOIN",	netjoin},
    {"PART",	netpart},
    {"TOPIC",	nettopic},
    {"NOTICE",	netprivmsg},
    {"QUIT",	netquit},
    {"INVITE",	netinvite},
    {"353",	netnames},
    {"319",	netwhoischannels},
    {"352",	netwho},
    {"302",	netisonuserhost},
    {"303",	netisonuserhost},
    {"*",	netdefault},
    {NULL,	NULL}
};

/* this is a bitch -> adding the network token to the currents network-Users-
   Inbound 									*/

int addtoken(int usern)
{
    char *pt,*pt2;
    char tmpircbuf[8192];
    char netname[20];
    char myparentnick[64];
    int intnet=0;
    int i,rc;
    pcontext;
#ifdef INTNET
    if(usern>10000)
    {
	strmncpy(netname,"int",sizeof(netname));
	usern=usern-10000;
	intnet=10000;
	strmncpy(myparentnick,user(usern)->nick,sizeof(myparentnick));
    } else {
#endif
	strmncpy(netname,user(usern)->network,sizeof(netname));
	strmncpy(myparentnick,user(user(usern)->parent)->nick,sizeof(myparentnick));
#ifdef INTNET
    }
#endif
    pcontext;
    /* general: convert our nick in a network to our main nick */
    if(intnet==0)
	parentnick(usern);
    /* general: check the servermsgs source 
       if : is the first digit, its a server message. should be true
       in all messages which get here.. still..
       if ircfrom ( the hostmask ) contains @, its a nick!user@host notation, only
       then the source needs to be changed to a network token.
       if my own nick in the parent network is the current nick in the from-
       directive, dont change it.
    */
    pcontext;
    if(ircbuf[0]==':' && strchr(ircfrom,'@') && strmncasecmp(myparentnick,ircnick)==0)
    {
	/* get the items behind the from notation */
	pt=strchr(ircbuf,' ');
	if(pt)
	{
	    pt++;
	    if(strmncasecmp(user(usern)->nick,ircnick)==1)
	    {
		pt2=strchr(ircfrom,'!');
		if(pt2)
		    ap_snprintf(netircfrom,sizeof(netircfrom),"%s%s",myparentnick,pt2);
		else
		    strmncpy(netircfrom,ircfrom,sizeof(netircfrom));
	    } else {
		ap_snprintf(netircfrom,sizeof(netircfrom),"%s'%s",netname,ircfrom);
	    }
	    ap_snprintf(tmpircbuf,sizeof(tmpircbuf),":%s %s",netircfrom,pt);
	    strmncpy(ircbuf,tmpircbuf,sizeof(ircbuf));
	}
    } else {
	strmncpy(netircfrom,ircfrom,sizeof(netircfrom));
    }
    pcontext;
    /* thats all for the general part. now the special handling */
    i=0;
    while(thash[i].command)
    {
	if(thash[i].command[0]=='*')
	{
	    if(thash[i].handler!=NULL)
	    {
		rc=(*thash[i].handler)(usern,netname,myparentnick);
		break;
	    }
	} else
	if(ifcommand(thash[i].command) && thash[i].handler!=NULL)
	{
	    rc=(*thash[i].handler)(usern,netname,myparentnick);
	    break;
	}
	i++;
    }
    pcontext;
    return 0x0;
}

/* checking, if network exists for a user */
int checkusernetwork(int usern)
{
    struct usernodes *th;
    int uind;
    pcontext;
#ifdef INTNET
    if(strmcmp(user(usern)->network,"int")==1) return 0x0;
#endif
    th=usernode;
    while(th!=NULL)
    {
	uind=th->uid;
	if (user(uind)->parent==usern) {
	    if (strmcmp(user(uind)->network,irccontent)) return uind;
	}
	th=th->next;
    }
    return 0;
}
