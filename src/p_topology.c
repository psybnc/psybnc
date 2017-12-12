/* $Id: p_topology.c,v 1.3 2005/06/04 18:00:14 hisi Exp $ */
/************************************************************************
 *   psybnc2.3.2, src/p_topology.c
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
static char rcsid[] = "@(#)$Id: p_topology.c,v 1.3 2005/06/04 18:00:14 hisi Exp $";
#endif

#define P_TOPOLOGY

#include <p_global.h>


#define TP_ROOT		0
#define TP_LTO		1
#define TP_LFROM	2

/* the local topology pointer */

struct topologyt *topology;

/* other local variables */

int maximumdigs=0;
char digname[60];
char dispbuffer[8192];
int displevel=0;
char nxchar[100];
char mm[2];

/* iterational search for the linked object */

struct topologyt *digtopology(struct topologyt *topo)
{
    struct topologyt *dig;
    int i;
    if(strmcmp(topo->server,digname)==1) return topo;
    maximumdigs--;
    if(maximumdigs==0) return NULL;    
    for(i=0;i<100;i++)
    {
	if(topo->linked[i]!=NULL)
	{
	    dig=digtopology(topo->linked[i]);
	    if(dig!=NULL) return dig;
	}
    }
    return NULL;
}

/* start to dig for a topology */

struct topologyt *gettopology(char *name)
{
    struct topologyt *topo;
    topo=topology;
    if(topo==NULL) return 0x0;
    if(strmcmp(topo->server,name)==1) return topo;
    maximumdigs=100;
    strmncpy(digname,name,sizeof(digname));
    return digtopology(topo);
}

/* callback for the topology display */

int(*displaytopo)(char *output);

/* for displaying the topology */

int *digdisptopology(struct topologyt *topo)
{
    int i,j,k,rc;
    char oem;
    if(displevel>100) return 0x0;
    displevel++;
    maximumdigs--;
    dispbuffer[0]=0;
    if(maximumdigs==0) return NULL;    
    for(i=0;i<100;i++)
    {
	if(topo->linked[i]!=NULL)
	{
	    nxchar[displevel-1]=' ';
	    oem='`';
	    for(k=i+1;k<100;k++)
		if(topo->linked[k]!=NULL) { nxchar[displevel-1]='|'; oem='|';}
	    dispbuffer[0]=0;
	    for(j=0;j<displevel-1;j++)
	    {
		mm[0]=nxchar[j];
		mm[1]=0;
		if(strlen(dispbuffer)+10<sizeof(dispbuffer))
		{
		    strcat(dispbuffer,mm);
		    strcat(dispbuffer,lngtxt(859));
		}
	    }
	    mm[0]=oem;
	    mm[1]=0;
	    if(strlen(dispbuffer)+11+strlen(topo->linked[i]->server)<sizeof(dispbuffer))
	    {
		strcat(dispbuffer,mm);
		strcat(dispbuffer,lngtxt(860));
		if(topo->linked[i]->linktype==TP_LTO)
		    strcat(dispbuffer,">");
		else
		    strcat(dispbuffer,"<");
		strcat(dispbuffer,topo->linked[i]->server);
		strcat(dispbuffer,"\n");
		rc=(*displaytopo)(dispbuffer);
	    }
	    digdisptopology(topo->linked[i]);
	}
    }
    displevel--;
    return 0x0;
}

/* start to dig for a topology */

int *displaytopology(int(*displayvia)(char *buffer))
{
    struct topologyt *topo;
    char ebuf[60];
    int rc;
    displevel=0;
    displaytopo=displayvia;
    topo=topology;
    if(topo==NULL) return 0x0;
    maximumdigs=100;
    strmncpy(ebuf,topo->server,sizeof(ebuf));
    rc=(*displaytopo)(ebuf);
    digdisptopology(topo);
}

/* add an item to the topology */

int addtopology(char *from, char *to)
{
    struct topologyt *top,*a,*b;
    int i;
    int litype;
    if(topology==NULL)
    {
	topology=(struct topologyt *)pmalloc(sizeof(struct topologyt));
	strmncpy(topology->server,me,sizeof(topology->server));
	topology->linktype=TP_ROOT;
    }
    a=gettopology(from);
    b=gettopology(to);
    if(a!=NULL && b!=NULL) return 0x0;
    if(a==NULL && b==NULL) return 0x0;
    if(a==NULL) 
    {
	top=b;
	litype=TP_LFROM;
    } else {
	top=a;
	litype=TP_LTO;
    }
    for(i=0;i<100;i++)
    {
	if(top->linked[i]==NULL)
	{
	    top->linked[i]=(struct topologyt *)pmalloc(sizeof(struct topologyt));
	    top=top->linked[i];
	    top->linktype=litype;
	    if(litype==TP_LFROM)
		strmncpy(top->server,from,sizeof(top->server));
	    else
		strmncpy(top->server,to,sizeof(top->server));
	    return 0x0;
	}
    }        
}

/* module-constat caller for the remove-call */

int(*removeit)(char *name);

/* iterative remover */

int digremove(struct topologyt *topo)
{
    int i;
    int rc;
    for(i=0;i<100;i++)
    {
	if(topo->linked[i]!=NULL)
	{
	    rc=(*removeit)(topo->linked[i]->server);
	    rc=digremove(topo->linked[i]);    
	    free(topo->linked[i]);
	    topo->linked[i]=NULL;
	}
    }
    return 0x0;
}

/* call for removing the topology item */

int removetopology(char *node, char *item,int(*removecall)(char *name))
{
    struct topologyt *tree,*nodetree;
    int i;
    removeit=removecall;
    tree=gettopology(item);
    nodetree=gettopology(node);
    if(tree==NULL || nodetree==NULL) return 0x0;
    for(i=0;i<100;i++)
    {
	if(nodetree->linked[i]==tree)
	{
	    i=(*removecall)(nodetree->linked[i]->server);
	    digremove(tree);
	    break;
	}
    }
    tree=gettopology(item);
    nodetree=gettopology(node);
    if(tree==NULL || nodetree==NULL) return 0x0;
    for(i=0;i<100;i++)
    {
	if(nodetree->linked[i]==tree)
	{
	    nodetree->linked[i]=NULL;
	    free(tree);    
	    break;
	}
    }
    return 0x0;
}

