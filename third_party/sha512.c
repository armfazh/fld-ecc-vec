#include <sha512.h>
/* $Id: sha2big.c 216 2010-06-08 09:46:57Z tp $ */
/*
 * SHA-384 / SHA-512 implementation.
 *
 * ==========================(LICENSE BEGIN)============================
 *
 * Copyright (c) 2007-2010  Projet RNRT SAPHIR
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *7
 * ===========================(LICENSE END)=============================
 *
 * @author   Thomas Pornin <thomas.pornin@cryptolog.com>
 */

#include <stddef.h>
#include <string.h>

//#include "sph_sha2.h"

#if SPH_64

#define CH(X, Y, Z)    ((((Y) ^ (Z)) & (X)) ^ (Z))
#define MAJ(X, Y, Z)   (((X) & (Y)) | (((X) | (Y)) & (Z)))

#define ROTR64    SPH_ROTR64

#define BSG5_0(x)      (ROTR64(x, 28) ^ ROTR64(x, 34) ^ ROTR64(x, 39))
#define BSG5_1(x)      (ROTR64(x, 14) ^ ROTR64(x, 18) ^ ROTR64(x, 41))
#define SSG5_0(x)      (ROTR64(x, 1) ^ ROTR64(x, 8) ^ SPH_T64((x) >> 7))
#define SSG5_1(x)      (ROTR64(x, 19) ^ ROTR64(x, 61) ^ SPH_T64((x) >> 6))

static const sph_u64 K512[80] = {
	SPH_C64(0x428A2F98D728AE22), SPH_C64(0x7137449123EF65CD),
	SPH_C64(0xB5C0FBCFEC4D3B2F), SPH_C64(0xE9B5DBA58189DBBC),
	SPH_C64(0x3956C25BF348B538), SPH_C64(0x59F111F1B605D019),
	SPH_C64(0x923F82A4AF194F9B), SPH_C64(0xAB1C5ED5DA6D8118),
	SPH_C64(0xD807AA98A3030242), SPH_C64(0x12835B0145706FBE),
	SPH_C64(0x243185BE4EE4B28C), SPH_C64(0x550C7DC3D5FFB4E2),
	SPH_C64(0x72BE5D74F27B896F), SPH_C64(0x80DEB1FE3B1696B1),
	SPH_C64(0x9BDC06A725C71235), SPH_C64(0xC19BF174CF692694),
	SPH_C64(0xE49B69C19EF14AD2), SPH_C64(0xEFBE4786384F25E3),
	SPH_C64(0x0FC19DC68B8CD5B5), SPH_C64(0x240CA1CC77AC9C65),
	SPH_C64(0x2DE92C6F592B0275), SPH_C64(0x4A7484AA6EA6E483),
	SPH_C64(0x5CB0A9DCBD41FBD4), SPH_C64(0x76F988DA831153B5),
	SPH_C64(0x983E5152EE66DFAB), SPH_C64(0xA831C66D2DB43210),
	SPH_C64(0xB00327C898FB213F), SPH_C64(0xBF597FC7BEEF0EE4),
	SPH_C64(0xC6E00BF33DA88FC2), SPH_C64(0xD5A79147930AA725),
	SPH_C64(0x06CA6351E003826F), SPH_C64(0x142929670A0E6E70),
	SPH_C64(0x27B70A8546D22FFC), SPH_C64(0x2E1B21385C26C926),
	SPH_C64(0x4D2C6DFC5AC42AED), SPH_C64(0x53380D139D95B3DF),
	SPH_C64(0x650A73548BAF63DE), SPH_C64(0x766A0ABB3C77B2A8),
	SPH_C64(0x81C2C92E47EDAEE6), SPH_C64(0x92722C851482353B),
	SPH_C64(0xA2BFE8A14CF10364), SPH_C64(0xA81A664BBC423001),
	SPH_C64(0xC24B8B70D0F89791), SPH_C64(0xC76C51A30654BE30),
	SPH_C64(0xD192E819D6EF5218), SPH_C64(0xD69906245565A910),
	SPH_C64(0xF40E35855771202A), SPH_C64(0x106AA07032BBD1B8),
	SPH_C64(0x19A4C116B8D2D0C8), SPH_C64(0x1E376C085141AB53),
	SPH_C64(0x2748774CDF8EEB99), SPH_C64(0x34B0BCB5E19B48A8),
	SPH_C64(0x391C0CB3C5C95A63), SPH_C64(0x4ED8AA4AE3418ACB),
	SPH_C64(0x5B9CCA4F7763E373), SPH_C64(0x682E6FF3D6B2B8A3),
	SPH_C64(0x748F82EE5DEFB2FC), SPH_C64(0x78A5636F43172F60),
	SPH_C64(0x84C87814A1F0AB72), SPH_C64(0x8CC702081A6439EC),
	SPH_C64(0x90BEFFFA23631E28), SPH_C64(0xA4506CEBDE82BDE9),
	SPH_C64(0xBEF9A3F7B2C67915), SPH_C64(0xC67178F2E372532B),
	SPH_C64(0xCA273ECEEA26619C), SPH_C64(0xD186B8C721C0C207),
	SPH_C64(0xEADA7DD6CDE0EB1E), SPH_C64(0xF57D4F7FEE6ED178),
	SPH_C64(0x06F067AA72176FBA), SPH_C64(0x0A637DC5A2C898A6),
	SPH_C64(0x113F9804BEF90DAE), SPH_C64(0x1B710B35131C471B),
	SPH_C64(0x28DB77F523047D84), SPH_C64(0x32CAAB7B40C72493),
	SPH_C64(0x3C9EBE0A15C9BEBC), SPH_C64(0x431D67C49C100D4C),
	SPH_C64(0x4CC5D4BECB3E42B6), SPH_C64(0x597F299CFC657E2A),
	SPH_C64(0x5FCB6FAB3AD6FAEC), SPH_C64(0x6C44198C4A475817)
};

