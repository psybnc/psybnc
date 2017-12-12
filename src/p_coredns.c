/* $Id: p_coredns.c,v 1.4 2005/06/04 18:00:14 hisi Exp $ */
/* coredns - Borrowed from eggdrop1.6.6
 * rewritten and optimized for psybnc and ipv6-resolves by psychoid
 */
/*
 * dnscore.c -- part of dns.mod
 *   This file contains all core functions needed for the eggdrop dns module.
 *   Many of them are only minimaly modified from the original source.
 *
 * Modified/written by Fabian Knittel <fknittel@gmx.de>
 *
 * $Id: p_coredns.c,v 1.4 2005/06/04 18:00:14 hisi Exp $
 */
/*
 * Portions Copyright (C) 1999, 2000, 2001 Eggheads Development Team
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

/*
 * Borrowed from mtr  --  a network diagnostic tool
 * Copyright (C) 1997,1998  Matt Kimball <mkimball@xmission.com>
 * Released under the GPL, as above.
 *
 * Non-blocking DNS portion --
 * Copyright (C) 1998  Simon Kirby <sim@neato.org>
 * Released under the GPL, as above.
 */

#include "config.h"
#define P_COREDNS

#ifndef BLOCKDNS

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <arpa/nameser.h>
#include <resolv.h>
#include <errno.h>
#include "p_global.h"

/* Defines */

#define BASH_SIZE	 8192		/* Size of hash tables */
#define HOSTNAMELEN 	  255		/* From RFC */
#define RES_RETRYDELAY      1
#define RES_MAXSENDS        3
#define RES_FAILEDDELAY   600		/* TTL for failed records (in
					   seconds). */
#define RES_MAX_TTL     86400		/* Maximum TTL (in seconds). */

#define RES_ERR "DNS Resolver error: "
#define RES_MSG "DNS Resolver: "
#define RES_WRN "DNS Resolver warning: "

#define MAX_PACKETSIZE (PACKETSZ)
#define MAX_DOMAINLEN (MmoAXDNAME)

/* Macros */

#define nonull(s) (s) ? s : nullstring
#define BASH_MODULO(x) ((x) & 8191)	/* Modulo for hash table size */

/* Non-blocking nameserver interface routines */

typedef struct {
    u_16bit_t	id;			/* Packet id */
    u_8bit_t	databyte_a;
    /* rd:1				recursion desired
     * tc:1				truncated message
     * aa:1				authoritive answer
     * opcode:4				purpose of message
     * qr:1				response flag
     */
    u_8bit_t	databyte_b;
    /* rcode:4				response code
     * unassigned:2			unassigned bits
     * pr:1				primary server required (non standard)
     * ra:1				recursion available
     */
    u_16bit_t	qdcount;	/* Query record count */
    u_16bit_t	ancount;	/* Answer record count */
    u_16bit_t	nscount;	/* Authority reference record count */
    u_16bit_t	arcount;	/* Resource reference record count */
} packetheader;

#ifndef HFIXEDSZ
#define HFIXEDSZ (sizeof(packetheader))
#endif

/*
 * Byte order independent macros for packetheader
 */
#define getheader_rd(x) (x->databyte_a & 1)
#define getheader_tc(x) ((x->databyte_a >> 1) & 1)
#define getheader_aa(x) ((x->databyte_a >> 2) & 1)
#define getheader_opcode(x) ((x->databyte_a >> 3) & 15)
#define getheader_qr(x) (x->databyte_a >> 7)
#define getheader_rcode(x) (x->databyte_b & 15)
#define getheader_pr(x) ((x->databyte_b >> 6) & 1)
#define getheader_ra(x) (x->databyte_b >> 7)

//#define sucknetword(x)  ((x)+=2,((u_16bit_t)  (((x)[-2] <<  8) | ((x)[-1] <<  0))))
#define sucknetword(x)  ((x)+=2,((u_16bit_t)  (((x)[-2] <<  8) | ((x)[-1] <<  0))))
#define sucknetshort(x) ((x)+=2,((short) (((x)[-2] <<  8) | ((x)[-1] <<  0))))
#define sucknetdword(x) ((x)+=4,((dword) (((x)[-4] << 24) | ((x)[-3] << 16) | \
                                          ((x)[-2] <<  8) | ((x)[-1] <<  0))))
#define sucknetlong(x)  ((x)+=4,((long)  (((x)[-4] << 24) | ((x)[-3] << 16) | \
                                          ((x)[-2] <<  8) | ((x)[-1] <<  0))))


static u_8bit_t resrecvbuf[((MAX_PACKETSIZE + 7) >> 2)<<2];	/* MUST BE DWORD ALIGNED */

static struct resolve *idbash[BASH_SIZE];
static struct resolve *ipbash[BASH_SIZE];
static struct resolve *hostbash[BASH_SIZE];
static struct resolve *ipv6bash[BASH_SIZE];
static struct resolve *expireresolves = NULL;

static u_32bit_t localhost;

static long idseed = 0xdeadbeef;
static long aseed;

static int resfd;

static struct resolve dummyrp;

static char tempstring[512];
static char namestring[1024 + 1];
static char stackstring[1024 + 1];
static const char nullstring[] = "";

/*
 *    Miscellaneous helper functions
 */

/* Allocate memory to hold one resolve request structure.
 */
static struct resolve *allocresolve()
{
    struct resolve *rp;

    rp = (struct resolve *) pmalloc(sizeof(struct resolve));
    return rp;
}

static int ip6cmp(u_8bit_t *ip61, u_8bit_t *ip62)
{
    int i;
    int ret=0;
    for(i=0;i<16;i++)
    {
	if(ip61[i]>ip62[i]) { ret=1; break; }
	if(ip61[i]<ip62[i]) { ret=-1; break; }
    }
    return ret;
}

/*
 *    Hash and linked-list related functions
 */

/* Return the hash bucket number for id.
 */
inline static u_32bit_t getidbash(u_16bit_t id)
{
    return (u_32bit_t) BASH_MODULO(id);
}

/* Return the hash bucket number for ip.
 */
