/* $Id: p_uchannel.c,v 1.6 2005/06/04 18:00:14 hisi Exp $ */
/************************************************************************
 *   psybnc2.3.2, src/p_uchannel.c
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
static char rcsid[] = "@(#)$Id: p_uchannel.c,v 1.6 2005/06/04 18:00:14 hisi Exp $";
#endif

#define P_UCHANNEL

#include <p_global.h>

/* bring the channel modes into the right order, return formulated mode */

char returnmode[300];

char *ordermodes(struct uchannelt *channel)
{
    int i;
    char *pt;
    char known[]="spmntilk";
    char mm[2];
    char newmode[30];
    char param[200];
    char limit[10];
    newmode[0]=0;mm[1]=0;
    param[0]=0;
    for(i=0;i<strlen(known);i++)
    {
	mm[0]=known[i];
	pt=strchr(channel->modes,known[i]);
	if(pt!=NULL)
	{
	    if(*pt=='k')
	    {
		if(strlen(param)+strlen(channel->key)+2<199)
		{
		    strcat(param,channel->key);
		    strcat(param," ");
		}
	    }
	    if(*pt=='l')
	    {
		ap_snprintf(limit,sizeof(limit),"%d",channel->limit);
		if(strlen(param)+strlen(limit)+2<199)
		{
		    strcat(param,limit);
		    strcat(param," ");
		}
	    }
	    if(strlen(newmode)<29) strcat(newmode,mm);
	    *pt=' ';
	}
    }
    pt=channel->modes;
    if(channel->usern!=-1)
    {
	while(*pt!=0)
	{
	    if(*pt!=' ')
	    {
		mm[0]=*pt;
		if(strlen(newmode)<29)
		    strcat(newmode,mm);
	    }
	pt++;
	}
    }
    strmncpy(channel->modes,newmode,sizeof(channel->modes));
    ap_snprintf(returnmode,sizeof(returnmode),lngtxt(877),newmode,param);
    return returnmode;
}

/* Add a channel to a user structure or return the existent channel */

struct uchannelt *addchanneltouser(int usern, char *channel,int nowrite)
{
    struct uchannelt *lkm,*llkm=NULL;
    char uc[255],uc1[255];
    char cfg[]="ENTRY%d";
    char bf[400];
    pcontext;
    strmncpy(uc,channel,sizeof(uc));
    ucase(uc);
#ifdef INTNET
    if(usern==-1)
	lkm=intchan;
    else
#endif
	lkm=user(usern)->channels;
    while(lkm!=NULL)
    {
	strmncpy(uc1,lkm->name,sizeof(uc1));
	ucase(uc1);
	if(strmcmp(uc1,uc)==1)
	    return lkm;
	llkm=lkm;
	lkm=lkm->next;
    }
    if(llkm==NULL)
    {
#ifdef INTNET
	if(usern==-1)
	{
	    intchan=(struct uchannelt *)pmalloc(sizeof(struct uchannelt));
	    llkm=intchan;
	}
	else
	{
#endif
	    user(usern)->channels=(struct uchannelt *)pmalloc(sizeof(struct uchannelt));
	    llkm=user(usern)->channels;
#ifdef INTNET
	}
#endif
    } else {
	llkm->next=(struct uchannelt *)pmalloc(sizeof(struct uchannelt));
	llkm=llkm->next;
    }
    strmncpy(llkm->name,channel,sizeof(llkm->name));
#ifdef INTNET
    if(usern==-1) /* intnet -> always ov and @+ only */
    {
	strcpy(llkm->prefixmodes,"ov");
	strcpy(llkm->prefixchars,"@+");
	strcpy(llkm->chanmodes,"b,k,l,imnpst");
    } else {
#endif
	strmncpy(llkm->prefixmodes,user(usern)->prefixmodes,sizeof(llkm->prefixmodes));
	strmncpy(llkm->prefixchars,user(usern)->prefixchars,sizeof(llkm->prefixchars));
	strmncpy(llkm->chanmodes,user(usern)->chanmodes,sizeof(llkm->chanmodes));
#ifdef INTNET
    }
#endif
    
