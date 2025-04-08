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
