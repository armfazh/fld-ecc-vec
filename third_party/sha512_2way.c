#include <stdio.h>
#include "multi_hash.h"

#define ZERO    ZERO_128
#define ADD     ADD64_128
#define OR      OR_128
#define XOR     XOR_128
#define AND     AND_128
#define SHL     SHL64_128
#define SHR     SHR64_128
#define SHUF8   SHUF8_128
#define SET1_64 SET1_64_128
#define LOAD    LOAD_128
#define STORE   STORE_128
#define BROAD   BROADCAST32_128
#define K       KSHA512

/*
static void printm128i(__m128i data)
{
    ALIGN uint64_t xz[2];
    STORE(xz,data);
    printf("%016lx %016lx\n",xz[1],xz[0]);
}
static void print_block(__m128i block[16])
{
    int i=0;
    uint64_t *ptr64 = (uint64_t*)block;
    for(i=0;i<16;i++)
    {
        printf("W_%02d (%016lx = %016lx) %02ld\n", i,ptr64[1],ptr64[0],(uint8_t*)ptr64 - (uint8_t*)block);
        ptr64+=2;
    }
}

static void print_state(__m128i state[8])
{
    int i=0;
    for(i=0;i<8;i++)
    {
        printf("block(%02d): ",i);printm128i(state[i]);
    }
}

static void print_letters(
        __m128i *a,__m128i *b,__m128i *c,__m128i *d,
        __m128i *e,__m128i *f,__m128i *g,__m128i *h)
{
    printm128i(*a);
    printm128i(*b);
    printm128i(*c);
    printm128i(*d);
    printm128i(*e);
    printm128i(*f);
    printm128i(*g);
    printm128i(*h);
}
*/

#define TRANSPOSE_2WAY(W0,W1)                  \
do{                                            \
    __m128i a0b0 = _mm_unpacklo_epi64(W0, W1); \
    __m128i a1b1 = _mm_unpackhi_epi64(W0, W1); \
    W0 = a0b0;                                 \
    W1 = a1b1;                                 \
}while(0)

static inline void transpose_state_2way(__m128i *w)
{
	TRANSPOSE_2WAY(w[0x0],w[0x1]);
    TRANSPOSE_2WAY(w[0x2],w[0x3]);
	TRANSPOSE_2WAY(w[0x4],w[0x5]);
    TRANSPOSE_2WAY(w[0x6],w[0x7]);
}

static inline void transpose_msg_2way(__m128i *w)
{
	TRANSPOSE_2WAY(w[0x0],w[0x1]);
    TRANSPOSE_2WAY(w[0x2],w[0x3]);
	TRANSPOSE_2WAY(w[0x4],w[0x5]);
    TRANSPOSE_2WAY(w[0x6],w[0x7]);
	TRANSPOSE_2WAY(w[0x8],w[0x9]);
    TRANSPOSE_2WAY(w[0xa],w[0xb]);
	TRANSPOSE_2WAY(w[0xc],w[0xd]);
    TRANSPOSE_2WAY(w[0xe],w[0xf]);
}

static inline void initialize(__m128i state[8])
{
    state[0] = SET1_64(0x6a09e667f3bcc908);
	state[1] = SET1_64(0xbb67ae8584caa73b);
	state[2] = SET1_64(0x3c6ef372fe94f82b);
	state[3] = SET1_64(0xa54ff53a5f1d36f1);
	state[4] = SET1_64(0x510e527fade682d1);
	state[5] = SET1_64(0x9b05688c2b3e6c1f);
	state[6] = SET1_64(0x1f83d9abfb41bd6b);
	state[7] = SET1_64(0x5be0cd19137e2179);
}
// ROT32(x, n)  SPH_t[3]2(((x) << (n)) | ((x) >> (32 - (n))))

//Ch(x,y,z)=(xey)xor(~xez)
//CH(a,b,c)    ((((b) ^ (c)) & (a)) ^ (c))
//Maj(x,y,z)=(xey)xor(xez)xor(yez)
//maj(a,b,c) (((b) & (c)) | (((b) | (c)) & (a)))
//Sig0=Rot2xorRot13xorRot22
//Sig1=Rot6xorRot11xorRot25
//T1=Kt+Wt+h+Sig1(e)+ch(e,f,g)
//T2=Sig0(a)+maj(a,b,c)
//a=T1+T2;b=a;c=b;d=c;e=d+T1;f=e;g=f;h=g;

