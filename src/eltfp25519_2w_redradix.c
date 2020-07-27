/**
 * Copyright 2017 Armando Faz Hernández
 * This file is part of faz_ecc_avx2.
 *
 * faz_ecc_avx2 is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * faz_ecc_avx2 is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with faz_ecc_avx2.  If not, see <http://www.gnu.org/licenses/>.
 */

#if defined(FP255)

#define FN(NAME) SUFFIX(NAME,2,redradix)
#define DECL(RETURN, NAME) SCOPE RETURN FN(NAME)

DECL(void, copy)(argElement_2w c, argElement_2w a) {
	int i = 0;
	for (i = 0; i < (NUM_DIGITS_FP25519 / 2); i++) {
		c[i] = a[i];
	}
}

DECL(void, zero)(argElement_2w c) {
	int i = 0;
	for (i = 0; i < (NUM_DIGITS_FP25519 / 2); i++) {
		c[i] = ZERO;
	}
}

/**
 *
 * @param a
 * @param a0
 * @param a1
 */
DECL(void, inter)(argElement_2w a, argElement_1w a0, argElement_1w a1) {
	__m256i A0 = LOAD(a0 + 0);
	__m256i A4 = LOAD(a0 + 1);
	__m256i A8 = LOAD(a0 + 2);
	__m256i B0 = LOAD(a1 + 0);
	__m256i B4 = LOAD(a1 + 1);
	__m256i B8 = LOAD(a1 + 2);

	a[0] = PERM128(A0, B0, 0x20);
	a[1] = PERM128(A0, B0, 0x31);
	a[2] = PERM128(A4, B4, 0x20);
	a[3] = PERM128(A4, B4, 0x31);
	a[4] = PERM128(A8, B8, 0x20);
}

/**
 *
 * @param a0
 * @param a1
 * @param a
 */
DECL(void, deinter)(argElement_1w a0, argElement_1w a1, argElement_2w a) {
	STORE(a0 + 0, PERM128(a[0], a[1], 0x20));
	STORE(a1 + 0, PERM128(a[0], a[1], 0x31));
	STORE(a0 + 1, PERM128(a[2], a[3], 0x20));
	STORE(a1 + 1, PERM128(a[2], a[3], 0x31));
	STORE(a0 + 2, a[4]);
	STORE(a1 + 2, CAST128TO256(EXTR(a[4], 1)));
}

DECL(void, cmv)(__m256i bit, argElement_2w c, argElement_2w a, argElement_2w b) {
	int i = 0;
	const __m256i ONE = SET64(0, 1, 0, 1);
	const __m256i mask = SHUF32(SUB(ZERO,AND(bit,ONE)),0x44);
	for (i = 0; i < (NUM_DIGITS_FP25519 / 2); i++){
	    c[i] = _mm256_blendv_epi8(a[i],b[i],mask);
    }
}

/**
 *
 * @param c
 * @param a
 * @param b
 */
DECL(void, add)(argElement_2w c, argElement_2w a, argElement_2w b) {
	int i = 0;
	for (i = 0; i < (NUM_DIGITS_FP25519 / 2); i++) {
		c[i] = ADD(a[i], b[i]);
	}
}

static const ALIGN uint64_t CONST_2P_2P_H0H5[2 * NUM_DIGITS_FP25519] = {
		0x7ffffda, 0x3fffffe, 0x7ffffda, 0x3fffffe,
		0x3fffffe, 0x7fffffe, 0x3fffffe, 0x7fffffe,
		0x7fffffe, 0x3fffffe, 0x7fffffe, 0x3fffffe,
		0x3fffffe, 0x7fffffe, 0x3fffffe, 0x7fffffe,
		0x7fffffe, 0x3fffffe, 0x7fffffe, 0x3fffffe
};

static const ALIGN uint64_t CONST_2P_00_H0H5[2 * NUM_DIGITS_FP25519] = {
		0x7ffffda, 0x3fffffe, 0x0000000, 0x0000000,
		0x3fffffe, 0x7fffffe, 0x0000000, 0x0000000,
		0x7fffffe, 0x3fffffe, 0x0000000, 0x0000000,
		0x3fffffe, 0x7fffffe, 0x0000000, 0x0000000,
		0x7fffffe, 0x3fffffe, 0x0000000, 0x0000000
};

/**
 *
 * @param c
 * @param a
 * @param b
 */
DECL(void, sub)(argElement_2w c, argElement_2w a, argElement_2w b) {
	argElement_2w _2P = (argElement_2w) CONST_2P_2P_H0H5;
	int i = 0;
	for (i = 0; i < (NUM_DIGITS_FP25519 / 2); i++) {
		c[i] = ADD(a[i], SUB(_2P[i], b[i]));
	}
}

