# Information cmake
cmake_minimum_required(VERSION 3.0.2)

# set variables

# If you only include this third party in PRIVATE source files, you do not need to install it
# when your main project gets installed.

# add subdirectory

# add executable and target libraries
add_executable(kagedefend main.c)

target_include_directories (kagedefend PRIVATE 
		../include/
		../packages/
		../interfaces
)

file(GLOB_RECURSE SOURCES
        *.c
)

target_sources(kagedefend PRIVATE 
		${SOURCES}
)


target_link_libraries(kagedefend PRIVATE )
