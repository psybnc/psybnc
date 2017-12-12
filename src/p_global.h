/* $Id: p_global.h,v 1.9 2005/06/04 18:00:14 hisi Exp $ */
/************************************************************************
 *   psybnc2.3.2, src/p_global.h
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

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/time.h>
#ifdef NOSYSTIME
#include <time.h>
#endif
#include <netinet/in.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <arpa/inet.h>
#include <ctype.h>
#include <signal.h>
#include <setjmp.h>
#include <errno.h>
#ifdef HAVE_CONFIG
#include <config.h>
#endif
#ifdef HAVE_PROV_CONFIG
#include "/psybnc/config.h"
#endif
#ifdef HAVE_SSL
#include <openssl/crypto.h>
#include <openssl/x509.h>
#include <openssl/pem.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <openssl/rand.h>
#else
#include <stdarg.h>
#endif
#ifndef va_list
#include <stdarg.h>
#endif
#include <p_version.h>
#ifndef LOGLEVEL
#define LOGLEVEL 0
#endif
#ifndef MAXUSER
#define MAXUSER 50
#endif
#ifndef MAXCONN
#define MAXCONN 50
#endif

#if (SIZEOF_INT == 4)
typedef unsigned int		u_32bit_t;
#else
typedef unsigned long		u_32bit_t;
#endif

typedef unsigned short int	u_16bit_t;
typedef unsigned char		u_8bit_t;

/* the old partychannel */

#define PARTYCHAN "&partyline"

/* system root config-name */

#define INIFILE "PSYBNC"

/* change this to support more users */

#define MAX_USER MAXUSER /* only the REAL used useramount uses memory and clock */

/* socket types */
#define ST_CONNECT	0x0000 /* SOCKET CONNECTS */
#define ST_LISTEN	0x0001 /* SOCKET LISTENS */
#define ST_RESOLVER	0x0003 /* SOCKET RESOLVER */

/* socket data */
#define SD_ASCII	0x0000 /* ASCII-Data - IRC/DCC-Chat/Client */
#define SD_STREAM	0x0001 /* STREAM-Data - DCC File Receive, SSH */

/* socket flags */
#define SOC_NOUSE	0x0000 /* SOCKET NOT IN USE */
#define SOC_SYN		0x0001 /* SYN SENT - CONNECTION REQUESTED */
#define SOC_CONN	0x0002 /* CONNECTED */
#define SOC_RESOLVE	0x0003 /* SOCKET IS RESOLVING */

/* we get a connection */
#define SOC_GOTREQ	0x0001 /* CONN REQUEST */
#define SOC_CONN	0x0002 /* CONNECTED */

/* close needed */
#define SOC_ERROR	0x0003 /* ERROR DISCOVERED */

/* socket error flags */
#define SERR_OK		0x0000 /* OK */
#define SERR_REFUSED	0x0002 /* CONNECTION REFUSED */
#define SERR_TIMEOUT	0x0003 /* CONNECTION TIMED OUT */
#define SERR_TERMINATED 0x0004 /* CONNECTION TERMINATED */
#define SERR_UNKNOWN	0x0005 /* HOST UNKNOWN */

/* standard flags */

#define STD_NOUSE       0x0000 /* NO USER INI, DELETED OR NEW */
#define STD_NOCON   	0x0001 /* VALID USER, BUT NOT CONNECTED */
#define STD_SRCON   	0x0002 /* CLIENT CONNECTED TO SERVER */	
#define STD_NEWCON  	0x0003 /* NEW CONNECTION */
#define STD_WHOIS   	0x0007 /* PERFORMED WHOIS */
#define STD_LOGIN       0x0007 /* DCC:LOGIN */
#define STD_CONN    	0x0008 /* USER CONNECTED */		 

#define RI_ADMIN	0x0001
#define RI_USER		0x0000

/* link types */

#define LI_ALLOW	0x0001 /* ALLOWED LINK FROM */
#define LI_LINK         0x0002 /* LINK TO */
#define LI_RELAY	0x0003 /* ACTIVE RELAY */
#define LI_DCC		0x0004 /* DCC CONNECTION */

/* newpeer types */

#define NP_USER		0x0000 /* USER */
#define NP_LINK		0x0001 /* DATALINK */
#define NP_RELAY	0x0002 /* RELAY */

/* queue definitions */

#define Q_NEXT		0x0000 /* flush next entry */
#define Q_FORCED	0x0001 /* flush next entry, dont care about timers */

#define SOC_TIMEOUT	20     /* 20 seconds to a timeouted connecting socket */

/* logging levels */

#define LOG_ERROR	0x0002 /* error */
#define LOG_WARNING	0x0001 /* warning */
#define LOG_INFO	0x0000 /* info */

