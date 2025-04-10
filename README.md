# ChaCha20

A 256-bit stream cipher based on the 8-round
cipher Salsa20/8, by Daniel J. Bernstein.

---

## Compilation

Preferably, use a UNIX based system, as the code
uses /dev/urandom for nonce generation.

These packages are required for cloning and compilation : `git gcc make cmake`

If you are using Debian based distributions :
```shell
sudo apt install git gcc make cmake
```

If you are using Arch Linux based distributions :
```shell
sudo pacman -S git gcc make cmake
```
You might also consider getting a life.

---

To compile the binary, run : 

```shell
cmake -B bin
cmake --build bin
```

The binary will be generated in the bin/ directory.

---

TL;DR :

```shell
cd ~
sudo apt install git gcc make cmake
git clone https://github.com/SubramanyaJ/ChaCha20.git
cd ChaCha20
cmake -B bin
cmake --build bin
```