/**
 * Given
 * 		A = [X | Y]
 * Cumputes:
 * 	if permutation == 0
 * 		C = [X|Y] + [2p|0] + [-Y|X]
 * 		C = [ X+2p-Y | Y+X ]
 * 	if permutation == 1
 * 		C = [Y|X] + [2p|0] + [-X|Y]
 * 		C = [ Y+2p-X | Y+X ]
 * 	if permutation != {0,1}
 * 		undefined
 * @param c
 * @param a
 * @param permutation
 */
DECL(void, subadd)(argElement_2w c, argElement_2w a, const int permutation) {
	const __m256i mask_subadd = SET64(0, 0, -1, -1);
	argElement_2w _2P_00 = (argElement_2w) CONST_2P_00_H0H5;
	int i = 0;
	for (i = 0; i < (NUM_DIGITS_FP25519 / 2); i++) {
		__m256i M = ZERO, N = ZERO, PA;
		PA = PERM64(a[i], 0x4E);
		if (permutation == 0) {
			M = a[i];
			N = PA;
		} else if (permutation == 1) {
			N = a[i];
			M = PA;
		}
		c[i] = ADD(ADD(M, _2P_00[i]), SUB(XOR(N, mask_subadd), mask_subadd));
	}
}

/**
 * Given
 * 		A = [X |Y ]
 * Returns
 * 		C = [2P-X | Y]
 * @param c
 * @param a
 */
DECL(void, ngz)(argElement_2w c, argElement_2w a) {
	const __m256i mask_subadd = SET64(0, 0, -1, -1);
	argElement_2w _2P_00 = (argElement_2w) CONST_2P_00_H0H5;
	int i = 0;
	for (i = 0; i < (NUM_DIGITS_FP25519 / 2); i++) {
		c[i] = ADD(_2P_00[i], SUB(XOR(a[i], mask_subadd), mask_subadd));
	}
}

DECL(void, addsub)(argElement_2w a, argElement_2w b) {
	argElement_2w _2P = (argElement_2w) CONST_2P_2P_H0H5;
	__m256i d, c;
	int i = 0;
	for (i = 0; i < (NUM_DIGITS_FP25519 / 2); i++) {
		d = ADD(a[i], b[i]);
		c = ADD(a[i], SUB(_2P[i], b[i]));
		a[i] = d;
		b[i] = c;
	}
}

static const ALIGN uint64_t GLOB_times_19[4] = {19, 19, 19, 19};
static const ALIGN uint64_t GLOB_sh_0[4] = {1, 0, 1, 0};
static const ALIGN uint64_t GLOB_sh_1[4] = {0, 1, 0, 1};