/* log exception commands */
#define	LOG_QUIT	"QUIT" /* event will be logged if irccommand matches QUIT */

/* pending DCC types */

#define PDC_CHATTORQ	0x0001 /* bouncer user sent a request */
#define PDC_CHATTO	0x0002 /* bouncer user sent a request */
#define PDC_CHATFROMRQ	0x0003 /* someone sent us a request */
#define PDC_CHATFROM	0x0004 /* the request was answered */
#define PDC_SENDTORQ	0x0005 /* bouncer user wants to send a file */
#define PDC_SENDTO	0x0006 /* bouncer user wants to send a file */
#define PDC_RECVFROMRQ	0x0007 /* someone wants to send us a file request */
#define PDC_RECVFROM	0x0008 /* we are receiving */

/* socket groups */

#define SGR_NONE	0x00000000
#define SGR_USERINBOUND	0x10000000

#ifdef SCRIPTING

/* script functions */

#define SCR_NONE	0x0000
#define SCR_DCCCHAT	0x0001
#define SCR_DCCSEND	0x0002
#define SCR_SERVER	0x0003
#define SCR_DIALOGUE	0x0004
#define SCR_CTCP	0x0005
#define	SCR_ALIAS	0x0006

/* user specific script structure */

struct scriptt {
    int function;
    char *event;
    char *from;
    char *to;
    char *content;
    char *command;
    int pid;				/* in the case of a dialogue */
    struct scriptt *next;
};

/* subtasks for scripting */

#define STD_OUT  0
#define STD_IN   1
#define STD_ERR  2

struct subtask {
    int uid;	 /* for user */
    int pid;     /* process id */
    int fdin;    /* input - is fed by psybnc if the task did an output */
    int fdout;   /* output - is added to a psocket and listened by psybnc */
    int fderr;   /* error - is fed by the task if an error occurs */
    char *desc;  /* description */
    struct scriptt *script;
    struct subtask *next; /* next subtask */
};

extern struct subtask *subtasks;
extern int SCRSOCK; /* if SCRSOCK is set to 1, the currentsocket likely is set not to a user-specific socket */

#endif

#define SSL_OFF		0x0000
#define SSL_ON		0x0001

/* Data Definitions */

struct stringarray {
    char *entry;
    struct stringarray *next;
};

/* psybnc sendq description */

struct sendqt {
    struct sendqt *next;
    char *data;
    size_t len; /* for SD_STREAM */
    int delay;
};

/* psybnc socket description */

struct psockett {
  int type;
  int flag;
  int nowfds;
  int syssock; /* number of the system socket */
  unsigned long sockgroup; /* socket of group index */
  int dataflow; /* SD_ASCII or SD_STREAM */
  int datalen; /* length of data in the case of a STREAM socket */
  int protocol; /* AF_INET or AF_INET6 */
#ifdef HAVE_SSL
  int ssl; /* = 1 ssl usage */
  SSL *sslfd; /* ssl-fd */
#endif
#ifdef IPV6
  char source[60];
  int sport;
  char dest[60];
  int dport;
#else
  char source[16];
  int sport;
  char dest[16];
  int dport;
#endif
  char since[20];
  int (*constructor)(int); /* when socket needs to be setup -> NOUSE */
  int (*constructed)(int); /* when socket was constructed */
  int (*errorhandler)(int,int); /* when an error was encountered */
  int (*handler)(int); /* the handler when socket receives data */
  int (*destructor)(int); /* the handler when the socket is closed */    
  int (*remapper)(int,int); /* remap a socket (when reopened by sockethandler) */
  char *commbuf; /* communication buffer */
  int bytesread; /* current number of bytes stored */
  int serversocket; /* automatic flood prot */
  int serverstoned; /* set to stoned, if # of bytes was sent, decrease to seconds left */
  int serverbytes; /* # of bytes sent to server (if >800, set stoned) */
  unsigned long bytesin;
  unsigned long bytesout;
  int param; /* the parameter */
  unsigned long delay;    
  struct sendqt *sendq; /* unused in the case of a STREAM socket */
  int entrys; /* number of entrys in the sendq .. we flush, if more than xx are entered */
};

/* The socket Nodes */

struct socketnodes {
  struct psockett *sock;
  struct socketnodes *next;
};

/* Data Structure :
   Pending DCCs             */

struct dcct {
    int type;
    char host[200];
    char file[200];
    char nick[64];
    FILE *fhandle;
    long filesize;
    long transferred;
    long lasttransferred;
    int port;
    int sock;
    int uid;
    int pid;
    int delay;
#ifdef HAVE_SSL
    int ssl;
#endif
    struct dcct *next;    
};

/* Data Structure :
   Network Topology   */
   
