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


uint32_t
load32_le(const uint8_t *src);

void
store32_le(uint8_t *dst, uint32_t w);

#endif /* input.h included */
