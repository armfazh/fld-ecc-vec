#include <faz_hash_avx2.h>
#include <prng.h>

#include "benchmark/benchmark.h"

using namespace faz::hash;
using namespace prng;

static void BM_HASH_25519_X64(benchmark::State &state) {
  const size_t MLEN = 256;
  uint8_t message[MLEN];
  Point P;

  prgn_random_bytes(message, MLEN);
  for (auto _ : state) {
    h2c25519_x64(&P, message, MLEN);
  }
}

static void BM_HASH_25519_AVX2(benchmark::State &state) {
  const size_t MLEN = 256;
  uint8_t message[MLEN];
  Point P;

  prgn_random_bytes(message, MLEN);
  for (auto _ : state) {
    h2c25519_avx2(&P, message, MLEN);
  }
}

BENCHMARK(BM_HASH_25519_X64)->Unit(benchmark::kMicrosecond);
BENCHMARK(BM_HASH_25519_AVX2)->Unit(benchmark::kMicrosecond);

BENCHMARK_MAIN();
