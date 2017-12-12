/* $Id: config.h,v 1.2 2005/06/04 18:05:33 hisi Exp $ */
/*
 * Configuration file for psyBNC, created by menuconf
 */

/* Encryption */

#define CRYPT

/* Encryption Type*/

#define BLOWFISH

/* Allow Translation */

#define TRANSLATE

/* Allow internal network */

#define INTNET

/* Allow traffic logging */

#define TRAFFICLOG

/* Allow linkage of bouncers */

#define LINKAGE

/* Allow the dcc File-Functions */

#define DCCFILES

/* Pipe dcc Chats */

#define DCCCHAT

/* Allow to add more users */

#define MULTIUSER

/* Number of max. Users */

#define MAXUSER 50

/* Number of max. Connections per User */

#define MAXCONN 99

/* Allow the usage of scripts */

#define SCRIPTING

/* Allow multiple irc connections per user */

#define NETWORK

/* Allow Proxy Support */

#define PROXYS

/* The logging level */

#define LOGLEVEL 0

/* SSL-Security */

#define SSLSEC 2

/* Blocking DNS is preferred. Non Blocking DNS is experimental */

#ifndef BLOCKDNS
#define BLOCKDNS
#endif
