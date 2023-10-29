# Information cmake
cmake_minimum_required(VERSION 3.0.2)

# add subdirectory
add_subdirectory(../libraries/cmocka build/cmocka)

add_definitions(-DUNIT_TESTS)
set(PROJECT_SOURCES
	Tests.c
)

add_executable(kagetest ${PROJECT_SOURCES})

target_include_directories (kagetest PRIVATE 
		../include/
		../packages/
		../libraries/cmocka/include
)


target_link_libraries(kagetest cmocka)