# Information cmake
cmake_minimum_required(VERSION 3.0.2)

# add executable and target libraries
add_executable(defender main.c)

file(GLOB_RECURSE sources
    ../scan/*.c
    ../logger/*.c
)

target_sources(defender PRIVATE ${sources})
target_link_libraries(defender PUBLIC yara logger)