inline static u_32bit_t getipbash(u_32bit_t ip)
{
    return (u_32bit_t) BASH_MODULO(ip);
}

/* Return the hash bucket number for host.
 */
static u_32bit_t gethostbash(char *host)
{
    u_32bit_t bashvalue = 0;

    for (; *host; host++) {
	bashvalue ^= *host;
	bashvalue += (*host >> 1) + (bashvalue >> 1);
    }
    return BASH_MODULO(bashvalue);
}

/* Return the hash bucket number for a ipv6 ip.
 */
static u_32bit_t getip6bash(u_8bit_t *ip6)
{
    u_32bit_t bashvalue = 0;
    int i;

    for (i=0; i<16; i++) {
	bashvalue ^= ip6[i];
	bashvalue += (ip6[i] >> 1) + (bashvalue >> 1);
    }
    return BASH_MODULO(bashvalue);
}

/* Insert request structure addrp into the id hash table.
 */
static void linkresolveid(struct resolve *addrp)
{
    struct resolve *rp;
    u_32bit_t bashnum;
    bashnum = getidbash(addrp->id);
    rp = idbash[bashnum];
    if (rp) {
	while ((rp->nextid) && (addrp->id > rp->nextid->id))
	    rp = rp->nextid;
	while ((rp->previousid) && (addrp->id < rp->previousid->id))
	    rp = rp->previousid;
	if (rp->id < addrp->id) {
	    addrp->previousid = rp;
	    addrp->nextid = rp->nextid;
	    if (rp->nextid)
		rp->nextid->previousid = addrp;
	    rp->nextid = addrp;
	} else if (rp->id > addrp->id) {
	    addrp->previousid = rp->previousid;
	    addrp->nextid = rp;
	    if (rp->previousid)
		rp->previousid->nextid = addrp;
	    rp->previousid = addrp;
	} else		/* Trying to add the same id! */
	    return;
    } else
	addrp->nextid = addrp->previousid = NULL;
    idbash[bashnum] = addrp;
}

/* Remove request structure rp from the id hash table.
 */
static void unlinkresolveid(struct resolve *rp)
{
    u_32bit_t bashnum;

    bashnum = getidbash(rp->id);
    if (idbash[bashnum] == rp) {
	if (rp->previousid)
	    idbash[bashnum] = rp->previousid;
	else
	    idbash[bashnum] = rp->nextid;
    }
    if (rp->nextid)
	rp->nextid->previousid = rp->previousid;
    if (rp->previousid)
	rp->previousid->nextid = rp->nextid;
	
}

/* Insert request structure addrp into the host hash table.
 */
static void linkresolvehost(struct resolve *addrp)
{
    struct resolve *rp;
    u_32bit_t bashnum;
    int ret;

    bashnum = gethostbash(addrp->hostn);
    rp = hostbash[bashnum];
    if (rp) {
	while ((rp->nexthost) &&
	       (strcasecmp(addrp->hostn, rp->nexthost->hostn) < 0))
	    rp = rp->nexthost;
	while ((rp->previoushost) &&
	       (strcasecmp(addrp->hostn, rp->previoushost->hostn) > 0))
	    rp = rp->previoushost;
	ret = strcasecmp(addrp->hostn, rp->hostn);
	if (ret < 0) {
	    addrp->previoushost = rp;
	    addrp->nexthost = rp->nexthost;
	    if (rp->nexthost)
		rp->nexthost->previoushost = addrp;
	    rp->nexthost = addrp;
	} else if (ret > 0) {
	    addrp->previoushost = rp->previoushost;
	    addrp->nexthost = rp;
	    if (rp->previoushost)
		rp->previoushost->nexthost = addrp;
	    rp->previoushost = addrp;
	} else		/* Trying to add the same host! */
	    return;
    } else
	addrp->nexthost = addrp->previoushost = NULL;
    hostbash[bashnum] = addrp;
}

/* Remove request structure rp from the host hash table.
 */
static void unlinkresolvehost(struct resolve *rp)
{
    u_32bit_t bashnum;

    bashnum = gethostbash(rp->hostn);
    if (hostbash[bashnum] == rp) {
	if (rp->previoushost)
	    hostbash[bashnum] = rp->previoushost;
	else
	    hostbash[bashnum] = rp->nexthost;
    }
    if (rp->nexthost)
	rp->nexthost->previoushost = rp->previoushost;
    if (rp->previoushost)
	rp->previoushost->nexthost = rp->nexthost;
}

/* Insert request structure addrp into the ipv6 hash table.
 */
static void linkresolveip6(struct resolve *addrp)
{
    struct resolve *rp;
    u_32bit_t bashnum;
    int ret;

    bashnum = getip6bash(addrp->ip6);
    rp = ipv6bash[bashnum];
    if (rp) {
	while ((rp->nextip6) &&
	       (ip6cmp(addrp->ip6, rp->nexthost->ip6) < 0))
	    rp = rp->nextip6;
	while ((rp->previousip6) &&
	       (ip6cmp(addrp->ip6, rp->previoushost->ip6) > 0))
	    rp = rp->previousip6;
	ret = ip6cmp(addrp->ip6, rp->ip6);
	if (ret < 0) {
	    addrp->previousip6 = rp;
	    addrp->nextip6 = rp->nextip6;
	    if (rp->nextip6)
		rp->nextip6->previousip6 = addrp;
	    rp->nextip6 = addrp;
	} else if (ret > 0) {
	    addrp->previousip6 = rp->previousip6;
	    addrp->nextip6 = rp;
	    if (rp->previousip6)
		rp->previousip6->nextip6 = addrp;
	    rp->previousip6 = addrp;
	} else		/* Trying to add the same host! */
	    return;
    } else
	addrp->nextip6 = addrp->previousip6 = NULL;
    ipv6bash[bashnum] = addrp;
}

/* Remove request structure rp from the ip6 hash table.
 */
