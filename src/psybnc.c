/* $Id: psybnc.c,v 1.4 2005/06/04 18:00:14 hisi Exp $ */
/************************************************************************
 *   psybnc2.3.2, src/psybnc.c
 *   Copyright (C) 2003 the most psychoid  and
 *                           the cool lam3rz IRC Group, IRCnet
 *			     http://www.psychoid.lam3rz.de
 *
 *            .-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-.
 *             ,----.,----.,-.  ,-.,---.,--. ,-.,----.
 *             |  O ||  ,-' \ \/ / | o ||   \| || ,--'
 *             |  _/ _\  \   \  /  | o< | |\   || |__
 *             |_|  |____/   |__|  |___||_|  \_| \___|
 *                    Version 2.3.2 (c) 1999-2003
 *                            the most psychoid
 *                    and the cool lam3rz Group IRCnet
 *
 *            `-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=tCl=-'
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
static char rcsid[] = "@(#)$Id: psybnc.c,v 1.4 2005/06/04 18:00:14 hisi Exp $";
#endif

#define P_MAIN

#include <p_global.h>
#include <p_data.h>

int slice=0;

#ifdef HAVE_SSL

void initSSL()
{
    SSL_load_error_strings();
    OpenSSL_add_ssl_algorithms();
    srvctx=SSL_CTX_new(SSLv23_server_method());
    /* setting up the server context */
    if(srvctx==NULL)
    {
        p_log(LOG_ERROR,-1,lngtxt(970));
	exit(0x1);
    }
    if (SSL_CTX_use_certificate_file(srvctx, SSLCERT, SSL_FILETYPE_PEM) <= 0)
    {
	p_log(LOG_ERROR,-1,lngtxt(971), SSLCERT);
	exit(0x1);
    }
    if (SSL_CTX_use_PrivateKey_file(srvctx, SSLKEY, SSL_FILETYPE_PEM) <= 0)
    {
	p_log(LOG_ERROR,-1,lngtxt(972), SSLKEY);
	exit(0x1);
    }
    if (!SSL_CTX_check_private_key(srvctx))
    {
    	p_log(LOG_ERROR,-1,lngtxt(973));
    	exit(0x1);
    }
    /* create client context */
    clnctx=SSL_CTX_new(SSLv23_client_method());
    if(clnctx==NULL)
    {
	p_log(LOG_ERROR,-1,lngtxt(974));
	exit(0x1);
    }
    if (SSL_CTX_use_certificate_file(clnctx, SSLCERT, SSL_FILETYPE_PEM) <= 0)
    {
	p_log(LOG_ERROR,-1,lngtxt(975), SSLCERT);
	exit(3);
    }
    if (SSL_CTX_use_PrivateKey_file(clnctx, SSLKEY, SSL_FILETYPE_PEM) <= 0)
    {
    	p_log(LOG_ERROR,-1,lngtxt(976), SSLKEY);
    	exit(4);
    }
    if (!SSL_CTX_check_private_key(clnctx))
    {
	p_log(LOG_ERROR,-1,lngtxt(977));
	exit(5);
    }
    p_log(LOG_INFO,-1,lngtxt(978));
    return;
}

#endif

/* alarm-loop */

int bncalarm(void)
{
    slice++;
    delayinc=1;
    if(slice==1)
	checkclients();
    else 
    if(slice==2)
        checklinks();
    else 
    if(slice==3)
    { 
	checkdccs();
#ifdef TRANSLATE
	cleartranslates();
#endif
	checkdcctimeouts();
	slice=0;
    }
    return;
}

/* main bounce-loop */

int bncmain(void) {
   unsigned long em=0;
   delayinc=1;
   while(1)
   {
       em+=socketdriver();
       if(em>=5)
       {
	   em=0;
           bncalarm();
       }
       checkstonednick();
   }
   return 0x0; /* i wonder how often we get here */
}

/* printing the banner */

int printbanner(void)
{
   int i;
   for(i=979;i<990;i++)
   {
       fprintf(stdout,"%s",lngtxt(i));
       if(i==984)
           fprintf(stdout,"%s", APPVER);
   }
   if(getuid()==0)
       fprintf(stdout,lngtxt(990));
   fflush(stdout);
   return 0x0;
}

/* installation loop */

