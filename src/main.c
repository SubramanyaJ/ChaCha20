#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <fcntl.h>

#include "input.h"
#include "operation.h"

int main(int argc, char **argv) {

	/**
	 * Validate command line arguments.
	 * argv[1]: input filename
	 * argv[2]: operation type: 'e' for encrypt or 'd' for decrypt
	 * Determine whether to encrypt or decrypt based on argv[2].
	 */

	if (argc != 3 || (strcmp(argv[2], "e") != 0 
				&& strcmp(argv[2], "d") != 0)) {
		fprintf(stderr, "Usage: %s <filename> <e|d>\n", argv[0]);
		return 1;
	}

	bool encrypting = (strcmp(argv[2], "e") == 0);

	/**
	 * Open the input file for reading.
	 * Create the output filename based on the mode.
	 * Open the output file for writing.
	 */

	int in_fd = open(argv[1], O_RDONLY);
	if (in_fd < 0) {
		fprintf(stderr, "Could not open the file %s\n", argv[1]);
		exit(EXIT_FAILURE);
	}

	char *out_filename = calloc(256, sizeof(char));
	if (encrypting) {
		snprintf(out_filename, 256, "%s.enc", argv[1]);
	}
	else {
		snprintf(out_filename, 256, "%.*s.dec", 
				(int)(strlen(argv[1]) - 4), argv[1]);
	}

	int out_fd = open(out_filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
	free(out_filename);

	/**
	 * Obtain the secret key.
	 * Handle the nonce.
	 * If encrypting, generate a fresh nonce and 
	 * write it to the output file.
	 * If decrypting, read the 12-byte nonce from the 
	 * beginning of the encrypted input file.
	 */
	uint8_t *key = get_key();

	uint8_t nonce[12];
	if (encrypting) {
		uint8_t *nonce_ptr = get_nonce();
		memcpy(nonce, nonce_ptr, 12);
		write(out_fd, nonce, 12);
		free(nonce_ptr);
	} else {
		if (read(in_fd, nonce, 12) != 12) {
			fprintf(stderr, "Failed to read nonce from input file.\n");
			exit(EXIT_FAILURE);
		}
	}

	/**
	 * in_buf: 64-byte buffer to read from input file
	 * keystream: 64-byte buffer to store ChaCha20 keystream block
	 * counter: 32-bit counter starting at 0
	 */
	uint8_t *in_buf = calloc(64, sizeof(uint8_t));
	uint8_t keystream[64];
	ssize_t in_read;
	uint32_t counter = 0;

	/**
	 * Read 64 bytes at a time from input.
	 * Construct ChaCha20 state using key, counter, and nonce.
	 * Generate keystream block with chacha20_block().
	 * XOR input data with keystream.
	 * Write result to output file.
	 */
	while ((in_read = read(in_fd, in_buf, 64)) > 0) {
		uint32_t state[16];
		state[0] = 0x61707865;
		state[1] = 0x3320646E;
		state[2] = 0x79622D32;
		state[3] = 0x6B206574;

		memcpy(&state[4], key, 32);
		state[12] = counter++;
		memcpy(&state[13], nonce, 12);

		chacha20_block(state, keystream);

		for (int i = 0; i < in_read; i++) {
			in_buf[i] ^= keystream[i];
		}

		write(out_fd, in_buf, in_read);
	}

	/**
	 * Free all dynamically allocated data
	 */
	free(in_buf);
	free(key);
	close(in_fd);
	close(out_fd);
	return 0;
}
