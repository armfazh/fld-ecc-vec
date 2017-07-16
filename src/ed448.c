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
#include "ecc_ed448.c"
#include "sha3.c"

/**
 *
 * @return
 */
static uint8_t * initEd448_Key()
{
	return (uint8_t*) allocate_bytes(ED448_KEY_SIZE_BYTES_PARAM * sizeof(uint8_t));
}

/**
 *
 * @return
 */
static uint8_t * initEd448_Signature()
{
	return (uint8_t*) allocate_bytes(ED448_SIG_SIZE_BYTES_PARAM * sizeof(uint8_t));
}

/**
 *
 * @param public_key
 * @param private_key
 * @return
 */
static int ed448_keygen(
		argEdDSA_PublicKey public_key,
		const argEdDSA_PrivateKey private_key
)
{
	Ed448_Digest h;
	PointXYZT_2w_H0H8 hB;
	SHAKE256(private_key,ED448_KEY_SIZE_BYTES_PARAM*8,h,ED448_HASH_BYTES_PARAM);

	h[0]  &= -((uint8_t)1<<ED448_C_PARAM);
	h[ED448_KEY_SIZE_BYTES_PARAM-2] |= 0x80;
	h[ED448_KEY_SIZE_BYTES_PARAM-1]  = 0x00;

	fixed_point_multiplication_ed448(&hB,h);
	point_encoding_ed448(public_key,&hB);
	return EDDSA_KEYGEN_OK;
}

/**
 * Calculates:
 *     s = r+k*a  (mod 2^246-\ell)
 *
 * such that size of r, k and a is 56-bytes.
 */
static void calculate_s_ed448(uint8_t *s_mod_l, uint8_t *r, uint8_t *h, uint8_t *a)
{
	int i;
	ALIGN uint64_t product[16];
	ALIGN uint64_t aa[7];
	ALIGN uint64_t hh[7];

	aa[6]=0;
	for(i=0;i<56;i++)
	{
		((uint8_t*)product)[i] = r[i];
		((uint8_t*)aa)[i]      = a[i];
		((uint8_t*)hh)[i]      = h[i];
	}
	for(i=7;i<16;i++)
	{
		product[i] = 0;
	}

	word64_multiplier(product,hh,7,aa,7);
	modular_reduction_ed448((uint8_t*)product);

	for(i=0;i<56;i++)
	{
		s_mod_l[i] = ((uint8_t*)product)[i];
	}
	s_mod_l[56] = 0;
}

/**
 *
 * @param signature
 * @param message
 * @param message_length
 * @param context
 * @param context_length
 * @param public_key
 * @param private_key
 * @param phflag
 * @return
 */
static int ed448_sign_all(
		argEdDSA_Signature signature,
		const uint8_t *message,
		uint64_t message_length,
		const uint8_t * context,
		uint64_t        context_length,
		const argEdDSA_PublicKey public_key,
		const argEdDSA_PrivateKey private_key,
		const uint8_t phflag
)
{
	PointXYZT_2w_H0H8 rB;
	Ed448_Digest H_RAM,r,ah,pre_hash_message;
	const int size_prefix = 10;
	hashState hash;
	unsigned char prefix[10] = "SigEd448";

	if(context_length > 255)
	{
		return EDDSA_ERROR_CONTEXT;
	}

	/* PHFLAG */
	prefix[8] = phflag;
	/* size context */
	prefix[9] = (uint8_t)context_length;

	SHAKE256(private_key,ED448_KEY_SIZE_BYTES_PARAM*8,ah,ED448_HASH_BYTES_PARAM);
	ah[0]  &= -((uint8_t)1<<ED448_C_PARAM);
	ah[ED448_KEY_SIZE_BYTES_PARAM-2] |= 0x80;
	ah[ED448_KEY_SIZE_BYTES_PARAM-1]  = 0x00;

	Init(&hash,256);
	Update(&hash,prefix,size_prefix*8);
	Update(&hash,context,context_length*8);
	Update(&hash,ah+ED448_KEY_SIZE_BYTES_PARAM,ED448_KEY_SIZE_BYTES_PARAM*8);
	if(phflag == 1)
	{
		SHAKE256(message,message_length*8,pre_hash_message,64);
		Update(&hash,pre_hash_message,64*8);
	}
	else if(phflag == 0)
	{
		Update(&hash,message,message_length*8);
	}
	else
	{
		return EDDSA_ERROR_PHFLAG;
	}
	Final2(&hash,r,ED448_HASH_BYTES_PARAM);
	modular_reduction_ed448(r);

	fixed_point_multiplication_ed448(&rB,r);
	point_encoding_ed448(signature,&rB);

	Init(&hash,256);
	Update(&hash,prefix,size_prefix*8);
	Update(&hash,context,context_length*8);
	Update(&hash,signature,ED448_KEY_SIZE_BYTES_PARAM*8);
	Update(&hash,public_key,ED448_KEY_SIZE_BYTES_PARAM*8);
	if(phflag == 1)
	{
		Update(&hash,pre_hash_message,64*8);
	}
	else if(phflag == 0)
	{
		Update(&hash,message,message_length*8);
	}
	else
	{
		return EDDSA_ERROR_PHFLAG;
	}
	Final2(&hash,H_RAM,ED448_HASH_BYTES_PARAM);

	modular_reduction_ed448(H_RAM);

	calculate_s_ed448(signature+ED448_KEY_SIZE_BYTES_PARAM,r,H_RAM,ah);
	return EDDSA_SIGNATURE_OK;
}

