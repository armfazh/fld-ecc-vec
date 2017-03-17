#include <faz_eddsa_avx2.h>
#include "vectors.c"

static void test_vector(const EdDSA_TestData * test)
{
    int valid = EDDSA_INVALID_SIGNATURE;

    printf("Test: %s\n",test->name_test);
	if(test->signature_scheme == Ed25519)
	{
		const SignatureScheme * scheme = &EdDSA.Ed25519;
		argEdDSA_PublicKey run_publickey = scheme->newKey();
		argEdDSA_Signature run_signature = scheme->newSignature();

		scheme->keygen(run_publickey, test->private_key);
		printf(LABEL, memcmp(run_publickey, test->public_key, scheme->key_size) == 0 ? OK : ERROR);
		scheme->sign(run_signature, test->message, test->message_length,
					 test->public_key,test->private_key);
		printf(LABEL, memcmp(run_signature, test->signature, scheme->signature_size) == 0 ? OK : ERROR);
		valid = scheme->verify(test->message, test->message_length,
							   run_publickey,run_signature);
		printf(LABEL, valid == EDDSA_VERIFICATION_OK ? OK : ERROR);

		scheme->cleanKey(run_publickey);
		scheme->cleanSignature(run_signature);
	}
	else
	{
		const SignatureSchemeCtx * schemectx;
		switch(test->signature_scheme)
		{
			case Ed448:
				schemectx = &EdDSA.Ed448;
				break;
			case Ed448ph:
				schemectx = &EdDSA.Ed448ph;
				break;
			case Ed25519ph:
				schemectx = &EdDSA.Ed25519ph;
				break;
			case Ed25519ctx:
				schemectx = &EdDSA.Ed25519ctx;
				break;
			default:
				schemectx = NULL;
				printf("Error in test.\n");
		}
		if(schemectx != NULL)
		{
			argEdDSA_PublicKey run_publickey = schemectx->newKey();
			argEdDSA_Signature run_signature = schemectx->newSignature();

			schemectx->keygen(run_publickey, test->private_key);
			printf(LABEL, memcmp(run_publickey, test->public_key, schemectx->key_size) == 0 ? OK : ERROR);
			schemectx->sign(run_signature, test->message, test->message_length,
							test->context, test->context_length, test->public_key,
							test->private_key);
			printf(LABEL, memcmp(run_signature, test->signature, schemectx->signature_size) == 0 ? OK : ERROR);
			valid = schemectx->verify(test->message, test->message_length,
									  test->context, test->context_length,
									  run_publickey, run_signature);
			printf(LABEL, valid == EDDSA_VERIFICATION_OK ? OK : ERROR);

			schemectx->cleanKey(run_publickey);
			schemectx->cleanSignature(run_signature);
		}
	}
}

static void test_ed25519()
{
	printf("=== RFC8032 test vectors ====\n");
	int i=0;
	for(i=0; i<NUM_TEST_VECTORS_ED25519; i++)
	{
		test_vector(vectors_ed25519+i);
	}
}
static void test_ed448()
{
	printf("=== RFC8032 test vectors ====\n");
	int i=0;
	for(i=0; i<NUM_TEST_VECTORS_ED448; i++)
	{
		test_vector(vectors_ed448+i);
	}
}
