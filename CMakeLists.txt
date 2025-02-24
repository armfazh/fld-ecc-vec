cmake_minimum_required(VERSION 3.28)
set(TARGET faz_ecc_avx2)
project(${TARGET} C)

set(CMAKE_BUILD_TYPE Release)
set(CMAKE_C_STANDARD 99)

set(EXECUTABLE_OUTPUT_PATH ../bin)
set(LIBRARY_OUTPUT_PATH ../lib)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Werror -Wall -Wextra -O3 -pedantic -std=c99 -m64 -mavx2 -mbmi2 -march=native -mtune=native")

option(ENABLE_AVX512 "Enables AVX512 code" OFF)
if(ENABLE_AVX512)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -mavx512f -mavx512bw")
    add_compile_definitions(ENABLED_AVX512)
endif(ENABLE_AVX512)

add_subdirectory(third_party)
add_subdirectory(src)
add_subdirectory(bench)
add_subdirectory(examples)
add_subdirectory(doc)
enable_testing()
add_subdirectory(tests)
