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
#include "faz_ecdh_avx2.h"

#include "x25519.c"
#include "x448.c"

const struct _struct_ECDHX ECDHX = {
	.X25519 = {
		.keygen = x25519_keygen_avx2,
		.shared = x25519_shared_avx2,
		.key_size = ECDH25519_KEY_SIZE_BYTES,
		.initKey = initX25519_Key,
		.clearKey = deallocate_bytes
	},
	.X25519_x64 = {
		.keygen = x25519_keygen_avx2,
		.shared = x25519_shared_x64,
		.key_size = ECDH25519_KEY_SIZE_BYTES,
		.initKey = initX25519_Key,
		.clearKey = deallocate_bytes
	},
	.X448 = {
		.keygen = x448_keygen,
		.shared = x448_shared,
		.key_size = ECDH448_KEY_SIZE_BYTES,
		.initKey = initX448_Key,
		.clearKey = deallocate_bytes
	}
};
/*!< ECDH global object.
 * This function provides a method for accesing the Elliptic
 * Curve Diffie-Hellman functions X22519 and X448.
 * */
