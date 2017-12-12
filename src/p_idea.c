/* $Id: p_idea.c,v 1.3 2005/06/04 18:00:14 hisi Exp $ */
/************************************************************************
 *   psybnc2.3.2, src/p_idea.c
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
 *
 *   ADDITIONAL INFORMATIONS ABOUT LICENSING:
 *   ----------------------------------------
 *
 *   The IDEA Algorithm itself is copyrighted. Commercial use of it requires
 *   the purchase of a license from it's current patent holder ascom,ltd. Switzerland.
 *   This source itself is free. But commercial Use is ONLY allowed under the conditions
 *   of the copyright holder.
 *
 *   More Infos about IDEA and how to buy your license: www.ascom.com
 *
 */


#ifndef lint
static char rcsid[] = "@(#)$Id: p_idea.c,v 1.3 2005/06/04 18:00:14 hisi Exp $";
#endif

/* IDEA - Encryption Algorithm 
 * Implementation by psychoid
 * normal c - should also be portable :)
 * version for psybnc2.2 - Talk & Connection Encryption
 *
 * idea is a powerful 128bit cipher. 
 *
 * if you are interested in cryptography, read:
 * - Bruce Schneier : applied cryptography (Addison Wesley)
 *
 * (c) for the IDEA algorithm by ascom ltd., switzerland
 * string encryption by psychoid
 */

#include <time.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <config.h>

extern char ctxt[50];
extern char cfunc[60];
extern int cline;

#define pcontext { strmncpy(ctxt,__FILE__,sizeof(ctxt));strmncpy(cfunc,__FUNCTION__,sizeof(cfunc)); cline=__LINE__; }
#define pmalloc(n) __pmalloc((n),__FILE__,__FUNCTION__,__LINE__)

#ifdef CRYPT

#define IDEAROUNDS 8
#define IDEAKEYLEN (6*IDEAROUNDS+4)
 
unsigned char IDEAUserKey[16];
unsigned short IDEADecryptionKey[IDEAKEYLEN];
unsigned short IDEAEncryptionKey[IDEAKEYLEN];

time_t time1,time2;
unsigned byte_pos,bit_pos;
unsigned size;
unsigned char *buffer;
unsigned IDEA_datasize;

unsigned short IDEA_MUL(unsigned short x,unsigned short y)
{
    unsigned short t16;
    unsigned int t32;
    pcontext;
    if(y!=0)
    {
	if(x!=0)
	{
	    t32=(unsigned int)x * y;
	    x=t32;
	    t16=t32>>16;
	    x=(x - t16) + (x < t16);
	}
	else
	    x=1 - y;
    }
    else
	x=1 - x;
    return x;
}

/* inverted Multiplication */

unsigned short MulInv(unsigned short x)
{
    unsigned short t0, t1, q, y;
    pcontext;
    if (x<=1)
    {
	return x;
    }
    t1=0x10001L / x;
    y =0x10001L % x;
    if (y==1)
    {
	return 1 -t1;
    }
    t0 = 1;
    do {
	if(y>0)
	{
	    q = x / y;
	    x = x % y;
	}
	t0 += q * t1;
	if (x <= 1)
	{
	    return t0;
	}
	q = y / x;
	y = y % x;
	t1 += q * t0;
    } while (y != 1);
    return 1 - t1;
}

/* generating the encryption and decryption keys */

