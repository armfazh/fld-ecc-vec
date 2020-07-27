#ifndef PRNG_H
#define PRNG_H

#ifdef __cplusplus
namespace prng {
extern "C" {
#endif

#include <stdint.h>
#include <stdlib.h>

void prgn_random_bytes(uint8_t *buffer, size_t num_bytes);

#ifdef __cplusplus
} /* extern C */
} /* namespace prng */
#endif

#endif /* PRNG_H */