//sig0(x)=t[6]=Rot7xorRot18xorRot3
//sig1(x)=t[7]=Rot17xorRot19xorRot10
//se (t<=15)Wt=Mt
//senão Wt=t[7](W_{t-2})+W_{t-7}+t[6](W_{t-15})+(W_{t-16})
static inline __m128i computeT1(__m128i e,__m128i f,__m128i g,__m128i h)
{
	__m128i t0,t1,t2,t3,t4,t5,T1;

	t0 = SHL(e, 64 - (28));
	t1 = SHL(e, 64 - (34));
	t2 = SHL(e, 64 - (39));
	t3 = SHR(e, (28));
	t4 = SHR(e, (34));
	t5 = SHR(e, (39));
	t3 = OR(t3,t0);
	t0 = XOR(f,g);
	t4 = OR(t4,t1);
	t5 = OR(t5,t2);
	t4 = XOR(t3,t4);
	t0 = AND(t0,e);
	t4 = XOR(t4,t5);
    T1 = ADD(h, t4);
	t0 = XOR(t0,g);
    T1 = ADD(T1,t0);
	return T1;
}

static inline __m128i computeT2(__m128i a,__m128i b,__m128i c)
{
    __m128i s0,s1,s2,s3,s4,s5,T2;
	s0 = OR(b, c);
	T2 = AND(b, c);
	s0 = AND(a,s0);
	s1 = SHL(a, 64 - (14));
	s2 = SHL(a, 64 - (18));
	s3 = SHL(a, 64 - (41));
	s4 = SHR(a, (14));
	s5 = SHR(a, (18));
	s4 = OR(s4,s1);
	s5 = OR(s5,s2);
	s1 = SHR(a, (41));
	s0 = OR(T2,s0);
	s1 = OR(s1,s3);
	s4 = XOR(s4,s5);
    T2 = XOR(s4,s1);
	T2 = ADD(T2, s0);
	return T2;
}

static inline __m128i msg_schedule(__m128i *W, uint8_t i)
{
    __m128i Wi,t1,t2,t3,t4,t5,t6,t7;

	t1 = SHL(W[i-2], 64 - (19));
	t2 = SHL(W[i-2], 64 - (61));
	t3 = SHR(W[i-2], 19);
	t4 = SHR(W[i-2], 61);
	t7 = OR(t1,t3);
	t4 = OR(t2,t4);
	t5 = SHR((W[i-2]), 6);
	t7 = XOR(t7,t4);
	t1 = SHR(W[i-15], (1));
	t2 = SHR(W[i-15], (8));
	t3 = SHL(W[i-15], 64 - (1));
	t4 = SHL(W[i-15], 64 - (8));
	t7 = XOR(t7,t5);
	t6 = OR(t1,t3);
	t4 = OR(t2,t4);
	t5 = SHR((W[i-15]), 7);
	t6 = XOR(t6,t4);
	t7 = ADD(t7, W[i-7]);
	t6 = XOR(t6,t5);
	t6 = ADD(t6, W[i-16]);
	Wi = ADD(t7,t6);
	return Wi;
}


static inline void sha512_permutation(__m128i *state, __m128i *message_block)
{
    int i;
    __m128i a, b, c, d, e, f, g, h;
    __m128i T1,T2,Ki;

    a = state[0];   b = state[1];
    c = state[2];   d = state[3];
    e = state[4];   f = state[5];
    g = state[6];   h = state[7];

    for(i=0;i<16;i++)
    {
		T1 = computeT1(e,f,g,h);
		T2 = computeT2(a,b,c);
		BROAD(Ki,K+i);
		T1 = ADD(T1,ADD(Ki,message_block[i]));
		h=g;g=f;f=e;e=ADD(d,T1);
		d=c;c=b;b=a;a=ADD(T1,T2);
	}
	for(i=16;i<80;i++)
	{
		message_block[i] = msg_schedule(message_block,i);
		T1 = computeT1(e,f,g,h);
		T2 = computeT2(a,b,c);
		BROAD(Ki,K+i);
		T1 = ADD(T1,ADD(Ki,message_block[i]));
		h=g;g=f;f=e;e=ADD(d,T1);
		d=c;c=b;b=a;a=ADD(T1,T2);
	}

    state[0] = ADD(state[0],a);
    state[1] = ADD(state[1],b);
    state[2] = ADD(state[2],c);
    state[3] = ADD(state[3],d);
    state[4] = ADD(state[4],e);
    state[5] = ADD(state[5],f);
    state[6] = ADD(state[6],g);
    state[7] = ADD(state[7],h);
}

