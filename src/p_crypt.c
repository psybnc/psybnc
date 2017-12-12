/* $Id: p_crypt.c,v 1.3 2005/06/04 18:00:14 hisi Exp $ */
/************************************************************************
 *   psybnc2.3.2, src/p_crypt.c
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

/*
 * this crypting method expires with psybnc2.1 for one-way passwords.
 * the password for proxy authorisation is from now on build by
 * blowfish, but still downwards compatible.
 */

#ifndef lint
static char rcsid[] = "@(#)$Id: p_crypt.c,v 1.3 2005/06/04 18:00:14 hisi Exp $";
#endif

/*
 * psyCrypt
 * this routines handle the passwordencryptions.
 * Due to the fact they need to be bidirectional-ciphered,
 * this algorithm is crackable with some effort.
 * The ciphered code is built out of 5 hashes
 * The hashes are created at compiling time.
 * Its needed to fully debug the compiled code to get the hashes.
 *
 * The needed file salt.h is created by makesalt.c, which 
 * is compiled before compiling the main bouncer.
 *
 * psyCrypt is a simple Offset-Encryption.
 *
 */

#define P_CRYPT

#include <p_global.h>
#include <salt.h>

/* those are now in p_idea.c */

unsigned char *hashdot(unsigned int r);
unsigned int unhashdot(unsigned char *hash);

char crybu[2000];

char *psycrypt(char *st)
{
    char *pte;
    char *ptt;
    char *pts1,*pts2;
    char *pt;
    char *hpt;
    char hbuf[3];
    int res;
    int slen=0;
    unsigned int zn;
    unsigned int tslt1 = CODE1;
    unsigned int tslt2 = CODE2;
    int p1,p2,p3,p4,p5;
    int erg;
    int de=0;
    pcontext;
    memset(crybu,0x0,sizeof(crybu));
    pt = crybu;
    pte = pt;
    ptt = st;
    if (*ptt=='+') {
       ptt++;
       de=1;
    } else {
       *pte++='+';
    }
    pts1 = slt1 +SA1;
    pts2 = slt2 +SA2;
    while(*ptt!=0)
    {
	if (slen>1990) break;
        if (tslt1>255 || tslt1 <0) tslt1=CODE1;
        if (tslt2>255 || tslt2 <0) tslt2=CODE2;
	if (*pts1==0) pts1=slt1;
	if (*pts2==0) pts2=slt2;
	res=0;
	if (de) {
	   hbuf[0]=*ptt++;
	   hbuf[1]=*ptt;
	   hbuf[2]=0;
	   p1=unhashdot(hbuf);              
	   p2=*pts1;p3=tslt1;p4=*pts2;p5=tslt2;
	   erg=p1-p2-p3+p4-p5;    
	   *pte=erg;
	   res=erg;
	} else {
	   p1=*ptt;p2=*pts1;p3=tslt1;p4=*pts2;p5=tslt2;
	   res=p1;
	   erg=p1+p2+p3-p4+p5;    
	   hpt=hashdot(erg);
	   *pte++=hpt[0];slen++;
	   *pte=hpt[1];
	}
	tslt1--;
	res=res/10;
	tslt2=tslt2+res;
	pte++;ptt++;pts1++;pts2++;slen=slen+1;
    } 
    *pte=0;
    return pt;
}

char *cryptit(char *tocipher)
{
    if (*tocipher=='+')
       return tocipher;
    else
       return psycrypt(tocipher);
}

char *decryptit(char *todecipher)
{
    if (todecipher[0]=='+')
	return psycrypt(todecipher);
    else
	return todecipher;
}

