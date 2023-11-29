# Information cmake
cmake_minimum_required(VERSION 3.0.2)

# add subdirectory
add_subdirectory(../../libraries/cmocka build/cmocka)

add_definitions(-DUNIT_TESTS)
set(PROJECT_SOURCES
	YaraTest.c
    ../../sources/scan/scan.c
	../../sources/scan/scanner.c
	../../sources/scan/skip_dirs.c
	../../sources/logger/logger.c
)

add_executable(yara_test ${PROJECT_SOURCES})

target_include_directories (yara_test PRIVATE 
		../../include/
		../../packages/
		../../libraries/cmocka/include
		../../libraries/uthash/include
		../../libraries/c-logger/
)

target_link_libraries(yara_test cmocka yara "logger")