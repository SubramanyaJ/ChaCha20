#ifndef OPERATION_H
#define OPERATION_H

#include <stdint.h>

/**
 * A macro to implement the left-shift
 * operation for 32-bit integers
 */

#define ROTL32(v, n) ((v << n) | (v >> (32 - n)))

/**
 * Implements the quarter round function as defined
 * by Bernstein :
 * Salsa20 invertibly updates 4 32-bit state words a, b, c, d as follows:
 * b ^= (a+d) <<< 7;
 * c ^= (b+a) <<< 9;
 * d ^= (c+b) <<< 13;
 * a ^= (d+c) <<< 18;
 */
void quarterround(uint32_t *a, uint32_t *b, uint32_t *c, uint32_t *d);

void chacha20_block(uint32_t state[16], uint8_t output[64]);

#endif /* operation.h included */