static const sph_u64 H384[8] = {
	SPH_C64(0xCBBB9D5DC1059ED8), SPH_C64(0x629A292A367CD507),
	SPH_C64(0x9159015A3070DD17), SPH_C64(0x152FECD8F70E5939),
	SPH_C64(0x67332667FFC00B31), SPH_C64(0x8EB44A8768581511),
	SPH_C64(0xDB0C2E0D64F98FA7), SPH_C64(0x47B5481DBEFA4FA4)
};

static const sph_u64 H512[8] = {
	SPH_C64(0x6A09E667F3BCC908), SPH_C64(0xBB67AE8584CAA73B),
	SPH_C64(0x3C6EF372FE94F82B), SPH_C64(0xA54FF53A5F1D36F1),
	SPH_C64(0x510E527FADE682D1), SPH_C64(0x9B05688C2B3E6C1F),
	SPH_C64(0x1F83D9ABFB41BD6B), SPH_C64(0x5BE0CD19137E2179)
};

/*
 * This macro defines the body for a SHA-384 / SHA-512 compression function
 * implementation. The "in" parameter should evaluate, when applied to a
 * numerical input parameter from 0 to 15, to an expression which yields
 * the corresponding input block. The "r" parameter should evaluate to
 * an array or pointer expression designating the array of 8 words which
 * contains the input and output of the compression function.
 *
 * SHA-512 is hard for the compiler. If the loop is completely unrolled,
 * then the code will be quite huge (possibly more than 100 kB), and the
 * performance will be degraded due to cache misses on the code. We
 * unroll only eight steps, which avoids all needless copies when
 * 64-bit registers are swapped.
 */

#define SHA3_STEP(A, B, C, D, E, F, G, H, i)   do { \
		sph_u64 T1, T2; \
		T1 = SPH_T64(H + BSG5_1(E) + CH(E, F, G) + K512[i] + W[i]); \
		T2 = SPH_T64(BSG5_0(A) + MAJ(A, B, C)); \
		D = SPH_T64(D + T1); \
		H = SPH_T64(T1 + T2); \
	} while (0)

