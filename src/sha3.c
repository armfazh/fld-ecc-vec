/**
 * Copyright 2017 Armando Faz Hernández
 * This file is part of faz_crypto_avx2.
 *
 * faz_crypto_avx2 is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * faz_crypto_avx2 is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with faz_crypto_avx2.  If not, see <http://www.gnu.org/licenses/>.
 */
/**
 * Code taken from === Keccak Code Package.
 * The Keccak sponge function, designed by Guido Bertoni, Joan Daemen,
 * Michaël Peeters and Gilles Van Assche. For more information, feedback or
 * questions, please refer to our website: http://keccak.noekeon.org/
 */
#include "sha3.h"
#include <string.h>

#define ROL64(a, offset) (((a) << offset) ^ (a >> (64-offset)))

static const uint64_t KeccakF21600RoundConstants[24] =
		{
				(uint64_t) 0x0000000000000001ULL,
				(uint64_t) 0x0000000000008082ULL,
				(uint64_t) 0x800000000000808aULL,
				(uint64_t) 0x8000000080008000ULL,
				(uint64_t) 0x000000000000808bULL,
				(uint64_t) 0x0000000080000001ULL,
				(uint64_t) 0x8000000080008081ULL,
				(uint64_t) 0x8000000000008009ULL,
				(uint64_t) 0x000000000000008aULL,
				(uint64_t) 0x0000000000000088ULL,
				(uint64_t) 0x0000000080008009ULL,
				(uint64_t) 0x000000008000000aULL,
				(uint64_t) 0x000000008000808bULL,
				(uint64_t) 0x800000000000008bULL,
				(uint64_t) 0x8000000000008089ULL,
				(uint64_t) 0x8000000000008003ULL,
				(uint64_t) 0x8000000000008002ULL,
				(uint64_t) 0x8000000000000080ULL,
				(uint64_t) 0x000000000000800aULL,
				(uint64_t) 0x800000008000000aULL,
				(uint64_t) 0x8000000080008081ULL,
				(uint64_t) 0x8000000000008080ULL,
				(uint64_t) 0x0000000080000001ULL,
				(uint64_t) 0x8000000080008008ULL
		};

