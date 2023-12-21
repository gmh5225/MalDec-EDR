# Information cmake
cmake_minimum_required(VERSION 3.0.2)

# add executable and target libraries
add_executable(defenderd main.c)

target_link_libraries(defenderd PUBLIC yara c-logger)