struct topologyt {
    char server[64];
    struct topologyt *linked[100];
    int linktype;
};

/* Data Structure :
   DATALINK : Structure of a bouncer link */
   
struct datalinkt {
  int type;
  char name[64];
  char user[64];
  char host[400];
  char pass[20];
  char vhost[400];
  char iam[64];
  int port;
  int allowrelay;
  int instate;
  int outstate;
  int insock;
  int outsock;      
  int delayed;
#ifdef HAVE_SSL
  int ssl;
  char cert[1024];
#endif
};

/* The Datalink Nodes */

struct linknodes {
  int uid;
  struct datalinkt *link;
  struct linknodes *next;
};

/* Data Structure :
   CHANNELUSER : User for a channel entry */
   
struct uchannelusert {
    char nick[64];
    char ident[64];
    char host[256];
    char mode[20];
    struct uchannelusert *next;
};

/* Data Structure :
   CHANNEL : Structure of a User-Channel, set into a Chain to the User */

struct uchannelt {
    char name[255];
    char modes[20];
    unsigned long limit;
    char key[64];
    char topic[600];
    char prefixmodes[30]; /* will be inherited from user, or if intnet, from (ov)@+ */
    char prefixchars[30]; /* dito */
    char chanmodes[100];
    int ison; /* is the user on the channel ? */
    int isentry;
    int usern; /* for user (re-reference) */
    struct stringarray *bans;
    struct stringarray *invites;
    struct stringarray *intops;
    struct uchannelusert *users;
    struct uchannelt *next;
};

/* Data Structure :
   USER   : Structure of the specified Users, each user has 2 SOCKET entrys (1:2) */

struct usert {
  int parent; /* is part of User # */
  char nick[64]; /* DAL */
  char wantnick[64]; /* DAL */
  char login[64];
  char user[64];
  char pass[20];
  char vhost[255];
  int vlink;    
  char host[255];
  char server[255];
  char proxy[255];
  char spass[64];
  int pport;
  char firstnick[64];    
  char away[64];
  char awaynick[64];
  char leavemsg[64];
  char network[10];    
  char last[20];
  char prefixmodes[30];
  char prefixchars[30];
  char chanmodes[100];
  char chantypes[30];
  int quitted;
  int welcome;
  int doaway;    
  time_t lastctcp;
  time_t lastdccchat;
  int innames;
  char inchan[300];
  int afterquit;
  int srinit;
  int port;
  int instate; /* state of insocket */
  int outstate; /* state of outsocket */
  int insock; /* points to sockit desc */
  int outsock; /* points to sockit desc */
  int rights;
  int kicks;    
  int gotop;
  int autopongreply;
  int autorejoin;
  int currentserver;
  int triggered;    
  int autogetdcc;
  int leavequit;
  int antiidle;
  int sysmsg;
  int dccenabled;
  int penalty;
  int pinged;
  time_t pingtime;
  time_t nickgain;
  char altnick[64];
  unsigned long pentime;
  long lastlog;
  long delayed; /* retry connect in x tix */
  struct uchannelt *channels;
#ifdef SCRIPTING
  struct scriptt *script;
#endif
  struct stringarray *bans;
  struct stringarray *ops;
  struct stringarray *aops;
  struct stringarray *askops;
  struct stringarray *logs;
  struct stringarray *keys;
  struct stringarray *ignores;
#ifdef CRYPT
  struct stringarray *encrypt;
#endif
#ifdef TRANSLATE
  struct stringarray *translates;
#endif
  struct dcct *pdcc;
  struct linknodes *dcc;
#ifdef TRAFFICLOG
  FILE *trafficlog;
#endif
#ifdef HAVE_SSL
  char cert[1024];
#endif
};

/* The User Nodes */

struct usernodes {
  int uid;
  struct usert *user;     
  struct usernodes *next;
};

/* Data Structure :
   NEWPEER : Structure of a newly connected bouncer / user */

struct newpeert {
  char user[64];
  char login[64];
  char name[64];
  char nick[64];
  char pass[20];
  char host[400];
  char vhost[400];
  char server[400];
  int port;    
  int lnkport;
  int insock;
  int state;
  int type; 
  long delayed;
};

/* The peer Nodes */

struct peernodes {
  int uid;
  struct newpeert *peer;     
  struct peernodes *next;
};

/* resulve-structures */

struct resolve {
  struct resolve *next;
  struct resolve *previous;
  struct resolve *nextid;
  struct resolve *previousid;
  struct resolve *nextip;
  struct resolve *previousip;
  struct resolve *nextip6;
  struct resolve *previousip6;
  struct resolve *nexthost;
  struct resolve *previoushost;
  int(*resolved)(struct resolve *); /* call in case of worked resolve */
  int(*unresolved)(struct resolve *); /* call in case of failed resolve */
  time_t expiretime;
  char data[512]; /* data given by application */
  char hostn[256];
  int type;
  unsigned long ip;
  unsigned int protocol;
  char ip6[17];
  unsigned short id;
  unsigned char state;
  unsigned char sends;
};

