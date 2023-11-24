# Information cmake
cmake_minimum_required(VERSION 3.0.2)

# set variables

# If you only include this third party in PRIVATE source files, you do not need to install it
# when your main project gets installed.

# add subdirectory

# add executable and target libraries
add_executable(defender main.c)

target_include_directories (defender PRIVATE 
		../../include/
		../../packages/
		../../interfaces/
		../../libraries/uthash/include/
)

file(GLOB_RECURSE SOURCES
        ../scan/*.c
		../inotify/*.c
)

target_sources(defender PRIVATE 
		${SOURCES}
)


target_link_libraries(defender PRIVATE yara)