static void unlinkresolveip6(struct resolve *rp)
{
    u_32bit_t bashnum;

    bashnum = getip6bash(rp->ip6);
    if (ipv6bash[bashnum] == rp) {
	if (rp->previousip6)
	    ipv6bash[bashnum] = rp->previousip6;
	else
	    ipv6bash[bashnum] = rp->nextip6;
    }
    if (rp->nextip6)
	rp->nextip6->previousip6 = rp->previousip6;
    if (rp->previousip6)
	rp->previousip6->nextip6 = rp->nextip6;
}

/* Insert request structure addrp into the ip hash table.
 */
static void linkresolveip(struct resolve *addrp)
{
    struct resolve *rp;
    u_32bit_t bashnum;

    bashnum = getipbash(addrp->ip);
    rp = ipbash[bashnum];
    if (rp) {
	while ((rp->nextip) && (addrp->ip > rp->nextip->ip))
	    rp = rp->nextip;
	while ((rp->previousip) && (addrp->ip < rp->previousip->ip))
	    rp = rp->previousip;
	if (rp->ip < addrp->ip) {
	    addrp->previousip = rp;
	    addrp->nextip = rp->nextip;
	    if (rp->nextip)
		rp->nextip->previousip = addrp;
	    rp->nextip = addrp;
	} else if (rp->ip > addrp->ip) {
	    addrp->previousip = rp->previousip;
	    addrp->nextip = rp;
	    if (rp->previousip)
		rp->previousip->nextip = addrp;
	    rp->previousip = addrp;
	} else		/* Trying to add the same ip! */
	    return;
    } else
	addrp->nextip = addrp->previousip = NULL;
    ipbash[bashnum] = addrp;
}

/* Remove request structure rp from the ip hash table.
 */
static void unlinkresolveip(struct resolve *rp)
{
    u_32bit_t bashnum;

    bashnum = getipbash(rp->ip);
    if (ipbash[bashnum] == rp) {
	if (rp->previousip)
	    ipbash[bashnum] = rp->previousip;
	else
	    ipbash[bashnum] = rp->nextip;
    }
    if (rp->nextip)
	rp->nextip->previousip = rp->previousip;
    if (rp->previousip)
	rp->previousip->nextip = rp->nextip;
}

/* Add request structure rp to the expireresolves list. Entries are sorted
 * by expire time.
 */
static void linkresolve(struct resolve *rp)
{
    struct resolve *irp;

    if (expireresolves) {
	irp = expireresolves;
	while (irp->next)
	{
	    if(irp==rp) return;    
	    irp = irp->next;	
	}
	irp = expireresolves;
	while ((irp->next) && (rp->expiretime >= irp->expiretime))
	    irp = irp->next;
	if (rp->expiretime >= irp->expiretime) {
	    rp->next = NULL;
	    rp->previous = irp;
	    irp->next = rp;
	} else {
	    rp->previous = irp->previous;
	    rp->next = irp;
	    if (irp->previous)
		irp->previous->next = rp;
	    else
		expireresolves = rp;
	    irp->previous = rp;
	}
    } else {
	rp->next = NULL;
	rp->previous = NULL;
	expireresolves = rp;
    }
}

/* Remove reqeust structure rp from the expireresolves list.
 */
static void untieresolve(struct resolve *rp)
{
    if (rp->previous)
	rp->previous->next = rp->next;
    else
	expireresolves = rp->next;
    if (rp->next)
	rp->next->previous = rp->previous;
}

/* Remove request structure rp from all lists and hash tables and
 * then delete and free the structure
 */
static void unlinkresolve(struct resolve *rp)
{

    untieresolve(rp);		/* Not really needed. Left in to be on the
				   safe side. */
    unlinkresolveid(rp);
    unlinkresolveip(rp);
    unlinkresolveip6(rp);
    if (rp->hostn[0])
	unlinkresolvehost(rp);
    free(rp);
}

/* Find request structure using the id.
 */
static struct resolve *findid(u_16bit_t id)
{
    struct resolve *rp;
    int bashnum;

    bashnum = getidbash(id);
    rp = idbash[bashnum];
    if (rp) {
	while ((rp->nextid) && (id >= rp->nextid->id))
	    rp = rp->nextid;
	while ((rp->previousid) && (id <= rp->previousid->id))
	    rp = rp->previousid;
	if (id == rp->id) {
	    idbash[bashnum] = rp;
	    return rp;
	} else
	    return NULL;
    }
    return rp;			/* NULL */
}

/* Find request structure using the host.
 */
struct resolve *findhost(char *hostn)
{
    struct resolve *rp;
    int bashnum;

    bashnum = gethostbash(hostn);
    rp = hostbash[bashnum];
    if (rp) {
	while ((rp->nexthost)
	       && (strcasecmp(hostn, rp->nexthost->hostn) >= 0))
	    rp = rp->nexthost;
	while ((rp->previoushost)
	       && (strcasecmp(hostn, rp->previoushost->hostn) <= 0))
	    rp = rp->previoushost;
	if (strcasecmp(hostn, rp->hostn))
	    return NULL;
	else {
	    hostbash[bashnum] = rp;
	    return rp;
	}
    }
    return rp;			/* NULL */
}

/* Find request structure using the v6 ip.
 */
struct resolve *findip6(char *ip6)
{
    struct resolve *rp;
    int bashnum;

    bashnum = getip6bash(ip6);
    rp = ipv6bash[bashnum];
    if (rp) {
	while ((rp->nextip6)
	       && (ip6cmp(ip6, rp->nextip6->ip6) >= 0))
	    rp = rp->nextip6;
	while ((rp->previousip6)
	       && (ip6cmp(ip6, rp->previousip6->ip6) <= 0))
	    rp = rp->previousip6;
	if (ip6cmp(ip6, rp->ip6))
	    return NULL;
	else {
	    ipv6bash[bashnum] = rp;
	    return rp;
	}
    }
    return rp;			/* NULL */
}

/* Find request structure using the ip.
 */
struct resolve *findip(u_32bit_t ip)
{
    struct resolve *rp;
    u_32bit_t bashnum;

