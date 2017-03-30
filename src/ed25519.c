#include "ecc_ed25519.c"
#include "sha512.c"

static uint8_t * initEd25519_Key()
{
	return (uint8_t*) allocate_bytes(ED25519_KEY_SIZE_BYTES_PARAM * sizeof(uint8_t));
}

static uint8_t * initEd25519_Signature()
{
	return (uint8_t*) allocate_bytes(ED25519_SIG_SIZE_BYTES_PARAM * sizeof(uint8_t));
}

/**
 *
 * @param public_key
 * @param secret_key
 * @return
 */
static int ed25519_keygen(
		argEdDSA_PublicKey public_key,
		const argEdDSA_PrivateKey private_key
)
{
	sph_sha512_context context;
	Ed25519_Digest az;
	PointXYZT_2w_H0H5 azB;

	sph_sha512_init(&context);
	sph_sha512(&context,private_key,ED25519_KEY_SIZE_BYTES_PARAM);
	sph_sha512_close(&context,az);
	az[0]  &= -((uint8_t)1<<ED25519_C_PARAM);
	az[31] &= 127;
	az[31] |= 64;
	point_multiplication_ed25519(&azB,az);
	point_encoding_ed25519(public_key,&azB);
	return EDDSA_KEYGEN_OK;
}

/**
 * Given:
 * 		'a' = a_1(2^256) + a_0
 * with |a_0| = |a_1| = 256 bits
 * calculate:
 * 		'a' = a_0 - 16*\ell*a_1
 * 	'a' could be as large as 385 bits.
 */
static __inline void reduce256bits(uint64_t *a)
{
	const uint64_t ellx16[] = {0x812631a5cf5d3ed0,0x4def9dea2f79cd65,0x1};
	__asm__ __volatile__ (
		"movq    (%0), %%rbx      ;"
		"movq   8(%0), %%rcx      ;"
		"movq  16(%0), %%r8       ;"
		"movq  24(%0), %%r9       ;"

		"xorl %%r10d, %%r10d      ;"
		"xorl %%r11d, %%r11d      ;"
		"xorl %%r12d, %%r12d      ;"
		"xorl %%r13d, %%r13d      ;"
		"movq  0(%1), %%rax       ;"
		"mulq 32(%0)              ;"
		"subq %%rax,  %%rbx       ;"
		"sbbq %%rdx,  %%rcx       ;"
		"sbbq $0x0,   %%r8        ;"
		"sbbq $0x0,   %%r9        ;"
		"sbbq $0x0,   %%r10       ;"
		"sbbq $0x0,   %%r11       ;"
		"sbbq $0x0,   %%r12       ;"
		"sbbq $0x0,   %%r13       ;"

		"movq  0(%1), %%rax       ;"
		"mulq 40(%0)              ;"
		"subq %%rax,  %%rcx       ;"
		"sbbq %%rdx,  %%r8        ;"
		"sbbq $0x0,   %%r9        ;"
		"sbbq $0x0,   %%r10       ;"
		"sbbq $0x0,   %%r11       ;"
		"sbbq $0x0,   %%r12       ;"
		"sbbq $0x0,   %%r13       ;"

		"movq  0(%1), %%rax       ;"
		"mulq 48(%0)              ;"
		"subq %%rax,  %%r8        ;"
		"sbbq %%rdx,  %%r9        ;"
		"sbbq $0x0,   %%r10       ;"
		"sbbq $0x0,   %%r11       ;"
		"sbbq $0x0,   %%r12       ;"
		"sbbq $0x0,   %%r13       ;"

		"movq  0(%1), %%rax       ;"
		"mulq 56(%0)              ;"
		"subq %%rax,  %%r9        ;"
		"sbbq %%rdx,  %%r10       ;"
		"sbbq $0x0,   %%r11       ;"
		"sbbq $0x0,   %%r12       ;"
		"sbbq $0x0,   %%r13       ;"

		"movq  8(%1), %%rax       ;"
		"mulq 32(%0)              ;"
		"subq %%rax,  %%rcx       ;"
		"sbbq %%rdx,  %%r8        ;"
		"sbbq $0x0,   %%r9        ;"
		"sbbq $0x0,   %%r10       ;"
		"sbbq $0x0,   %%r11       ;"
		"sbbq $0x0,   %%r12       ;"
		"sbbq $0x0,   %%r13       ;"

		"movq  8(%1), %%rax       ;"
		"mulq 40(%0)              ;"
		"subq %%rax, %%r8         ;"
		"sbbq %%rdx, %%r9         ;"
		"sbbq $0x0,  %%r10        ;"
		"sbbq $0x0,  %%r11        ;"
		"sbbq $0x0,  %%r12        ;"
		"sbbq $0x0,  %%r13        ;"

		"movq  8(%1), %%rax       ;"
		"mulq 48(%0)              ;"
		"subq %%rax,  %%r9        ;"
		"sbbq %%rdx,  %%r10       ;"
		"sbbq $0x0,   %%r11       ;"
		"sbbq $0x0,   %%r12       ;"
		"sbbq $0x0,   %%r13       ;"

		"movq  8(%1), %%rax       ;"
		"mulq 56(%0)              ;"
		"subq %%rax, %%r10        ;"
		"sbbq %%rdx, %%r11        ;"
		"sbbq $0x0,  %%r12        ;"
		"sbbq $0x0,  %%r13        ;"

		"subq 32(%0), %%r8        ;"
		"sbbq 40(%0), %%r9        ;"
		"sbbq 48(%0), %%r10       ;"
		"sbbq 56(%0), %%r11       ;"
		"sbbq $0x0,   %%r12       ;"
		"sbbq $0x0,   %%r13       ;"

		"movq %%rbx,  0(%0)       ;"
		"movq %%rcx,  8(%0)       ;"
		"movq %%r8,  16(%0)       ;"
		"movq %%r9,  24(%0)       ;"
		"movq %%r10, 32(%0)       ;"
		"movq %%r11, 40(%0)       ;"
		"movq %%r12, 48(%0)       ;"
		"movq %%r13, 56(%0)       ;"
	:/* out  */
	:/* in   */ "r" (a),"r" (ellx16)
	:/* regs */
	"memory", "cc", "%rax", "%rbx",
	"%rcx", "%rdx",	 "%r8", "%r9" ,
	"%r10", "%r11", "%r12", "%r13"
	);
}

