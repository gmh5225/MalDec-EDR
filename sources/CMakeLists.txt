# add subdirectory libraries
add_subdirectory(../libraries/elfo libraries/elfo)
add_subdirectory(../libraries/c-logger libraries/c-logger)

# add subdirectory compiler
add_subdirectory(deamon/)
cmake_minimum_required(VERSION 3.5)

include_directories(
    ../include
    ../libraries
    ../packages
)

add_subdirectory(cli)