    strcpy(llkm->modes,"nt"); /* standardmodes */
    llkm->usern=usern;
    pcontext;
    /* write to config */
#ifdef INTNET
    if(usern==-1) return llkm;
#endif
    if(nowrite==1) return llkm;
    ap_snprintf(bf,sizeof(bf),lngtxt(878),usern);
    countconfentries(lngtxt(879),lngtxt(880),bf);
    llkm->isentry=lastfree;
    ap_snprintf(bf,sizeof(bf),lngtxt(881),usern);
    ap_snprintf(uc,sizeof(uc),lngtxt(882),lastfree);
    pcontext;
    writeini(lngtxt(883),uc,bf,channel);
    flushconfig(); /* this was missing, result: channels where not saved in time */
    pcontext;
    return llkm;
}

/* Get a channel from a user structure */

struct uchannelt *getuserchannel(int usern, char *channel)
{
    struct uchannelt *lkm,*llkm=NULL;
    char uc[255],uc1[255];
    strmncpy(uc,channel,sizeof(uc));
    ucase(uc);
#ifdef INTNET
    if(usern==-1)
	lkm=intchan;
    else
#endif
	lkm=user(usern)->channels;
    while(lkm!=NULL)
    {
	strmncpy(uc1,lkm->name,sizeof(uc1));
	ucase(uc1);
	if(strmcmp(uc1,uc)==1)
	    return lkm;
	llkm=lkm;
	lkm=lkm->next;
    }
    return NULL;
}

/* get a nickname of a userlist from a channel of a user */

struct uchannelusert *getchannelnick(struct uchannelt *channel, char *nick)
{
    struct uchannelusert *ukm;
    char uu[64],uu1[64];
    if(channel==NULL) return NULL;
    ukm=channel->users;
    strmncpy(uu,nick,sizeof(uu));
    ucase(uu);
    while(ukm!=NULL)
    {
	strmncpy(uu1,ukm->nick,sizeof(uu1));
	ucase(uu1);
	if (strmcmp(uu,uu1)==1)
	{
	    return ukm;
	}
	ukm=ukm->next;
    }
    return NULL;
}

/* check, if a nick of a channel of a user got a special mode */

int hasmode(int usern, char *nick, char *channel,char modesymbol)
{
    struct uchannelt *chan;
    struct uchannelusert *chanuser;
    int rc=0;
    chan=getuserchannel(usern,channel);
    if(chan)
    {
	chanuser=getchannelnick(chan,nick);
	if(chanuser)
	{
	    if(strchr(chanuser->mode,modesymbol))
		rc=0x1;
	}
    }
    return rc;
}

/* add a nick to a channel of a user (on join) */

struct uchannelusert *addnicktochannel(struct uchannelt *chan, char *nick, char *ident, char *host)
{
    struct uchannelusert *chanuser,*scu;
    chanuser=getchannelnick(chan,nick);
    if(chanuser!=NULL)
    {
	scu=chanuser;
    } else {
	chanuser=chan->users;
	scu=chanuser;
        while(chanuser!=NULL)
        {
	    scu=chanuser;
	    chanuser=chanuser->next;
	}
	if(scu==NULL)
	{
	    chan->users=(struct uchannelusert *)pmalloc(sizeof(struct uchannelusert));
	    scu=chan->users;
	} else {
	    scu->next=(struct uchannelusert *)pmalloc(sizeof(struct uchannelusert));
	    scu=scu->next;
	}
    }
    strmncpy(scu->nick,nick,sizeof(scu->nick));
    strmncpy(scu->ident,ident,sizeof(scu->ident));
    strmncpy(scu->host,host,sizeof(scu->host));
    return scu;
}

/* remove a nick from a channel */

int removenickfromchannel(struct uchannelt *channel, char *nick)
{
    struct uchannelusert *chanuser,*previous=NULL;
    chanuser=channel->users;
    while(chanuser!=NULL)
    {
	if(strmcmp(chanuser->nick,nick)==1)
	{
	    if(previous==NULL)
	    {
		channel->users=chanuser->next;
	    } else {
		previous->next=chanuser->next;
	    }
	    free(chanuser);
	    return 0x0;
	}
	previous=chanuser;
	chanuser=chanuser->next;
    }
    return 0x0;    
}

/* remove a nick from all channels of a user */

int removenickfromallchannels(int usern, char *nick)
{
    struct uchannelt *channel,*echannel;
#ifdef INTNET
    if(usern==-1)
	channel=intchan;
    else
#endif
	channel=user(usern)->channels;
    while(channel!=NULL)
    {
	removenickfromchannel(channel,nick);
	echannel=channel;
	channel=channel->next;
	if(echannel->users==NULL)
	{
	    removechannelfromuser(usern,echannel->name);
	}
    }
    return 0x0;
}

