/*
 * kernel_libs/crypt/DES.c
 *
 * Copyright (c) 2003-2004 Lukasz Dembinski <dembol@linux.net>
 * All Rights Reserved
 *
 * * DES Alghoritm
 * * Implemented by Lukasz Dembinski <dembol@linux.net>
 * * 2002
 * 
 * Date:	2002
 * Author: 	Lukasz Dembinski
 * Info:	DES.c core file
 * Contact:	mailto: <dembol@linux.net>
 * Status:	implemented
 */

#include <agnix/agnix.h>
#include <agnix/memory.h>
#include <agnix/crypt.h>

u8 PSW[] = {	64, 60, 56, 52, 48, 44, 40, 36,
		32, 28, 24, 20, 16, 12,  8,  4,
		63, 59, 55, 51, 47, 43, 39, 35,
		31, 27, 23, 19, 15, 11,  7,  3,
		62, 58, 54, 50, 46, 42, 38, 34,
		30, 26, 22, 18, 14, 10,  6,  2,
		61, 57, 53, 49, 45, 41, 37, 33,
		29, 25, 21, 17, 13,  9,  5,  1};

u8 IP[] =  {	58, 50, 42, 34, 26, 18, 10,  2,
		60, 52, 44, 36, 28, 20, 12,  4,
		62, 54, 46, 38, 30, 22, 14,  6,
		64, 56, 48, 40, 32, 24, 16,  8,
		57, 49, 41, 33, 25, 17,  9,  1,
		59, 51, 43, 35, 27, 19, 11,  3,
		61, 53, 45, 37, 29, 21, 13,  5,
		63, 55, 47, 39, 31, 23, 15,  7};

u8 IPR[] = {	40,  8, 48, 16, 56, 24, 64, 32,
		39,  7, 47, 15, 55, 23, 63, 31,
		38,  6, 46, 14, 54, 22, 62, 30,
		37,  5, 45, 13, 53, 21, 61, 29,
		36,  4, 44, 12, 52, 20, 60, 28,
		35,  3, 43, 11, 51, 19, 59, 27,
		34,  2, 42, 10, 50, 18, 58, 26,
		33,  1, 41,  9, 49, 17, 57, 25};

u8 PC1[] = {	57, 49, 41, 33, 25, 17, 9,
		 1, 58, 50, 42, 34, 26, 18,
		10,  2, 59, 51, 43, 35, 27,
		19, 11,  3, 60, 52, 44, 36,
		63, 55, 47, 39, 31, 23, 15,
		 7, 62, 54, 46, 38, 30, 22,
		14,  6, 61, 53, 45, 37, 29,
		21, 13,  5, 28, 20, 12,  4 };

u8 PC2[] = {	14, 17, 11, 24,  1,  5,
		 3, 28, 15,  6, 21, 10,
		23, 19, 12,  4, 21,  8,
		16,  7, 27, 20, 13,  2,
		41, 52, 31, 37, 47, 55,
		30, 40, 51, 45, 33, 48,
		44, 49, 39, 56, 34, 53,
		46, 42, 50, 36, 29, 32 };

u8 tab_rotate[] = { 1, 1, 2, 2, 2, 2, 2, 2, 1, 2, 2, 2, 2, 2, 2, 1 };
	
u8 E[] = {	32,  1,  2,  3,  4,  5,
		 4,  5,  6,  7,  8,  9,
		 8,  9, 10, 11, 12, 13,
		12, 13, 14, 15, 16, 17,
		16, 17, 18, 19, 20, 21,
		20, 21, 22, 23, 24, 25,
		24, 25, 26, 27, 28, 29,
		28, 29, 30, 31, 32,  1 };

/* S-Boxes */	 
u8 S1[] = {	14,  4, 13,  1,  2, 15, 11,  8,  3, 10,  6, 12,  5,  9,  0,  7,
	         0, 15,  7,  4, 14,  2, 13,  1, 10,  6, 12, 11,  9,  5,  3,  8,
		 4,  1, 14,  8, 13,  6,  2, 11, 15,  2,  9,  7,  3, 10,  5,  0,
		15, 12,  8,  2,  4,  9,  1,  7,  5, 11,  3, 14, 10,  0,  6, 13};

