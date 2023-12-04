# Information cmake
cmake_minimum_required(VERSION 3.0.2)

# add subdirectory
add_subdirectory(../libraries/cmocka bin/cmocka)

set(tests
    yara_test
    yara_scanner_test
)

include_directories(
		../include/
		../packages/
		../libraries/
)

file(GLOB_RECURSE sources
    ../sources/scan/*.c
    ../sources/logger/*.c
)

foreach(test IN LISTS tests)
	add_executable(${test} ${test}.c)
	target_link_libraries(${test} PUBLIC cmocka yara logger)
	target_sources(${test} PRIVATE ${sources})
	add_test(NAME ${test}
			COMMAND ${test}
			WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/tests)
endforeach()