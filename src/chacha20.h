/**
 * @file chacha20.h
 * @brief Header file for ChaCha20 stream cipher implementation.
 *
 * @author Subramanya J
 * subramanyajaradhya@gmail.com
 *
 * This file provides the function declarations and macros needed
 * to use the ChaCha20 encryption block function, along with helper
 * routines for key and nonce handling.
 */

#ifndef CHACHA_20_H
#define CHACHA_20_H

#include <stdint.h>
#include <stdio.h>
#include <stdint.h>

/**
 * @brief Prompts the user for a password and returns a 32-byte key.
 *
 * The password entered by the user is truncated or zero-padded to 32 bytes.
 *
 * @return Pointer to a heap-allocated 32-byte buffer containing the key.
 *         Must be freed by the caller.
 */
uint8_t *
get_key();

/**
 * @brief Generates a 12-byte nonce using /dev/urandom or fallback to rand().
 *
 * Tries to read from `/dev/urandom`; falls back to `rand()` if unavailable.
 *
 * @return Pointer to a heap-allocated 12-byte buffer containing the nonce.
 *         Must be freed by the caller.
 */
uint8_t *
get_nonce();

/**
 * @brief Loads a 32-bit unsigned integer from a little-endian byte array.
 *
 * @param src Pointer to 4 bytes in memory in little-endian order.
 * @return 32-bit unsigned integer.
 */
uint32_t
load32_le(const uint8_t *src);

/**
 * @brief Stores a 32-bit unsigned integer into a byte array in little-endian format.
 *
 * @param dst Destination pointer to a 4-byte memory location.
 * @param w 32-bit unsigned integer to store.
 */
void
store32_le(uint8_t *dst, uint32_t w);

/**
 * @brief Rotates a 32-bit unsigned integer `v` left by `n` bits.
 */
#define ROTL32(v, n) ((v << n) | (v >> (32 - n)))


/**
 * @brief Applies the ChaCha20 quarter round function to four 32-bit words.
 *
 * This function performs 4 rounds of ARX (Add-Rotate-XOR) operations on the inputs.
 * b ^= (a+d) <<< 7;
 * c ^= (b+a) <<< 9;
 * d ^= (c+b) <<< 13;
 * a ^= (d+c) <<< 18;
 */
void quarterround(uint32_t *a, uint32_t *b, uint32_t *c, uint32_t *d);

/**
 * @brief Executes the ChaCha20 block function on a given 512-bit state.
 *
 * The output is 64 bytes (512 bits) of keystream, generated from 10 double-rounds.
 *
 * @param state Pointer to the 16-word (64-byte) input state.
 * @param output Pointer to a 64-byte buffer where output will be stored.
 */
void chacha20_block(uint32_t state[16], uint8_t output[64]);

#endif /* chacha20.h included */
