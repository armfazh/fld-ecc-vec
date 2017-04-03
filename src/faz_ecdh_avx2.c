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