void FN(intmul2)(
    argElement_2w d, argElement_2w c,
    argElement_2w a, argElement_2w b) {
  argElement_2w const times_19 = (argElement_2w)GLOB_times_19;
  argElement_2w const sh_0 = (argElement_2w)GLOB_sh_0;
  argElement_2w const sh_1 = (argElement_2w)GLOB_sh_1;

  __m256i ai,aj,ak,al;
	__m256i b0,b1,b2,b3,b4;
	__m256i d0,d1,d2,d3,d4;
	__m256i e0,e1,e2,e3,e4;
	__m256i c0,c1,c2,c3,c4;
	__m256i p0,p1,p2,p3,p4;
	__m256i k0,k1,k2,k3,k4;
	__m256i m0,m1,m2,m3,m4;

	b0 = b[0];
	b1 = b[1];
	b2 = b[2];
	b3 = b[3];
	b4 = b[4];

    e0 = MUL(b0,*times_19); d0 = ALIGNR(b0,e0);
    e1 = MUL(b1,*times_19); d1 = ALIGNR(b1,e1);
    e2 = MUL(b2,*times_19); d2 = ALIGNR(b2,e2);
    e3 = MUL(b3,*times_19); d3 = ALIGNR(b3,e3);
    e4 = MUL(b4,*times_19); d4 = ALIGNR(b4,e4);

	ai = SHUF32(a[0],0x44);          ak = PERM64(a[0],0x0A);
	c0 = MUL(b0,ai);       		     p0 = MUL(b0,ak);
	c1 = MUL(b1,ai);       		     p1 = MUL(b1,ak);
	c2 = MUL(b2,ai);       		     p2 = MUL(b2,ak);
	c3 = MUL(b3,ai);       		     p3 = MUL(b3,ak);
	c4 = MUL(b4,ai);       		     p4 = MUL(b4,ak);

	ai = SHUF32(a[2],0x44);          ak = PERM64(a[2],0x0A);
	c0 = ADD(c0,MUL(d3,ai));	     p0 = ADD(p0,MUL(d3,ak));
	c1 = ADD(c1,MUL(d4,ai));	     p1 = ADD(p1,MUL(d4,ak));
	c2 = ADD(c2,MUL(b0,ai));	     p2 = ADD(p2,MUL(b0,ak));
	c3 = ADD(c3,MUL(b1,ai));	     p3 = ADD(p3,MUL(b1,ak));
	c4 = ADD(c4,MUL(b2,ai));	     p4 = ADD(p4,MUL(b2,ak));

	ai = SHUF32(a[4],0x44);          ak = PERM64(a[4],0x0A);
	c0 = ADD(c0,MUL(d1,ai));	     p0 = ADD(p0,MUL(d1,ak));
	c1 = ADD(c1,MUL(d2,ai));	     p1 = ADD(p1,MUL(d2,ak));
	c2 = ADD(c2,MUL(d3,ai));	     p2 = ADD(p2,MUL(d3,ak));
	c3 = ADD(c3,MUL(d4,ai));	     p3 = ADD(p3,MUL(d4,ak));
	c4 = ADD(c4,MUL(b0,ai));	     p4 = ADD(p4,MUL(b0,ak));

	ai = SHUF32(a[1],0xEE);          ak = PERM64(a[1],0x5F);
	c0 = ADD(c0,MUL(e4,ai));	     p0 = ADD(p0,MUL(e4,ak));
	c1 = ADD(c1,MUL(d0,ai));	     p1 = ADD(p1,MUL(d0,ak));
	c2 = ADD(c2,MUL(d1,ai));	     p2 = ADD(p2,MUL(d1,ak));
	c3 = ADD(c3,MUL(d2,ai));	     p3 = ADD(p3,MUL(d2,ak));
	c4 = ADD(c4,MUL(d3,ai));	     p4 = ADD(p4,MUL(d3,ak));

	ai = SHUF32(a[3],0xEE);          ak = PERM64(a[3],0x5F);
	c0 = ADD(c0,MUL(e2,ai));	     p0 = ADD(p0,MUL(e2,ak));
	c1 = ADD(c1,MUL(e3,ai));	     p1 = ADD(p1,MUL(e3,ak));
	c2 = ADD(c2,MUL(e4,ai));	     p2 = ADD(p2,MUL(e4,ak));
	c3 = ADD(c3,MUL(d0,ai));	     p3 = ADD(p3,MUL(d0,ak));
	c4 = ADD(c4,MUL(d1,ai));	     p4 = ADD(p4,MUL(d1,ak));

	d[0] = c0;        		         c[0] = p0;
	d[1] = c1;        		         c[1] = p1;
	d[2] = c2;        		         c[2] = p2;
	d[3] = c3;        		         c[3] = p3;
	d[4] = c4;        		         c[4] = p4;

	aj = SHUF32(a[1],0x44);          al = PERM64(a[1],0x0A);
	k0 = MUL(d4,aj);        		 m0 = MUL(d4,al);
	k1 = MUL(b0,aj);        		 m1 = MUL(b0,al);
	k2 = MUL(b1,aj);        		 m2 = MUL(b1,al);
	k3 = MUL(b2,aj);        		 m3 = MUL(b2,al);
	k4 = MUL(b3,aj);        		 m4 = MUL(b3,al);

	aj = SHUF32(a[3],0x44);          al = PERM64(a[3],0x0A);
	k0 = ADD(k0,MUL(d2,aj));		 m0 = ADD(m0,MUL(d2,al));
	k1 = ADD(k1,MUL(d3,aj));		 m1 = ADD(m1,MUL(d3,al));
	k2 = ADD(k2,MUL(d4,aj));		 m2 = ADD(m2,MUL(d4,al));
	k3 = ADD(k3,MUL(b0,aj));		 m3 = ADD(m3,MUL(b0,al));
	k4 = ADD(k4,MUL(b1,aj));		 m4 = ADD(m4,MUL(b1,al));

    aj = SHUF32(a[0],0xEE);          al = PERM64(a[0],0x5F);
    k0 = ADD(k0,MUL(d0,aj));		 m0 = ADD(m0,MUL(d0,al));
    k1 = ADD(k1,MUL(d1,aj));		 m1 = ADD(m1,MUL(d1,al));
    k2 = ADD(k2,MUL(d2,aj));		 m2 = ADD(m2,MUL(d2,al));
    k3 = ADD(k3,MUL(d3,aj));		 m3 = ADD(m3,MUL(d3,al));
    k4 = ADD(k4,MUL(d4,aj));		 m4 = ADD(m4,MUL(d4,al));

    aj = SHUF32(a[2],0xEE);          al = PERM64(a[2],0x5F);
    k0 = ADD(k0,MUL(e3,aj));		 m0 = ADD(m0,MUL(e3,al));
    k1 = ADD(k1,MUL(e4,aj));		 m1 = ADD(m1,MUL(e4,al));
    k2 = ADD(k2,MUL(d0,aj));		 m2 = ADD(m2,MUL(d0,al));
    k3 = ADD(k3,MUL(d1,aj));		 m3 = ADD(m3,MUL(d1,al));
    k4 = ADD(k4,MUL(d2,aj));		 m4 = ADD(m4,MUL(d2,al));

    aj = SHUF32(a[4],0xEE);          al = PERM64(a[4],0x5F);
    k0 = ADD(k0,MUL(e1,aj));		 m0 = ADD(m0,MUL(e1,al));
    k1 = ADD(k1,MUL(e2,aj));		 m1 = ADD(m1,MUL(e2,al));
    k2 = ADD(k2,MUL(e3,aj));		 m2 = ADD(m2,MUL(e3,al));
    k3 = ADD(k3,MUL(e4,aj));		 m3 = ADD(m3,MUL(e4,al));
    k4 = ADD(k4,MUL(d0,aj));		 m4 = ADD(m4,MUL(d0,al));

	d[0] = ADD(d[0],SHLV(k0,*sh_0));  c[0] = ADD(c[0],SHLV(m0,*sh_0));
	d[1] = ADD(d[1],SHLV(k1,*sh_1));  c[1] = ADD(c[1],SHLV(m1,*sh_1));
	d[2] = ADD(d[2],SHLV(k2,*sh_0));  c[2] = ADD(c[2],SHLV(m2,*sh_0));
	d[3] = ADD(d[3],SHLV(k3,*sh_1));  c[3] = ADD(c[3],SHLV(m3,*sh_1));
	d[4] = ADD(d[4],SHLV(k4,*sh_0));  c[4] = ADD(c[4],SHLV(m4,*sh_0));
}