/**
 * Given:
 * 		'a' = a_257,a_256,a_255,a_254, ... , a_0
 * calculate:
 * 		'a' = a[252:0] - \ell*a_[257:252]
 * 	'a' could be as large as 252 bits.
 */
static __inline void reduce64bits(uint64_t *a)
{
	const uint64_t ell[2] = {0x5812631a5cf5d3ed,0x14def9dea2f79cd6};
	__asm__ __volatile__ (
		"movq     (%0), %%rbx  \n\t"
		"movq    8(%0), %%rcx  \n\t"
		"movq   16(%0), %%r8   \n\t"
		"movq   24(%0), %%r9   \n\t"

		"movq    0(%1), %%rax  \n\t"
		"mulq   32(%0)         \n\t"
		"subq    %%rax, %%rbx  \n\t"
		"sbbq    %%rdx, %%rcx  \n\t"
		"sbbq     $0x0,  %%r8  \n\t"
		"sbbq     $0x0,  %%r9  \n\t"

		"movq    8(%1), %%rax  \n\t"
		"mulq   32(%0)         \n\t"
		"subq    %%rax, %%rcx  \n\t"
		"sbbq    %%rdx,  %%r8  \n\t"
		"sbbq     $0x0,  %%r9  \n\t"

		"movq    %%rbx,  0(%0) \n\t"
		"movq    %%rcx,  8(%0) \n\t"
		"movq     %%r8, 16(%0) \n\t"
		"movq     %%r9, 24(%0) \n\t"
	:/* out  */
	:/* in   */ "r" (a),"r" (ell)
	:/* regs */ "memory", "cc","%rax","%rbx", "%rcx", "%rdx", "%r8", "%r9"
	);
}
/**
 * This function performs a modular reduction
 * (mod \ell) where:
 * 		\ell = 2^252+0x14def9dea2f79cd65812631a5cf5d3ed
 * this is a prime number which denotes the
 * size of the main subgroup of the elliptic curve.
 */