    bashnum = getipbash(ip);
    rp = ipbash[bashnum];
    if (rp) {
	while ((rp->nextip) && (ip >= rp->nextip->ip))
	    rp = rp->nextip;
	while ((rp->previousip) && (ip <= rp->previousip->ip))
	    rp = rp->previousip;
	if (ip == rp->ip) {
	    ipbash[bashnum] = rp;
	    return rp;
	} else
	    return NULL;
    }
    return rp;			/* NULL */
}


/*
 *    Network and resolver related functions
 */

/* Create packet for the request and send it to all available nameservers.
 */
static void dorequest(char *s, int type, u_16bit_t id)
{
    packetheader *hp;
    int r, i;
    u_8bit_t buf[(MAX_PACKETSIZE / sizeof(u_8bit_t)) + 1];

    r = res_mkquery(QUERY, s, C_IN, type, NULL, 0, NULL, buf,
		    MAX_PACKETSIZE);
    if (r == -1) {
	return;
    }
    hp = (packetheader *) buf;
    hp->id = id;	/* htons() deliberately left out (redundant) */
    for (i = 0; i < _res.nscount; i++)
	(void) sendto(resfd, buf, r, 0,
		      (struct sockaddr *) &_res.nsaddr_list[i],
		      sizeof(struct sockaddr));
}

/* (Re-)send request with existing id.
 */