DECL(void, intmul)(argElement_2w c, argElement_2w a, argElement_2w b) {
  argElement_2w const times_19 = (argElement_2w)GLOB_times_19;
  argElement_2w const sh_0 = (argElement_2w)GLOB_sh_0;
  argElement_2w const sh_1 = (argElement_2w)GLOB_sh_1;

	__m256i ai,aj;
	__m256i b0,b1,b2,b3,b4;
	__m256i d0,d1,d2,d3,d4;
	__m256i e0,e1,e2,e3,e4;
	__m256i c0,c1,c2,c3,c4;
	__m256i k0,k1,k2,k3,k4;
	__m256i t0,t1,t2,t3,t4;

	b0 = b[0];
	b1 = b[1];
	b2 = b[2];
	b3 = b[3];
	b4 = b[4];

	e4 = MUL(b4,*times_19); d4 = ALIGNR(b4,e4);
	e3 = MUL(b3,*times_19); d3 = ALIGNR(b3,e3);
	e2 = MUL(b2,*times_19); d2 = ALIGNR(b2,e2);
	e1 = MUL(b1,*times_19); d1 = ALIGNR(b1,e1);
	e0 = MUL(b0,*times_19); d0 = ALIGNR(b0,e0);

	ai = SHUF32(a[0],0x44);  		aj = SHUF32(a[1],0x44);
	c0 = MUL(b0,ai);       			k0 = MUL(d4,aj);
	c1 = MUL(b1,ai);       			k1 = MUL(b0,aj);
	c2 = MUL(b2,ai);       			k2 = MUL(b1,aj);
	c3 = MUL(b3,ai);       			k3 = MUL(b2,aj);
	c4 = MUL(b4,ai);       			k4 = MUL(b3,aj);

	ai = SHUF32(a[2], 0x44);    	aj = SHUF32(a[3], 0x44);
	c0 = ADD(c0,MUL(d3,ai));		k0 = ADD(k0,MUL(d2,aj));
	c1 = ADD(c1,MUL(d4,ai));		k1 = ADD(k1,MUL(d3,aj));
	c2 = ADD(c2,MUL(b0,ai));		k2 = ADD(k2,MUL(d4,aj));
	c3 = ADD(c3,MUL(b1,ai));		k3 = ADD(k3,MUL(b0,aj));
	c4 = ADD(c4,MUL(b2,ai));		k4 = ADD(k4,MUL(b1,aj));

	ai = SHUF32(a[4],0x44);         aj = SHUF32(a[0],0xEE);
	c0 = ADD(c0,MUL(d1,ai));        k0 = ADD(k0,MUL(d0,aj));
	c1 = ADD(c1,MUL(d2,ai));        k1 = ADD(k1,MUL(d1,aj));
	c2 = ADD(c2,MUL(d3,ai));        k2 = ADD(k2,MUL(d2,aj));
	c3 = ADD(c3,MUL(d4,ai));        k3 = ADD(k3,MUL(d3,aj));
	c4 = ADD(c4,MUL(b0,ai));        k4 = ADD(k4,MUL(d4,aj));

	ai = SHUF32(a[1],0xEE);         aj = SHUF32(a[2],0xEE);
	c0 = ADD(c0,MUL(e4,ai));        k0 = ADD(k0,MUL(e3,aj));
	c1 = ADD(c1,MUL(d0,ai));        k1 = ADD(k1,MUL(e4,aj));
	c2 = ADD(c2,MUL(d1,ai));        k2 = ADD(k2,MUL(d0,aj));
	c3 = ADD(c3,MUL(d2,ai));        k3 = ADD(k3,MUL(d1,aj));
	c4 = ADD(c4,MUL(d3,ai));        k4 = ADD(k4,MUL(d2,aj));

	ai = SHUF32(a[3],0xEE);         aj = SHUF32(a[4],0xEE);
	c0 = ADD(c0,MUL(e2,ai));        k0 = ADD(k0,MUL(e1,aj));
	c1 = ADD(c1,MUL(e3,ai));        k1 = ADD(k1,MUL(e2,aj));
	c2 = ADD(c2,MUL(e4,ai));        k2 = ADD(k2,MUL(e3,aj));
	c3 = ADD(c3,MUL(d0,ai));        k3 = ADD(k3,MUL(e4,aj));
	c4 = ADD(c4,MUL(d1,ai));        k4 = ADD(k4,MUL(d0,aj));

	t0 = SHLV(k0,*sh_0);
	t1 = SHLV(k1,*sh_1);
	t2 = SHLV(k2,*sh_0);
	t3 = SHLV(k3,*sh_1);
	t4 = SHLV(k4,*sh_0);

	c[0] = ADD(c0,t0);
	c[1] = ADD(c1,t1);
	c[2] = ADD(c2,t2);
	c[3] = ADD(c3,t3);
	c[4] = ADD(c4,t4);
}

