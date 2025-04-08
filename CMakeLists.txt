# ChaCha20/CMakeLists.txt

cmake_minimum_required(VERSION 3.10)
project(ChaCha20 C)

# cflags
add_compile_options(-Wall -Werror -Wextra -O3)

# src directory
add_subdirectory(src)