static void KeccakF2(uint64_t *state) {

	uint64_t Ba, Be, Bi, Bo, Bu;
	uint64_t Ca, Ce, Ci, Co, Cu;
	uint64_t Da, De, Di, Do, Du;
	int i;
#define Aba state[ 0]
#define Abe state[ 1]
#define Abi state[ 2]
#define Abo state[ 3]
#define Abu state[ 4]
#define Aga state[ 5]
#define Age state[ 6]
#define Agi state[ 7]
#define Ago state[ 8]
#define Agu state[ 9]
#define Aka state[10]
#define Ake state[11]
#define Aki state[12]
#define Ako state[13]
#define Aku state[14]
#define Ama state[15]
#define Ame state[16]
#define Ami state[17]
#define Amo state[18]
#define Amu state[19]
#define Asa state[20]
#define Ase state[21]
#define Asi state[22]
#define Aso state[23]
#define Asu state[24]

	for (i = 0; i < 24; i += 4) {
		/* --- Code for 4 rounds */
		/* --- 64-bit lanes mapped to 64-bit words */
		Ca = Aba ^ Aga ^ Aka ^ Ama ^ Asa;
		Ce = Abe ^ Age ^ Ake ^ Ame ^ Ase;
		Ci = Abi ^ Agi ^ Aki ^ Ami ^ Asi;
		Co = Abo ^ Ago ^ Ako ^ Amo ^ Aso;
		Cu = Abu ^ Agu ^ Aku ^ Amu ^ Asu;
		Da = Cu ^ ROL64(Ce, 1);
		De = Ca ^ ROL64(Ci, 1);
		Di = Ce ^ ROL64(Co, 1);
		Do = Ci ^ ROL64(Cu, 1);
		Du = Co ^ ROL64(Ca, 1);

		Ba = (Aba ^ Da);
		Be = ROL64((Age ^ De), 44);
		Bi = ROL64((Aki ^ Di), 43);
		Bo = ROL64((Amo ^ Do), 21);
		Bu = ROL64((Asu ^ Du), 14);
		Aba = Ba ^ ((~Be) & Bi);
		Aba ^= KeccakF21600RoundConstants[i + 0];
		Age = Be ^ ((~Bi) & Bo);
		Aki = Bi ^ ((~Bo) & Bu);
		Amo = Bo ^ ((~Bu) & Ba);
		Asu = Bu ^ ((~Ba) & Be);

		Bi = ROL64((Aka ^ Da), 3);
		Bo = ROL64((Ame ^ De), 45);
		Bu = ROL64((Asi ^ Di), 61);
		Ba = ROL64((Abo ^ Do), 28);
		Be = ROL64((Agu ^ Du), 20);
		Aka = Ba ^ ((~Be) & Bi);
		Ame = Be ^ ((~Bi) & Bo);
		Asi = Bi ^ ((~Bo) & Bu);
		Abo = Bo ^ ((~Bu) & Ba);
		Agu = Bu ^ ((~Ba) & Be);

		Bu = ROL64((Asa ^ Da), 18);
		Ba = ROL64((Abe ^ De), 1);
		Be = ROL64((Agi ^ Di), 6);
		Bi = ROL64((Ako ^ Do), 25);
		Bo = ROL64((Amu ^ Du), 8);
		Asa = Ba ^ ((~Be) & Bi);
		Abe = Be ^ ((~Bi) & Bo);
		Agi = Bi ^ ((~Bo) & Bu);
		Ako = Bo ^ ((~Bu) & Ba);
		Amu = Bu ^ ((~Ba) & Be);

		Be = ROL64((Aga ^ Da), 36);
		Bi = ROL64((Ake ^ De), 10);
		Bo = ROL64((Ami ^ Di), 15);
		Bu = ROL64((Aso ^ Do), 56);
		Ba = ROL64((Abu ^ Du), 27);
		Aga = Ba ^ ((~Be) & Bi);
		Ake = Be ^ ((~Bi) & Bo);
		Ami = Bi ^ ((~Bo) & Bu);
		Aso = Bo ^ ((~Bu) & Ba);
		Abu = Bu ^ ((~Ba) & Be);

		Bo = ROL64((Ama ^ Da), 41);
		Bu = ROL64((Ase ^ De), 2);
		Ba = ROL64((Abi ^ Di), 62);
		Be = ROL64((Ago ^ Do), 55);
		Bi = ROL64((Aku ^ Du), 39);
		Ama = Ba ^ ((~Be) & Bi);
		Ase = Be ^ ((~Bi) & Bo);
		Abi = Bi ^ ((~Bo) & Bu);
		Ago = Bo ^ ((~Bu) & Ba);
		Aku = Bu ^ ((~Ba) & Be);

		Ca = Aba ^ Aka ^ Asa ^ Aga ^ Ama;
		Ce = Age ^ Ame ^ Abe ^ Ake ^ Ase;
		Ci = Aki ^ Asi ^ Agi ^ Ami ^ Abi;
		Co = Amo ^ Abo ^ Ako ^ Aso ^ Ago;
		Cu = Asu ^ Agu ^ Amu ^ Abu ^ Aku;
		Da = Cu ^ ROL64(Ce, 1);
		De = Ca ^ ROL64(Ci, 1);
		Di = Ce ^ ROL64(Co, 1);
		Do = Ci ^ ROL64(Cu, 1);
		Du = Co ^ ROL64(Ca, 1);

		Ba = (Aba ^ Da);
		Be = ROL64((Ame ^ De), 44);
		Bi = ROL64((Agi ^ Di), 43);
		Bo = ROL64((Aso ^ Do), 21);
		Bu = ROL64((Aku ^ Du), 14);
		Aba = Ba ^ ((~Be) & Bi);
		Aba ^= KeccakF21600RoundConstants[i + 1];
		Ame = Be ^ ((~Bi) & Bo);
		Agi = Bi ^ ((~Bo) & Bu);
		Aso = Bo ^ ((~Bu) & Ba);
		Aku = Bu ^ ((~Ba) & Be);

		Bi = ROL64((Asa ^ Da), 3);
		Bo = ROL64((Ake ^ De), 45);
		Bu = ROL64((Abi ^ Di), 61);
		Ba = ROL64((Amo ^ Do), 28);
		Be = ROL64((Agu ^ Du), 20);
		Asa = Ba ^ ((~Be) & Bi);
		Ake = Be ^ ((~Bi) & Bo);
		Abi = Bi ^ ((~Bo) & Bu);
		Amo = Bo ^ ((~Bu) & Ba);
		Agu = Bu ^ ((~Ba) & Be);

		Bu = ROL64((Ama ^ Da), 18);
		Ba = ROL64((Age ^ De), 1);
		Be = ROL64((Asi ^ Di), 6);
		Bi = ROL64((Ako ^ Do), 25);
		Bo = ROL64((Abu ^ Du), 8);
		Ama = Ba ^ ((~Be) & Bi);
		Age = Be ^ ((~Bi) & Bo);
		Asi = Bi ^ ((~Bo) & Bu);
		Ako = Bo ^ ((~Bu) & Ba);
		Abu = Bu ^ ((~Ba) & Be);

		Be = ROL64((Aka ^ Da), 36);
		Bi = ROL64((Abe ^ De), 10);
		Bo = ROL64((Ami ^ Di), 15);
		Bu = ROL64((Ago ^ Do), 56);
		Ba = ROL64((Asu ^ Du), 27);
		Aka = Ba ^ ((~Be) & Bi);
		Abe = Be ^ ((~Bi) & Bo);
		Ami = Bi ^ ((~Bo) & Bu);
		Ago = Bo ^ ((~Bu) & Ba);
		Asu = Bu ^ ((~Ba) & Be);

		Bo = ROL64((Aga ^ Da), 41);
		Bu = ROL64((Ase ^ De), 2);
		Ba = ROL64((Aki ^ Di), 62);
		Be = ROL64((Abo ^ Do), 55);
		Bi = ROL64((Amu ^ Du), 39);
		Aga = Ba ^ ((~Be) & Bi);
		Ase = Be ^ ((~Bi) & Bo);
		Aki = Bi ^ ((~Bo) & Bu);
		Abo = Bo ^ ((~Bu) & Ba);
		Amu = Bu ^ ((~Ba) & Be);

		Ca = Aba ^ Asa ^ Ama ^ Aka ^ Aga;
		Ce = Ame ^ Ake ^ Age ^ Abe ^ Ase;
		Ci = Agi ^ Abi ^ Asi ^ Ami ^ Aki;
		Co = Aso ^ Amo ^ Ako ^ Ago ^ Abo;
		Cu = Aku ^ Agu ^ Abu ^ Asu ^ Amu;
		Da = Cu ^ ROL64(Ce, 1);
		De = Ca ^ ROL64(Ci, 1);
		Di = Ce ^ ROL64(Co, 1);
		Do = Ci ^ ROL64(Cu, 1);
		Du = Co ^ ROL64(Ca, 1);

		Ba = (Aba ^ Da);
		Be = ROL64((Ake ^ De), 44);
		Bi = ROL64((Asi ^ Di), 43);
		Bo = ROL64((Ago ^ Do), 21);
		Bu = ROL64((Amu ^ Du), 14);
		Aba = Ba ^ ((~Be) & Bi);
		Aba ^= KeccakF21600RoundConstants[i + 2];
		Ake = Be ^ ((~Bi) & Bo);
		Asi = Bi ^ ((~Bo) & Bu);
		Ago = Bo ^ ((~Bu) & Ba);
		Amu = Bu ^ ((~Ba) & Be);

		Bi = ROL64((Ama ^ Da), 3);
		Bo = ROL64((Abe ^ De), 45);
		Bu = ROL64((Aki ^ Di), 61);
		Ba = ROL64((Aso ^ Do), 28);
		Be = ROL64((Agu ^ Du), 20);
		Ama = Ba ^ ((~Be) & Bi);
		Abe = Be ^ ((~Bi) & Bo);
		Aki = Bi ^ ((~Bo) & Bu);
		Aso = Bo ^ ((~Bu) & Ba);
		Agu = Bu ^ ((~Ba) & Be);

		Bu = ROL64((Aga ^ Da), 18);
		Ba = ROL64((Ame ^ De), 1);
		Be = ROL64((Abi ^ Di), 6);
		Bi = ROL64((Ako ^ Do), 25);
		Bo = ROL64((Asu ^ Du), 8);
		Aga = Ba ^ ((~Be) & Bi);
		Ame = Be ^ ((~Bi) & Bo);
		Abi = Bi ^ ((~Bo) & Bu);
		Ako = Bo ^ ((~Bu) & Ba);
		Asu = Bu ^ ((~Ba) & Be);

		Be = ROL64((Asa ^ Da), 36);
		Bi = ROL64((Age ^ De), 10);
		Bo = ROL64((Ami ^ Di), 15);
		Bu = ROL64((Abo ^ Do), 56);
		Ba = ROL64((Aku ^ Du), 27);
		Asa = Ba ^ ((~Be) & Bi);
		Age = Be ^ ((~Bi) & Bo);
		Ami = Bi ^ ((~Bo) & Bu);
		Abo = Bo ^ ((~Bu) & Ba);
		Aku = Bu ^ ((~Ba) & Be);

		Bo = ROL64((Aka ^ Da), 41);
		Bu = ROL64((Ase ^ De), 2);
		Ba = ROL64((Agi ^ Di), 62);
		Be = ROL64((Amo ^ Do), 55);
		Bi = ROL64((Abu ^ Du), 39);
		Aka = Ba ^ ((~Be) & Bi);
		Ase = Be ^ ((~Bi) & Bo);
		Agi = Bi ^ ((~Bo) & Bu);
		Amo = Bo ^ ((~Bu) & Ba);
		Abu = Bu ^ ((~Ba) & Be);

		Ca = Aba ^ Ama ^ Aga ^ Asa ^ Aka;
		Ce = Ake ^ Abe ^ Ame ^ Age ^ Ase;
		Ci = Asi ^ Aki ^ Abi ^ Ami ^ Agi;
		Co = Ago ^ Aso ^ Ako ^ Abo ^ Amo;
		Cu = Amu ^ Agu ^ Asu ^ Aku ^ Abu;
		Da = Cu ^ ROL64(Ce, 1);
		De = Ca ^ ROL64(Ci, 1);
		Di = Ce ^ ROL64(Co, 1);
		Do = Ci ^ ROL64(Cu, 1);
		Du = Co ^ ROL64(Ca, 1);

		Ba = (Aba ^ Da);
		Be = ROL64((Abe ^ De), 44);
		Bi = ROL64((Abi ^ Di), 43);
		Bo = ROL64((Abo ^ Do), 21);
		Bu = ROL64((Abu ^ Du), 14);
		Aba = Ba ^ ((~Be) & Bi);
		Aba ^= KeccakF21600RoundConstants[i + 3];
		Abe = Be ^ ((~Bi) & Bo);
		Abi = Bi ^ ((~Bo) & Bu);
		Abo = Bo ^ ((~Bu) & Ba);
		Abu = Bu ^ ((~Ba) & Be);

		Bi = ROL64((Aga ^ Da), 3);
		Bo = ROL64((Age ^ De), 45);
		Bu = ROL64((Agi ^ Di), 61);
		Ba = ROL64((Ago ^ Do), 28);
		Be = ROL64((Agu ^ Du), 20);
		Aga = Ba ^ ((~Be) & Bi);
		Age = Be ^ ((~Bi) & Bo);
		Agi = Bi ^ ((~Bo) & Bu);
		Ago = Bo ^ ((~Bu) & Ba);
		Agu = Bu ^ ((~Ba) & Be);

		Bu = ROL64((Aka ^ Da), 18);
		Ba = ROL64((Ake ^ De), 1);
		Be = ROL64((Aki ^ Di), 6);
		Bi = ROL64((Ako ^ Do), 25);
		Bo = ROL64((Aku ^ Du), 8);
		Aka = Ba ^ ((~Be) & Bi);
		Ake = Be ^ ((~Bi) & Bo);
		Aki = Bi ^ ((~Bo) & Bu);
		Ako = Bo ^ ((~Bu) & Ba);
		Aku = Bu ^ ((~Ba) & Be);

		Be = ROL64((Ama ^ Da), 36);
		Bi = ROL64((Ame ^ De), 10);
		Bo = ROL64((Ami ^ Di), 15);
		Bu = ROL64((Amo ^ Do), 56);
		Ba = ROL64((Amu ^ Du), 27);
		Ama = Ba ^ ((~Be) & Bi);
		Ame = Be ^ ((~Bi) & Bo);
		Ami = Bi ^ ((~Bo) & Bu);
		Amo = Bo ^ ((~Bu) & Ba);
		Amu = Bu ^ ((~Ba) & Be);

		Bo = ROL64((Asa ^ Da), 41);
		Bu = ROL64((Ase ^ De), 2);
		Ba = ROL64((Asi ^ Di), 62);
		Be = ROL64((Aso ^ Do), 55);
		Bi = ROL64((Asu ^ Du), 39);
		Asa = Ba ^ ((~Be) & Bi);
		Ase = Be ^ ((~Bi) & Bo);
		Asi = Bi ^ ((~Bo) & Bu);
		Aso = Bo ^ ((~Bu) & Ba);
		Asu = Bu ^ ((~Ba) & Be);
	}
#undef Aba
#undef Abe
#undef Abi
#undef Abo
#undef Abu
#undef Aga
#undef Age
#undef Agi
#undef Ago
#undef Agu
#undef Aka
#undef Ake
#undef Aki
#undef Ako
#undef Aku
#undef Ama
#undef Ame
#undef Ami
#undef Amo
#undef Amu
#undef Asa
#undef Ase
#undef Asi
#undef Aso
#undef Asu
}