DECL(void, intsqr)(argElement_2w c) {
  argElement_2w const times_19 = (argElement_2w)GLOB_times_19;
  argElement_2w const sh_0 = (argElement_2w)GLOB_sh_0;
  argElement_2w const sh_1 = (argElement_2w)GLOB_sh_1;

	__m256i ai,aj,a2i,a2j;
	__m256i b0,b1,b2,b3,b4;
	__m256i d0,d1,d2,d3,d4;
	__m256i e0,e1,e2,e3,e4;
	__m256i c0,c1,c2,c3,c4;
	__m256i k0,k1,k2,k3,k4;
	__m256i t0,t1,t2,t3,t4;

	b0 = c[0];
	b1 = c[1];
	b2 = c[2];
	b3 = c[3];
	b4 = c[4];

	e4 = MUL(b4,*times_19); d4 = ALIGNR(b4,e4);
	e3 = MUL(b3,*times_19); d3 = ALIGNR(b3,e3);
	e2 = MUL(b2,*times_19); d2 = ALIGNR(b2,e2);
	e1 = MUL(b1,*times_19); d1 = ALIGNR(b1,e1);
	e0 = MUL(b0,*times_19); d0 = ALIGNR(b0,e0);

	ai = SHUF32(c[0],0x44);  	    aj = SHUF32(c[1],0x44);
	a2i = ADD(ai,ai);				a2j = ADD(aj,aj);
	c0 = MUL(b0,ai);       			k0 = MUL(d4,a2j);
	c1 = MUL(b1,a2i);      			/*k1 = MUL(b0,aj);*/
	c2 = MUL(b2,a2i);      			k2 = MUL(b1,aj);
	c3 = MUL(b3,a2i);      			k3 = MUL(b2,a2j);
	c4 = MUL(b4,a2i);      			k4 = MUL(b3,a2j);

	ai = SHUF32(c[2], 0x44);    	aj = SHUF32(c[3], 0x44);
	a2i = ADD(ai,ai);				a2j = ADD(aj,aj);
	c0 = ADD(c0,MUL(d3,a2i));		/*k0 = ADD(k0, MUL(d2,aj));*/
	c1 = ADD(c1,MUL(d4,a2i));		k1 = MUL(d3,aj);
	/*c2 = ADD(c2,MUL(b0,ai));*/	k2 = ADD(k2,MUL(d4,a2j));
	/*c3 = ADD(c3,MUL(b1,ai));*/	/*k3 = ADD(k3,MUL(b0,aj));*/
	c4 = ADD(c4,MUL(b2,ai));		/*k4 = ADD(k4,MUL(b1,aj));*/

	ai = SHUF32(c[4],0x44);         aj = SHUF32(c[0],0xEE);
	a2i = ADD(ai,ai);				a2j = ADD(aj,aj);
	/*c0 = ADD(c0,MUL(d1,ai));*/    k0 = ADD(k0,MUL(d0,aj));
	/*c1 = ADD(c1,MUL(d2,ai));*/    k1 = ADD(k1,MUL(d1,a2j));
	/*c2 = ADD(c2,MUL(d3,ai));*/    k2 = ADD(k2,MUL(d2,a2j));
	c3 = ADD(c3,MUL(d4,ai));        k3 = ADD(k3,MUL(d3,a2j));
	/*c4 = ADD(c4,MUL(b0,ai));*/    k4 = ADD(k4,MUL(d4,a2j));

	ai = SHUF32(c[1],0xEE);         aj = SHUF32(c[2],0xEE);
	a2i = ADD(ai,ai);				a2j = ADD(aj,aj);
	c0 = ADD(c0,MUL(e4,a2i));       k0 = ADD(k0,MUL(e3,a2j));
	/*c1 = ADD(c1,MUL(d0,ai));*/    k1 = ADD(k1,MUL(e4,a2j));
	c2 = ADD(c2,MUL(d1,ai));        /*k2 = ADD(k2,MUL(d0,aj));*/
	c3 = ADD(c3,MUL(d2,a2i));       /*k3 = ADD(k3,MUL(d1,aj));*/
	c4 = ADD(c4,MUL(d3,a2i));       k4 = ADD(k4,MUL(d2,aj));

	ai = SHUF32(c[3],0xEE);         aj = SHUF32(c[4],0xEE);
	a2i = ADD(ai,ai);
	/*c0 = ADD(c0,MUL(e2,ai));*/    /*k0 = ADD(k0,MUL(e1,aj));*/
	c1 = ADD(c1,MUL(e3,ai));        /*k1 = ADD(k1,MUL(e2,aj));*/
	c2 = ADD(c2,MUL(e4,a2i));       /*k2 = ADD(k2,MUL(e3,aj));*/
	/*c3 = ADD(c3,MUL(d0,ai));*/    k3 = ADD(k3,MUL(e4,aj));
	/*c4 = ADD(c4,MUL(d1,ai));*/    /*k4 = ADD(k4,MUL(d0,aj));*/

	t0 = SHLV(k0,*sh_0);
	t1 = SHLV(k1,*sh_1);
	t2 = SHLV(k2,*sh_0);
	t3 = SHLV(k3,*sh_1);
	t4 = SHLV(k4,*sh_0);

	c[0] = ADD(c0,t0);
	c[1] = ADD(c1,t1);
	c[2] = ADD(c2,t2);
	c[3] = ADD(c3,t3);
	c[4] = ADD(c4,t4);
}

