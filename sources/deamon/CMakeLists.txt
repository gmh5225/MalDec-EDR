# Information cmake
cmake_minimum_required(VERSION 3.0.2)

# add executable and target libraries
add_executable(defenderd main.c)

target_include_directories (defenderd PRIVATE 
    ../../include/
    ../../packages/
    ../../interfaces/
    ../../libraries/uthash/include/
    ../../libraries/c-logger/src
)

file(GLOB_RECURSE SOURCES
        ../scan/*.c
		../inotify/*.c
)

target_sources(defenderd PRIVATE 
		${SOURCES}
)


target_link_libraries(defenderd PRIVATE yara)