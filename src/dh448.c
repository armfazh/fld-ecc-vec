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

#if defined(DH448)

#include "faz_ecdh_avx2.h"

#ifdef __cplusplus
namespace faz{
  namespace dh448{
extern "C" {
#endif /* __cplusplus */

#include "ladder448.c"

const X_ECDH X448_AVX2 = {
    .keygen = x448_keygen_avx2,
    .shared = x448_shared_avx2,
    .key_size = ECDH448_KEY_SIZE_BYTES,
    .allocKey = allocX448_Key,
    .freeKey  = deallocate_bytes,
    .randKey  = randX448_Key,
    .printKey  = printX448_Key,

};
/*!< ECDH global object.
 * This function provides a method for accesing the Elliptic
 * Curve Diffie-Hellman functions X448.
 * */


#ifdef __cplusplus
} /* extern C */
} /* namespace dh448 */
} /* namespace faz */
#endif /* __cplusplus */

#else
extern int version;
#endif /* DH448 */
