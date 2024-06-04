cmake_minimum_required(VERSION 3.0.2)

# Add executable and target libraries
add_executable(maldec-edr-cli maldec-edr-cli.c)

set(FLAGS_OPTIMIZATIONS OFF)
set(FLAGS_DEBUG ON)

target_sources(maldec-edr-cli PRIVATE)
target_link_libraries(maldec-edr-cli PUBLIC systemd zlib)
target_include_directories(maldec-edr-cli PRIVATE ${PROJECT_SOURCE_DIR}/libraries/uthash)

if(FLAGS_OPTIMIZATIONS)
    target_compile_options(maldec-edr-cli PUBLIC
        -O2           
        -ftree-vectorize
        -mavx
        -fno-stack-protector
        -march=native
        -fdelete-null-pointer-checks
        -fdelete-dead-exceptions
    )
elseif(FLAGS_DEBUG)
    target_compile_options(maldec-edr-cli PUBLIC
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