#define SHA3_ROUND_BODY(in, r)   do { \
		int i; \
		sph_u64 A, B, C, D, E, F, G, H; \
		sph_u64 W[80]; \
 \
 		for (i = 0; i < 16; i ++) \
			W[i] = in(i); \
		for (i = 16; i < 80; i ++) \
 			W[i] = SPH_T64(SSG5_1(W[i - 2]) + W[i - 7] \
				+ SSG5_0(W[i - 15]) + W[i - 16]); \
		A = (r)[0]; \
		B = (r)[1]; \
		C = (r)[2]; \
		D = (r)[3]; \
		E = (r)[4]; \
		F = (r)[5]; \
		G = (r)[6]; \
		H = (r)[7]; \
		for (i = 0; i < 80; i += 8) { \
			SHA3_STEP(A, B, C, D, E, F, G, H, i + 0); \
			SHA3_STEP(H, A, B, C, D, E, F, G, i + 1); \
			SHA3_STEP(G, H, A, B, C, D, E, F, i + 2); \
			SHA3_STEP(F, G, H, A, B, C, D, E, i + 3); \
			SHA3_STEP(E, F, G, H, A, B, C, D, i + 4); \
			SHA3_STEP(D, E, F, G, H, A, B, C, i + 5); \
			SHA3_STEP(C, D, E, F, G, H, A, B, i + 6); \
			SHA3_STEP(B, C, D, E, F, G, H, A, i + 7); \
		} \
		(r)[0] = SPH_T64((r)[0] + A); \
		(r)[1] = SPH_T64((r)[1] + B); \
		(r)[2] = SPH_T64((r)[2] + C); \
		(r)[3] = SPH_T64((r)[3] + D); \
		(r)[4] = SPH_T64((r)[4] + E); \
		(r)[5] = SPH_T64((r)[5] + F); \
		(r)[6] = SPH_T64((r)[6] + G); \
		(r)[7] = SPH_T64((r)[7] + H); \
	} while (0)

/*
 * One round of SHA-384 / SHA-512. The data must be aligned for 64-bit access.
 */
static void
sha3_round(const unsigned char *data, sph_u64 r[8])
{
#define SHA3_IN(x)   sph_dec64be_aligned(data + (8 * (x)))
	SHA3_ROUND_BODY(SHA3_IN, r);
#undef SHA3_IN
}

/* see sph_sha3.h */
void
sph_sha384_init(void *cc)
{
	sph_sha384_context *sc;

	sc = (sph_sha384_context *)cc;
	memcpy(sc->val, H384, sizeof H384);
	sc->count = 0;
}

/* see sph_sha3.h */
void
sph_sha512_init(void *cc)
{
	sph_sha512_context *sc;

	sc = (sph_sha512_context *)cc;
	memcpy(sc->val, H512, sizeof H512);
	sc->count = 0;
}

#define RFUN   sha3_round
#define HASH   sha384
#define BE64   1