enum resolve_states {
    STATE_FINISHED,
    STATE_FAILED,
    STATE_PTRREQ,
    STATE_AREQ,
    STATE_AAAAREQ,
    STATE_PTR6REQ
};

#define IS_PTR(x) (x->state == STATE_PTRREQ)
#define IS_A(x)   (x->state == STATE_AREQ)
#define IS_AAAA(x) (x->state == STATE_AAAAREQ)
#define IS_PTR6(x) (x->state == STATE_PTR6REQ)

#ifdef TRANSLATE

#define TR_TO	0x0000
#define TR_FROM 0x0001

struct translatet {
  int uid;
  int usern;
  int delayed;
  int sock;
  char *translatetext;
  char *translatedtext;
  char *dest;
  char *source;
  char *lang;
  int direction;   /* 0=to user ,1=from user */
  struct translatet *next;
};

#endif

#ifndef P_MAIN

/* Dummy Stringarray */

extern struct stringarray *first;

#ifdef PARTYCHANNEL
/* List of Users online, the topic for the partychannel */

extern struct stringarray *partyusers;
extern char partytopic[81]; /* topic of the partychannel */
#endif

/* The config file - cache */

extern struct stringarray *conf;

/* CREATE-Variables */

extern int U_CREATE; /* if U_CREATE == 1, user() will return a newly created user if not available */
extern int P_CREATE; /* if P_CREATE == 1, newpeer() will return a newly created peer */
extern int D_CREATE; /* if D_CREATE == 1, datalink() will return a newly created link */
extern int errno;

/* The mother of all sockets */

extern struct socketnodes *socketnode;

/* The current socket */

extern struct socketnodes *currentsocket;
extern struct socketnodes *mastersocket;

/* The mother of all Users */

extern struct usernodes *usernode;

/* The mother of all peers */

extern struct peernodes *peernode;

/* The mother of all datalinks */

extern struct linknodes *linknode;

/* Current node supported by user() */

extern struct usernodes *thisuser;

/* Current node supported by newpeer() */

extern struct peernodes *thispeer;

/* Current node supported by datalink() */

extern struct linknodes *thislink;

/* Dummy User for not existent user Structures */

extern struct usert *dummyuser;
extern struct usert *nulluser;

/* Dummy peer for not existent user peers */

extern struct newpeert *dummypeer;

/* Dummy link for not existent datalinks */

extern struct datalinkt *dummylink;

/* main externals */

extern char logfile[200];
extern char value[200];
extern char accepthost[400];
extern char acceptip[60];
extern unsigned int acceptport;
extern int pid;
extern int highestsocket;
extern int errn;
extern struct hostent *hostinfo;
extern FILE *mainlog;
extern char configfile[100];
extern char dcchost[200];
extern char dcc6host[200];
extern int safemode;
extern int notsocket;

#ifdef INTNET

extern struct uchannelt *intchan;

#endif

/* main communication buffers */

extern char ircbuf[8192]; 
extern int datalen;

/* splitted parameters */

extern char ircfrom[300]; /* from nick!user@host, will be set to "" if servermsg or client */
extern char ircident[32]; /* from ident, will be set to "", if servermsg or client */
extern char ircnick[64]; /* from nick, will be set to "" if servermsg or client */
extern char irchost[200]; /* from host, will be set to "" if from client, name of server if from server */
extern char irccommand[64]; /* command or numerical code */
extern char ircto[800]; /* to */
extern char irccontent[6000]; /* content */
extern int ircserver;
extern int inwrite;
extern char rbuf[100];
extern char ehost[400];
extern char eparm[200];
extern char echan[200];
extern int nousers;
extern char me[200];
extern int delayinc;
extern char ctxt[50];
extern char cfunc[60];
extern int cline;

/* hostallows */

extern struct stringarray *hostallows;

#ifdef TRANSLATE

extern struct translatet *translate;


#endif

#ifdef SUNOS
extern int error_num;
#endif

#ifdef HAVE_SSL

extern SSL_CTX *srvctx;
extern SSL_CTX *clnctx;

#endif

#endif

/* p_client externals */