/* nickchange */

int nickchange(int usern, char *oldnick,char *newnick)
{
    struct uchannelt *channel;
    struct uchannelusert *chanuser;
#ifdef INTNET
    if(usern==-1)
	channel=intchan;
    else
#endif
	channel=user(usern)->channels;
    while(channel!=NULL)
    {
	chanuser=getchannelnick(channel,oldnick);
	if(chanuser!=NULL)
	{
	    strmncpy(chanuser->nick,newnick,sizeof(chanuser->nick));
	}
	channel=channel->next;
    }
    return 0x0;
}

/* remove all users from a channel (if the channel was left in any way) */

int removeallusersfromchannel(struct uchannelt *lkm)
{
    struct uchannelusert *ukm,*uukm=NULL;
    ukm=lkm->users;
    while(ukm!=NULL)
    {
	uukm=ukm->next;
	free(ukm);
	ukm=uukm;
    }
    lkm->users=NULL;
    return 0x0;
}

/* remove all channels from the user (for clearing the user) */

int removeallchannelsfromuser(int usern)
{
    struct uchannelt *lkm,*llkm=NULL;
    struct uchannelusert *ukm,*uukm;
#ifdef INTNET
    if(usern==-1)
	lkm=intchan;
    else
#endif
	lkm=user(usern)->channels;
    while(lkm!=NULL)
    {
	llkm=lkm->next;
	ukm=lkm->users;
	while(ukm!=NULL)
	{
	    uukm=ukm->next;
	    free(ukm);
	    ukm=uukm;
	}
	free(lkm);
	lkm=llkm;
    }
    /* we dont need to cleanup conf, this will be done later */
#ifdef INTNET
    if(usern==-1)
	intchan=NULL;
    else
#endif
	user(usern)->channels=NULL;
    return 0x0;
}

/* remove a specific channel from a user */

int removechannelfromuser(int usern, char *channel)
{
    struct uchannelt *lkm,*llkm=NULL;
    struct uchannelusert *ukm,*uukm;
    struct stringarray *l1,*l2;
    char uc[255],uc1[255];
    strmncpy(uc,channel,sizeof(uc));
    ucase(uc);
#ifdef INTNET
    if(usern==-1)
	lkm=intchan;
    else
#endif
	lkm=user(usern)->channels;
    while(lkm!=NULL)
    {
	strmncpy(uc1,lkm->name,sizeof(uc1));
	ucase(uc1);
	if(strmcmp(uc1,uc)==1)
	{
	    if(llkm==NULL)
	    {
#ifdef INTNET
		if(usern==-1)
		    intchan=lkm->next;
		else
#endif
		    user(usern)->channels=lkm->next;
	    } else {
		llkm->next=lkm->next;
	    }
	    ukm=lkm->users;
	    while(ukm!=NULL)
	    {
		uukm=ukm->next;
		free(ukm);
		ukm=uukm;
	    }
	    l1=lkm->bans;
	    while(l1!=NULL)
	    {
		l2=l1->next;
		free(l1->entry);
		free(l1);
		l1=l2;
	    }
	    l1=lkm->invites;
	    while(l1!=NULL)
	    {
		l2=l1->next;
		free(l1->entry);
		free(l1);
		l1=l2;
	    }
	    l1=lkm->intops;
	    while(l1!=NULL)
	    {
		l2=l1->next;
		free(l1->entry);
		free(l1);
		l1=l2;
	    }
	    if(usern!=-1)
	    {
		ap_snprintf(uc,sizeof(uc),lngtxt(884),usern,lkm->isentry);
		clearsectionconfig(uc);
		ap_snprintf(uc,sizeof(uc),lngtxt(885),usern,lkm->isentry);
		clearsectionconfig(uc);
		flushconfig();
	    }
	    free(lkm);
	    lkm=llkm;
	    return 0x0;
	}
	llkm=lkm;
	lkm=lkm->next;
    }
    return 0x0;
}

/* set initial channelmodes */