/*
 * state: a structure that holds ....
 */
HashReturn Init(hashState *state, int hashbitlen) {
	int i = 0;

	for (i = 0; i < 200; i++)
		state->s[i] = 0;

	state->statebitlen = 0;
	state->r = (1600 - hashbitlen * 2);

	state->hashbitlen = hashbitlen;

	return SUCCESS;
}

/*
 * .....
 */
HashReturn Update(hashState *state, const BitSequence *data, DataLength databitlen) {

	const BitSequence *tmp = data;
	DataLength lentmp = databitlen;
	unsigned int i, j, start, end;

	while ((state->statebitlen + lentmp) >= state->r) {
		for (j = 0, i = state->statebitlen / 8; i < state->r / 8; i++, j++) {
			state->s[i] = state->s[i] ^ tmp[j];
		}

		KeccakF2((uint64_t *) state->s);

		tmp += ((state->r - state->statebitlen) / 8);
		lentmp -= (state->r - state->statebitlen);
		state->statebitlen = 0;
	}

	j = 0;
	start = state->statebitlen / 8;
	end = (state->statebitlen + lentmp) / 8;

	for (i = start; i < end; i++) {
		state->s[i] = state->s[i] ^ tmp[j++];
	}

	state->statebitlen = lentmp + state->statebitlen;

	return SUCCESS;

}