#define ones26 (((uint64_t) 1 << BASE0_FP25519) - 1 )
#define ones25 (((uint64_t) 1 << BASE1_FP25519) - 1 )
static const ALIGN uint64_t GLOB_mask0[4] = {ones26, ones25,ones26, ones25};
static const ALIGN uint64_t GLOB_mask1[4] = {ones25, ones26,ones25, ones26};
#undef ones26
#undef ones25
static const ALIGN uint64_t GLOB_shift0[4] = {BASE0_FP25519, BASE1_FP25519, BASE0_FP25519, BASE1_FP25519};
static const ALIGN uint64_t GLOB_shift1[4] = {BASE1_FP25519, BASE0_FP25519, BASE1_FP25519, BASE0_FP25519};
static const ALIGN uint64_t GLOB_shift_4[4] = {64,4,64,4};
static const ALIGN uint64_t GLOB_shift_1[4] = {64,1,64,1};

#define mul19(A) \
  A = SHUF32(ADD(ADD( \
	SHLV(A, *((argElement_2w)GLOB_shift_4)),  \
	SHLV(A, *((argElement_2w)GLOB_shift_1))), \
	A),0x4E)

DECL(void, compress)(argElement_2w c) {
  argElement_2w const mask0 = (argElement_2w)GLOB_mask0;
  argElement_2w const mask1 = (argElement_2w)GLOB_mask1;
  argElement_2w const shift0 = (argElement_2w)GLOB_shift0;
  argElement_2w const shift1 = (argElement_2w)GLOB_shift1;

  __m256i c0 = c[0];
  __m256i c1 = c[1];
  __m256i c2 = c[2];
  __m256i c3 = c[3];
  __m256i c4 = c[4];

  __m256i h0_5, h1_6, h2_7, h3_8, h4_9;

  h0_5 = SHRV(c0, *shift0);
  c0 = AND(c0, *mask0);
  c1 = ADD(c1, h0_5);

  h1_6 = SHRV(c1, *shift1);
  c1 = AND(c1, *mask1);
  c2 = ADD(c2, h1_6);

  h2_7 = SHRV(c2, *shift0);
  c2 = AND(c2, *mask0);
  c3 = ADD(c3, h2_7);

  h3_8 = SHRV(c3, *shift1);
  c3 = AND(c3, *mask1);
  c4 = ADD(c4, h3_8);

  h4_9 = SHRV(c4,* shift0);
  c4 = AND(c4, *mask0);

  mul19(h4_9);

  c0 = ADD(c0, h4_9);

  h0_5 = SHRV(c0,* shift0);
  c0 = AND(c0, *mask0);
  c1 = ADD(c1, h0_5);

  c[0] = c0;
  c[1] = c1;
  c[2] = c2;
  c[3] = c3;
  c[4] = c4;
}

