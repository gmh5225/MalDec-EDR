cmake_minimum_required(VERSION 3.0.2)

# Add executable and target libraries
add_executable(defender main.c)

set(FLAGS_OPTIMIZATIONS ON)
set(FLAGS_DEBUG OFF)

file(GLOB_RECURSE sources
    ../scan/*.c
    ../logger/*.c
    ../cjson/*.c
    ../telekinesis/*.c
    ../inotify/*.c
)

target_sources(defender PRIVATE ${sources})
target_link_libraries(defender PUBLIC yara c-logger json-c)

if(FLAGS_OPTIMIZATIONS)
    target_compile_options(defender PUBLIC
        -O3            # Enable high-level optimization
        -ftree-vectorize
        -mavx
        -fno-stack-protector
        -march=native
        -fdelete-null-pointer-checks
        -fdelete-dead-exceptions
    )
elseif(FLAGS_DEBUG)
    target_compile_options(defender PUBLIC
        -ggdb3
        -fms-extensions
        --all-warnings
        -DDEBUG
        -g
        -Warray-bounds
        -Werror
        --extra-warnings
    )
endif()