/**
 *
 * @param signature
 * @param message
 * @param message_length
 * @param public_key
 * @param private_key
 * @return
 */
static int ed448_signctx(
	argEdDSA_Signature signature,
	const uint8_t *message,
	uint64_t message_length,
	const uint8_t * context,
	uint64_t        context_length,
	const argEdDSA_PublicKey public_key,
	const argEdDSA_PrivateKey private_key
)
{
	return ed448_sign_all(signature,message,message_length,context,context_length,public_key,private_key,0);
}

/**
 *
 * @param signature
 * @param message
 * @param message_length
 * @param context
 * @param context_length
 * @param public_key
 * @param private_key
 * @return
 */
static int ed448ph_signctx(
	argEdDSA_Signature signature,
	const uint8_t *message,
	uint64_t message_length,
	const uint8_t * context,
	uint64_t        context_length,
	const argEdDSA_PublicKey public_key,
	const argEdDSA_PrivateKey private_key
)
{
	return ed448_sign_all(signature,message,message_length,context,context_length,public_key,private_key,1);
}

/**
 *
 * @param message
 * @param message_length
 * @param context
 * @param context_length
 * @param public_key
 * @param signature
 * @param phflag
 * @return
 */
static int ed448_verify_all(
		const uint8_t *message,
		uint64_t message_length,
		const uint8_t * context,
		uint64_t        context_length,
		const argEdDSA_PublicKey public_key,
		const argEdDSA_Signature signature,
		const uint8_t phflag
)
{
	const int size_prefix = 10;
	Ed448_Digest H_RAM,pre_hash_message;
	PointXYZT_2w_H0H8 A;
	Ed448_Signature Q;
	hashState hash;
	unsigned char prefix[10] = "SigEd448";

	if(context_length > 255)
	{
		return EDDSA_ERROR_CONTEXT;
	}

	/* PHFLAG */
	prefix[8] = phflag;
	/* size context */
	prefix[9] = (uint8_t)context_length;

	Init(&hash,256);
	Update(&hash,prefix,size_prefix*8);
	Update(&hash,context,context_length*8);
	Update(&hash,signature,ED448_KEY_SIZE_BYTES_PARAM*8);
	Update(&hash,public_key,ED448_KEY_SIZE_BYTES_PARAM*8);
	if(phflag == 1)
	{
		SHAKE256(message, message_length*8,pre_hash_message,64);
		Update(&hash,pre_hash_message,64*8);
	}
	else if(phflag == 0)
	{
		Update(&hash,message,message_length*8);
	}
	else
	{
		return EDDSA_ERROR_PHFLAG;
	}
	Final2(&hash,H_RAM,ED448_HASH_BYTES_PARAM);
	modular_reduction_ed448(H_RAM);

	if(point_decoding_ed448(&A,(uint8_t *)public_key) != 0)
	{
		return EDDSA_ERROR_PUBLICKEY;
	}

	double_point_multiplication_ed448(Q,signature+ED448_KEY_SIZE_BYTES_PARAM,H_RAM,&A);
	return (memcmp(signature,Q,ED448_KEY_SIZE_BYTES_PARAM) == 0) ? EDDSA_VERIFICATION_OK : EDDSA_INVALID_SIGNATURE;
}

/**
 *
 * @param message
 * @param message_length
 * @param context
 * @param context_length
 * @param public_key
 * @param signature
 * @return
 */
static int ed448_verifyctx(
	const uint8_t *message,
	uint64_t message_length,
	const uint8_t * context,
	uint64_t        context_length,
	const argEdDSA_PublicKey public_key,
	const argEdDSA_Signature signature
)
{
	return ed448_verify_all(message,message_length,context,context_length,public_key,signature,0);
}

/**
 *
 * @param message
 * @param message_length
 * @param context
 * @param context_length
 * @param public_key
 * @param signature
 * @return
 */
static int ed448ph_verifyctx(
	const uint8_t *message,
	uint64_t message_length,
	const uint8_t * context,
	uint64_t        context_length,
	const argEdDSA_PublicKey public_key,
	const argEdDSA_Signature signature
)
{
	return ed448_verify_all(message,message_length,context,context_length,public_key,signature,1);
}

