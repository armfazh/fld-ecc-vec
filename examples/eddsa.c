#include<faz_eddsa_avx2.h>
#include<stdio.h>
#include<string.h>

int main(void) {
  Ed25519_PublicKey pub_key;
  Ed25519_PrivateKey pri_key;
  Ed25519_Signature signature;
  size_t i = 0;
  int response = 0;

  uint8_t *message = (uint8_t *) "Keep Calm and Carry On";
  const size_t size_msg = strlen((const char *) message);

  printf("=== Ed25519 Example ===\n");

  /* Key generation */
  Ed25519.Pure.randKey(pri_key);
  Ed25519.Pure.keygen(pub_key, pri_key);
  printf("Alice's Private Key:\n");
  Ed25519.Pure.printKey(stdout, pri_key);
  printf("Alice's Public Key:\n");
  Ed25519.Pure.printKey(stdout, pub_key);

  printf("Message in ascii: \n%s\n", message);
  printf("Message in hex: \n");
  for (i = 0; i < size_msg; i++)
    printf("%02x", message[i]);
  printf("\n");

  /* Signature generation */
  Ed25519.Pure.sign(signature, message, size_msg, pub_key, pri_key);
  printf("Ed25519 Signature:\n");
  Ed25519.Pure.printSignature(stdout, signature);

  /* Signature verification */
  response = Ed25519.Pure.verify(message, size_msg, pub_key, signature);
  if (response == EDDSA_VERIFICATION_OK)
    printf("Valid signature.\n");
  else
    printf("Signature invalid.\n");

  return 0;
}