/*
 * ......
 */
HashReturn Final(hashState *state, BitSequence *hashval) {
	int sbytelen = (state->statebitlen / 8);

	state->s[sbytelen] ^= 0x06;
	state->s[(state->r / 8) - 1] ^= 0x80;

	KeccakF2((uint64_t *) state->s);

	memcpy(hashval, state->s, state->r / 8);

	return SUCCESS;
}

/*
 * ......
 */
HashReturn Final2(hashState *state, BitSequence *hashval, unsigned int d_byte) {
	int sbytelen = (state->statebitlen / 8);
	int j = 0;
	unsigned int r = (state->r / 8);

	state->s[sbytelen] ^= 0x1F;
	state->s[r - 1] ^= 0x80;

	KeccakF2((uint64_t *) state->s);


	while (d_byte > r) {

		memcpy((hashval + j), state->s, r);

		KeccakF2((uint64_t *) state->s);
		j += r;
		d_byte -= r;
	}

	memcpy((hashval + j), state->s, d_byte);

	return SUCCESS;
}

HashReturn Hash2(int hashbitlen, const BitSequence *data,
				 DataLength databitlen, BitSequence *hashval, int d) {
	hashState state;

	Init(&state, hashbitlen);
	Update(&state, data, databitlen);
	Final2(&state, hashval, d);

	return SUCCESS;
}

