cmake_minimum_required(VERSION 2.9)
set(TARGET faz_ecc_avx2)
set(CMAKE_BUILD_TYPE Release)
project(${TARGET} C)

set(EXECUTABLE_OUTPUT_PATH ../bin)
set(LIBRARY_OUTPUT_PATH ../lib)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Werror -Wall -Wextra -O3 -pedantic -std=c99 -m64 -mavx2 -mbmi2 -march=native -mtune=native ")
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

add_subdirectory(third_party)
add_subdirectory(src)
add_subdirectory(bench)
add_subdirectory(examples)
add_subdirectory(doc)
enable_testing()
add_subdirectory(tests)
