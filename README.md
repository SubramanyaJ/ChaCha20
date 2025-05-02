# ChaCha20 Stream Cipher Utility

## Introduction

ChaCha20 is a secure, high-speed 256-bit stream cipher designed by Daniel J. Bernstein.  
It is a refinement of the Salsa20 cipher and is widely used in modern cryptography, including TLS 1.3.  
This project provides a simple CLI-based tool to encrypt and decrypt files using ChaCha20, based on the specification described in [RFC 7539](https://www.rfc-editor.org/rfc/rfc7539).

This implementation uses:

- 256-bit key (32 bytes)
- 96-bit nonce (12 bytes)
- 32-bit block counter
- 20-round core (ChaCha20 block function)

This tool is intended for educational, research, and light production purposes where manual control of encryption is desired.

---

## Compilation

This project is intended to be compiled and run on UNIX-based systems such as Linux or macOS.  
The nonce is generated using the `/dev/urandom` source of entropy, which may not be available or reliable on non-POSIX platforms like Windows.

### Dependencies

You will need the following tools installed:

- `git` – for cloning the repository
- `clang` – for compiling the C code (or `gcc`)
- `make` – for building using the provided Makefile

---

### For Debian/Ubuntu-based systems:

```sh
sudo apt update
sudo apt install git clang make
```

### For Arch Linux or derivatives:

```sh
sudo pacman -S git clang make
```

_(You might also consider getting a life.)_

---

## Building the Project

To compile the binary:

```sh
cd src
make
```

This will compile the source files and produce an executable binary named `chacha20`.

---

## Usage

Once compiled, you can use the tool to encrypt or decrypt files. Move it to ~/.local/bin or something.

### Syntax

```sh
./chacha20 <filename> <e|d>
```

- `<filename>`: Path to the file you wish to encrypt or decrypt
- `<e|d>`:
  - `e` to encrypt
  - `d` to decrypt

### Example

```sh
./chacha20 secret.txt e        # Encrypts secret.txt to secret.txt.enc
./chacha20 secret.txt.enc d   # Decrypts it back to secret.txt.dec
```

---

## Project Structure

- `src/` – Contains all C source files and headers
- `Makefile` – For building the project
- `README.md` – This file

---

## References

- [RFC 7539: ChaCha20 and Poly1305 for IETF Protocols](https://www.rfc-editor.org/rfc/rfc7539)
- [Daniel J. Bernstein's ChaCha page](https://cr.yp.to/chacha.html)

---

## Author

Maintained by **Subramanya J**  
<subramanyajaradhya@gmail.com>
