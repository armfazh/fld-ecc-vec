#include <stdio.h>
#include <cpu_caps.h>
#include "bench.h"

int main(void) {
  printf("==========================================================\n");
  printf("  High Performance Implementation of Elliptic Curve       \n");
  printf("        Cryptography using Vector Instructions            \n");
  printf("==========================================================\n\n");

  printf("==== Start of Benchmarking  ====\n");
  machine_info();

  bench_hash();
  bench_fp25519();
  bench_x25519();
  bench_ed25519();

  bench_fp448();
  bench_x448();
  bench_ed448();

  printf("====  End of Benchmarking  ====\n");
  return 0;
}
