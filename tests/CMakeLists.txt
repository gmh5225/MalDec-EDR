# Information cmake
cmake_minimum_required(VERSION 3.0.2)

# add subdirectory
add_subdirectory(../libraries/cmocka bin/cmocka)

set(tests
    yara_test
    yara_scanner_test
	json_parser_config_test
)

include_directories(
		../include/
		../packages/
		../libraries/
		../sources
)

file(GLOB_RECURSE sources
	../sources/cjson/*.c
    ../sources/scan/*.c
    ../sources/logger/*.c
	../sources/inotify/*.c
)

foreach(test IN LISTS tests)
	add_executable(${test} ${test}.c)
	target_link_libraries(${test} PUBLIC cmocka yara c-logger json-c)
	target_sources(${test} PRIVATE ${sources})
	add_test(NAME ${test}
			COMMAND ${test}
			WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/tests)
endforeach()