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

#if defined(ED255)

#include "faz_eddsa_avx2.h"

#ifdef __cplusplus
namespace faz
{
namespace ed255
{
extern "C" {
#endif /* __cplusplus */

#include "edwards255.c"
#include "sign255.c"

const struct _struct_Ed255 Ed25519 = {
    .Pure = {
        .keygen = ed25519_keygen,
        .sign = ed25519_sign,
        .verify = ed25519_verify,
        .key_size = ED25519_KEY_SIZE_BYTES_PARAM,
        .signature_size = ED25519_SIG_SIZE_BYTES_PARAM,
        .allocKey = allocEd25519_Key,
        .allocSignature = allocEd25519_Signature,
        .printKey = printEd25519_Key,
        .printSignature = printEd25519_Signature,
        .randKey = randEd25519_Key,
        .freeKey = deallocate_bytes,
        .freeSignature = deallocate_bytes,
    },
    .PreHash = {
        .keygen = ed25519_keygen,
        .sign = ed25519ph_signctx,
        .verify = ed25519ph_verifyctx,
        .key_size = ED25519_KEY_SIZE_BYTES_PARAM,
        .signature_size = ED25519_SIG_SIZE_BYTES_PARAM,
        .allocKey = allocEd25519_Key,
        .allocSignature = allocEd25519_Signature,
        .printKey = printEd25519_Key,
        .printSignature = printEd25519_Signature,
        .randKey = randEd25519_Key,
        .freeKey = deallocate_bytes,
        .freeSignature = deallocate_bytes,
    },
    .Context = {
        .keygen = ed25519_keygen,
        .sign = ed25519_signctx,
        .verify = ed25519_verifyctx,
        .key_size = ED25519_KEY_SIZE_BYTES_PARAM,
        .signature_size = ED25519_SIG_SIZE_BYTES_PARAM,
        .allocKey = allocEd25519_Key,
        .allocSignature = allocEd25519_Signature,
        .printKey = printEd25519_Key,
        .printSignature = printEd25519_Signature,
        .randKey = randEd25519_Key,
        .freeKey = deallocate_bytes,
        .freeSignature = deallocate_bytes,
    },
};

#ifdef __cplusplus
} /* extern C */
} /* namespace ed255 */
} /* namespace faz */
#endif /* __cplusplus */

#else
extern int version;
#endif /* ED255 */