static __inline void modular_reduction_ed25519(uint8_t *a)
{
	uint64_t *pA = (uint64_t *)a;
	reduce256bits(pA);
	reduce256bits(pA);
	pA[4] = ((pA[4]&0x3)<<4) | pA[3]>>60;
	pA[3] &= ((uint64_t)1<<60)-1;
	reduce64bits(pA);
}
/**
 * Calculates:
 *     s = r+k*a  (mod \ell)
 *
 * such that size of r,k and a is 32-bytes.
 */
static __inline void calculate_s_ed25519(uint8_t *s_mod_l, uint8_t *r, uint8_t *k, const uint8_t *a)
{
	int i;
	ALIGN uint64_t product[12] = {0};
	for(i=0;i<4;i++)
	{
		product[i] = ((uint64_t*)r)[i];
	}

	word64_multiplier(product,(uint64_t*)k,4,(uint64_t*)a,4);
	modular_reduction_ed25519((uint8_t*)product);
	for(i=0;i<4;i++)
	{
		((uint64_t*)s_mod_l)[i] = product[i];
	}
}


static int ed25519_sign_all(
		argEdDSA_Signature signature,
		const uint8_t *message,
		uint64_t message_length,
		const uint8_t * context,
		uint64_t        context_length,
		const argEdDSA_PublicKey public_key,
		const argEdDSA_PrivateKey private_key,
		const uint8_t pureEdDSA,
		const uint8_t phflag
)
{
	PointXYZT_2w_H0H5 rB;
	sph_sha512_context hash_context;
	Ed25519_Digest H_RAM,r,ah,pre_hash_message;
	unsigned char prefix[34] = "SigEd25519 no Ed25519 collisions";

	if(pureEdDSA == 0  && context_length > 255)
	{
		return EDDSA_ERROR_CONTEXT;
	}

	/* PHFLAG */
	prefix[32] = phflag;
	/* size context */
	prefix[33] = (uint8_t)context_length;

	sph_sha512_init(&hash_context);
	sph_sha512(&hash_context,private_key,ED25519_KEY_SIZE_BYTES_PARAM);
	sph_sha512_close(&hash_context,ah);
	ah[0]  &= -((uint8_t)1<<ED25519_C_PARAM);
	ah[31] &= 127;
	ah[31] |= 64;

	sph_sha512_init(&hash_context);
	if(pureEdDSA == 0)
	{
		sph_sha512(&hash_context,prefix,34);
		sph_sha512(&hash_context,context,context_length);
	}
	sph_sha512(&hash_context,ah+ED25519_KEY_SIZE_BYTES_PARAM,ED25519_KEY_SIZE_BYTES_PARAM);
	if(phflag == 1)
	{
		sph_sha512_context prehash_context;
		sph_sha512_init(&prehash_context);
		sph_sha512(&prehash_context,message,message_length);
		sph_sha512_close(&prehash_context,pre_hash_message);

		sph_sha512(&hash_context,pre_hash_message,64);
	}
	else if(phflag == 0)
	{
		sph_sha512(&hash_context,message,message_length);
	}
	else
	{
		return EDDSA_ERROR_PHFLAG;
	}
	sph_sha512_close(&hash_context,r);
	modular_reduction_ed25519(r);

	point_multiplication_ed25519(&rB,r);
	point_encoding_ed25519(signature,&rB);

	sph_sha512_init(&hash_context);
	if(pureEdDSA == 0 )
	{
		sph_sha512(&hash_context,prefix,34);
		sph_sha512(&hash_context,context,context_length);
	}
	sph_sha512(&hash_context,signature,ED25519_KEY_SIZE_BYTES_PARAM);
	sph_sha512(&hash_context,public_key,ED25519_KEY_SIZE_BYTES_PARAM);
	if(phflag == 1)
	{
		sph_sha512(&hash_context,pre_hash_message,64);
	}
	else if(phflag == 0)
	{
		sph_sha512(&hash_context,message,message_length);
	}
	else
	{
		return EDDSA_ERROR_PHFLAG;
	}
	sph_sha512_close(&hash_context,H_RAM);
	modular_reduction_ed25519(H_RAM);

	calculate_s_ed25519(signature+ED25519_KEY_SIZE_BYTES_PARAM,r,H_RAM,ah);
	return EDDSA_SIGNATURE_OK;
}