int
main (int argc, char **argv)
{
  int rc;
  char *pt;
  char buf[200];
  char *bversion;
  FILE *pidfile,*conffile;
  int i;
  if(argc==2)
  {
      strmncpy(configfile,argv[1],sizeof(configfile));
  } else {
      strcpy(configfile,"psybnc.conf");  /* rcsid */
  }
  conffile=fopen(configfile,"r");
  if(conffile==NULL)
  {
     printf("Configuration File %s not found, aborting\nRun 'make menuconfig' for creating a configuration or create the file manually.\n",conffile); /* rcsid */
     exit (0x0);
  }
  fclose(conffile);
  readconfig();
  rc = getini("SYSTEM","LANGUAGE",INIFILE);  /* rcsid */
  if(rc!=0)
  {
      rc=loadlanguage("english");  /* rcsid */
  } else {
      rc=loadlanguage(value);  
      if(rc<0)
          rc=loadlanguage("english");  /* rcsid */
  }
  if(rc<0)
  {
      printf("Could not load language file, aborting.\n");  /* rcsid */
      exit(0x0);	  
  }
  printbanner();
  printf(lngtxt(991),configfile);
  printf(lngtxt(992),langname);
  ap_snprintf(logfile,sizeof(logfile),lngtxt(993));
  rc = getini(lngtxt(994),lngtxt(995),INIFILE);
  if (rc != 0) {
     printf(lngtxt(996));
     exit (0x0);
  }
  listenport = atoi(value);
  rc = getini(lngtxt(997),"ME",INIFILE);
  if (rc < 0) {
     memset(value,0x0,sizeof(value));       
  }
  pt=strchr(value,' '); /* shortening bouncername to no spaces */
  if(pt)
      *pt=0;
  ap_snprintf(me,sizeof(me),"%s",value);
  rc = getini(lngtxt(998),lngtxt(999),INIFILE);
  if (rc < 0) {
     printf(lngtxt(1000));
     ap_snprintf(value,sizeof(value),lngtxt(1001));
  }
  ap_snprintf(logfile,sizeof(logfile),"%s",value);
  oldfile(logfile);
  /* creating the socket-root */
  socketnode=(struct socketnodes *) pmalloc(sizeof(struct usernodes));
  socketnode->sock=NULL;
  socketnode->next=NULL;
  /* creating the demon socket */
  rc = createlisteners();
  if (rc == 0) {
    printf(lngtxt(1002));
    exit (0x0);
  }
  /* creating background */
  pidfile = fopen(lngtxt(1003),"w");
  if(pidfile==NULL)
  {
      printf(lngtxt(1004));
      exit(0x0);
  }
  if(mainlog!=NULL)
  {
      fclose(mainlog);
      mainlog=NULL;
  }
  fflush(stdout);
  pid = fork();
  if (pid < 0) {
  
  }
  if (pid == 0) {
     rc= errorhandling();
     makesalt();
#ifdef HAVE_SSL
     initSSL();
     pcontext;
#endif
     U_CREATE=0;
#ifdef PARTYCHANNEL
     /* partychannel setup */
     strmncpy(partytopic,lngtxt(1005),sizeof(partytopic));
     partyusers=NULL;
#endif
     /* creating the usernode-root */
     usernode=(struct usernodes *) pmalloc(sizeof(struct usernodes));
     usernode->uid=0;
     usernode->user=NULL;
     usernode->next=NULL;
     /* creating the newpeer-root */
     peernode=(struct peernodes *) pmalloc(sizeof(struct peernodes));
     peernode->uid=0;
     peernode->peer=NULL;
     peernode->next=NULL;
     /* creating the datalink-root */
     linknode=(struct linknodes *) pmalloc(sizeof(struct linknodes));
     linknode->uid=0;
     linknode->link=NULL;
     linknode->next=NULL;
     /* loading the users */
     loadusers();
     loadlinks();
     pcontext;
     /* loading the hostallows */
     hostallows=loadlist(lngtxt(1006),hostallows);
  }
  pcontext;
  if (pid) {
     bversion=buildversion();
     printf(lngtxt(1007),bversion,pid);
     p_log(LOG_INFO,-1,lngtxt(1008),bversion,pid);
     fprintf( pidfile,"%d\n",pid);
     fclose(pidfile);
     exit (0x0);
  }  
  pcontext;
#ifndef BLOCKDNS
  if(init_dns_core()==0)
  {
     dns_err(0,1);
  }
#endif
  bncmain();
}

