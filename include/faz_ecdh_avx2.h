#ifndef _FAZ_ECDH_AVX2_H_
#define _FAZ_ECDH_AVX2_H_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <stdint.h>

#define ALIGN_BYTES 32
#ifdef __INTEL_COMPILER
#define ALIGN __declspec(align(ALIGN_BYTES))
#else
#define ALIGN __attribute__ ((aligned (ALIGN_BYTES)))
#endif

#define ZeroOperandReturnKey(X) uint8_t * (*X)()
#define OneOperandGeneric(X) void (*X)(void* C)

typedef uint8_t * argECDHX_Key;

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
	ZeroOperandReturnKey(initKey);
	OneOperandGeneric(clearKey);
} DiffieHellmanXFunction;

struct _struct_ECDHX {
	DiffieHellmanXFunction X25519, X25519_x64, X448;
};

extern const struct _struct_ECDHX ECDHX;

/** X25519 */
#define ECDH25519_KEY_SIZE_BYTES 32
typedef ALIGN uint8_t ECDH_X25519_KEY[ECDH25519_KEY_SIZE_BYTES];

/** X448 */
#define ECDH448_KEY_SIZE_BYTES 56
typedef ALIGN uint8_t ECDH_X448_KEY[ECDH448_KEY_SIZE_BYTES];


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

#ifdef __cplusplus
};
#endif /* __cplusplus */

#endif /* _FAZ_ECDH_AVX2_H_ */