#ifndef P_CLIENT
#ifdef PARTYCHANNEL
int cmdjoin(int usern);
int cmdpart(int usern);
int cmdtopic(int usern);
#endif
int repeatserverinit(int usern);
int cmdbwho(int usern);
int cmdadduser(int usern);
int cmddeluser(int usern);
int cmdaddnetwork(int usern);
int cmddelnetwork(int usern);
int cmdpassword(int usern);
int cmdnick(int usern);
int cmdjump(int usern);
int cmdvhost(int usern);
int cmdautogetdcc(int usern);
int cmdautorejoin(int usern);
int cmdaidle(int usern);
int cmdleavequit(int usern);
int cmdswitchnet(int usern);
int cmdsysmsg(int usern);
int cmdsetaway(int usern);
int cmdsetleavemsg(int usern);
int cmdsetawaynick(int usern);
int cmdsetusername(int usern);
int cmddccenable(int usern);
int cmdaddserver(int usern);
int cmddelserver(int usern);
int cmdlistservers(int usern);
#ifdef PROXYS
int cmdproxy(int usern);
#endif
int cmdlinkto(int usern);
int cmdlinkfrom(int usern);
int cmdrelaylink(int usern);
int cmddellink(int usern);
int cmdlistlinks(int usern);
#ifdef DCCCHAT
int cmdadddcc(int usern);
int cmddeldcc(int usern);
int cmdlistdcc(int usern);
int cmddccchat(int usern);
int cmddccanswer(int usern);
int cmddcccancel(int usern);
#define DCCANCEL
#endif
#ifdef DCCFILES
int cmddccsend(int usern);
int cmddccget(int usern);
int cmddccsendme(int usern);
int cmddccfiles(int usern);
#ifndef DCCANCEL
int cmddcccancel(int usern);
#define DCCANCEL
#endif
#endif
int printhelp(int ausern);
int cmdaddop(int usern);
int cmdaddautoop(int usern);
int cmdaddkey(int usern);
int cmdaddask(int usern);
int cmdaddban(int usern); 
int cmdaddignore(int usern);
#ifdef TRAFFICLOG
int cmdaddlog(int usern); 
#endif
int cmddelop(int usern); 
int cmddelautoop(int usern); 
int cmddelkey(int usern); 
int cmddelask(int usern); 
int cmddelban(int usern); 
int cmddelignore(int usern);
#ifdef TRAFFICLOG
int cmddellog(int usern); 
#endif
int cmdlistops(int usern); 
int cmdlistautoops(int usern); 
int cmdlistkey(int usern); 
int cmdlistask(int usern); 
int cmdlistbans(int usern); 
int cmdlistignores(int usern);
#ifdef TRAFFICLOG
int cmdlistlogs(int usern); 
#endif
int cmdrehash(int usern); 
int cmdadmin(int usern); 
int cmdunadmin(int usern); 
int cmdbkill(int usern); 
int cmdbquit(int usern); 
int cmdbconnect(int usern);
int cmdname(int usern);
int cmdsetlang(int usern);
int cmdquit(int usern);
int cmdping(int usern);
int cmdprivmsg(int usern);
int cmdwho(int usern);
int cmduser(int usern);
int cmdsockstat(int usern);
int cmdrelink(int usern);
#ifdef CRYPT
int cmdencrypt(int usern);
int cmddelencrypt(int usern);
int cmdlistencrypt(int usern);
#endif
#ifdef TRANSLATE
int cmdtranslate(int usern);
int cmddeltranslate(int usern);
int cmdlisttranslate(int usern);
#endif
int cmdaddallow(int usern);
int cmddelallow(int usern);
int cmdlistallow(int usern);
#ifdef CRYPT
int checkcrypt(int usern);
#endif
#ifdef TRANSLATE
int checktranslate(int usern);
#endif
int quitclient(int usern);
int userinerror(int usern, int errn);
int userinkill(int usern);
#ifdef SCRIPTING
int cmdreloadscript(int usern);
int cmdlisttasks(int usern);
#endif
#endif

#ifndef P_CRYPT
char *cryptit(char *tocipher);
char *decryptit(char *tocipher);
extern char slt1[];
extern char slt2[];
#endif

#ifndef P_COREDNS
extern int init_dns_core(void);
extern void dns_check_expires(void);
extern int dns_ack(int flag);
extern int dns_err(int flag, int ern);
extern int resfd;
extern struct resolve *findip(unsigned long ip);
extern struct resolve *findhost(char *hostn);
extern struct resolve *findip6(char *ip6);
extern int dns_forward(char *hostname,int(*resolved)(struct resolve *),int(*unresolved)(struct resolve *), char *data);
extern int dns_lookup(unsigned long ip,int(*resolved)(struct resolve *),int(*unresolved)(struct resolve *), char *data);
extern int dns_lookupv6(unsigned char *v6ip,int(*resolved)(struct resolve *),int(*unresolved)(struct resolve *), char *data);
extern void dns_stat(int usern);
#endif

