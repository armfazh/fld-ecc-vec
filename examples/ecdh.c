#include <faz_ecdh_avx2.h>
#include <stdio.h>

int main(void) {
  X25519_KEY alice_private, alice_session, alice_shared;
  X25519_KEY bob_private, bob_session, bob_shared;

  printf("=== X25519 Example ===\n");

  /* Alice session key generation */
  X25519_AVX2.randKey(alice_private);
  printf("Alice private key: \n");
  X25519_AVX2.printKey(stdout, alice_private);
  X25519_AVX2.keygen(alice_session, alice_private);
  printf("Alice session key: \n");
  X25519_AVX2.printKey(stdout, alice_session);

  /* Bob session key generation */
  X25519_AVX2.randKey(bob_private);
  printf("Bob private key: \n");
  X25519_AVX2.printKey(stdout, bob_private);
  X25519_AVX2.keygen(bob_session, bob_private);
  printf("Bob session key: \n");
  X25519_AVX2.printKey(stdout, bob_session);

  /* Shared secret generation */
  X25519_AVX2.shared(alice_shared, bob_session, alice_private);
  printf("Alice shared secret: \n");
  X25519_AVX2.printKey(stdout, alice_shared);
  X25519_AVX2.shared(bob_shared, alice_session, bob_private);
  printf("Bob shared secret: \n");
  X25519_AVX2.printKey(stdout, bob_shared);

  return 0;
}