int setchannelmode(struct uchannelt *lkm,char *mode, char *param)
{
    int i;
    char *par,*pt,*md;
    char sb[20],eb[20];
    strmncpy(lkm->modes,mode,sizeof(lkm->modes));
    par=param;
    md=mode;
    while(*md)
    {
	if(*md==0) return 0x0;
	switch(*md)
	{
	    case 'l':
		if (par==NULL) break;
		pt=strchr(par,' ');
		if(pt!=NULL)
		{
		    *pt=0;
		    pt++;
		}
		lkm->limit=atoi(par);
		par=pt;
		break;
	    case 'k':
		if (par==NULL) break;
		pt=strchr(par,' ');
		if(pt!=NULL)
		{
		    *pt=0;
		    pt++;
		}
		strmncpy(lkm->key,par,sizeof(lkm->key));
		if(lkm->usern>0 && lkm->usern!=-1)
		{
		    ap_snprintf(sb,sizeof(sb),lngtxt(886),lkm->usern);
		    ap_snprintf(eb,sizeof(eb),lngtxt(887),lkm->isentry);
		    writeini(lngtxt(888),eb,sb,cryptit(lkm->key));
		    flushconfig();
		}
		par=pt;
		break;
	}
	md++;
    }
    return 0x0;
}

/* sub: add a mode to a user on a channel */

int addchannelusermode(struct uchannelt *channel, char *nickname, char mode)
{
    struct uchannelusert *chanuser;
    char order[20];
    char *md;
    char *mc;
    int coff=0;
    memset(&order[0],0x0,sizeof(order));
    chanuser=getchannelnick(channel,nickname);
    if(chanuser==NULL) return 0x0;
    md=channel->prefixmodes;
    mc=channel->prefixchars;
    while(*md)
    {
	if((strchr(chanuser->mode,*mc)!=NULL || mode==*md) && coff<sizeof(order)) 
	{
	    order[coff]=*mc;
	    coff++;
	}
	mc++;
	md++;
    }
    strcpy(chanuser->mode,order);
    return 0x0;
}

/* sub: remove a single mode from a user on a channel */

int removechannelusermode(struct uchannelt *channel, char *nickname, char mode)
{
    char temp[20];
    struct uchannelusert *chanuser;
    char *pt;
    char imode;
    imode=mode;
    pt=strchr(channel->prefixmodes,mode);
    if(pt)
    {
	pt=pt-channel->prefixmodes+channel->prefixchars;
	imode=*pt;
    } else {
	imode=mode;
    }
    chanuser=getchannelnick(channel,nickname);
    if(chanuser==NULL) return 0x0;
    strmncpy(temp,chanuser->mode,sizeof(temp));
    pt=strchr(temp,imode);
    if(pt==NULL) return 0x0;
    *pt=0;pt++;
    ap_snprintf(chanuser->mode,sizeof(chanuser->mode),"%s%s",temp,pt);
    return 0x0;
}

/* sub: add a single mode to a channel */

int addmodetochannel(struct uchannelt *channel, char mode)
{
    char mm[2];
    mm[1]=0;
    if(strlen(channel->modes)<sizeof(channel->modes))
    {
	mm[0]=mode;
	if(strstr(channel->modes,mm)==NULL)
    	    strcat(channel->modes,mm);
    }
    return 0x0;
}

/* sub: delete a single mode from a channel */

int deletemodefromchannel(struct uchannelt *channel, char mode)
{
    char buf[60];
    char *pt;
    pt=strchr(channel->modes,mode);
    if(pt!=NULL)
    {
	*pt=0;
	pt++;
	ap_snprintf(buf,sizeof(buf),"%s%s",channel->modes,pt);
	strmncpy(channel->modes,buf,sizeof(channel->modes));
    }
    return 0x0;
}

/* send the names of a channel in a right command syntax to a given userinbound */

