/**
 * @file main.c
 * @brief Entry point for the ChaCha20 file encryption/decryption utility.
 *
 * This program encrypts or decrypts a file using the ChaCha20 stream cipher.
 * It reads from an input file, processes 64-byte blocks, and writes to an
 * output file. It uses a password-derived key and a 12-byte nonce.
 *
 * Usage: ./program <filename> <e|d>
 *   - 'e' to encrypt the file
 *   - 'd' to decrypt a previously encrypted file
 *
 * @author Subramanya J
 * subramanyajaradhya@gmail.com
 */

#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <fcntl.h>

#include "chacha20.h"

/**
 * @brief Main function to handle encryption/decryption based on 
 * command-line arguments.
 *
 * @param argc Argument count
 * @param argv Argument vector. 
 *             argv[1]: input filename
 *             argv[2]: 'e' for encryption, 'd' for decryption
 *
 * @return int Exit status code
 */
int main(int argc, char **argv) {

	/**
	 * @brief Validate command line arguments
	 * Ensure correct usage and valid mode ('e' or 'd')
	 */
	if (argc != 3 || (strcmp(argv[2], "e") != 0 && strcmp(argv[2], "d") != 0)) {
		fprintf(stderr, "Usage: %s <filename> <e|d>\n", argv[0]);
		return 1;
	}

	bool encrypting = (strcmp(argv[2], "e") == 0);

	/**
	 * @brief Open the input file for reading.
	 * Generate output filename:
	 *   - Add '.enc' for encryption
	 *   - Replace '.enc' with '.dec' for decryption
	 */
	int in_fd = open(argv[1], O_RDONLY);
	if (in_fd < 0) {
		fprintf(stderr, "Could not open the file %s\n", argv[1]);
		exit(EXIT_FAILURE);
	}

	char *out_filename = calloc(256, sizeof(char));
	if (encrypting) {
		snprintf(out_filename, 256, "%s.enc", argv[1]);
	} else {
		snprintf(out_filename, 256, "%.*s.dec", (int)(strlen(argv[1]) - 4), argv[1]);
	}

	int out_fd = open(out_filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
	free(out_filename);

	/**
	 * @brief Obtain encryption key and nonce.
	 * - Call get_key() to receive 32-byte key from user.
	 * - If encrypting, generate a new 12-byte nonce and write to output.
	 * - If decrypting, read the 12-byte nonce from the input file.
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
	 * @brief Set up buffers and counter for ChaCha20 encryption.
	 * - in_buf: temporary buffer for 64-byte chunks from input file
	 * - keystream: buffer to hold 64-byte ChaCha20 keystream block
	 * - counter: 32-bit block counter initialized to 0
	 */
	uint8_t *in_buf = calloc(64, sizeof(uint8_t));
	uint8_t keystream[64];
	ssize_t in_read;
	uint32_t counter = 0;

	/**
	 * @brief Process input in 64-byte blocks.
	 * - Construct ChaCha20 state for each block
	 * - Generate keystream
	 * - XOR keystream with input data
	 * - Write encrypted/decrypted block to output
	 */
	while ((in_read = read(in_fd, in_buf, 64)) > 0) {
		uint32_t state[16];

		// Initialize ChaCha20 constants
		state[0] = 0x61707865;
		state[1] = 0x3320646E;
		state[2] = 0x79622D32;
		state[3] = 0x6B206574;

		// Load 32-byte key into state[4..11]
		for (int i = 0; i < 8; i++) {
			state[4 + i] = load32_le(key + i * 4);
		}

		// Set counter and nonce
		state[12] = counter++;
		state[13] = load32_le(nonce);
		state[14] = load32_le(nonce + 4);
		state[15] = load32_le(nonce + 8);

		// Generate ChaCha20 keystream block
		chacha20_block(state, keystream);

		// XOR keystream with input buffer
		for (int i = 0; i < in_read; i++) {
			in_buf[i] ^= keystream[i];
		}

		// Write result to output file
		write(out_fd, in_buf, in_read);
	}

	/**
	 * @brief Clean up: free buffers and close file descriptors.
	 */
	free(in_buf);
	free(key);
	close(in_fd);
	close(out_fd);

	return 0;
}
