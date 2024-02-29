cmake_minimum_required(VERSION 3.0.2)

# Add executable and target libraries
add_executable(defender-gui main.c)

set(FLAGS_OPTIMIZATIONS ON)
set(FLAGS_DEBUG OFF)

file(GLOB_RECURSE sources
    ../defender/*.c
    ../scanner/*.c
    ../logger/*.c
    ../cjson/*.c
    ../telekinesis/*.c
    ../inotify/*.c
    ../inspector/*.c
    ../compression/*.c
)

# Encontre o pacote GTK4
find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK4 REQUIRED gtk4)

# Inclua os diretórios de inclusão do GTK4
include_directories(${GTK4_INCLUDE_DIRS})

target_sources(defender-gui PRIVATE ${sources})

# Include GLib directories
include_directories(${GLIB_INCLUDE_DIRS})

target_link_libraries(defender-gui PUBLIC ${GTK4_LIBRARIES} yara c-logger json-c sqlite3 zlib)

if(FLAGS_OPTIMIZATIONS)
    target_compile_options(defender-gui PUBLIC
        -O2           
        -ftree-vectorize
        -mavx
        -fno-stack-protector
        -march=native
        -fdelete-null-pointer-checks
        -fdelete-dead-exceptions
    )
elseif(FLAGS_DEBUG)
    target_compile_options(defender-gui PUBLIC
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