int sendnames(int usern, struct uchannelt *chan)
{
    int thsock;
    int userp;
    char token[60];
    char nameline[300];
    char line[1024];
    char mm[2];
    struct uchannelusert *users;
    char uc1[200],uc2[200];
#ifdef INTNET
    int intnet=0;
#endif
    userp=usern;
    token[0]=0;
    token[1]=0;
    mm[1]=0;
    if(usern<10000)
    {
	if(user(usern)->innames==1)
	{
	    strmncpy(uc1,rtrim(chan->name),sizeof(uc1));
	    ucase(uc1);
	    strmncpy(uc2,rtrim(user(usern)->inchan),sizeof(uc2));
	    ucase(uc2);
	    if(strmcmp(uc1,uc2)==1)
	    {
		user(usern)->innames=2;	
		return 0x0;
	    }
	}
	if(user(usern)->parent!=0) 
	{
	    userp=user(usern)->parent;
	    ap_snprintf(token,sizeof(token),"#%s'",user(usern)->network);
	}
    }
#ifdef INTNET
    if(usern>10000)
    {
	userp=usern-10000;
	ap_snprintf(token,sizeof(token),lngtxt(889));
	usern-=10000;
	intnet=1;
    }
#endif
    thsock=user(userp)->insock;
#ifdef INTNET
    if(intnet)
	ap_snprintf(nameline,sizeof(nameline),lngtxt(890),"intnet-psybnc.net", user(usern)->nick,token,chan->name);    
    else
#endif
	ap_snprintf(nameline,sizeof(nameline),lngtxt(890),user(usern)->server, user(usern)->nick,token,chan->name);    
    users=chan->users;
    strmncpy(line,nameline,sizeof(line));
    while(users!=NULL)
    {
	if(strlen(line)>500)
	{
	    strcat(line,"\n");
	    writesock(user(userp)->insock,line);
	    strmncpy(line,nameline,sizeof(line));
	} else {
	    if(users->mode[0]!=0)
	    {
		mm[0]=users->mode[0];
		strcat(line,mm);
	    }
	    if(strmcmp(users->nick,user(usern)->nick)==1)
	    {
		strcat(line,user(userp)->nick);
	    } else {
		strcat(line,token+1);
		strcat(line,users->nick);
	    }
	    strcat(line," ");
	    users=users->next;
	}
    }    
    strcat(line,"\n");
    writesock(user(userp)->insock,line);
#ifdef INTNET
    if(intnet)
	ap_snprintf(line,sizeof(line),lngtxt(891),"intnet-psybnc.net", user(usern)->nick,token,chan->name);
    else
#endif
	ap_snprintf(line,sizeof(line),lngtxt(891),user(usern)->server, user(usern)->nick,token,chan->name);
    writesock(user(userp)->insock,line);
    return 0x0;
}

#ifdef SCRIPTING

/* raise a mode event to scripting */

int raisemodeevent(struct uchannelt *chan,char prefix,char mode,char *param)
{
    static char siccontent[4096];
    static char siccontent2[4096];
    strmncpy(siccontent,irccontent,sizeof(siccontent));
    if(param!=NULL)
	ap_snprintf(siccontent2,sizeof(siccontent2),lngtxt(892),prefix,mode,param);
    else
	ap_snprintf(siccontent2,sizeof(siccontent2),"%c%c",prefix,mode);
    strmncpy(irccontent,siccontent2,sizeof(irccontent));
    serverevent(chan->usern);
    strmncpy(irccontent,siccontent,sizeof(irccontent));
    return 0x0;
}

#endif

/* add or delete multiple channelmodes from the userchannel */

