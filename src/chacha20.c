#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>

#include "chacha20.h"

uint8_t *get_key() {
	char buffer[256];
	uint8_t *key = calloc(32, sizeof(uint8_t));

	fprintf(stdout, "Enter password: ");
	fflush(stdout);

	if (!fgets(buffer, sizeof(buffer), stdin)) {
		fprintf(stderr, "Error reading input.\n");
		free(key);
		return NULL;
	}

	size_t len = strlen(buffer);
	if (len > 0 && buffer[len - 1] == '\n') {
		buffer[len - 1] = '\0';
		len--;
	}
	memcpy(key, buffer, len > 32 ? 32 : len);

	return key;
}

uint8_t *get_nonce() {
	uint8_t *nonce = calloc(12, sizeof(uint8_t));

	int fd = open("/dev/urandom", O_RDONLY);
	if (fd < 0) {
	random_failure:
		fprintf(stderr, "Warning: /dev/urandom could not be used.\nUsing stdlib's rand() instead.\n");
		for (int i = 0; i < 12; i++) {
			nonce[i] = rand();
		}
		return nonce;
	}

	ssize_t bytes_read = read(fd, nonce, 12);
	close(fd);

	if (bytes_read != 12) {
		goto random_failure;
	}

	return nonce;
}

uint32_t load32_le(const uint8_t *src) {
	return ((uint32_t)src[0]) |
	((uint32_t)src[1] << 8) |
	((uint32_t)src[2] << 16) |
	((uint32_t)src[3] << 24);
}

void store32_le(uint8_t *dst, uint32_t w) {
	dst[0] = w & 0xff;
	dst[1] = (w >> 8) & 0xff;
	dst[2] = (w >> 16) & 0xff;
	dst[3] = (w >> 24) & 0xff;
}

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

void poly1305_auth(uint8_t *mac, const uint8_t *msg, size_t msg_len, const uint8_t key[32]) {
	uint32_t r[4], s[4], acc[5] = {0};
	memcpy(r, key, 16);
	memcpy(s, key + 16, 16);

	r[0] &= 0x3FFFFFFF;
	r[1] &= 0xFFFFFFC0;
	r[2] &= 0xFFFFFFC0;
	r[3] &= 0x0FFFFFFF;

	while (msg_len > 0) {
		uint32_t block[4] = {0};
		size_t block_len = (msg_len >= 16) ? 16 : msg_len;
		memcpy(block, msg, block_len);
		block[block_len / 4] |= (1 << (8 * (block_len % 4))); // 1 bit padding

		for (int i = 0; i < 4; i++) {
			acc[i] += block[i];
		}

		uint64_t t = 0;
		for (int i = 0; i < 4; i++) {
			t += (uint64_t)acc[i] * r[i];
			acc[i] = (uint32_t)t;
			t >>= 32;
		}

		msg += block_len;
		msg_len -= block_len;
	}

	for (int i = 0; i < 4; i++) {
		acc[i] += s[i];
	}

	memcpy(mac, acc, 16);
}