static int ed25519_sign(
		argEdDSA_Signature signature,
		const uint8_t *message,
		uint64_t message_length,
		const argEdDSA_PublicKey public_key,
		const argEdDSA_PrivateKey private_key
)
{
	return ed25519_sign_all(signature,message,message_length,
							EDDSA_NOCONTEXT,0,public_key,private_key,1,0);
}
static int ed25519_signctx(
		argEdDSA_Signature signature,
		const uint8_t *message,
		uint64_t message_length,
		const uint8_t * context,
		uint64_t        context_length,
		const argEdDSA_PublicKey public_key,
		const argEdDSA_PrivateKey private_key
)
{
	return ed25519_sign_all(signature,message,message_length,
							context,context_length,public_key,private_key,0,0);
}
static int ed25519ph_signctx(
		argEdDSA_Signature signature,
		const uint8_t *message,
		uint64_t message_length,
		const uint8_t * context,
		uint64_t        context_length,
		const argEdDSA_PublicKey public_key,
		const argEdDSA_PrivateKey private_key
)
{
	return ed25519_sign_all(signature,message,message_length,
							context,context_length,public_key,private_key,0,1);
}

static int ed25519_verify_all(
		const uint8_t *message,
		uint64_t message_length,
		const uint8_t * context,
		uint64_t        context_length,
		const argEdDSA_PublicKey public_key,
		const argEdDSA_Signature signature,
		const uint8_t pureEdDSA,
		const uint8_t phflag
)
{
	sph_sha512_context hash_context;
	Ed25519_Digest H_RAM,pre_hash_message;
	PointXYZT_2w_H0H5 A;
	Ed25519_Signature Q;
	unsigned char prefix[34] = "SigEd25519 no Ed25519 collisions";

	if(pureEdDSA == 0  && context_length > 255)
	{
		return EDDSA_ERROR_CONTEXT;
	}

	/* PHFLAG */
	prefix[32] = phflag;
	/* size context */
	prefix[33] = (uint8_t)context_length;

	point_decoding_ed25519(&A,(uint8_t *)public_key);

	sph_sha512_init(&hash_context);
	if(pureEdDSA == 0 )
	{
		sph_sha512(&hash_context,prefix,34);
		sph_sha512(&hash_context,context,context_length);
	}
	sph_sha512(&hash_context,signature,ED25519_KEY_SIZE_BYTES_PARAM);
	sph_sha512(&hash_context,public_key,ED25519_KEY_SIZE_BYTES_PARAM);
	if(phflag == 1)
	{
		sph_sha512_context prehash_context;
		sph_sha512_init(&prehash_context);
		sph_sha512(&prehash_context,message,message_length);
		sph_sha512_close(&prehash_context,pre_hash_message);

		sph_sha512(&hash_context,pre_hash_message,64);
	}
	else if(phflag == 0)
	{
		sph_sha512(&hash_context,message,message_length);
	}
	else
	{
		return EDDSA_ERROR_PHFLAG;
	}
	sph_sha512_close(&hash_context,H_RAM);
	modular_reduction_ed25519(H_RAM);

	double_point_multiplication_ed25519(Q,(uint8_t*)(signature+ED25519_KEY_SIZE_BYTES_PARAM),H_RAM,&A);
	return (memcmp(signature,Q,ED25519_KEY_SIZE_BYTES_PARAM) == 0)? EDDSA_VERIFICATION_OK : EDDSA_INVALID_SIGNATURE;
}

static int ed25519_verify(
		const uint8_t *message,
		uint64_t message_length,
		const argEdDSA_PublicKey public_key,
		const argEdDSA_Signature signature
)
{
	return ed25519_verify_all(message,message_length,EDDSA_NOCONTEXT,0,public_key,signature,1,0);
}
static int ed25519_verifyctx(
		const uint8_t *message,
		uint64_t message_length,
		const uint8_t * context,
		uint64_t        context_length,
		const argEdDSA_PublicKey public_key,
		const argEdDSA_Signature signature
)
{
	return ed25519_verify_all(message,message_length,context,context_length,public_key,signature,0,0);
}
static int ed25519ph_verifyctx(
		const uint8_t *message,
		uint64_t message_length,
		const uint8_t * context,
		uint64_t        context_length,
		const argEdDSA_PublicKey public_key,
		const argEdDSA_Signature signature
)
{
	return ed25519_verify_all(message,message_length,context,context_length,public_key,signature,0,1);
}



