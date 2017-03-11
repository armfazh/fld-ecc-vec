/**
 * Code taken from === Keccak Code Package.
 * The Keccak sponge function, designed by Guido Bertoni, Joan Daemen,
 * Michaël Peeters and Gilles Van Assche. For more information, feedback or
 * questions, please refer to our website: http://keccak.noekeon.org/
 */
#ifndef _SHA3_H_
#define _SHA3_H_
#include <stdint.h>

typedef unsigned char BitSequence;
typedef unsigned long long DataLength; /* a typical 64-bit value */

typedef enum { SUCCESS = 0, FAIL = 1, BAD_HASHBITLEN = 2 } HashReturn;

typedef struct{
  uint8_t s[200] __attribute__ ((aligned (32)));
  unsigned int hashbitlen;
  unsigned int statebitlen;
  unsigned int r;
}hashState;


/*
 * state: a structure that holds ....
 */
HashReturn Init (hashState *state, int hashbitlen);

/*
 * .....
 */
HashReturn Update(hashState *state, const BitSequence *data, DataLength databitlen);

/*
 * ......
 */
HashReturn Final(hashState *state, BitSequence *hashval);

/*used in SHAKE*/
HashReturn Final2(hashState *state, BitSequence *hashval, unsigned  int d);

/*
 * .....
 */
HashReturn Hash(int hashbitlen, const BitSequence *data, 
                DataLength databitlen, BitSequence *hashval);

/*hashbitlen = 128 to SHAKE128 and 256 to SHAKE256*/
HashReturn SHAKE(int hashbitlen, const BitSequence *data, 
                DataLength databitlen, BitSequence *hashval, unsigned int d);


HashReturn SHAKE128(const BitSequence *data, DataLength databitlen, BitSequence *hashval, unsigned int d_byte);
HashReturn SHAKE256(const BitSequence *data, DataLength databitlen, BitSequence *hashval, unsigned int d_byte);

#endif /* _SHA3_H_ */
