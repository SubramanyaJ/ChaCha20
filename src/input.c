#include "input.h"
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>

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