void sha512_2w_avx_256b(uint8_t *message_2x[2], uint8_t *digest_2x[2])
{
    const __m128i big_endian = _mm_set_epi32(0x0c0d0e0f,0x08090a0b,0x04050607,0x00010203);
    int msg=0;
    unsigned int b=0;
    ALIGN __m128i state[8];
    ALIGN __m128i block[80];

    initialize(state);
    for (msg = 0; msg < 2; msg++)
    {
        for (b = 0; b < 2; b++)
        {
            block[2*b+msg] = SHUF8(LOAD(message_2x[msg]+b),big_endian);
        }
    }
	TRANSPOSE_2WAY(block[0x0],block[0x1]);
    TRANSPOSE_2WAY(block[0x2],block[0x3]);
    block[4] = SET1_64(0x80000000);
    for(b=5; b < 15; b++)
    {
        block[b] = ZERO;
    }
	block[15] = SET1_64(256);
	sha512_permutation(state, block);
    transpose_state_2way(state);
    for (msg = 0; msg < 2; msg++)
    {
        for (b = 0; b < 2; b++)
        {
            STORE(digest_2x[msg]+b,SHUF8(state[2*b+msg],big_endian));
        }
    }
}

void sha512_2w_avx(uint8_t *message_2x[2], unsigned int message_length, uint8_t *digest_2x[2])
{
    const __m128i big_endian = _mm_set_epi32(0x0c0d0e0f,0x08090a0b,0x04050607,0x00010203);
    const unsigned int num_blocks = message_length>>7;
    const unsigned int remainder_bytes = message_length-(num_blocks<<7);
    const uint64_t mlen_bits = message_length*8;
    ALIGN __m128i state[8];
    ALIGN __m128i block[80];
    ALIGN uint8_t buffer[80*2] = {0};
    int i=0,msg=0;
    unsigned int b=0;

    initialize(state);
    for(b=0;b<num_blocks;b++)
    {
        /* Load a 1024-bit message_2x */
        for (msg = 0; msg < 2; msg++)
        {
            for (i = 0; i < 2; i++)
            {
                block[2*i+msg] = SHUF8(LOAD(message_2x[msg]+2*b+i),big_endian);
            }
        }
		transpose_msg_2way(block);
		sha512_permutation(state, block);
    }
    /* Load a remainder of the message_4x */
	if(remainder_bytes<56)
	{
		for (msg = 0; msg < 2; msg++)
		{
			for (b = 0; b < remainder_bytes; b++)
			{
				buffer[80*msg+b] = message_2x[msg][80*num_blocks+b];
			}
			buffer[64*msg+remainder_bytes] = 0x80;
			for (b = 0; b < 8; b++)
			{
				buffer[80*(msg+1)-b-1] = (uint8_t)((mlen_bits>>(8*b))&0xFF);
			}
		}
		for (msg = 0; msg < 2; msg++)
		{
			for (b = 0; b < 2; b++)
			{
				block[2*msg+b] = SHUF8(LOAD(buffer+2*b+msg),big_endian);
			}
		}
		transpose_msg_2way(block);
		sha512_permutation(state, block);
	}
	else if(remainder_bytes<64)
	{
		/* Load a 512-bit message_2x */
		for (msg = 0; msg < 2; msg++)
		{
			for (b = 0; b < remainder_bytes; b++)
			{
				buffer[80*msg+b] = message_2x[msg][64*num_blocks+b];
			}
			buffer[80*msg+remainder_bytes] = 0x80;
		}
		for (msg = 0; msg < 2; msg++)
		{
			for (b = 0; b < 4; b++)
			{
				block[2*msg+b] = SHUF8(LOAD(buffer+4*b+msg),big_endian);
			}
		}
		transpose_msg_2way(block);
		sha512_permutation(state, block);
		for (msg = 0; msg < 2; msg++)
		{
			for (b = 0; b < 56; b++)
			{
				buffer[80*msg+b] = 0x00;
			}
			for (b = 0; b < 8; b++)
			{
				buffer[80*(msg+1)-b-1] = (uint8_t)((mlen_bits>>(8*b))&0xFF);
			}
		}
		for (msg = 0; msg < 2; msg++)
		{
			for (b = 0; b < 4; b++)
			{
				block[4*msg+b] = SHUF8(LOAD(buffer+4*b+msg),big_endian);
			}
		}
		transpose_msg_2way(block);
		sha512_permutation(state, block);
	}
    transpose_state_2way(state);
    for (msg = 0; msg < 2; msg++)
    {
        for (b = 0; b < 2; b++)
        {
            STORE(digest_2x[msg]+b,SHUF8(state[2*b+msg],big_endian));
        }
    }
}

#undef TRANSPOSE_4WAY
#undef ZERO
#undef ADD
#undef OR
#undef XOR
#undef AND
#undef SHL
#undef SHR
#undef SHUF8
#undef SET1_64
#undef LOAD
#undef STORE
#undef BROAD
#undef K