u8 S2[] = {   15,  1,  8, 14,  6, 11,  3,  4,  9,  7,  2, 13, 12,  0,  5, 10,
	         3, 13,  4,  7, 15,  2,  8, 14, 12,  0,  1, 10,  6,  9, 11,  5,
		 0, 14,  7, 11, 10,  4, 13,  1,  5,  8, 12,  6,  9,  3,  2, 15,
		13,  8, 10,  1,  3, 15,  4,  2, 11,  6,  7, 12,  0,  5, 14,  9};

u8 S3[] = {   10,  0,  9, 14,  6,  3, 15,  5,  1, 13, 12,  7, 11,  4,  2,  8,
	        13,  7,  0,  9,  3,  4,  6, 10,  2,  8,  5, 14, 12, 11, 15,  1,
		13,  6,  4,  9,  8, 15,  3,  0, 11,  1,  2, 12,  5, 10, 14,  7,
		 1, 10, 13,  0,  6,  9,  8,  7,  4, 15, 14,  3, 11,  5,  2, 12};

u8 S4[] = {	 7, 13, 14,  3,  0,  6,  9, 10,  1,  2,  8,  5,  1, 12,  4, 15,
		13,  8, 11,  5,  6, 15,  0,  3,  4,  7,  2, 12,  1, 10, 14,  9,
		10,  6,  9,  0, 12, 11,  7, 13, 15,  1,  3, 14,  5,  2,  8,  4,
		 3, 15,  0,  6, 10,  1, 13,  8,  9,  4,  5, 11, 12,  7,  2, 14};

u8 S5[] = {	 2, 12,  4,  1,  7, 10, 11,  6,  8,  5,  3, 15, 13,  0, 14,  9,
	        14, 11,  2, 12,  4,  7, 13,  1,  5,  0, 15, 10,  3,  9,  8,  6,
		 4,  2,  1, 11, 10, 13,  7,  8, 15,  9, 12,  5,  6,  3,  0, 14,
		11,  8, 12,  7,  1, 14,  2, 13,  6, 15,  0,  9, 10,  4,  5,  3};

u8 S6[] = {	12,  1, 10, 15,  9,  2,  6,  8,  0, 13,  3,  4, 14,  7,  5, 11,
		10, 15,  4,  2,  7, 12,  9,  5,  6,  1, 13, 14,  0, 11,  3,  8,
		 9, 14, 15,  5,  2,  8, 12,  3,  7,  0,  4, 10,  1, 13, 11,  6,
		 4,  3,  2, 12,  9,  5, 15, 10, 11, 14,  1,  7,  6,  0,  8, 13};

u8 S7[] = {	 4, 11,  2, 14, 15,  0,  8, 13,  3, 12,  9,  7,  5, 10,  6,  1,
		13,  0, 11,  7,  4,  9,  1, 10, 14,  3,  5, 12,  2, 15,  8,  6,
		 1,  4, 11, 13, 12,  3,  7, 14, 10, 15,  6,  8,  0,  5,  9,  2,
		 6, 11, 13,  8,  1,  4, 10,  7,  9,  5,  0, 15, 14,  2,  3, 12};

u8 S8[] = {	13,  2,  8,  4,  6, 15, 11,  1, 10,  9,  3, 14,  5,  0, 12,  7,
		 1, 15, 13,  8, 10,  3,  7,  4, 12,  5,  6, 11,  0, 14,  9,  2,
		 7, 11,  4,  1,  9, 12, 14,  2,  0,  6, 10, 13, 15,  3,  5,  8,
		 2,  1, 14,  7,  4, 10,  8, 13, 15, 12,  9,  0,  3,  5,  6, 11};


u8 P[] = {	16,  7, 20, 21,
		29, 12, 28, 17,
		 1, 15, 23, 26,
		 5, 18, 31, 10,
		 2,  8, 24, 14,
		32, 27,  3,  9,
		19, 13, 30,  6,
		22, 11,  4, 25 };

int DES_d;

