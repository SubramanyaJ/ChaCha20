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
 * @brief Main function to handle encryption/decryption based on command-line arguments.
 *
 * The program uses a password-derived key for encryption or decryption of a file.
 * For encryption, it generates a random 12-byte nonce and writes it to the output file.
 * For decryption, it reads the nonce from the input file.
 *
 * @param argc Argument count.
 * @param argv Argument vector.
 *             argv[1]: input filename
 *             argv[2]: 'e' for encryption, 'd' for decryption
 *
 * @return int Exit status code (0 for success, 1 for argument error, or other error codes).
 */
int main(int argc, char **argv) {
	if (argc != 3 || (strcmp(argv[2], "e") != 0 && strcmp(argv[2], "d") != 0)) {
		fprintf(stderr, "Usage: %s <filename> <e|d>\n", argv[0]);
		return 1;
	}

	bool encrypting = (strcmp(argv[2], "e") == 0);
	int in_fd = open(argv[1], O_RDONLY);
	if (in_fd < 0) {
		fprintf(stderr, "Could not open the file %s\n", argv[1]);
		exit(EXIT_FAILURE);
	}

	char *out_filename = calloc(256, sizeof(char));
	snprintf(out_filename, 256, "%s.%s", argv[1], encrypting ? "enc" : "dec");
	int out_fd = open(out_filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
	free(out_filename);

	uint8_t *key = get_key();
	uint8_t nonce[12];
	uint8_t mac[16];

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

	uint8_t *in_buf = calloc(64, sizeof(uint8_t));
	uint8_t *ciphertext = calloc(64, sizeof(uint8_t));
	uint8_t keystream[64];
	ssize_t in_read;
	uint32_t counter = 0;

	while ((in_read = read(in_fd, in_buf, 64)) > 0) {
		uint32_t state[16] = {
			0x61707865, 0x3320646E, 0x79622D32, 0x6B206574,
		};
		for (int i = 0; i < 8; i++) {
			state[4 + i] = load32_le(key + i * 4);
		}
		state[12] = counter++;
		state[13] = load32_le(nonce);
		state[14] = load32_le(nonce + 4);
		state[15] = load32_le(nonce + 8);

		chacha20_block(state, keystream);

		for (int i = 0; i < in_read; i++) {
			ciphertext[i] = in_buf[i] ^ keystream[i];
		}

		write(out_fd, ciphertext, in_read);
	}

	if (encrypting) {
		lseek(out_fd, 0, SEEK_END);
		poly1305_auth(mac, ciphertext, in_read, key);
		write(out_fd, mac, 16);
	} else {
		lseek(in_fd, -16, SEEK_END);
		read(in_fd, mac, 16);
		uint8_t computed_mac[16];
		poly1305_auth(computed_mac, ciphertext, in_read, key);

		if (memcmp(mac, computed_mac, 16) != 0) {
			fprintf(stderr, "Authentication failed. File may be corrupted.\n");
			exit(EXIT_FAILURE);
		}
	}

	free(in_buf);
	free(ciphertext);
	free(key);
	close(in_fd);
	close(out_fd);
	return 0;
}