DECL(void, compress2)(argElement_2w c, argElement_2w d) {
  argElement_2w const mask0 = (argElement_2w)GLOB_mask0;
  argElement_2w const mask1 = (argElement_2w)GLOB_mask1;
  argElement_2w const shift0 = (argElement_2w)GLOB_shift0;
  argElement_2w const shift1 = (argElement_2w)GLOB_shift1;

  __m256i c0 = c[0];
  __m256i d0 = d[0];
  __m256i c1 = c[1];
  __m256i d1 = d[1];
  __m256i c2 = c[2];
  __m256i d2 = d[2];
  __m256i c3 = c[3];
  __m256i d3 = d[3];
  __m256i c4 = c[4];
  __m256i d4 = d[4];

  __m256i h0_5, h1_6, h2_7, h3_8, h4_9;
  __m256i H0_5, H1_6, H2_7, H3_8, H4_9;

  h0_5 = SHRV(c0,* shift0);
  H0_5 = SHRV(d0,* shift0);
  c0 = AND(c0, *mask0);
  d0 = AND(d0, *mask0);
  c1 = ADD(c1, h0_5);
  d1 = ADD(d1, H0_5);

  h1_6 = SHRV(c1, *shift1);
  H1_6 = SHRV(d1, *shift1);
  c1 = AND(c1, *mask1);
  d1 = AND(d1, *mask1);
  c2 = ADD(c2, h1_6);
  d2 = ADD(d2, H1_6);

  h2_7 = SHRV(c2, *shift0);
  H2_7 = SHRV(d2, *shift0);
  c2 = AND(c2, *mask0);
  d2 = AND(d2, *mask0);
  c3 = ADD(c3, h2_7);
  d3 = ADD(d3, H2_7);

  h3_8 = SHRV(c3, *shift1);
  H3_8 = SHRV(d3, *shift1);
  c3 = AND(c3, *mask1);
  d3 = AND(d3, *mask1);
  c4 = ADD(c4, h3_8);
  d4 = ADD(d4, H3_8);

  h4_9 = SHRV(c4, *shift0);
  H4_9 = SHRV(d4, *shift0);
  c4 = AND(c4, *mask0);
  d4 = AND(d4, *mask0);

  mul19(h4_9);
  mul19(H4_9);

  c0 = ADD(c0, h4_9);
  d0 = ADD(d0, H4_9);

  h0_5 = SHRV(c0, *shift0);
  H0_5 = SHRV(d0, *shift0);
  c0 = AND(c0, *mask0);
  d0 = AND(d0, *mask0);
  c1 = ADD(c1, h0_5);
  d1 = ADD(d1, H0_5);

  c[0] = c0;
  d[0] = d0;
  c[1] = c1;
  d[1] = d1;
  c[2] = c2;
  d[2] = d2;
  c[3] = c3;
  d[3] = d3;
  c[4] = c4;
  d[4] = d4;
}

