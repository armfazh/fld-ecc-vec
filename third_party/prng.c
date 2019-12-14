#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

/** Random number Generator:
 * Taken from: https://github.com/relic-toolkit/relic/src/rand/relic_rand_call.c
 *
 * @warning Provide a secure random number generator.
 * @param buffer
 * @param num_bytes
 */
#include <fcntl.h>
#include <unistd.h>
void prgn_random_bytes(uint8_t *buffer, size_t num_bytes) {
  size_t l = 0;
  int c, fd = open("/dev/urandom", O_RDONLY);

  if (fd == -1) {
    printf("Error opening /dev/urandom\n");
  }

  l = 0;
  do {
    c = read(fd, buffer + l, num_bytes - l);
    l += c;
    if (c == -1) {
      printf("Error reading /dev/urandom\n");
    }
  } while (l < num_bytes);

  close(fd);
}
