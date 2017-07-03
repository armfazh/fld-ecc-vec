#include<faz_eddsa_avx2.h>
#include<faz_fp_avx2.h>
#include<stdio.h>
#include<string.h>

int main()
{
    size_t size_key = ED25519_KEY_SIZE_BYTES_PARAM;
    size_t size_sig = ED25519_SIG_SIZE_BYTES_PARAM;
    Ed25519_PublicKey pub_key;
    Ed25519_PrivateKey pri_key;
    Ed25519_Signature signature;
    int response = 0;

    uint8_t * message = (uint8_t *)"Keep Calm and Carry On";
    const size_t size_msg = strlen((const char *)message);

    printf("=== Ed25519 Example ===\n");

    /* Key generation */
    random_bytes(pri_key,size_key);
    EdDSA.Ed25519.keygen(pub_key,pri_key);
    printf("Alice's Private Key:\n"); print_bytes(pri_key,size_key);
    printf("Alice's Public Key:\n");  print_bytes(pub_key,size_key);

    printf("Message in ascii: \n%s\n",message);
    printf("Message in hex: \n"); print_bytes(message,size_msg);

    /* Signature generation */
    EdDSA.Ed25519.sign(signature,message,size_msg,pub_key,pri_key);
    printf("Ed25519 Signature:\n");  print_bytes(signature,size_sig);

    /* Signature verification */
    response = EdDSA.Ed25519.verify(message,size_msg,pub_key,signature);
    if( response == EDDSA_VERIFICATION_OK )
        printf("Valid signature.\n");
    else
        printf("Signature invalid.\n");

    return 0;
}