#ifndef P_DCC
int adddcc(int usern, char *host, int port, char *luser, char *pass, char *npt, int noini);
int loaddccs(int usern);
int listdccs(int usern);
int erasedcc(int usern, int dccn);
struct datalinkt *checkdcc(int usern, char *dccname);
int dcchandler(int usern);
int connecteddcc(int usern);
int killeddcc(int usern);
int errordcc(int usern, int errn);
int checkdcclink(int usern, struct datalinkt *th);
int checkdccs();
int randport();
char *stripdccfile(char *orig,int rec);
struct dcct *createpdcc(int usern);
struct dcct *getcurrentpdcc(int usern);
int removepdcc(int usern);
int pdccconnected(int usern);
int pdccerror(int usern, int r);
int pdccquery(int usern, int r);
int pdccclosed(int usern);
int acceptpdccchat(int usern);
int pdccfileerror(int usern, int r);
int pdccfileclosed(int usern);
int pdccfilesendack(int usern);
int acceptdccfile(int usern);
int pdccfconnected(int usern);
int pdccferror(int usern, int r);
int pdccfget(int usern);
int pdccfclosed(int usern);
int addpendingdcc(int usern, int type, char *host, int port, char *nick, char *file, char *newfile, unsigned long filesize, int proto);
int querydccchat(int usern, char *nick);
int parsectcps(int usern);
int checkdcctimeouts();
int listpdccs(int usern);
#endif

#ifndef P_HASH
int userhelp(int usern, char *cmd);
int userinbound(int usern);
int useroutbound(int usern);
#endif

#ifndef P_INIFUNC
int resetconfig();
int readconfig();
int flushconfig();
int clearsectionconfig(char *pattern);
int getini(char *section, char *param, char *inidat);
int writeini(char *section, char *param, char *inidat, char *data);
int countconfentries(char *section, char *param, char *fname);
int getserver(int srvnr, int usernum);
int getnextserver(int usern);
int deluser(int uid);
extern int lastfree;
#endif

#ifndef P_LINK
int getlink(int peern);
int getlinkbyname(char *namelink);
int getnewlink();
int linkrelay(int npeer, int rootlink);
int linkrelayconnected(int npeer);
int errorrelaylink(int nlink, int errn);
int killrelaylink(int nlink);
int connectlink(int nlink);
int connectedlink(int nlink);
int errorlink(int nlink, int errn);
int checklinks();
int relay(int nlink);
int processlink(int nlink, int sock, int state);
int checklinkerror(int nlink, int errn);
int checklinkkill(int nlink);
int checklinkdata(int nlink);
#endif

#ifndef P_LOG
int oldfile(char *fname);
int p_log(int loglevel,int usern, char *format,...);
int checkforlog(int usern);
int privatelog(int usern);
int playprivatelog(int usern);
int playmainlog(int usern);
#ifdef TRAFFICLOG
int playtrafficlog(int usern);
#endif
int eraseprivatelog(int usern);
int erasemainlog(int usern);
int erasetrafficlog(int usern);
#ifdef TRAFFICLOG
int checklogging(int usern);
#endif
#endif

#ifndef P_MEMORY
unsigned long *__pmalloc(unsigned long size,char *module,char *function,int line);
void _pfree(void * pointer,char *module, char *function, int line);
struct usert *user(int usern);
struct newpeert *newpeer(int usern);
struct datalinkt *datalink(int usern);
int clearuser(int usern);
int clearpeer(int peern);
int clearlink(int linkn);
int errorhandling();
void usr1_error(int r); /* for calling from cmdbsockets */
#endif

#ifndef P_NETWORK
int checkparents(int usernum);
int parentnick(int usern);
int checknetwork(int usern);
int addtoken(int usern);
int checkusernetwork(int usern);
#endif

#ifndef P_PARSE
int parse();
int ifcommand(char *cmd);
#endif

#ifndef P_PEER
int checkpeerhostname(char *hostname);
int getnewpeer();
int erasepeer(int npeer);
int linkpeer(int npeer);
int checknewlistener(int dummy);
void killed();
void errored();
int createlisteners();
int erroroldlistener(int npeer, int ern);
int killoldlistener(int npeer);
int checkoldlistener(int npeer);
#endif

