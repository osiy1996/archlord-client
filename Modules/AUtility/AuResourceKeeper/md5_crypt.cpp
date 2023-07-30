/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <phk@login.dknet.dk> wrote this file.  As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return.   Poul-Henning Kamp
 * ----------------------------------------------------------------------------
 *
 * Origin: Id: crypt.c,v 1.3 1995/05/30 05:42:22 rgrimes Exp
 *
 */

#include "stdafx.h"
#include "RKInclude.h"

static unsigned char itoa64[] =		/* 0 ... 63 => ascii - 64 */
	"./0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";

static void
to64(char *s, unsigned long v, int n)
{
	while (--n >= 0) {
		*s++ = itoa64[v&0x3f];
		v >>= 6;
	}
}

/*
 * i64c - convert an integer to a radix 64 character
 */
int i64c(int i)
{
    if (i < 0)
        return ('.');
    else if (i > 63)
        return ('z');
    if (i == 0)
        return ('.');
    if (i == 1)
        return ('/');
    if (i >= 2 && i <= 11)
        return ('0' - 2 + i);
    if (i >= 12 && i <= 37)
        return ('A' - 12 + i);
    if (i >= 38 && i <= 63)
        return ('a' - 38 + i);
    return ('\0');
}

/*
 * UNIX password
 *
 * Use MD5 for what it is best at...
 */

char * crypt_md5(const char *pw, const char *salt)
{
	const char *magic = "$1$";
	/* This string is magic for this algorithm.  Having
	 * it this way, we can get get better later on */
	static char passwd[120], *p;
	static const char *sp,*ep;
	unsigned char	final[16];
	int sl,pl,i,j;
	MD5_CTX	ctx,ctx1;
	unsigned long l;

	/* Refine the Salt first */
	sp = salt;

	/* If it starts with the magic string, then skip that */
	if(!strncmp(sp,magic,strlen(magic)))
		sp += strlen(magic);

	/* It stops at the first '$', max 8 chars */
	for(ep=sp;*ep && *ep != '$' && ep < (sp+8);ep++)
		continue;

	/* get the length of the true salt */
	sl = ep - sp;

	MD5Init(&ctx);

	/* The password first, since that is what is most unknown */
	MD5Update(&ctx,(unsigned const char *)pw,strlen(pw));

	/* Then our magic string */
	MD5Update(&ctx,(unsigned const char *)magic,strlen(magic));

	/* Then the raw salt */
	MD5Update(&ctx,(unsigned const char *)sp,sl);

	/* Then just as many characters of the MD5(pw,salt,pw) */
	MD5Init(&ctx1);
	MD5Update(&ctx1,(unsigned const char *)pw,strlen(pw));
	MD5Update(&ctx1,(unsigned const char *)sp,sl);
	MD5Update(&ctx1,(unsigned const char *)pw,strlen(pw));
	MD5Final(final,&ctx1);
	for(pl = strlen(pw); pl > 0; pl -= 16)
		MD5Update(&ctx,(unsigned const char *)final,pl>16 ? 16 : pl);

	/* Don't leave anything around in vm they could use. */
	memset(final,0,sizeof final);

	/* Then something really weird... */
	for (j=0,i = strlen(pw); i ; i >>= 1)
		if(i&1)
		    MD5Update(&ctx, (unsigned const char *)final+j, 1);
		else
		    MD5Update(&ctx, (unsigned const char *)pw+j, 1);

	/* Now make the output string */
	strcpy(passwd,magic);
	strncat(passwd,sp,sl);
	strcat(passwd,"$");

	MD5Final(final,&ctx);

	/*
	 * and now, just to make sure things don't run too fast
	 * On a 60 Mhz Pentium this takes 34 msec, so you would
	 * need 30 seconds to build a 1000 entry dictionary...
	 */
	for(i=0;i<1000;i++) {
		MD5Init(&ctx1);
		if(i & 1)
			MD5Update(&ctx1,(unsigned const char *)pw,strlen(pw));
		else
			MD5Update(&ctx1,(unsigned const char *)final,16);

		if(i % 3)
			MD5Update(&ctx1,(unsigned const char *)sp,sl);

		if(i % 7)
			MD5Update(&ctx1,(unsigned const char *)pw,strlen(pw));

		if(i & 1)
			MD5Update(&ctx1,(unsigned const char *)final,16);
		else
			MD5Update(&ctx1,(unsigned const char *)pw,strlen(pw));
		MD5Final(final,&ctx1);
	}

	p = passwd + strlen(passwd);

	l = (final[ 0]<<16) | (final[ 6]<<8) | final[12]; to64(p,l,4); p += 4;
	l = (final[ 1]<<16) | (final[ 7]<<8) | final[13]; to64(p,l,4); p += 4;
	l = (final[ 2]<<16) | (final[ 8]<<8) | final[14]; to64(p,l,4); p += 4;
	l = (final[ 3]<<16) | (final[ 9]<<8) | final[15]; to64(p,l,4); p += 4;
	l = (final[ 4]<<16) | (final[10]<<8) | final[ 5]; to64(p,l,4); p += 4;
	l =                    final[11]                ; to64(p,l,2); p += 2;
	*p = '\0';

	/* Don't leave anything around in vm they could use. */
	memset(final,0,sizeof final);

	return passwd;
}

char mksalt(int seed)
{
    int num=seed % 64;

    if (num <26) 
        return 'a'+num;
    else if (num<52) 
        return 'A'+(num-26);
    else if (num<62)
        return '0'+(num-52);
    else if (num==63)
        return '.';
    else
        return '/';
}

char* getRandomSalt(void)
{
    static char strSalt[9];
    
    srand(clock());

    strSalt[0]=mksalt(rand());
    strSalt[1]=mksalt(rand());
    strSalt[2]=mksalt(rand());
    strSalt[3]=mksalt(rand());
    strSalt[4]=mksalt(rand());
    strSalt[5]=mksalt(rand());
    strSalt[6]=mksalt(rand());
    strSalt[7]=mksalt(rand());
    strSalt[8]='\0';
    
    return strSalt;
}
