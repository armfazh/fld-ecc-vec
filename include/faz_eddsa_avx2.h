#ifndef _EDDSA_AVX2_H_
#define _EDDSA_AVX2_H_

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

#define argEdDSA_PrivateKey uint8_t *
#define argEdDSA_PublicKey  uint8_t *
#define argEdDSA_Signature  uint8_t *

typedef int (*SignKeyGen)(argEdDSA_PublicKey, const argEdDSA_PrivateKey);

typedef int (*Sign)(
		argEdDSA_Signature ,
		const uint8_t * message,
		uint64_t        message_length,
		const argEdDSA_PublicKey publicKey,
		const argEdDSA_PrivateKey privateKey
);

typedef int (*Verify)(
		const uint8_t * message,
		uint64_t message_length,
		const argEdDSA_PublicKey publicKey,
		const argEdDSA_Signature signature
);

typedef int (*SignCtx)(
		argEdDSA_Signature ,
		const uint8_t * message,
		uint64_t        message_length,
		const uint8_t * context,
		uint64_t        context_length,
		const argEdDSA_PublicKey publicKey,
		const argEdDSA_PrivateKey privateKey
);

typedef int (*VerifyCtx)(
		const uint8_t * message,
		uint64_t message_length,
		const uint8_t * context,
		uint64_t        context_length,
		const argEdDSA_PublicKey publicKey,
		const argEdDSA_Signature signature
);

typedef struct _struct_SignatureScheme {
	SignKeyGen keygen;
	Sign sign;
	Verify verify;
	uint64_t key_size;
	uint64_t signature_size;
	ZeroOperandReturnKey (initKey);
	ZeroOperandReturnKey (initSignature);
	OneOperandGeneric(clearKey);
	OneOperandGeneric(clearSignature);
} SignatureScheme;

typedef struct _struct_SignatureSchemeCtx {
	SignKeyGen keygen;
	SignCtx sign;
	VerifyCtx verify;
	uint64_t key_size;
	uint64_t signature_size;
    ZeroOperandReturnKey(initKey);
    ZeroOperandReturnKey(initSignature);
    OneOperandGeneric(clearKey);
    OneOperandGeneric(clearSignature);
} SignatureSchemeCtx;

struct _struct_EdDSA {
	SignatureScheme Ed25519;
	SignatureSchemeCtx Ed25519ph;
	SignatureSchemeCtx Ed25519ctx;
	SignatureSchemeCtx Ed448;
	SignatureSchemeCtx Ed448ph;
};

/**
 * Global structures that provide access
 * to the library.
 */
extern const struct _struct_EdDSA EdDSA;

/**
 * Flag for sign/verify indicating
 * an empty context.
 */
#define EDDSA_NOCONTEXT 0


/**
 * Types and sizes of Ed25519
 **/
#define ED25519_B_PARAM	256
#define ED25519_C_PARAM	3
#define ED25519_N_PARAM	254
#define ED25519_HASH_BYTES_PARAM 64
#define ED25519_KEY_SIZE_BYTES_PARAM ((ED25519_B_PARAM)/8)
#define ED25519_SIG_SIZE_BYTES_PARAM (2*(ED25519_B_PARAM)/8)

typedef ALIGN uint8_t Ed25519_PrivateKey[ED25519_KEY_SIZE_BYTES_PARAM];
typedef ALIGN uint8_t Ed25519_PublicKey[ED25519_KEY_SIZE_BYTES_PARAM];
typedef ALIGN uint8_t Ed25519_Signature[ED25519_SIG_SIZE_BYTES_PARAM];
typedef ALIGN uint8_t Ed25519_Digest[ED25519_HASH_BYTES_PARAM];


/**
 * Types and sizes of Ed448
 **/
#define ED448_B_PARAM	456
#define ED448_C_PARAM	2
#define ED448_N_PARAM	447
#define ED448_HASH_BYTES_PARAM 114
#define ED448_KEY_SIZE_BYTES_PARAM ((ED448_B_PARAM)/8)
#define ED448_SIG_SIZE_BYTES_PARAM (2*(ED448_B_PARAM)/8)

typedef ALIGN uint8_t Ed448_PrivateKey[ED448_KEY_SIZE_BYTES_PARAM];
typedef ALIGN uint8_t Ed448_PublicKey[ED448_KEY_SIZE_BYTES_PARAM];
typedef ALIGN uint8_t Ed448_Signature[ED448_SIG_SIZE_BYTES_PARAM];
typedef ALIGN uint8_t Ed448_Digest[ED448_HASH_BYTES_PARAM];


/**
 * Error codes
 **/
enum EDDSA_FLAGS {
	EDDSA_KEYGEN_OK,
	EDDSA_SIGNATURE_OK,
	EDDSA_VERIFICATION_OK,
	EDDSA_INVALID_SIGNATURE,
	EDDSA_ERROR_PUBLICKEY,
	EDDSA_ERROR_CONTEXT,
	EDDSA_ERROR_PHFLAG,
};

#ifdef __cplusplus
};
#endif /* __cplusplus */

#endif  /* _EDDSA_AVX2_H_ */
