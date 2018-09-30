#include <stdio.h>
#include <cpu_caps.h>
#include "tests.h"

int main(void) {
  printf("==========================================================\n");
  printf("  High Performance Implementation of Elliptic Curve       \n");
  printf("        Cryptography using Vector Instructions            \n");
  printf("==========================================================\n\n");

  printf("=== Start of Test Driver ===\n");
  machine_info();

  test_fp25519();
  test_x25519();
  test_ed25519();

  test_fp448();
  test_x448();
  test_ed448();

  printf("=== End of Test Driver ====\n");
  return 0;
}
