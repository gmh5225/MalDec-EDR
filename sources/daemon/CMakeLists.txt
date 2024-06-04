# Information cmake
cmake_minimum_required(VERSION 3.0.2)

# add executable and target libraries
add_executable(maldec-edr-daemon maldec-edr-daemon.c)

set(FLAGS_OPTIMIZATIONS ON)
set(FLAGS_DEBUG ON)

file(GLOB_RECURSE sources
    ./*.c
    sig-hdlr/*.c
    bus-ifc/*.c
    ../edr/*.c
    ../scanner/*.c
    ../logger/*.c
    ../cjson/*.c
    ../inotify/*.c
    ../inspector/*.c
    ../compression/*.c
    ../crowarmor/*.c
    ../memory/*.c
    ../modules/*.c
)

target_sources(maldec-edr-daemon PRIVATE ${sources})
target_link_libraries(maldec-edr-daemon PUBLIC yara c-logger json-c systemd zlib sqlite3)
target_include_directories(maldec-edr-daemon PRIVATE ${PROJECT_SOURCE_DIR}/libraries/uthash)

if(FLAGS_OPTIMIZATIONS)
    target_compile_options(maldec-edr-daemon PUBLIC
        -O3            # Enable high-level optimization
        -ftree-vectorize
        -mavx
        -fno-stack-protector
        -march=native
        -fdelete-null-pointer-checks
        -fdelete-dead-exceptions
    )
elseif(FLAGS_DEBUG)
    target_compile_options(maldec-edr-daemon PUBLIC
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