void generatePartKeys()
{
    unsigned i,j;
    unsigned short *ek = IDEAEncryptionKey;
    unsigned char *userkey = IDEAUserKey;
    unsigned short t1,t2,t3,*p = IDEADecryptionKey + IDEAKEYLEN;
    pcontext;

    for(j=0;j<8;j++) {
#ifdef BIGIDEA
	ek[j]=(userkey[1]<<8)+userkey[0];
#else
	ek[j]=(userkey[0]<<8)+userkey[1];
#endif
	userkey+=2;
    }
    for(i=0;j<IDEAKEYLEN;j++) {
	i++;
	ek[i+7]=(ek[i & 7] <<9) | (ek[i + 1 & 7] >> 7);
	ek+=i & 8;
	i &= 7;
    }
    ek=IDEAEncryptionKey;
    t1 = MulInv(*ek++);
    t2 = -*ek++;
    t3 = -*ek++;
    *--p = MulInv(*ek++);
    *--p = t3;
    *--p = t2;
    *--p = t1;
    for(i=0; i<IDEAROUNDS -1;i++) {
	t1=*ek++;
	*--p = *ek++;
	*--p = t1;
	t1 = MulInv(*ek++);
	t2 = -*ek++;
	t3 = -*ek++;
	*--p = MulInv(*ek++);
	*--p = t2;
	*--p = t3;
	*--p = t1;
    }
    t1 = *ek++;
    *--p = *ek++;
    *--p = t1;
    t1 = MulInv(*ek++);
    t2 = -*ek++;
    t3 = -*ek++;
    *--p = MulInv(*ek++);
    *--p = t3;
    *--p = t2;
    *--p = t1;
}

void generateKey_start()
{
    time(&time2);
    memset((void *)IDEAUserKey,0,16);
    byte_pos=0;
    bit_pos=0;
}

void generateKey_action()
{
    int cnt=2000;
    time1 = time2;
    time(&time2);
    while(cnt>0)    
    {
	IDEAUserKey[byte_pos] |= ((time2-time1) &3) << bit_pos;
	if(bit_pos==6) {
	    byte_pos++;
	    bit_pos=0;
	} else
	    bit_pos+=2;
	cnt--;
    }
}

void generateKey_end()
{
    generatePartKeys();
}

int IDEAKEY_generate(char *UserKey)
{
    pcontext;
    if (*UserKey==0)
	memset(IDEAUserKey,0x0,sizeof(IDEAUserKey));
    else
	memcpy((void *)IDEAUserKey,(void *)UserKey,16);
    generatePartKeys();
}

int IDEAKEY_clear()
{
    pcontext;
    memset(IDEAUserKey,0x0,sizeof(IDEAUserKey));    
    memset(IDEAEncryptionKey,0x0,sizeof(IDEAEncryptionKey));    
    memset(IDEADecryptionKey,0x0,sizeof(IDEADecryptionKey));    
}

/* the cipher routine, main part of this little snippet */
void cipher(unsigned char const inbuf[8], unsigned char outbuf[8], unsigned short const *key)
{
    register unsigned short x1,x2,x3,x4,s2,s3;
    unsigned short *in,*sout;
    unsigned short out[4];    
    unsigned char cout[8];
    int r = IDEAROUNDS;
    pcontext;

#ifdef BIGIDEA
    x1=(inbuf[0]<<8)+inbuf[1];
    x2=(inbuf[2]<<8)+inbuf[3];
    x3=(inbuf[4]<<8)+inbuf[5];
    x4=(inbuf[6]<<8)+inbuf[7];
#else
    x1=(inbuf[1]<<8)+inbuf[0];
    x2=(inbuf[3]<<8)+inbuf[2];
    x3=(inbuf[5]<<8)+inbuf[4];
    x4=(inbuf[7]<<8)+inbuf[6];
#endif
    
    x1 = (x1 >> 8) | (x1 << 8);
    x2 = (x2 >> 8) | (x2 << 8);
    x3 = (x3 >> 8) | (x3 << 8);
    x4 = (x4 >> 8) | (x4 << 8);
    
    do { /* well, the hard stuff */
	x1=IDEA_MUL(x1,*key++);
	x2 += *key++;
	x3 += *key++;
	x4=IDEA_MUL(x4, *key++);
	
	s3 = x3;
	x3 ^= x1;
	x3=IDEA_MUL(x3, *key++);
	s2 = x2;
	x2 ^= x4;
	x2 += x3;
	x2=IDEA_MUL(x2, *key++);
	x3 += x2;
	
	x1 ^= x2;
	x4 ^= x3;
	
	x2 ^= s3;
	x3 ^= s2;
    } while (--r);
    x1=IDEA_MUL(x1, *key++);
    x3 += *key++;
    x2 += *key++;
    x4=IDEA_MUL(x4, *key);
    
    out[0] = (x1 >> 8) | (x1 << 8);
    out[1] = (x3 >> 8) | (x3 << 8);
    out[2] = (x2 >> 8) | (x2 << 8);
    out[3] = (x4 >> 8) | (x4 << 8);
    memcpy(&cout[0],&out[0],8);

#ifdef BIGIDEA
    x1=(cout[0]<<8)+cout[1];
    x2=(cout[2]<<8)+cout[3];
    x3=(cout[4]<<8)+cout[5];
    x4=(cout[6]<<8)+cout[7];
#else
    x1=(cout[1]<<8)+cout[0];
    x2=(cout[3]<<8)+cout[2];
    x3=(cout[5]<<8)+cout[4];
    x4=(cout[7]<<8)+cout[6];
#endif
    sout=(unsigned short *)outbuf;
    *sout++=x1;
    *sout++=x2;
    *sout++=x3;
    *sout=x4;
}