//#include "md_helper.i"
/* $Id: md_helper.c 216 2010-06-08 09:46:57Z tp $ */
/*
 * This file contains some functions which implement the external data
 * handling and padding for Merkle-Damgard hash functions which follow
 * the conventions set out by MD4 (little-endian) or SHA-1 (big-endian).
 *
 * API: this file is meant to be included, not compiled as a stand-alone
 * file. Some macros must be defined:
 *   RFUN   name for the round function
 *   HASH   "short name" for the hash function
 *   BE32   defined for big-endian, 32-bit based (e.g. SHA-1)
 *   LE32   defined for little-endian, 32-bit based (e.g. MD5)
 *   BE64   defined for big-endian, 64-bit based (e.g. SHA-512)
 *   LE64   defined for little-endian, 64-bit based (no example yet)
 *   PW01   if defined, append 0x01 instead of 0x80 (for Tiger)
 *   BLEN   if defined, length of a message block (in bytes)
 *   PLW1   if defined, length is defined on one 64-bit word only (for Tiger)
 *   PLW4   if defined, length is defined on four 64-bit words (for WHIRLPOOL)
 *   SVAL   if defined, reference to the context state information
 *
 * BLEN is used when a message block is not 16 (32-bit or 64-bit) words:
 * this is used for instance for Tiger, which works on 64-bit words but
 * uses 512-bit message blocks (eight 64-bit words). PLW1 and PLW4 are
 * ignored if 32-bit words are used; if 64-bit words are used and PLW1 is
 * set, then only one word (64 bits) will be used to encode the input
 * message length (in bits), otherwise two words will be used (as in
 * SHA-384 and SHA-512). If 64-bit words are used and PLW4 is defined (but
 * not PLW1), four 64-bit words will be used to encode the message length
 * (in bits). Note that regardless of those settings, only 64-bit message
 * lengths are supported (in bits): messages longer than 2 Exabytes will be
 * improperly hashed (this is unlikely to happen soon: 2 Exabytes is about
 * 2 millions Terabytes, which is huge).
 *
 * If CLOSE_ONLY is defined, then this file defines only the sph_XXX_close()
 * function. This is used for Tiger2, which is identical to Tiger except
 * when it comes to the padding (Tiger2 uses the standard 0x80 byte instead
 * of the 0x01 from original Tiger).
 *
 * The RFUN function is invoked with two arguments, the first pointing to
 * aligned data (as a "const void *"), the second being state information
 * from the context structure. By default, this state information is the
 * "val" field from the context, and this field is assumed to be an array
 * of words ("sph_u32" or "sph_u64", depending on BE32/LE32/BE64/LE64).
 * from the context structure. The "val" field can have any type, except
 * for the output encoding which assumes that it is an array of "sph_u32"
 * values. By defining NO_OUTPUT, this last step is deactivated; the
 * includer code is then responsible for writing out the hash result. When
 * NO_OUTPUT is defined, the third parameter to the "close()" function is
 * ignored.
 *
 * ==========================(LICENSE BEGIN)============================
 *
 * Copyright (c) 2007-2010  Projet RNRT SAPHIR
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * ===========================(LICENSE END)=============================
 *
 * @author   Thomas Pornin <thomas.pornin@cryptolog.com>
 */

#ifdef _MSC_VER
#pragma warning (disable: 4146)
#endif

#undef SPH_XCAT
#define SPH_XCAT(a, b)     SPH_XCAT_(a, b)
#undef SPH_XCAT_
#define SPH_XCAT_(a, b)    a ## b

#undef SPH_BLEN
#undef SPH_WLEN
#if defined BE64 || defined LE64
#define SPH_BLEN    128U
#define SPH_WLEN      8U
#else
#define SPH_BLEN     64U
#define SPH_WLEN      4U
#endif

#ifdef BLEN
#undef SPH_BLEN
#define SPH_BLEN    BLEN
#endif

#undef SPH_MAXPAD
#if defined PLW1
#define SPH_MAXPAD   (SPH_BLEN - SPH_WLEN)
#elif defined PLW4
#define SPH_MAXPAD   (SPH_BLEN - (SPH_WLEN << 2))
#else
#define SPH_MAXPAD   (SPH_BLEN - (SPH_WLEN << 1))
#endif

#undef SPH_VAL
#undef SPH_NO_OUTPUT
#ifdef SVAL
#define SPH_VAL         SVAL
#define SPH_NO_OUTPUT   1
#else
#define SPH_VAL   sc->val
#endif

#ifndef CLOSE_ONLY

#ifdef SPH_UPTR
static void
SPH_XCAT(HASH, _short)(void *cc, const void *data, size_t len)
#else
void
SPH_XCAT(sph_, HASH)(void *cc, const void *data, size_t len)
#endif
{
	SPH_XCAT(sph_, SPH_XCAT(HASH, _context)) *sc;
	unsigned current;

	sc = (SPH_XCAT(sph_, SPH_XCAT(HASH, _context)) *) cc;
#if SPH_64
	current = (unsigned)sc->count & (SPH_BLEN - 1U);
#else
	current = (unsigned)sc->count_low & (SPH_BLEN - 1U);
#endif
	while (len > 0) {
		unsigned clen;
#if !SPH_64
		sph_u32 clow, clow2;
#endif

		clen = SPH_BLEN - current;
		if (clen > len)
			clen = len;
		memcpy(sc->buf + current, data, clen);
		data = (const unsigned char *)data + clen;
		current += clen;
		len -= clen;
		if (current == SPH_BLEN) {
			RFUN(sc->buf, SPH_VAL);
			current = 0;
		}
#if SPH_64
		sc->count += clen;
#else
		clow = sc->count_low;
		clow2 = SPH_T32(clow + clen);
		sc->count_low = clow2;
		if (clow2 < clow)
			sc->count_high ++;
#endif
	}
}