#ifndef P_SERVER
int serverinit(int usern, int msgn);
int pong(int usern);
int nickused(int usern);
int gotnick(int usern);
int rejoinchannels(int usern);
int partleavemsg(int usern);
int sendleavemsg(int usern);
int rejoinclient(int usern);
int checkignores(int usern);
int checkbans(int usern);
int checkautoop(int usern);
int checkop(int usern);
int askforop(int usern, char *channel);
int getops(int usern);
int sendwho(int usern);
int performwho(int usern);
int gotjoin(int usern);
int gotpart(int usern);
int gotkick(int usern);
int gotquit(int usern);
int gotmode(int usern);
int gottopic(int usern);
int msgERROR(int usern);
int msg001(int usern);
int msg002to005(int usern);
int msg251to255265to266(int usern);
int msg372to375(int usern);
int msg376(int usern);
int msg352(int usern);
int msg353(int usern);
int msg315(int usern);
int msg432to437(int usern);
int msg319(int usern);
int msg324(int usern);
int msg332(int usern);
int msg366(int usern);
int msgprivmsg(int usern);
int connectuser(int usern);
int userconnected(int usern);
int usererror(int usern,int errn);
int userclosed(int usern);
int checkclients();
int checkstonednick();
#endif

#ifndef P_SOCKET
extern struct socketnodes *previous;
struct socketnodes *getpsocketbysock(int syssock);
struct socketnodes *getpsocketbygroup(struct socketnodes *first, unsigned long group, int notsock);
int createsocket(int syssock,int type, int index, unsigned long group,int(*constructor)(int),int(*constructed)(int),int(*errorhandler)(int,int),int(*handler)(int),int(*destructor)(int),int(*remapper)(int,int), int protocol,int ssl);
int killsocket(int syssock);
int createlistener(char *host, int port, int proto, int pending, int(*listenhandler)(int),int(*errorhandler)(int,int),int(*datahandler)(int),int(*destructor)(int));
int connectto(int socket,char *host, int port, char *vhost);
int flushsendq(int socket, int forced);
int addq(int socket, char *data, int sqdelay);
#ifdef HAVE_SSL
char *sslgetcert(SSL *sssl); /* returns the cert-string */
int sslcheckcert(int socket,char *cert, char *logitem);
#endif
extern int urgent;
int writesock(int socket, char *data);
int ssnprintf(int socket, char *format,...);
int getprotocol(char *host);
int writesock_URGENT(int socket, char *data);
int writesock_DELAY(int socket, char *data, int delay);
int writesock_STREAM(int socket, unsigned char *data, unsigned int size);
int receivesock(struct psockett *sock);
int socket_accept();
int socket_connect();
unsigned long socketdriver();
int socketdatawaiting(int syssock);
#endif

#ifndef P_STRING
int ucase(char *inc);
char *rtrim(char *totrim);
int replace(char *rps, char whatc, char toc);
char *nobreak(char *tobreak);
char *randstring(int length);
char *strmcat(char *first,char *second);
char *strmncpy(char *dest, char *source, unsigned int len);
int strmncasecmp(char *one, char *two);
int strmcmp(char *one, char *two);
int strmwildcmp(char *line, char *wildcard);
struct stringarray *addstring(char *toadd, struct stringarray *ltm);
char *getstring(int entry, struct stringarray *ltm);
struct stringarray *removestring(int entry, struct stringarray *ltm);
struct stringarray *loadlist(char *afile, struct stringarray *lth);
struct stringarray *writelist(char *host, char *param, char *afile, struct stringarray *th);
struct stringarray *eraselist(int entry, char *afile, struct stringarray *th);
int liststrings(struct stringarray *th, int usern);
int checkstrings(struct stringarray *th);
int loadlanguage(char *lang);
#ifdef lngtxt
#undef lngtxt
#endif
char *lngtxt(int msgnum);
extern char langname[100];
#endif

#ifndef P_SYSMSG
extern int issys;
int sysparty(char *format,...);
int systemnotice(int usernum, char *format,...);
int noticeall(int adminonly,char *format,...);
int broadcast(int nlink);
int broadcasttolink(int nlink);
int querybounce(int usern);
int querybot(int usern);
#ifdef PARTYCHANNEL
int partyadd(char *xuser);
int partyremove(char *xuser);
int partylinklost();
int joinparty(int usern);
#endif
int lostlink(char *which);
#endif

#ifndef P_USERFILE
int loaduser(int usernum);
int writeuser(int usern);
int writelink(int linkn);
int eraselinkini(int linkn);
int loadusers(void);
int loadlink(int linkn);
int loadlinks();
int checkuser(char *nick);
#endif

#ifndef P_UCHANNEL

char *ordermodes(struct uchannelt *channel);
struct uchannelt *addchanneltouser(int usern, char *channel, int nowrite);
struct uchannelt *getuserchannel(int usern, char *channel);
struct uchannelusert *getchannelnick(struct uchannelt *channel, char *nick);
struct uchannelusert *addnicktochannel(struct uchannelt *chan, char *nick, char *ident, char *host);
int hasmode(int usern, char *nick, char *channel, char modesymbol);
int removenickfromchannel(struct uchannelt *chan, char *nick);
int removenickfromallchannels(int usern, char *nick);
int nickchange(int usern, char *oldnick, char *newnick);
int removeallusersfromchannel(struct uchannelt *channel);
int removeallchannelsfromuser(int usern);
int removechannelfromuser(int usern, char *channel);
int resetallchannels(int usern);
int setchannelmode(struct uchannelt *channel, char *mode, char *param);
int sendnames(int usern, struct uchannelt *channel);
int addchannelmode(struct uchannelt *channel, char *mode, char *param);
int setchanneltopic(struct uchannelt *channel, char *topic);
int convertlastwhois(int usern, char *lastwhois);
int loadchannels(int usern);

