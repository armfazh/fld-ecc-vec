#include<faz_ecdh_avx2.h>
#include<faz_fp_avx2.h>
#include<stdio.h>

int main(void)
{
    size_t keysize = ECDH25519_KEY_SIZE_BYTES;
    ECDH_X25519_KEY alice_private,alice_session,alice_shared;
    ECDH_X25519_KEY bob_private,bob_session,bob_shared;

    printf("=== X25519 Example ===\n");

    /* Alice session key generation */
    random_bytes(alice_private,keysize);
    printf("Alice private key: \n");  print_bytes(alice_private,keysize);
    ECDHX.X25519.keygen(alice_session,alice_private);
    printf("Alice session key: \n");   print_bytes(alice_session,keysize);

    /* Bob session key generation */
    random_bytes(bob_private,keysize);
    printf("Bob private key: \n");  print_bytes(bob_private,keysize);
    ECDHX.X25519.keygen(bob_session,bob_private);
    printf("Bob session key: \n");   print_bytes(bob_session,keysize);

    /* Shared secret generation */
    ECDHX.X25519.shared(alice_shared,bob_session,alice_private);
    printf("Alice shared secret: \n");  print_bytes(alice_shared,keysize);
    ECDHX.X25519.shared(bob_shared,alice_session,bob_private);
    printf("Bob shared secret: \n");    print_bytes(bob_shared,keysize);

    return 0;
}
