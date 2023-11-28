# Information cmake
cmake_minimum_required(VERSION 3.0.2)

# add executable and target libraries
add_executable(defender main.c)

target_include_directories (defender PRIVATE 
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

target_sources(defender PRIVATE 
		${SOURCES}
)

target_link_libraries(defender PRIVATE yara "logger")