/*
 * .....
 */
HashReturn Hash(int hashbitlen, const BitSequence *data,
				DataLength databitlen, BitSequence *hashval) {
	hashState state;

	Init(&state, hashbitlen);
	Update(&state, data, databitlen);
	Final(&state, hashval);

	return SUCCESS;
}


HashReturn SHAKE128(const BitSequence *data, DataLength databitlen, BitSequence *hashval, unsigned int d_byte) {

	unsigned int j;
	unsigned int r = 168, nr;
	DataLength inlen = databitlen / 8;
	const BitSequence *tmp = data;
	uint64_t s[25] __attribute__ ((aligned (32)));

	unsigned char temp[170];

	nr = r / 8;

	memset(s, 0, sizeof(s));

	for ( /* empty */; inlen >= r; inlen -= r, tmp += r) {
		for (j = 0; j < nr; j++) {
			s[j] = s[j] ^ ((uint64_t *) tmp)[j];
		}
		KeccakF2(s);
	}

	//    padding
	memcpy(temp, tmp, (size_t) inlen);
	temp[inlen++] = 0x1F;
	memset(temp + inlen, 0, r - (size_t) inlen);
	temp[r - 1] |= 0x80;

	for (j = 0; j < nr; j++) {
		s[j] = s[j] ^ ((uint64_t *) temp)[j];
	}
	KeccakF2(s);

	j = 0;
	while (d_byte > r) {

		memcpy((hashval + j), s, r);

		KeccakF2((uint64_t *) s);
		j += r;
		d_byte -= r;
	}

	memcpy((hashval + j), s, d_byte);


	return SUCCESS;
}