/* the main encryption routine */

char * IDEAencrypt(const unsigned char *input, unsigned size)
{
    unsigned n,m;
    unsigned char tmpbuf[8];
    unsigned char *output;
    unsigned msize;
    pcontext;
    msize=(size % 8 > 0 ? 8 - size % 8 : 0);
    output=(char *)pmalloc(msize+size);
    if (output==NULL) return NULL;
    for(n = 0; n<size - size % 8; n += 8)
	cipher(&input[n],&output[n],IDEAEncryptionKey);
    if(size % 8 > 0) {
	memset((void*)tmpbuf,0,8);
	for(m=0;m<size % 8; m++)
	    tmpbuf[m]=input[n+m];
	cipher(tmpbuf,tmpbuf,IDEAEncryptionKey);
	for(m=0;m<8;m++)
	    output[n+m]=tmpbuf[m];
    }
    IDEA_datasize=msize+size;
    return output;
}

/* the main decryption routine */

char * IDEAdecrypt(const unsigned char *input, unsigned size)
{
    unsigned n;
    char *output;
    pcontext;
    output=(char *)pmalloc(size+9);
    for(n=0;n<size;n+=8)
	cipher(&input[n],&output[n],IDEADecryptionKey);
    return output;
}

/* general key routine */

void IDEA_keyset(char *key)
{
    IDEAKEY_clear();
    pcontext;
    if (*key==0)
    {
	generateKey_start;
	generateKey_action;
	generateKey_end;
    } else
	IDEAKEY_generate(key);
    return;
}

#endif

/* hashing routines for string driven systems */

unsigned char base[]="'`0123456789abcdefghijklmnopqrstuvxyzABCDEFGHIJKLMNOPQRSTUVWXYZ@$=&*-#";

int baselen=67;

unsigned char xres[3];

unsigned char *hashdot(unsigned int r)
{
    unsigned int cnt;
    unsigned int hh=0;
    unsigned int hl=0;
    cnt=r;
    for(;cnt>0;cnt--)
    {
	hl++;
	if (hl==baselen) {hl=0;hh++;}
    }
    xres[0]=base[hh];
    xres[1]=base[hl];
    xres[2]=0;
    return xres;
}

int wrong=0;

unsigned int unhashdot(unsigned char *hash)
{
    unsigned int lf=baselen;
    unsigned char *pt;
    unsigned int erg=0;
    unsigned long ln=0;
    wrong=0;
    while (ln<baselen && base[ln] != hash[0]) {
	ln++;
    }
    if (ln!=baselen) {
	erg=ln * lf;
    } else {
	wrong=1;
    }
    ln=0;
    while (ln<baselen && base[ln] != hash[1]) {
	ln++;
    }
    if (ln!=baselen) {
	erg=erg+ln;
    } else {
	wrong=1;
    }
    return erg;
}

