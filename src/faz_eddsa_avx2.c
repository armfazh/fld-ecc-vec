#include "faz_eddsa_avx2.h"
#include "fp.c"
#include "ed25519.c"
#include "ed448.c"
#include "faz_ecdh_avx2.c"

const struct _struct_EdDSA EdDSA = {
		.Ed25519 = {
				.keygen = ed25519_keygen,
				.sign = ed25519_sign,
				.verify = ed25519_verify,
				.key_size = ED25519_KEY_SIZE_BYTES_PARAM,
				.signature_size = ED25519_SIG_SIZE_BYTES_PARAM,
				.newKey = newEd25519_Key,
				.newSignature = newEd25519_Signature,
				.cleanKey = deallocate_bytes,
				.cleanSignature = deallocate_bytes
		},
		.Ed25519ph = {
				.keygen = ed25519_keygen,
				.sign = ed25519ph_signctx,
				.verify = ed25519ph_verifyctx,
				.key_size = ED25519_KEY_SIZE_BYTES_PARAM,
				.signature_size = ED25519_SIG_SIZE_BYTES_PARAM,
				.newKey = newEd25519_Key,
				.newSignature = newEd25519_Signature,
				.cleanKey = deallocate_bytes,
				.cleanSignature = deallocate_bytes
		},
		.Ed25519ctx = {
				.keygen = ed25519_keygen,
				.sign = ed25519_signctx,
				.verify = ed25519_verifyctx,
				.key_size = ED25519_KEY_SIZE_BYTES_PARAM,
				.signature_size = ED25519_SIG_SIZE_BYTES_PARAM,
				.newKey = newEd25519_Key,
				.newSignature = newEd25519_Signature,
				.cleanKey = deallocate_bytes,
				.cleanSignature = deallocate_bytes
		},
		.Ed448 = {
				.keygen = ed448_keygen,
				.sign   = ed448_signctx,
				.verify = ed448_verifyctx,
				.key_size = ED448_KEY_SIZE_BYTES_PARAM,
				.signature_size = ED448_SIG_SIZE_BYTES_PARAM,
				.newKey = newEd448_Key,
				.newSignature = newEd448_Signature,
				.cleanKey = deallocate_bytes,
				.cleanSignature = deallocate_bytes
		},
		.Ed448ph = {
				.keygen = ed448_keygen,
				.sign   = ed448ph_signctx,
				.verify = ed448ph_verifyctx,
				.key_size = ED448_KEY_SIZE_BYTES_PARAM,
				.signature_size = ED448_SIG_SIZE_BYTES_PARAM,
				.newKey = newEd448_Key,
				.newSignature = newEd448_Signature,
				.cleanKey = deallocate_bytes,
				.cleanSignature = deallocate_bytes
		}
};

