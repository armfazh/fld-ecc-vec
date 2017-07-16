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
#include "faz_eddsa_avx2.h"
#include "ed25519.c"
#include "ed448.c"

const struct _struct_EdDSA EdDSA = {
		.Ed25519 = {
				.keygen = ed25519_keygen,
				.sign = ed25519_sign,
				.verify = ed25519_verify,
				.key_size = ED25519_KEY_SIZE_BYTES_PARAM,
				.signature_size = ED25519_SIG_SIZE_BYTES_PARAM,
				.initKey = initEd25519_Key,
				.initSignature = initEd25519_Signature,
				.clearKey = deallocate_bytes,
				.clearSignature = deallocate_bytes
		},
		.Ed25519ph = {
				.keygen = ed25519_keygen,
				.sign = ed25519ph_signctx,
				.verify = ed25519ph_verifyctx,
				.key_size = ED25519_KEY_SIZE_BYTES_PARAM,
				.signature_size = ED25519_SIG_SIZE_BYTES_PARAM,
				.initKey = initEd25519_Key,
				.initSignature = initEd25519_Signature,
				.clearKey = deallocate_bytes,
				.clearSignature = deallocate_bytes
		},
		.Ed25519ctx = {
				.keygen = ed25519_keygen,
				.sign = ed25519_signctx,
				.verify = ed25519_verifyctx,
				.key_size = ED25519_KEY_SIZE_BYTES_PARAM,
				.signature_size = ED25519_SIG_SIZE_BYTES_PARAM,
				.initKey = initEd25519_Key,
				.initSignature = initEd25519_Signature,
				.clearKey = deallocate_bytes,
				.clearSignature = deallocate_bytes
		},
		.Ed448 = {
				.keygen = ed448_keygen,
				.sign   = ed448_signctx,
				.verify = ed448_verifyctx,
				.key_size = ED448_KEY_SIZE_BYTES_PARAM,
				.signature_size = ED448_SIG_SIZE_BYTES_PARAM,
				.initKey = initEd448_Key,
				.initSignature = initEd448_Signature,
				.clearKey = deallocate_bytes,
				.clearSignature = deallocate_bytes
		},
		.Ed448ph = {
				.keygen = ed448_keygen,
				.sign   = ed448ph_signctx,
				.verify = ed448ph_verifyctx,
				.key_size = ED448_KEY_SIZE_BYTES_PARAM,
				.signature_size = ED448_SIG_SIZE_BYTES_PARAM,
				.initKey = initEd448_Key,
				.initSignature = initEd448_Signature,
				.clearKey = deallocate_bytes,
				.clearSignature = deallocate_bytes
		}
};