#ifdef SPH_UPTR
void
SPH_XCAT(sph_, HASH)(void *cc, const void *data, size_t len)
{
	SPH_XCAT(sph_, SPH_XCAT(HASH, _context)) *sc;
	unsigned current;
	size_t orig_len;
#if !SPH_64
	sph_u32 clow, clow2;
#endif

	if (len < (2 * SPH_BLEN)) {
		SPH_XCAT(HASH, _short)(cc, data, len);
		return;
	}
	sc = (SPH_XCAT(sph_, SPH_XCAT(HASH, _context)) *)cc;
#if SPH_64
	current = (unsigned)sc->count & (SPH_BLEN - 1U);
#else
	current = (unsigned)sc->count_low & (SPH_BLEN - 1U);
#endif
	if (current > 0) {
		unsigned t;

		t = SPH_BLEN - current;
		SPH_XCAT(HASH, _short)(cc, data, t);
		data = (const unsigned char *)data + t;
		len -= t;
	}
#if !SPH_UNALIGNED
	if (((SPH_UPTR)data & (SPH_WLEN - 1U)) != 0) {
		SPH_XCAT(HASH, _short)(cc, data, len);
		return;
	}
#endif
	orig_len = len;
	while (len >= SPH_BLEN) {
		RFUN((const unsigned char *)data, SPH_VAL);
		len -= SPH_BLEN;
		data = (const unsigned char *)data + SPH_BLEN;
	}
	if (len > 0)
		memcpy(sc->buf, data, len);
#if SPH_64
	sc->count += (sph_u64)orig_len;
#else
	clow = sc->count_low;
	clow2 = SPH_T32(clow + orig_len);
	sc->count_low = clow2;
	if (clow2 < clow)
		sc->count_high ++;
	/*
	 * This code handles the improbable situation where "size_t" is
	 * greater than 32 bits, and yet we do not have a 64-bit type.
	 */
	orig_len >>= 12;
	orig_len >>= 10;
	orig_len >>= 10;
	sc->count_high += orig_len;
#endif
}
#endif

#endif

/*
 * Perform padding and produce result. The context is NOT reinitialized
 * by this function.
 */