int addchannelmode(struct uchannelt *lkm, char *mode, char *param)
{
    char *par,*pt,*md,*ept;
    char prefix;
    static char sb[20],eb[20];
    static char mod[200];
    static char pr[4096];
    int isusermode=0;
    int ischanmode=0;
    int hasparam=0;
    int aftercomma=0;
    strmncpy(mod,mode,sizeof(mod));
    strmncpy(pr,param,sizeof(pr));
    prefix='+';
    par=pr;
    md=mod;
    while(*md!=0)
    {
	isusermode=0;
	ischanmode=0;
	hasparam=0;
	if(*md=='+' || *md=='-') 
	    prefix=*md;
	else 
	{
	    pt=strchr(lkm->chanmodes,*md);
	    if(pt) 
	    {
		ischanmode=1;
		ept=pt;
		aftercomma=0;
		while(ept>lkm->chanmodes)
		{
		    if(*ept==',') aftercomma++;
		    ept--;
		}
		if(aftercomma<2 || (aftercomma==2 && prefix!='-'))
		    hasparam=1;
	    }
	    pt=strchr(lkm->prefixmodes,*md);
	    if(pt)
	    {	
    		isusermode=1;
		hasparam=1; /* usermodes always have parameters */
	    }
	    if(isusermode)
	    {
		if (par==NULL) break;
		pt=strchr(par,' ');
		if(pt!=NULL)	
		{
	    	    *pt=0;pt++;
	    	    if(*pt==0) pt=NULL;
		}
		if(prefix=='+')
	    	    addchannelusermode(lkm,par,*md);
		else
	    	    removechannelusermode(lkm,par,*md);
#ifdef SCRIPTING
		if(lkm->usern>0)
	    	    raisemodeevent(lkm,prefix,*md,par);
#endif
		par=pt;
	    } else 
	    if(ischanmode) 
	    {
		if(hasparam)
		{
		    if (par==NULL) break;
		    pt=strchr(par,' ');
		    if(pt!=NULL)	
		    {
			*pt=0;pt++;
			if(*pt==0) pt=NULL;
		    }
		    switch(*md)
		    {
			case 'l':
			    lkm->limit=atoi(par);
			    break;
			case 'k':
			    if(prefix=='+')
			    {
				strmncpy(lkm->key,par,sizeof(lkm->key));
				if(lkm->usern>0)
				{
				    ap_snprintf(sb,sizeof(sb),lngtxt(893),lkm->usern);
    				    ap_snprintf(eb,sizeof(eb),lngtxt(894),lkm->isentry);
				    writeini(lngtxt(895),eb,sb,cryptit(lkm->key));
				    flushconfig();
				}
			    } else {
				lkm->key[0]=0;
				if(lkm->usern>0)
				{
				    ap_snprintf(sb,sizeof(sb),lngtxt(896),lkm->usern);
    				    ap_snprintf(eb,sizeof(eb),lngtxt(897),lkm->isentry);
				    writeini(lngtxt(898),eb,sb,NULL);
				    flushconfig();
				}
			    }
			    break;
			case 's':
			    if(prefix=='+')
				deletemodefromchannel(lkm,'p');
			    break;
			case 'p':
			    if(prefix=='+')
				deletemodefromchannel(lkm,'s');
			    break;
			default:		    			    
			    break;
		    }
		    if(prefix=='+')
			addmodetochannel(lkm,*md);
		    else
			deletemodefromchannel(lkm,*md);
#ifdef SCRIPTING
		    if(lkm->usern>0)
			raisemodeevent(lkm,prefix,*md,par);
#endif
		    par=pt;
		} else {	
		    if(prefix=='+')
			addmodetochannel(lkm,*md);
		    else
			deletemodefromchannel(lkm,*md);
		    if(*md=='l') lkm->limit=0;
#ifdef SCRIPTING
		    if(lkm->usern>0)
			raisemodeevent(lkm,prefix,*md,NULL);
#endif
		}
	    }
	}
	md++;
    }
    ordermodes(lkm);
    return 0x0;
}

int setchanneltopic(struct uchannelt *chan, char *topic)
{
    strmncpy(chan->topic,topic,sizeof(chan->topic));
    return 0x0;
}

int convertlastwhois(int usern, char *lastwhois)
{
    char *pt,*ept,*pt2;
    char lastsic[2048];
    int chan=0;
    strmncpy(lastsic,lastwhois,sizeof(lastsic));
    ept=lastsic;
    while(ept!=NULL)
    {
	while(*ept==' ') ept++;
	if(*ept==0) return 0x0;
	pt=strchr(ept,' ');
	if(pt!=NULL)
	    {*pt=0;pt++;}
	pt2=ept+1;
	if(strchr(user(usern)->prefixchars,*ept)!=NULL)
	    ept++;
	addchanneltouser(usern,ept,0);
	ept=pt;
    }
    return 0x0;
}

/* reset all channels for a user (on disconnect) */

int resetallchannels(int usern)
{
    struct uchannelt *chan;
    chan=user(usern)->channels;
    while(chan!=NULL)
    {
	removeallusersfromchannel(chan);
	chan->ison=0;
	chan->topic[0]=0;
	chan=chan->next;
    }
    return 0x0;
}

/* load all channels for a user from the config */

int loadchannels(int usern)
{
    char bf[30],bf2[30];
    struct uchannelt *chan;
    int i,rc;
    ap_snprintf(bf,sizeof(bf),lngtxt(899),usern);
    for(i=0;i<40;i++)
    {
	ap_snprintf(bf2,sizeof(bf2),lngtxt(900),i);	
	rc=getini(lngtxt(901),bf2,bf);
	if(rc==0)
	{
	    chan=addchanneltouser(usern,value,1);
	    ap_snprintf(bf2,sizeof(bf2),lngtxt(902),i);
	    rc=getini(lngtxt(903),bf2,bf);
	    if(chan!=NULL) 
	    {
		chan->isentry=i;
		if(rc==0)
		{
		    strcpy(chan->modes,"tnk");
		    if(value[0]=='+') /* if its crypted */
			strmncpy(chan->key,decryptit(value),sizeof(chan->key));
		    else /* or not */
			strmncpy(chan->key,value,sizeof(chan->key));
		}
	    }
	}
    }
}
