# src/CMakeLists.txt

# executable target, with sources
add_executable(chacha20 main.c input.c operation.c)

# set output directory
set_target_properties(chacha20 PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin"
)
