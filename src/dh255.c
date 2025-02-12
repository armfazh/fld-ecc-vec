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

#if defined(DH255) || 1

#include "faz_ecdh_avx2.h"

#ifdef __cplusplus
namespace faz
{
namespace dh255
{
extern "C" {
#endif /* __cplusplus */

#include "ladder255.c"

const X_ECDH_x2 X25519_AVX512 = {
    .keygen = x25519_keygen_avx512,
    .shared = x25519_shared_avx512,
    .key_size = ECDH25519_KEY_SIZE_BYTES,
    .allocKey = allocX25519_Key,
    .freeKey  = deallocate_bytes,
    .randKey  = randX25519_Key,
    .printKey  = printX25519_Key,
};

const X_ECDH X25519_AVX2 = {
    .keygen = x25519_keygen_avx2,
    .shared = x25519_shared_avx2,
    .key_size = ECDH25519_KEY_SIZE_BYTES,
    .allocKey = allocX25519_Key,
    .freeKey  = deallocate_bytes,
    .randKey  = randX25519_Key,
    .printKey  = printX25519_Key,
};

const X_ECDH X25519_x64 = {
    .keygen = x25519_keygen_avx2,
    .shared = x25519_shared_x64,
    .key_size = ECDH25519_KEY_SIZE_BYTES,
    .freeKey  = deallocate_bytes,
    .allocKey = allocX25519_Key,
    .randKey  = randX25519_Key,
    .printKey  = printX25519_Key,
};
/*!< ECDH global object.
 * This function provides a method for accesing the Elliptic
 * Curve Diffie-Hellman functions X25519.
 * */

#ifdef __cplusplus
} /* extern C */
} /* namespace dh255 */
} /* namespace faz */
#endif /* __cplusplus */

#else
extern int version;
#endif /* DH255 */
