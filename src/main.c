#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <fcntl.h>

#include "input.h"
#include "operation.h"

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
    if (encrypting) {
        snprintf(out_filename, 256, "%s.enc", argv[1]);
    } else {
        snprintf(out_filename, 256, "%.*s.dec", (int)(strlen(argv[1]) - 4), argv[1]);
    }

    int out_fd = open(out_filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    free(out_filename);

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

    uint8_t *in_buf = calloc(64, sizeof(uint8_t));
    uint8_t keystream[64];
    ssize_t in_read;
    uint32_t counter = 0;

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

    free(in_buf);
    free(key);
    close(in_fd);
    close(out_fd);
    return 0;
}