static void
SPH_XCAT(HASH, _addbits_and_close)(void *cc,
	unsigned ub, unsigned n, void *dst, unsigned rnum)
{
	SPH_XCAT(sph_, SPH_XCAT(HASH, _context)) *sc;
	unsigned current, u;
#if !SPH_64
	sph_u32 low, high;
#endif

	sc = (SPH_XCAT(sph_, SPH_XCAT(HASH, _context)) *)cc;
#if SPH_64
	current = (unsigned)sc->count & (SPH_BLEN - 1U);
#else
	current = (unsigned)sc->count_low & (SPH_BLEN - 1U);
#endif
#ifdef PW01
	sc->buf[current ++] = (0x100 | (ub & 0xFF)) >> (8 - n);
#else
	{
		unsigned z;

		z = 0x80 >> n;
		sc->buf[current ++] = ((ub & -z) | z) & 0xFF;
	}
#endif
	if (current > SPH_MAXPAD) {
		memset(sc->buf + current, 0, SPH_BLEN - current);
		RFUN(sc->buf, SPH_VAL);
		memset(sc->buf, 0, SPH_MAXPAD);
	} else {
		memset(sc->buf + current, 0, SPH_MAXPAD - current);
	}
#if defined BE64
#if defined PLW1
	sph_enc64be_aligned(sc->buf + SPH_MAXPAD,
		SPH_T64(sc->count << 3) + (sph_u64)n);
#elif defined PLW4
	memset(sc->buf + SPH_MAXPAD, 0, 2 * SPH_WLEN);
	sph_enc64be_aligned(sc->buf + SPH_MAXPAD + 2 * SPH_WLEN,
		sc->count >> 61);
	sph_enc64be_aligned(sc->buf + SPH_MAXPAD + 3 * SPH_WLEN,
		SPH_T64(sc->count << 3) + (sph_u64)n);
#else
	sph_enc64be_aligned(sc->buf + SPH_MAXPAD, sc->count >> 61);
	sph_enc64be_aligned(sc->buf + SPH_MAXPAD + SPH_WLEN,
		SPH_T64(sc->count << 3) + (sph_u64)n);
#endif
#elif defined LE64
#if defined PLW1
	sph_enc64le_aligned(sc->buf + SPH_MAXPAD,
		SPH_T64(sc->count << 3) + (sph_u64)n);
#elif defined PLW1
	sph_enc64le_aligned(sc->buf + SPH_MAXPAD,
		SPH_T64(sc->count << 3) + (sph_u64)n);
	sph_enc64le_aligned(sc->buf + SPH_MAXPAD + SPH_WLEN, sc->count >> 61);
	memset(sc->buf + SPH_MAXPAD + 2 * SPH_WLEN, 0, 2 * SPH_WLEN);
#else
	sph_enc64le_aligned(sc->buf + SPH_MAXPAD,
		SPH_T64(sc->count << 3) + (sph_u64)n);
	sph_enc64le_aligned(sc->buf + SPH_MAXPAD + SPH_WLEN, sc->count >> 61);
#endif
#else
#if SPH_64
#ifdef BE32
	sph_enc64be_aligned(sc->buf + SPH_MAXPAD,
		SPH_T64(sc->count << 3) + (sph_u64)n);
#else
	sph_enc64le_aligned(sc->buf + SPH_MAXPAD,
		SPH_T64(sc->count << 3) + (sph_u64)n);
#endif
#else
	low = sc->count_low;
	high = SPH_T32((sc->count_high << 3) | (low >> 29));
	low = SPH_T32(low << 3) + (sph_u32)n;
#ifdef BE32
	sph_enc32be(sc->buf + SPH_MAXPAD, high);
	sph_enc32be(sc->buf + SPH_MAXPAD + SPH_WLEN, low);
#else
	sph_enc32le(sc->buf + SPH_MAXPAD, low);
	sph_enc32le(sc->buf + SPH_MAXPAD + SPH_WLEN, high);
#endif
#endif
#endif
	RFUN(sc->buf, SPH_VAL);
#ifdef SPH_NO_OUTPUT
	(void)dst;
	(void)rnum;
	(void)u;
#else
	for (u = 0; u < rnum; u ++) {
#if defined BE64
		sph_enc64be((unsigned char *)dst + 8 * u, sc->val[u]);
#elif defined LE64
		sph_enc64le((unsigned char *)dst + 8 * u, sc->val[u]);
#elif defined BE32
		sph_enc32be((unsigned char *)dst + 4 * u, sc->val[u]);
#else
		sph_enc32le((unsigned char *)dst + 4 * u, sc->val[u]);
#endif
	}
#endif
}

static void
SPH_XCAT(HASH, _close)(void *cc, void *dst, unsigned rnum)
{
	SPH_XCAT(HASH, _addbits_and_close)(cc, 0, 0, dst, rnum);
}


/* see sph_sha3.h */
void
sph_sha384_close(void *cc, void *dst)
{
	sha384_close(cc, dst, 6);
	sph_sha384_init(cc);
}

/* see sph_sha3.h */
void
sph_sha384_addbits_and_close(void *cc, unsigned ub, unsigned n, void *dst)
{
	sha384_addbits_and_close(cc, ub, n, dst, 6);
	sph_sha384_init(cc);
}

/* see sph_sha3.h */
void
sph_sha512_close(void *cc, void *dst)
{
	sha384_close(cc, dst, 8);
	sph_sha512_init(cc);
}

/* see sph_sha3.h */
void
sph_sha512_addbits_and_close(void *cc, unsigned ub, unsigned n, void *dst)
{
	sha384_addbits_and_close(cc, ub, n, dst, 8);
	sph_sha512_init(cc);
}

/* see sph_sha3.h */
void
sph_sha384_comp(const sph_u64 msg[16], sph_u64 val[8])
{
#define SHA3_IN(x)   msg[x]
	SHA3_ROUND_BODY(SHA3_IN, val);
#undef SHA3_IN
}

#endif
