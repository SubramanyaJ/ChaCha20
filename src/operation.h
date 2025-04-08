#ifndef OPERATION_H
#define OPERATION_H

#include <stdint.h>

#define ROTL32(v, n) ((v << n) | (v >> (32 - n)))

// Quarter Round
void quarterround(uint32_t *a, uint32_t *b, uint32_t *c, uint32_t *d);

#endif /* operation.h included */
