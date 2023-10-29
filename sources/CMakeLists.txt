# Information cmake
cmake_minimum_required(VERSION 3.0.2)

# set variables

# If you only include this third party in PRIVATE source files, you do not need to install it
# when your main project gets installed.

# add subdirectory

# add executable and target libraries
add_executable(kagedefender main.c)

target_include_directories (kagedefender PRIVATE 
		../include/
		../packages/
		../interfaces
)

file(GLOB_RECURSE SOURCES
        *.c
)

target_sources(kagedefender PRIVATE 
		${SOURCES}
)


target_link_libraries(kagedefender PRIVATE yara)
