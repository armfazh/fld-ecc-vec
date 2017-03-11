#include "eddsa_avx2.h"
#include "ed448.c"
#include "x448.c"

const struct _struct_ECDHX ECDHX = {
		.X448 = {
				.keygen = x448_keygen,
				.shared = x448_shared,
				.key_size = ECDH448_KEY_SIZE_BYTES
		},
/*		.X25519 = {
//				.keygen = x25519_keygen,
//				.shared = x25519_shared,
//				.key_size = ECDH25519_KEY_SIZE_BYTES
//		}
*/
};

const struct _struct_EdDSA EdDSA = {
		.Ed448 = {
				.keygen = ed448_keygen,
				.sign   = ed448_signctx,
				.verify = ed448_verifyctx,
				.key_size = ED448_KEY_SIZE_BYTES_PARAM,
				.signature_size = ED448_SIG_SIZE_BYTES_PARAM
		},
		.Ed448ph = {
				.keygen = ed448_keygen,
				.sign   = ed448ph_signctx,
				.verify = ed448ph_verifyctx,
				.key_size = ED448_KEY_SIZE_BYTES_PARAM,
				.signature_size = ED448_SIG_SIZE_BYTES_PARAM
		}
/*		.Ed25519 = {
//				.keygen = ed448_keygen,
//				.sign = ed448_signctx,
//				.verify = ed448_verifyctx,
//				.key_size = ED448_KEY_SIZE_BYTES_PARAM,
//				.signature_size = ED25519_SIG_SIZE_BYTES_PARAM
//		},
//		.Ed25519 = {
//				.keygen = ed448_keygen,
//				.sign = ed448_signctx,
//				.verify = ed448_verifyctx,
//				.key_size = 32,
//				.signature_size = 64
//		},
//		.Ed25519 = {
//				.keygen = ed448_keygen,
//				.sign = ed448_signctx,
//				.verify = ed448_verifyctx,
//				.key_size = 32,
//				.signature_size = 64
//		},
//		.Ed25519 = {
//				.keygen = ed448_keygen,
//				.sign = ed448_signctx,
//				.verify = ed448_verifyctx
//		}
*/
};

