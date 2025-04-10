#ifndef INPUT_H
#define INPUT_H

#include <stdint.h>
#include <stdio.h>

/*
 * Returns a pointer to the buffer containing 
 * the user's password
 */

uint8_t *
get_key();

/**
 * Generates 12 random 8 bit integers and returns
 * a pointer to the array holding them
 */

uint8_t *
get_nonce();

#endif /* input.h included */