static inline void keys_gen(u8 *keys, u8 *des_key)
{
    u8 PCP[58];	//56
    u8 PCR[58];	//56
    u8 cycle;
    u8 r;
    u8 j;
    u8 l;
    
    memset(PCP, 0, sizeof(PCP));
    memset(PCR, 0, sizeof(PCR));

    for (j = 0; j < 56; j++)
    {
	l = PC1[j] - 1;
	PCP[j] = (des_key[l >> 3] >> (l & 7)) & 1;
    }

    for ( cycle = 0; cycle < 16; cycle++)
    {
    for ( j = 0; j < 28; j++)
    {
	r = j + tab_rotate[cycle];
	if (r >= 28)
	     r -= 28;
	PCR[j] = PCP[r];
    }
    
    for ( j = 28; j < 56; j++)
    {
	r = j + tab_rotate[cycle];
	if (r >= 56)
    	    r -= 28;
	PCR[j] = PCP[r];
    }

    for ( j = 0; j < 56; j++)
	PCP[j] = PCR[j];

    for ( j = 0; j < 48; j++)
    {
	l = PC2[j] - 1;
	keys[j >> 3] |= (PCR[l] << (j & 7));
    }
	keys += 6;
    }
}

static inline void s_box(u8 *PES, u8 *PE, u8 *S)
{
    u8 w;
    u8 k;
    u8 l;
    u8 eS;
    u8 i;
    
    w = (PE[5] << 1) + PE[0];
    k = (PE[4] << 3) + (PE[3] << 2) + (PE[2] << 1) + PE[1];
    l = (w << 4) + k;
    
    eS = S[l];
    
    for (i = 0; i < 4; i++)
    	PES[i] = (eS >> i) & 1;
}

static inline void f(u8 *Ri, u8 *Ki, u8 *PP)
{
    u8 PES[34];	//32
    u8 PE[50];	//48
    int i, l;

    memset(PE,  0, sizeof(PE));
    memset(PES, 0, sizeof(PES));
    
    for (i = 0; i < 48; i++)
    {
	l = E[i] - 1;
	PE[i] =  (Ri[l >> 3] >> (l & 7)) & 1;
	PE[i] ^= (Ki[i >> 3] >> (i & 7)) & 1;
    }
    
    s_box(PES, PE, S8);
    s_box(PES + 4, PE + 6, S7);
    s_box(PES + 8, PE + 12, S6);
    s_box(PES + 12, PE + 18, S5);
    s_box(PES + 16, PE + 24, S4);
    s_box(PES + 20, PE + 30, S3);
    s_box(PES + 24, PE + 36, S2);
    s_box(PES + 28, PE + 42, S1);
    
    for (i = 0; i < 32; i++)
    {
	l = P[i] - 1;
	PP[i >> 3] |= PES[l] << (i & 7);
    }
}

static inline void one_cycle(u8 *L, u8 *R, u8 *key)
{
    int i;
    u8 PP[6]; 

    memset(PP, 0, sizeof(PP));
    
    f(R, key, PP);
    for (i = 0; i < 32; i++)
    {
	L[i] ^= (PP[i >> 3] >> (i & 7)) & 1;
    }
}

static inline void generate_key(u8 *des_key, char *passwd)
{
//    u32 key_l;    
//    u32 key_h;    
    int i, j;
    
    for (i = 0, j = 0; passwd[i] != 0; i++)
    {
	des_key[j] += passwd[j];
	j++;
	if (j >= 8) j = 0;
    }
}

static inline void rotate_key(u8 *des_key)
{
    int i, l;
    u8 des_key2[10];
    
    memset(des_key2, 0, sizeof(des_key2));
    for (i = 0; i < 64; i++)
    {
	l = PSW[i] - 1;
	des_key2[i >> 3] |= ((des_key[l >> 3] >> (l & 7)) & 1) << (i & 7);
    }

    for (i = 0; i < 8; i++)
	des_key[i] ^= des_key2[i] + 1;
}

