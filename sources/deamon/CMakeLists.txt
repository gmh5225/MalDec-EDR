# Information cmake
cmake_minimum_required(VERSION 3.0.2)

# add executable and target libraries
add_executable(maldec-edrd main.c)

set(FLAGS_OPTIMIZATIONS ON)
set(FLAGS_DEBUG OFF)

target_link_libraries(maldec-edrd PUBLIC yara c-logger)

target_sources(maldec-edrd PRIVATE ${sources})
target_link_libraries(maldec-edrd PUBLIC yara c-logger json-c)

if(FLAGS_OPTIMIZATIONS)
    target_compile_options(maldec-edrd PUBLIC
        -O3            # Enable high-level optimization
        -ftree-vectorize
        -mavx
        -fno-stack-protector
        -march=native
        -fdelete-null-pointer-checks
        -fdelete-dead-exceptions
    )
elseif(FLAGS_DEBUG)
    target_compile_options(maldec-edrd PUBLIC
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