# add subdirectory libraries
add_subdirectory(../libraries/elfo libraries/elfo)
add_subdirectory(../libraries/c-logger libraries/c-logger)
add_subdirectory(../libraries/json-c libraries/json-c)
add_subdirectory(../libraries/zlib libraries/zlib)

# add subdirectory compiler
cmake_minimum_required(VERSION 3.5)

set(compiler_gui FALSE)

include_directories(
    ../include
    ../libraries
    ../packages
    ../sources
    ../drivers/crowarmor/include
)

add_subdirectory(daemon)
add_subdirectory(cli)

if(compiler_gui)
    add_subdirectory(gui)
endif()