static void resendrequest(struct resolve *rp, int type)
{
    rp->type=type;
    rp->sends++;
    /* Update expire time */
    rp->expiretime = time(NULL) + (RES_RETRYDELAY * rp->sends);
    /* Add (back) to expire list */
    untieresolve(rp);
    linkresolve(rp);

#ifdef IPV6
    if (type == T_A || type == T_AAAA) {
#else
    if (type == T_A) {
#endif
	dorequest(rp->hostn, type, rp->id);
    } else if (type == T_PTR) {
	if(IS_PTR6(rp))
	{
	    /* this is an IPV6 Pointer-Request */
	    ap_snprintf(tempstring,sizeof(tempstring),
	        "%-01x.%-01x.%-01x.%-01x.%-01x.%-01x.%-01x.%-01x.%-01x.%-01x.%-01x.%-01x.%-01x.%-01x.%-01x.%-01x.%-01x.%-01x.%-01x.%-01x.%-01x.%-01x.%-01x.%-01x.%-01x.%-01x.%-01x.%-01x.%-01x.%-01x.%-01x.%-01x.ip6.int",
		    ((u_8bit_t)rp->ip6[15] & 0xf),
		    ((u_8bit_t)rp->ip6[15] >>4),		
		    ((u_8bit_t)rp->ip6[14] & 0xf),
		    ((u_8bit_t)rp->ip6[14] >>4),		
		    ((u_8bit_t)rp->ip6[13] & 0xf),
		    ((u_8bit_t)rp->ip6[13] >>4),		
		    ((u_8bit_t)rp->ip6[12] & 0xf),
		    ((u_8bit_t)rp->ip6[12] >>4),		
		    ((u_8bit_t)rp->ip6[11] & 0xf),
		    ((u_8bit_t)rp->ip6[11] >>4),		
		    ((u_8bit_t)rp->ip6[10] & 0xf),
		    ((u_8bit_t)rp->ip6[10] >>4),		
		    ((u_8bit_t)rp->ip6[9] & 0xf),
		    ((u_8bit_t)rp->ip6[9] >>4),		
		    ((u_8bit_t)rp->ip6[8] & 0xf),
		    ((u_8bit_t)rp->ip6[8] >>4),		
		    ((u_8bit_t)rp->ip6[7] & 0xf),
		    ((u_8bit_t)rp->ip6[7] >>4),		
		    ((u_8bit_t)rp->ip6[6] & 0xf),
		    ((u_8bit_t)rp->ip6[6] >>4),		
		    ((u_8bit_t)rp->ip6[5] & 0xf),
		    ((u_8bit_t)rp->ip6[5] >>4),		
		    ((u_8bit_t)rp->ip6[4] & 0xf),
		    ((u_8bit_t)rp->ip6[4] >>4),		
		    ((u_8bit_t)rp->ip6[3] & 0xf),
		    ((u_8bit_t)rp->ip6[3] >>4),		
		    ((u_8bit_t)rp->ip6[2] & 0xf),
		    ((u_8bit_t)rp->ip6[2] >>4),		
		    ((u_8bit_t)rp->ip6[1] & 0xf),
		    ((u_8bit_t)rp->ip6[1] >>4),		
		    ((u_8bit_t)rp->ip6[0] & 0xf),
		    ((u_8bit_t)rp->ip6[0] >>4));
	    dorequest(tempstring, type, rp->id);
	} else {
	    /* this is an IPV4 Pointer-Request */
	    ap_snprintf(tempstring,sizeof(tempstring), "%u.%u.%u.%u.in-addr.arpa",
		((u_8bit_t *) & rp->ip)[3],
		((u_8bit_t *) & rp->ip)[2],
		((u_8bit_t *) & rp->ip)[1], ((u_8bit_t *) & rp->ip)[0]);
	    dorequest(tempstring, type, rp->id);
	}
    }
}

/* Send request for the first time.
 */
static void sendrequest(struct resolve *rp, int type)
{
    /* Create unique id */
    do {
	idseed = (((idseed + idseed) | (long) time(NULL))
		  + idseed - 0x54bad4a) ^ aseed;
	aseed ^= idseed;
	rp->id = (u_16bit_t) idseed;
    } while (findid(rp->id));
    linkresolveid(rp);		/* Add id to id hash table */
    resendrequest(rp, type);	/* Send request */
}

/* Events on resolving / not resolving */

void dns_event_success(struct resolve *rp,int type)
{
    int rc;
    if(!rp) return;
    if(rp->resolved!=NULL)
    {
	rc=(*rp->resolved)(rp);
    }
}

void dns_event_failure(struct resolve *rp,int type)
{
    int rc;
    if(!rp) return;
    if(rp->resolved!=NULL)
    {
	rc=(*rp->unresolved)(rp);
    }
}

/* Gets called as soon as the request turns out to be successful. Calls
 * the event handlers.
 */
static void passrp(struct resolve *rp, long ttl, int type)
{
    rp->state = STATE_FINISHED;

    /* Do not cache entries for too long. */
    if (ttl < RES_MAX_TTL)
	rp->expiretime = time(NULL) + (time_t) ttl;
    else
	rp->expiretime = time(NULL) + RES_MAX_TTL;

    /* Expire time was changed, reinsert entry to maintain order */
    untieresolve(rp);
    linkresolve(rp);

    dns_event_success(rp, type);
}

/* Gets called as soon as the request turns out to have failed. Calls
 * the events.
 */
static void failrp(struct resolve *rp, int type)
{
    char *ntoa;
    struct in_addr inaddr;
    if (rp->state == STATE_FINISHED)
	return;
    if(type==T_PTR)
    {
	/* If a pointer fails, map it on ntoa */
#ifdef IPV6
	if(rp->protocol==AF_INET6)
	{
	    ap_snprintf(rp->hostn,sizeof(rp->hostn),"%01x:%01x:%01x:%01x:%01x:%01x:%01x:%01x",
			((((u_16bit_t)rp->ip6[0]) << 8) + (u_16bit_t)rp->ip6[1]),
			((((u_16bit_t)rp->ip6[2]) << 8) + (u_16bit_t)rp->ip6[3]),
			((((u_16bit_t)rp->ip6[4]) << 8) + (u_16bit_t)rp->ip6[5]),
			((((u_16bit_t)rp->ip6[6]) << 8) + (u_16bit_t)rp->ip6[7]),
			((((u_16bit_t)rp->ip6[8]) << 8) + (u_16bit_t)rp->ip6[9]),
			((((u_16bit_t)rp->ip6[10]) << 8) + (u_16bit_t)rp->ip6[11]),
			((((u_16bit_t)rp->ip6[12]) << 8) + (u_16bit_t)rp->ip6[13]),
			((((u_16bit_t)rp->ip6[14]) << 8) + (u_16bit_t)rp->ip6[15]));
	    passrp(rp,300,type); /* only 5 minutes ttl on no resolve */
	} else {	    	
#endif      
	    inaddr.s_addr=rp->ip;
	    ntoa=inet_ntoa(inaddr);
	    strmncpy(rp->hostn,ntoa,sizeof(rp->hostn));
	    passrp(rp,300,type); /* 5 minutes, see above */
#ifdef IPV6
	}
#endif
	return;
    }
    rp->expiretime = time(NULL) + RES_FAILEDDELAY;
    rp->state = STATE_FAILED;
    /* Expire time was changed, reinsert entry to maintain order */
    untieresolve(rp);
    linkresolve(rp);
    dns_event_failure(rp, type);
}

/* Parses the response packets received.
 */
static void parserespacket(u_8bit_t *s, unsigned long l)
{
    struct resolve *rp;
    packetheader *hp;
    unsigned long eob;
    u_8bit_t *c;
    unsigned long g;
    long ttl;
    int r, usefulanswer;
    u_16bit_t rr, datatype, class, qdatatype, qclass;
    u_16bit_t rdatalength;

    if (l < sizeof(packetheader)) {
	return;
    }
    if (l == sizeof(packetheader)) {
	return;
    }
    hp = (packetheader *) s;
    /* Convert data to host byte order
     *
     * hp->id does not need to be redundantly byte-order flipped, it
     * is only echoed by nameserver
     */
    rp = findid(hp->id);
    if (!rp)
	return;
    if ((rp->state == STATE_FINISHED) || (rp->state == STATE_FAILED))
	return;
    hp->qdcount = ntohs(hp->qdcount);
    hp->ancount = ntohs(hp->ancount);
    hp->nscount = ntohs(hp->nscount);
    hp->arcount = ntohs(hp->arcount);
    if (getheader_tc(hp)) {	/* Packet truncated */
	return;
    }
    if (!getheader_qr(hp)) {	/* Not a reply */
	return;
    }
    if (getheader_opcode(hp)) {	/* Not opcode 0 (standard query) */
	return;
    }
    eob = (unsigned long)s + l;
    c = s + HFIXEDSZ;
    switch (getheader_rcode(hp)) {
    case NOERROR:
	if (hp->ancount) {
	    if (hp->qdcount != 1) {
		return;
	    }
	    if ((unsigned long)c > eob) {
		return;
	    }
	    switch (rp->state) {	/* Construct expected query reply */
	    case STATE_PTR6REQ:
		/* this is an IPV6 Pointer-Request */
		ap_snprintf(stackstring,sizeof(stackstring),
	    		"%-01x.%-01x.%-01x.%-01x.%-01x.%-01x.%-01x.%-01x.%-01x.%-01x.%-01x.%-01x.%-01x.%-01x.%-01x.%-01x.%-01x.%-01x.%-01x.%-01x.%-01x.%-01x.%-01x.%-01x.%-01x.%-01x.%-01x.%-01x.%-01x.%-01x.%-01x.%-01x.ip6.int",
			    ((u_8bit_t)rp->ip6[15] & 0xf),
			    ((u_8bit_t)rp->ip6[15] >>4),		
			    ((u_8bit_t)rp->ip6[14] & 0xf),
			    ((u_8bit_t)rp->ip6[14] >>4),		
			    ((u_8bit_t)rp->ip6[13] & 0xf),
			    ((u_8bit_t)rp->ip6[13] >>4),		
			    ((u_8bit_t)rp->ip6[12] & 0xf),
			    ((u_8bit_t)rp->ip6[12] >>4),		
			    ((u_8bit_t)rp->ip6[11] & 0xf),
			    ((u_8bit_t)rp->ip6[11] >>4),		
			    ((u_8bit_t)rp->ip6[10] & 0xf),
			    ((u_8bit_t)rp->ip6[10] >>4),		
			    ((u_8bit_t)rp->ip6[9] & 0xf),
			    ((u_8bit_t)rp->ip6[9] >>4),		
			    ((u_8bit_t)rp->ip6[8] & 0xf),
			    ((u_8bit_t)rp->ip6[8] >>4),		
			    ((u_8bit_t)rp->ip6[7] & 0xf),
			    ((u_8bit_t)rp->ip6[7] >>4),		
			    ((u_8bit_t)rp->ip6[6] & 0xf),
			    ((u_8bit_t)rp->ip6[6] >>4),		
			    ((u_8bit_t)rp->ip6[5] & 0xf),
			    ((u_8bit_t)rp->ip6[5] >>4),		
			    ((u_8bit_t)rp->ip6[4] & 0xf),
			    ((u_8bit_t)rp->ip6[4] >>4),		
			    ((u_8bit_t)rp->ip6[3] & 0xf),
			    ((u_8bit_t)rp->ip6[3] >>4),		
			    ((u_8bit_t)rp->ip6[2] & 0xf),
			    ((u_8bit_t)rp->ip6[2] >>4),		
			    ((u_8bit_t)rp->ip6[1] & 0xf),
			    ((u_8bit_t)rp->ip6[1] >>4),		
			    ((u_8bit_t)rp->ip6[0] & 0xf),
			    ((u_8bit_t)rp->ip6[0] >>4));
		break;	    
	    case STATE_PTRREQ:
		ap_snprintf(stackstring,sizeof(stackstring),
    		    "%u.%u.%u.%u.in-addr.arpa",
			((u_8bit_t *) & rp->ip)[3],
			((u_8bit_t *) & rp->ip)[2],
			((u_8bit_t *) & rp->ip)[1], ((u_8bit_t *) & rp->ip)[0]);
		break;
	    case STATE_AREQ:
		strmncpy(stackstring, rp->hostn, sizeof(stackstring));
	    case STATE_AAAAREQ:
		strmncpy(stackstring, rp->hostn, sizeof(stackstring));
	    }
	    *namestring = '\0';
	    r = dn_expand(s, s+l, c, namestring, MAXDNAME);
	    if (r == -1) {
		return;
	    }
	    namestring[strlen(stackstring)] = '\0';
	    if (strcasecmp(stackstring, namestring)) {
		return;
	    }
	    c+=r;
	    if ((unsigned long)c + 4 > eob) {
		return;
	    }
	    qdatatype = sucknetword(c);
	    qclass = sucknetword(c);
	    if (qclass != C_IN) {
		return;
	    }
	    switch (qdatatype) {
	    case T_PTR:
		if (!IS_PTR(rp) && (!IS_PTR6(rp))) {
		    return;
		}
		break;
	    case T_A:
		if (!IS_A(rp)) {
		    return;
		}
		break;
#ifdef IPV6
	    case T_AAAA:
		if (!IS_AAAA(rp)) {
		    return;
		}
		break;
#endif
	    default:
		return;
	    }
	    for (rr = hp->ancount + hp->nscount + hp->arcount; rr; rr--) {
		if ((unsigned long)c > eob) {
		    return;
		}
		*namestring = '\0';
		r = dn_expand(s, s + l, c, namestring, MAXDNAME);
		if (r == -1) {
		    return;
		}
		namestring[strlen(stackstring)] = '\0';
		if (strcasecmp(stackstring, namestring))
		    usefulanswer = 0;
		else
		    usefulanswer = 1;
		c+=r;
		if ((unsigned long)c + 10 > eob) {
		    return;
		}
		datatype = sucknetword(c);
		class = sucknetword(c);
		ttl = sucknetlong(c);
		rdatalength = sucknetword(c);
		if (class != qclass) {
		    return;
		}
		if (!rdatalength) {
		    return;
		}
		if ((unsigned long)c + rdatalength > eob) {
		    return;
		}
		if (datatype == qdatatype) {
		    if (usefulanswer)
			switch (datatype) {
			case T_A:
			    if (rdatalength != 4) {
				return;
			    }
			    memcpy(&rp->ip, (u_32bit_t *) c, sizeof(u_32bit_t));
			    linkresolveip(rp);
			    passrp(rp, ttl, T_A);
			    return;
#ifdef IPV6
			case T_AAAA:
			    if (rdatalength != 16) {
				return;
			    }
			    memcpy(&rp->ip6,(u_8bit_t*) c, 16);
			    linkresolveip6(rp);
			    passrp(rp, ttl, T_AAAA);
			    return;
#endif
			case T_PTR:
			    *namestring = '\0';
			    r =	dn_expand(s, s+l, c, namestring, MAXDNAME);
			    if (r == -1) {
				return;
			    }
			    if (r > HOSTNAMELEN) {
				failrp(rp, T_PTR);
				return;
			    }
			    if (rp->hostn[0]==0) {
				strmncpy(rp->hostn, namestring,sizeof(rp->hostn));
				linkresolvehost(rp);
				passrp(rp, ttl, T_PTR);
				return;
			    }
			    break;
			default:
			}
		} else if (datatype == T_CNAME) {
		    *namestring = '\0';
		    r =	dn_expand(s, s+l, c, namestring, MAXDNAME);
		    if (r == -1) {
			return;
		    }
		    /* The next responses will be related to the domain
		     * pointed to by CNAME, so we need to update which
		     * respones we regard as important.
		     */
		    strmncpy(stackstring, namestring, 1024);
		} else {
		    /* ! */
		}
		c+=rdatalength;
	    }
	}
	break;
    case NXDOMAIN:
	switch (rp->state) {
	case STATE_PTRREQ:
	case STATE_PTR6REQ:
		failrp(rp, T_PTR);
		break;
	case STATE_AREQ:
		failrp(rp, T_A);
		break;
	case STATE_AAAAREQ:
	        /* AAAA failed, fall back to A */
	        rp->sends=1;
	        rp->protocol=AF_INET;
	        rp->expiretime = time(NULL) + (RES_RETRYDELAY * rp->sends);
	        rp->state=STATE_AREQ;
		untieresolve(rp);
		linkresolve(rp);
	        resendrequest(rp, T_A);
		break;
	default:
		failrp(rp, 0);
		break;
	}
	break;
    default:
    }
}

/* Read data received on our dns socket. This function is called
 * as soon as traffic is detected.
 */
int dns_ack(int whocares)
{
    struct sockaddr_in from;
    unsigned int fromlen = sizeof(struct sockaddr_in);
    int r, i;

    r =	recvfrom(resfd, (u_8bit_t *) resrecvbuf, MAX_PACKETSIZE, 0,
		 (struct sockaddr *) &from, &fromlen);
    if (r <= 0) {
	return;
    }
    /* Check to see if this server is actually one we sent to */
    if (from.sin_addr.s_addr == localhost) {
        for (i = 0; i < _res.nscount; i++)
	    /* 0.0.0.0 replies as 127.0.0.1 */
	    if ((_res.nsaddr_list[i].sin_addr.s_addr == from.sin_addr.s_addr)
		|| (!_res.nsaddr_list[i].sin_addr.s_addr))
		break;
    } else {
        for (i = 0; i < _res.nscount; i++)
	    if (_res.nsaddr_list[i].sin_addr.s_addr == from.sin_addr.s_addr)
		break;
    }
    if (i == _res.nscount) {
	/* ? */
    } else
        parserespacket((u_8bit_t *) resrecvbuf, r);
}

/* Remove or resend expired requests. Called once a second.
 */
void dns_check_expires(void)
{
    struct resolve *rp, *nextrp;

    /* Walk through sorted list ... */
    for (rp = expireresolves; (rp) && (time(NULL) >= rp->expiretime);
	 rp = nextrp) {
	nextrp = rp->next;
	untieresolve(rp);
	switch (rp->state) {
	case STATE_FINISHED:	/* TTL has expired */
	case STATE_FAILED:	/* Fake TTL has expired */
	    unlinkresolve(rp);
	    break;
	case STATE_PTRREQ:
	case STATE_PTR6REQ: /* T_PTR send timed out */
	    if (rp->sends <= RES_MAXSENDS) {
	      resendrequest(rp, T_PTR);
	    } else {
	      failrp(rp, T_PTR);
	    }
	    break;
	case STATE_AREQ:	/* T_A send timed out */
	    if (rp->sends <= RES_MAXSENDS) {
	      resendrequest(rp, T_A);
	    } else {
	      failrp(rp, T_A);
	    }
	    break;
#ifdef IPV6
	case STATE_AAAAREQ:	/* T_AAAA send timed out */
	    if (rp->sends <= 2) {
	      resendrequest(rp, T_AAAA);
	    } else {
	      /* AAAA failed, fall back to A */
	      rp->sends=1;
	      rp->protocol=AF_INET;
	      rp->expiretime = time(NULL) + (RES_RETRYDELAY * rp->sends);
	      rp->state=STATE_AREQ;
	      untieresolve(rp);
	      linkresolve(rp);
	      resendrequest(rp, T_A);
	    }
	    break;
#endif
	default:		/* Unknown state, let it expire */
	    failrp(rp, 0);
	}
    }
}

/* Start searching for a host-name, using it's ip-address.
 */
int dns_lookup(u_32bit_t ip,int(*resolved)(struct resolve *),int(*unresolved)(struct resolve *),char *itemdata)
{
    struct resolve *rp;
    char *ntoa;

    if ((rp = findip(ip))) {
	if (rp->state == STATE_FINISHED || rp->state == STATE_FAILED) {
	
	    rp->resolved=resolved;
	    rp->unresolved=unresolved;
	    strmncpy(rp->data,itemdata,sizeof(rp->data));
	    if (rp->state == STATE_FINISHED && rp->hostn[0]!=0) {
		dns_event_success(rp, T_PTR);
		return 0x1;
	    } else {
		dns_event_failure(rp, T_PTR);
		return 0x2;
	    }
	}
    }
    
    rp = allocresolve();
    rp->resolved=resolved;
    rp->unresolved=unresolved;
    strmncpy(rp->data,itemdata,sizeof(rp->data));
    rp->state = STATE_PTRREQ;
    rp->sends = 1;
    rp->protocol=AF_INET;
    rp->ip = ip;
    linkresolveip(rp);
    sendrequest(rp, T_PTR);
    return 0x0;
}

#ifdef IPV6

/* Start searching for a host-name, using it's ip-address.
 */
int dns_lookupv6(unsigned char *v6ip,int(*resolved)(struct resolve *),int(*unresolved)(struct resolve *),char *itemdata)
{
    struct resolve *rp;

    if ((rp = findip6(v6ip))) {
	if (rp->state == STATE_FINISHED || rp->state == STATE_FAILED) {
	    rp->resolved=resolved;
	    rp->unresolved=unresolved;
	    strmncpy(rp->data,itemdata,sizeof(rp->data));
	    if (rp->state == STATE_FINISHED && rp->hostn[0]!=0) {
		dns_event_success(rp, T_PTR);
		return 0x1;
	    } else {
		dns_event_failure(rp, T_PTR);
		return 0x2;
	    }
	}
    }
    rp = allocresolve();
    rp->resolved=resolved;
    rp->unresolved=unresolved;
    strmncpy(rp->data,itemdata,sizeof(rp->data));
    rp->state = STATE_PTR6REQ;
    rp->protocol = AF_INET6;
    rp->sends = 1;
    rp->ip = 0;
    memcpy(&rp->ip6,v6ip,16);
    linkresolveip(rp);
    sendrequest(rp, T_PTR);
    return 0x0;
}

#endif

/* Start searching for an ip-address, using it's host-name.
 */
int dns_forward(char *hostn,int(*resolved)(struct resolve *),int(*unresolved)(struct resolve *),char *itemdata)
{
    struct resolve *rp;
    struct in_addr inaddr;
#ifdef IPV6
    u_8bit_t inaddr6[16];
#endif
    /* Check if someone passed us an IP address as hostname
     * and return it straight away.
     */
    if (inet_aton(hostn, &inaddr)) {
      rp=findhost(hostn);
      if(!rp)
          rp=allocresolve();
      rp->state=STATE_FINISHED;
      rp->protocol=AF_INET;
      rp->sends=1;
      rp->expiretime = time(NULL) + (RES_RETRYDELAY * rp->sends);
      rp->ip=inaddr.s_addr;
      rp->resolved=resolved;
      rp->unresolved=unresolved;
      rp->type=T_A;
      strmncpy(rp->hostn, hostn,sizeof(rp->hostn));
      strmncpy(rp->data,itemdata,sizeof(rp->data));
      linkresolvehost(rp);
      dns_event_success(rp, T_A);
      return 0x1;
    }
#ifdef IPV6
    if (inet_pton(AF_INET6,hostn, &inaddr6[0])>0) {
	rp=findhost(hostn);
	if(!rp)
	    rp=allocresolve();
	rp->state = STATE_FINISHED;
	rp->protocol=AF_INET6;
	rp->sends=1;
        rp->expiretime = time(NULL) + (RES_RETRYDELAY * rp->sends);
	rp->ip=0;
        rp->type=T_AAAA;
	memcpy(&rp->ip6,&inaddr6[0],16);
	rp->resolved=resolved;
	rp->unresolved=unresolved;
	strmncpy(rp->data,itemdata,sizeof(rp->data));
	linkresolvehost(rp);
	dns_event_success(rp, T_AAAA);
	return 0x1;
    }
#endif
    if ((rp = findhost(hostn))) {
	if (rp->state == STATE_FINISHED || rp->state == STATE_FAILED) {
	    rp->resolved=resolved;
	    rp->unresolved=unresolved;
	    strmncpy(rp->data,itemdata,sizeof(rp->data));
	    if (rp->state == STATE_FINISHED && rp->ip) {
		dns_event_success(rp, rp->type);
		return 0x1;
	    } else {
		dns_event_failure(rp, rp->type);
		return 0x2;
	    }
	}
    }
    rp = allocresolve();
    rp->resolved=resolved;
    rp->unresolved=unresolved;
    strmncpy(rp->data,itemdata,sizeof(rp->data));
#ifdef IPV6
    rp->state = STATE_AAAAREQ;
    rp->sends = 1;
    strmncpy(rp->hostn, hostn,sizeof(rp->hostn));
    linkresolvehost(rp);
    sendrequest(rp, T_AAAA);
#else
    rp->state = STATE_AREQ;
    rp->sends = 1;
    strmncpy(rp->hostn, hostn,sizeof(rp->hostn));
    linkresolvehost(rp);
    sendrequest(rp, T_A);
#endif
    return 0x0;
}

static int init_dns_network(void);

int dns_err(int whocares, int ern)
{
    killsocket(resfd);
    p_log(LOG_INFO,-1,lngtxt(1351));
    if(init_dns_network()==0)
    {
	p_log(LOG_ERROR,-1,lngtxt(1352));
	exit(0x1);
    }
}

/* Initialise the network.
 */
static int init_dns_network(void)
{
    struct socketnodes *snode;
    int option;
    struct in_addr inaddr;

    resfd = socket(AF_INET, SOCK_DGRAM, 0);
    if(resfd>=0)
    {
    	resfd = createsocket(resfd,ST_RESOLVER,0,SGR_NONE,NULL,NULL,dns_err,dns_ack,NULL,NULL,AF_INET,SSL_OFF);
	snode = getpsocketbysock(resfd);
	if(snode)
	    if(snode->sock)
		/* socket is connected */
		snode->sock->flag=SOC_CONN;
    }
    if (resfd == -1) {
	p_log(LOG_ERROR, -1,lngtxt(1353),strerror(errno));
	return 0;
    }
    option = 1;
    if (setsockopt(resfd, SOL_SOCKET, SO_BROADCAST, (char *) &option,
	 sizeof(option))) {
	p_log(LOG_ERROR, -1,lngtxt(1354),strerror(errno));
	killsocket(resfd);
	return 0;
    }

    inet_aton("127.0.0.1", &inaddr);
    localhost = inaddr.s_addr;
    return 1;
}

/* Initialise the core dns system, returns 1 if all goes well, 0 if not.
 */
int init_dns_core(void)
{
    int i;

    /* Initialise the resolv library. */
    res_init();
    if (!_res.nscount) {
	p_log(LOG_ERROR, -1,lngtxt(1355));
	return 0;
    }
    _res.options |= RES_RECURSE | RES_DEFNAMES | RES_DNSRCH;
    for (i = 0; i < _res.nscount; i++)
	_res.nsaddr_list[i].sin_family = AF_INET;

    if (!init_dns_network())
	return 0;

    /* Initialise the hash tables. */
    aseed = time(NULL) ^ (time(NULL) << 3) ^ (u_32bit_t) getpid();
    for (i = 0; i < BASH_SIZE; i++) {
	idbash[i] = NULL;
	ipbash[i] = NULL;
	hostbash[i] = NULL;
    }
    expireresolves = NULL;
    p_log(LOG_INFO,-1,lngtxt(1356));
    return 1;
}

void dns_stat(int usern)
{
    u_32bit_t openq=0;
    u_32bit_t finishedq=0;
    u_32bit_t failedq=0;
    u_32bit_t cachedq=0;
    struct resolve *rs;
    rs=expireresolves;
    while(rs)
    {
	switch(rs->state)
	{
	    case STATE_FINISHED:
		finishedq++;
		cachedq++;
		break;
	    case STATE_FAILED:
		failedq++;
		cachedq++;
		break;
	    default:
		openq++;
		break;
	}
	rs=rs->next;
    }
    p_log(LOG_INFO,usern,"Logging DNS-Stats");
    p_log(LOG_INFO,usern,"%-8lu open DNS-Queries",openq);
    p_log(LOG_INFO,usern,"%-8lu finished DNS-Queries",finishedq);
    p_log(LOG_INFO,usern,"%-8lu failed DNS-Queries",failedq);
    p_log(LOG_INFO,usern,"%-8lu cached Answers",cachedq);
    p_log(LOG_INFO,usern,"End of DNS-Stats");
    return;
}

#endif
