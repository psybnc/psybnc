/* $Id: p_data.h,v 1.2 2005/06/04 18:00:14 hisi Exp $ */
/************************************************************************
 *   psybnc2.3.2, src/p_data.h
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

/* Dummy Stringarray */

struct stringarray *first;

#ifdef PARTYCHANNEL
/* partychannel defs */

struct stringarray *partyusers;
char partytopic[80];
#endif

/* CREATE-Variables */

int U_CREATE=0; /* if U_CREATE == 1, user() will return a newly created user if not available */
int P_CREATE=0; /* if P_CREATE == 1, newpeer() will return a newly created peer */
int D_CREATE=0; /* if D_CREATE == 1, datalink() will return a newly created link */

/* static data, only included in the main file */

char logfile[200];
char value[200];
char accepthost[400];
char acceptip[60];
unsigned int acceptport;
int pid;
int listensocket;
int listenport;
int highestsocket;
int errn;
struct hostent *hostinfo;
FILE *mainlog=NULL;
char configfile[100];
int safemode=0;
int notsocket=0;
/* main communication buffer */

char ircbuf[8192]; 
int datalen;

/* splitted parameters */

char ircfrom[300]; /* from nick!user@host, will be set to "" if servermsg or client */
char ircnick[64]; /* from nick, will be set to "" if servermsg or client */
char ircident[32]; /* from ident, will be set to "" if servermsg or client */
char irchost[200]; /* from host, will be set to "" if from client, name of server if from server */
char irccommand[64]; /* command or numerical code */
char ircto[800]; /* to */
char irccontent[6000]; /* content */
int ircserver;
int inwrite = 0;
char rbuf[100];
char ehost[400];
char eparm[200];
char echan[200];
int nousers = 1;
char me[200];
int delayinc=0;
char ctxt[50];
char cfunc[60];
int cline;
char dcchost[200];
char dcc6host[200];

#ifdef INTNET

struct uchannelt *intchan=NULL;

#endif

/* The mother of all sockets */

struct socketnodes *socketnode;

/* The current socket */

struct socketnodes *currentsocket;
struct socketnodes *mastersocket;

/* The mother of all Users */

struct usernodes *usernode;

/* The mother of all peers */

struct peernodes *peernode;

/* The mother of all datalinks */

struct linknodes *linknode;

/* Current node supported by user() */

struct usernodes *thisuser;

/* Current node supported by newpeer() */

struct peernodes *thispeer;

/* Current node supported by datalink() */

struct linknodes *thislink;

/* Dummy User for not existent user Structures */

struct usert *dummyuser=NULL;
struct usert *nulluser=NULL;

/* Dummy peer for not existent user peers */

struct newpeert *dummypeer=NULL;

/* Dummy link for not existent datalinks */

struct datalinkt *dummylink=NULL;

/* Host allow List */

struct stringarray *hostallows=NULL;

/* the configfile - cached */

struct stringarray *conf=NULL;

#ifdef TRANSLATE

struct translatet *translate=NULL;

#endif

#ifdef SCRIPTING

struct subtask *subtasks;
int SCRSOCK=0;

#endif

#ifdef SUNOS
int error_num;
#endif

#ifdef HAVE_SSL
SSL_CTX *srvctx=NULL;
SSL_CTX *clnctx=NULL;
#endif