DECL(void, compressfast)(argElement_2w c) {
  argElement_2w const mask0 = (argElement_2w)GLOB_mask0;
  argElement_2w const mask1 = (argElement_2w)GLOB_mask1;
  argElement_2w const shift0 = (argElement_2w)GLOB_shift0;
  argElement_2w const shift1 = (argElement_2w)GLOB_shift1;

  __m256i L[5], M[5];

  L[0] = AND(c[0], *mask0);
  L[1] = AND(c[1], *mask1);
  L[2] = AND(c[2], *mask0);
  L[3] = AND(c[3], *mask1);
  L[4] = AND(c[4], *mask0);

  M[0] = SHRV(c[0], *shift0);
  M[1] = SHRV(c[1], *shift1);
  M[2] = SHRV(c[2], *shift0);
  M[3] = SHRV(c[3], *shift1);
  M[4] = SHRV(c[4], *shift0);

  mul19(M[4]);

  c[0] = ADD(L[0], M[4]);
  c[1] = ADD(L[1], M[0]);
  c[2] = ADD(L[2], M[1]);
  c[3] = ADD(L[3], M[2]);
  c[4] = ADD(L[4], M[3]);
}

#undef mul19

DECL(void, neg)(argElement_2w c) {
	argElement_2w _2P = (argElement_2w) CONST_2P_2P_H0H5;
    int i = 0;
    for (i = 0; i < (NUM_DIGITS_FP25519 / 2); i++) {
  	  c[i] = SUB(_2P[i], c[i]);
    }
	FN(compress)(c);
}

DECL(void, mul)(argElement_2w c, argElement_2w a, argElement_2w b) {
  FN(intmul)(c,a,b);
  FN(compress)(c);
}

DECL(void, sqr)(argElement_2w c) {
  FN(intsqr)(c);
  FN(compress)(c);
}

DECL(void, sqrn)(argElement_2w c, const unsigned int times) {
	unsigned int it;
	for(it=0;it<times;it++){
  		FN(intsqr)(c);
  		FN(compress)(c);
	}
}

DECL(void, rand)(argElement_2w a) {
  EltFp25519_1w_redradix a0, a1;
  rand_Fp255_1w_redradix(a0);
  rand_Fp255_1w_redradix(a1);
  FN(inter)(a, a0, a1);
}

DECL(void, print)(FILE*file,argElement_2w a) {
  EltFp25519_1w_redradix a0, a1;
  FN(deinter)(a0, a1, a);
  print_Fp255_1w_redradix(file,a0);
  print_Fp255_1w_redradix(file,a1);
}

DECL(__m256i, sgn)(argElement_2w a) {
  int s0, s1;
  EltFp25519_1w_redradix a0, a1;
  FN(deinter)(a0, a1, a);
  s0 = sgn_Fp255_1w_redradix(a0);
  s1 = sgn_Fp255_1w_redradix(a1);
  return SET64(0, s1, 0, s0);
}

DECL(__m256i, cmp)(argElement_2w a, argElement_2w b) {
  int c0, c1;
  EltFp25519_1w_redradix a0, a1, b0, b1;
  FN(deinter)(a0, a1, a);
  FN(deinter)(b0, b1, b);
  c0 = cmp_Fp255_1w_redradix(a0, b0);
  c1 = cmp_Fp255_1w_redradix(a1, b1);
  return SET64(0, c1, 0, c0);
}

DECL(argElement_2w, alloc)() {
  return (argElement_2w) allocate_bytes((NUM_DIGITS_FP25519 / 2) * sizeof(__m256i));
}

DECL(void, unser)(argElement_2w a, uint8_t * buf) {
  EltFp25519_1w_redradix a0, a1;
  unser_Fp255_1w_redradix(a0, buf);
  unser_Fp255_1w_redradix(a1, buf + SIZE_FP25519);
  FN(inter)(a, a0, a1);
}

DECL(void, ser)(uint8_t *buf, argElement_2w a) {
  EltFp25519_1w_redradix a0, a1;
  FN(deinter)(a0, a1, a);
  ser_Fp255_1w_redradix(buf,a0);
  ser_Fp255_1w_redradix(buf+SIZE_FP25519,a1);
}

DECL(void, naddsub)(argElement_2w C, argElement_2w D,argElement_2w A, argElement_2w B) {
  argElement_2w _2P = (argElement_2w)CONST_2P_2P_H0H5;

  int i=0;
  for(i=0;i<(NUM_DIGITS_FP25519/2);i++)
  {
    __m256i T = SUB(_2P[i],A[i]);
    C[i] = SUB(T,B[i]);
    D[i] = ADD(T,B[i]);
  }
}

#undef FN
#undef DECL

#else
extern int version;
#endif /* FP255 */