char *hashstring(unsigned char *unhashed,unsigned size)
{
    unsigned usize;
    unsigned char *bstring;
    unsigned char *hash;
    int chr;
    unsigned char *hsh;
    hsh=unhashed;
    usize=size;
    usize++;
    bstring=(char *)pmalloc(usize*2+1);
    usize=size;
    while(usize>0)
    {
	chr=*hsh++;
	hash=hashdot(chr);
	strcat(bstring,hash);
	usize--;
    }
    return bstring;
}

char *unhashstring(unsigned char *hashed)
{
    unsigned size;
    char *pi;
    char *result;
    unsigned char *uhshd,*uhash;
    size=strlen(hashed) / 2;
    size+=2;    
    result=(char *)pmalloc(size+9);
    if(result==NULL) return NULL;
    uhash=result;
    uhshd=hashed;
    while(*uhshd)
    {
	pi=uhshd+1;
	if(*pi==0 || *uhshd==0) return result;
	*uhash++=unhashdot(uhshd);
	if(wrong==1) return result;
	uhshd+=2;
    }
    return result;
} 

#ifdef CRYPT

/* the exports */

/* return a key */

char *IDEA_getencryptionkey()
{
    return (char *) IDEAEncryptionKey;
}

char *IDEA_getdecryptionkey()
{
    return (char *) IDEADecryptionKey;
}

/* return a hashed key */

char *IDEA_gethashedencryptionkey()
{
    char *dkey;
    char *hsh;
    unsigned size;
    char *ekey;
    int chr;
    pcontext;
    dkey=IDEA_getdecryptionkey();
    size=IDEAKEYLEN * 2;
}

char *IDEA_gethasheddecryptionkey()
{
    char *ekey;
    char *hsh;
    unsigned size;
    char *hkey;
    int chr;
    pcontext;
    ekey=IDEA_getencryptionkey();
    size=IDEAKEYLEN * 2;
    size++;
    hkey=(char *)pmalloc(size);
    memset(hkey,0x0,size);
    size=IDEAKEYLEN;
    while(size--)
    {
	chr=*ekey++;
	hsh=hashdot(chr);
	strcat(hkey,hsh);
    }
    return hkey;
}

/* binary encrypt, decrypt routines. If no key is given, a random key is created */

char *IDEA_encrypt(unsigned char *input, unsigned char *key, unsigned size)
{
	IDEA_keyset(key);
	return IDEAencrypt(input,size);
}

char *IDEA_decrypt(unsigned char *input, unsigned char *key, unsigned size)
{
	IDEA_keyset(key);
	return IDEAdecrypt(input,size);
}	

/* string encrypt / decrypt with hashing routines */

char *IDEA_stringencrypt(unsigned char *input, unsigned char *key)
{
    unsigned char *result,*hashed,*pt;
    pcontext;
    pt=strchr(input,'\n');
    if (pt==NULL) pt=strchr(input,'\r');
    if (pt!=NULL) *pt=0;
    result=IDEA_encrypt(input,key,strlen(input)+1);
    hashed=hashstring(result,IDEA_datasize);
    free(result);
    if(*input==0) *hashed=0;
    return hashed;
}

char *IDEA_stringdecrypt(unsigned char *input, unsigned char *key)
{
    unsigned char *result,*unhashed,*pt;
    int uhsize;
    int size;
    pcontext;
    pt=strchr(input,'\n');
    if (pt==NULL) pt=strchr(input,'\r');
    if (pt!=NULL) *pt=0;
    size=strlen(input);
    uhsize=size/2;
    unhashed=unhashstring(input);
    if(wrong==1) return unhashed;
    result=IDEA_decrypt(unhashed,key,uhsize);
    free(unhashed);
    if(*input==0) *result=0;
    return result;
}

#endif