#endif

#ifdef SCRIPTING
#ifndef P_SCRIPT

struct subtask *getsubtaskbypid(int pid);
int addsubtask(int uid, int pid, int fdin, int fdout, int fderr, char *program, struct scriptt *script);
int startpipe(int usern, char *program,	struct scriptt *script,	int(*setenvironment)(int,struct scriptt *), int(*inboundhandler)(int), int(*outboundhandler)(int), int(*errorlog)(int), int(*terminated)(int,int), int(*destroyed)(int));
int terminatetask(int pid,int errn);
int terminatedest(int pid);
int clearuserscript(int usern);
int getscriptfunction(char *function);
int processscriptline(int usern, char *line);
int loadscript(int usern);
struct scriptt *getnextfunctionentry(struct scriptt *msc, int func);
int setupstandardenv(int usern, struct scriptt *script);
int scripterror(int pid);
int setdialogueenv(int usern, struct scriptt *script);
int dialogueevent(int pid);
int startdialogues(int usern);
int stopdialogues(int usern);
int senddialoguequery(int usern);
int checkserverevent(int usern, struct scriptt *script);
int setserverenv(int usern,struct scriptt *script);
int servertoserverevent(int pid);
int servertoclientevent(int pid);
int serverevent(int usern);
int setaliasenv(int usern, struct scriptt *script);
int checkalias(int usern, struct scriptt *script);
int aliasbhelp(int usern);
int executealias(int usern);
int setdccchatenv(int usern, struct scriptt *script);
int tasktochatevent(int usern);
int dccchatscript(int usern, char *from);
int dccfilescript(int usern, char *from);
int ctcpscript(int usern, char *ctcp);

#endif
#endif

#ifndef P_TOPOLOGY
int displaytopology(int(*callback)(char *output));
int addtopology(char *from, char *to);
int removetopology(char *node, char *item, int(*callback)(char *item));
struct topologyt *gettopology(char *name);
#endif

#ifndef P_INTNET
#ifdef INTNET
int cmdintprivmsg(int usern, int link);
int cmdintnotice(int usern, int link);
int rejoinintchannels(int usern);
int cmdintjoin(int usern, int link);
int cmdintmode(int usern, int link);
int cmdintpart(int usern, int link);
int cmdintnick(int usern, int link);
int cmdintkick(int usern, int link);
int cmdintnames(int usern, int link);
int cmdintquit(int usern, int link);
int cmdintwho(int usern, int link);
int cmdintwhois(int usern, int link);
int cmdintuser(int usern, int link);
int cmdinttopic(int usern, int link);
int cmdintinvite(int usern, int link);
int cmdintison(int usern, int link);
int cmdintuserhost(int usern, int link);
int removeinternal(char *server);
int joinintnettolink(int link);


#endif
#endif

#ifdef TRANSLATE
#ifndef P_TRANSLATE
int addtranslate(int usern, char *totranslate, char *source, char *dest, int direction, char *lang, char *command);
int cleartranslates();
#endif
#endif

/* from 2.2.2: always use the external snprintf. nice feature to allow features */

int ap_snprintf(char *buf,size_t n,char *format,...);
int ap_vsnprintf(char *buf,size_t n, char *format,va_list ap);

/* idea definitions */

#ifdef CRYPT

extern char * IDEA_stringencrypt(unsigned char *input, unsigned char *key);
extern char * IDEA_stringdecrypt(unsigned char *input, unsigned char *key);

#endif

/* blowfish definitions */

extern char * BLOW_stringencrypt(unsigned char *input, unsigned char *key);

#ifdef CRYPT

extern char * BLOW_stringdecrypt(unsigned char *input, unsigned char *key);

#endif

#define pcontext { strmncpy(ctxt,__FILE__,sizeof(ctxt)); strmncpy(cfunc,__FUNCTION__,sizeof(cfunc)); cline=__LINE__; }
#define pmalloc(n) __pmalloc((n),__FILE__,__FUNCTION__,__LINE__)

#define SSLCERT "key/psybnc.cert.pem"
#define SSLKEY "key/psybnc.key.pem"

#ifdef HAVE_SSL
#ifndef SSLSEC
#define SSLSEC 0
#endif
#endif
