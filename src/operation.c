#include "operation.h"
#include <stdint.h>
#include <string.h>

void quarterround(uint32_t *a, uint32_t *b, uint32_t *c, uint32_t *d) {
	*a += *b; *d ^= *a; *d = ROTL32(*d, 16);
	*c += *d; *b ^= *c; *b = ROTL32(*b, 12);
	*a += *b; *d ^= *a; *d = ROTL32(*d, 8);
	*c += *d; *b ^= *c; *b = ROTL32(*b, 7);
}

void chacha20_block(uint32_t state[16], uint8_t output[64]) {
	uint32_t working_state[16];
	memcpy(working_state, state, 64);

	for (int i = 0; i < 10; i++) {
		quarterround(&working_state[0], &working_state[4], &working_state[8], &working_state[12]);
		quarterround(&working_state[1], &working_state[5], &working_state[9], &working_state[13]);
		quarterround(&working_state[2], &working_state[6], &working_state[10], &working_state[14]);
		quarterround(&working_state[3], &working_state[7], &working_state[11], &working_state[15]);

		quarterround(&working_state[0], &working_state[5], &working_state[10], &working_state[15]);
		quarterround(&working_state[1], &working_state[6], &working_state[11], &working_state[12]);
		quarterround(&working_state[2], &working_state[7], &working_state[8], &working_state[13]);
		quarterround(&working_state[3], &working_state[4], &working_state[9], &working_state[14]);
	}

	for (int i = 0; i < 16; i++) {
		working_state[i] += state[i];
	}

	memcpy(output, working_state, 64);
}
