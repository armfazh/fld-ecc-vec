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

#if defined(ED448)

#include "faz_eddsa_avx2.h"

#ifdef __cplusplus
namespace faz
{
namespace ed448
{
extern "C" {
#endif /* __cplusplus */

#include "edwards448.c"
#include "sign448.c"

const struct _struct_Ed448 Ed448 = {
    .PureContext = {
        .keygen = ed448_keygen,
        .sign   = ed448_signctx,
        .verify = ed448_verifyctx,
        .key_size = ED448_KEY_SIZE_BYTES_PARAM,
        .signature_size = ED448_SIG_SIZE_BYTES_PARAM,
        .allocKey = allocEd448_Key,
        .allocSignature = allocEd448_Signature,
        .printKey = printEd448_Key,
        .printSignature = printEd448_Signature,
        .randKey = randEd448_Key,
        .freeKey = deallocate_bytes,
        .freeSignature = deallocate_bytes,
    },
    .PreHash = {
        .keygen = ed448_keygen,
        .sign   = ed448ph_signctx,
        .verify = ed448ph_verifyctx,
        .key_size = ED448_KEY_SIZE_BYTES_PARAM,
        .signature_size = ED448_SIG_SIZE_BYTES_PARAM,
        .allocKey = allocEd448_Key,
        .allocSignature = allocEd448_Signature,
        .printKey = printEd448_Key,
        .printSignature = printEd448_Signature,
        .randKey = randEd448_Key,
        .freeKey = deallocate_bytes,
        .freeSignature = deallocate_bytes,
    }
};

#ifdef __cplusplus
} /* extern C */
} /* namespace ed448 */
} /* namespace faz */
#endif /* __cplusplus */

#else
extern int version;
#endif /* ED448 */