int DES_crypt(char *in_buf, char *out_buf, char *passwd, int size)
{
    u32 ile;
    int i;
    int l;
    u8 PIP[72];
    u8 PIPr[72];
    u8 keys[6*16];
    u8 des_key[10];
//    u8 *out_buf = (u8 *)get_free_pages(0);
//    u8 *out_buf_ptr = out_buf;

    if (size > PAGE_SIZE)
	return -1;

    memset(PIP, 0, sizeof(PIP));
    memset(out_buf, 0, size + 2);    
    memset(des_key, 0, sizeof(des_key));

    generate_key(des_key, passwd);

    for (ile = 0; ile <= ((size - 1) >> 3); ile++)
    {

//        rotate_key(des_key);	/* changed by dembol */
	memset(keys, 0, sizeof(keys));
    
        keys_gen(keys, des_key);

        for (i = 0; i < 8; i++)
	{
	    des_key[i] = keys[i];
    	    des_key[i] ^= keys[i + 4];
	}

        for (i = 0; i < 64; i++)
	{
	    l = IP[i] - 1;
	    PIP[i] = (in_buf[l >> 3] >> (l & 7)) & 1;
	}

	for (i = 0; i < 8; i++)
        {
	    one_cycle(PIP + 32, PIP, keys + i * 2 * 6);
    	    one_cycle(PIP, PIP + 32, keys + i * 2 * 6 + 6);
	}

	for (i = 0; i < 32; i++)
	{
	    PIPr[i] = PIP[32 + i];
	    PIPr[i + 32] = PIP[i];
	}

	for (i = 0; i < 64; i++)
	{
	    l = IPR[i] - 1;
	    out_buf[i >> 3] |= PIPr[l] << (i & 7);
	}
    
	in_buf += 8;
	out_buf += 8;
    }

//    memcpy(in_buf, out_buf_ptr, size);
//    put_free_pages((u32)out_buf_ptr, 0);

    return 0;
}

int DES_decrypt(char *in_buf, char *out_buf, char *passwd, int size)
{
    u32 ile;
    int i;
    int j;
    int l;
    u8 PIP[70];
    u8 PIPr[70];
    u8 keys[6*16];
    u8 des_key[10];
//    u8 *out_buf = (u8 *)get_free_pages(0);
//    u8 *out_buf_ptr = out_buf;

    if (size > PAGE_SIZE)
	return -1;
    
    memset(PIP, 0, sizeof(PIP));
    memset(out_buf, 0, size + 2);    
    memset(des_key, 0, sizeof(des_key));

    generate_key(des_key, passwd);

    for (ile = 0; ile <= ((size - 1) >> 3); ile++)
    {

//	rotate_key(des_key);
	memset(keys, 0, sizeof(keys));
	keys_gen(keys, des_key);

	for (i = 0; i < 8; i++)
	{
	    des_key[i] = keys[i];
	    des_key[i] ^= keys[i + 4];
	}

	for (i = 0; i < 64; i++)
	{
	    l = IP[i] - 1;
	    PIP[i] = (in_buf[l >> 3] >> (l & 7)) & 1;
	}

	j = 7;
	for (i = 0; i < 8; i++)
	{
    	    one_cycle(PIP + 32, PIP, keys + j * 2 * 6 + 6);
	    one_cycle(PIP, PIP + 32, keys + j * 2 * 6);
    	    j--;
	}

	for (i = 0; i < 32; i++)
	{
	    PIPr[i] = PIP[32 + i];
	    PIPr[i + 32] = PIP[i];
	}

	for (i = 0; i < 64; i++)
	{
	    l = IPR[i] - 1;
	    out_buf[i >> 3] |= PIPr[l] << (i & 7);
	}

	in_buf += 8;
	out_buf += 8;
    }
    
//    memcpy(in_buf, out_buf_ptr, size);
//    put_free_pages((u32)out_buf_ptr, 0);

    return 0;
}

int DES_setup(void)
{
    return 0;
}

struct crypt_algo_s DES_algo = {
    "DES",
    DES_setup,
    DES_crypt,
    DES_decrypt
};

int DES_init(void)
{
    DES_d = register_crypt_algo(&DES_algo);
    
    if (DES_d < 0)
	return -1;
    
    return 0;
}
