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

#ifndef FAZ_ECDH_AVX2_H
#define FAZ_ECDH_AVX2_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <stdint.h>
#include <stdio.h>

#ifndef ALIGN_BYTES
#define ALIGN_BYTES 64
#endif /* ALIGN_BYTES */
#ifndef ALIGN
#ifdef __INTEL_COMPILER
#define ALIGN __declspec(align(ALIGN_BYTES))
#else
#define ALIGN __attribute__ ((aligned (ALIGN_BYTES)))
#endif
#endif /* ALIGN */

#ifdef __cplusplus
namespace faz
{
namespace ecdh
{
#endif /* __cplusplus */

/**
* Swap method
* ===========
*
* The flag SWAP_METHOD defines the method used for
* performing constant time conditional move.
*  1) PERMUTATION. Uses AVX2 permutation instructions
*  2) LOGIC. Uses the AVX2 LOGIC instructions.
*  3) CMOV. Uses the 64-bit CMOV instruction.
*/
#define PERMUTATION 0x1
#define LOGIC       0x2
#define CMOV        0x4
#define SWAP_METHOD PERMUTATION

#define Oper0Retr(NAME, TYPE) TYPE (*NAME)(void)
#define Oper1File(NAME, TYPE) void (*NAME)(FILE* file, TYPE A)
#define Oper1Void(NAME, TYPE) void (*NAME)(TYPE C)

typedef uint8_t *argECDHX_Key;

typedef int (*XKeyGen)(
    argECDHX_Key session_key,
    argECDHX_Key private_key
);
typedef int (*XSharedSecret)(
    argECDHX_Key shared_secret,
    argECDHX_Key session_key,
    argECDHX_Key private_key
);

typedef struct _struct_DiffieHellmanXFunction {
    XKeyGen keygen;
    XSharedSecret shared;
    uint64_t key_size;
    Oper0Retr(allocKey, uint8_t*);
    Oper1Void(freeKey, void*);
    Oper1File(printKey, uint8_t*);
    Oper1Void(randKey, uint8_t*);
} X_ECDH;

extern const X_ECDH X25519_AVX2;
extern const X_ECDH X25519_x64;
extern const X_ECDH X448_AVX2;

/** X25519 */
#define ECDH25519_KEY_SIZE_BYTES 32
typedef ALIGN uint8_t X25519_KEY[ECDH25519_KEY_SIZE_BYTES];

/** X448 */
#define ECDH448_KEY_SIZE_BYTES 56
typedef ALIGN uint8_t X448_KEY[ECDH448_KEY_SIZE_BYTES];

#if defined(ENABLED_AVX512)
struct ALIGN X25519_KEY_x2 {
    X25519_KEY k0, k1;
};

typedef struct X25519_KEY_x2* argECDHX_Key_x2;

typedef int (*XKeyGen_x2)(
    argECDHX_Key_x2 session_key,
    argECDHX_Key_x2 private_key);

typedef int (*XSharedSecret_x2)(
    argECDHX_Key_x2 shared_secret,
    argECDHX_Key_x2 session_key,
    argECDHX_Key_x2 private_key
);

typedef struct _struct_DiffieHellmanXFunction_x2 {
    XKeyGen_x2 keygen;
    XSharedSecret_x2 shared;
    uint64_t key_size;
    Oper0Retr(allocKey, uint8_t*);
    Oper1Void(freeKey, void*);
    Oper1File(printKey, uint8_t*);
    Oper1Void(randKey, uint8_t*);
} X_ECDH_x2;

extern const X_ECDH_x2 X25519_AVX512;
#endif /* defined(ENABLED_AVX512) */

#ifdef __cplusplus
} /* namespace ecdh */
} /* namespace faz  */
} /* extern C */
#endif /* __cplusplus */

#endif /* FAZ_ECDH_AVX2_H */