HashReturn SHAKE256(const BitSequence *data, DataLength databitlen,
					BitSequence *hashval, unsigned int d_byte) {

	unsigned int j;
	unsigned int r = 136, nr;
	DataLength inlen = databitlen / 8;
	const BitSequence *tmp = data;
	uint64_t s[25] __attribute__ ((aligned (32)));

	unsigned char temp[140];

	nr = r / 8;

	memset(s, 0, sizeof(uint64_t)*25);

	for ( /* empty */; inlen >= r; inlen -= r, tmp += r) {
		for (j = 0; j < nr; j++) {
			s[j] = s[j] ^ ((uint64_t *) tmp)[j];
		}
		KeccakF2(s);
	}

	//    padding
	memcpy(temp, tmp, (size_t) inlen);
	temp[inlen++] = 0x1F;
	memset(temp + inlen, 0, r - (size_t) inlen);
	temp[r - 1] |= 0x80;

	for (j = 0; j < nr; j++) {
		s[j] = s[j] ^ ((uint64_t *) temp)[j];
	}
	KeccakF2(s);

	j = 0;
	while (d_byte > r) {

		memcpy((hashval + j), s, r);

		KeccakF2((uint64_t *) s);
		j += r;
		d_byte -= r;
	}

	memcpy((hashval + j), s, d_byte);


	return SUCCESS;
}

