cmake_minimum_required(VERSION 3.13.5)
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

include_directories(./include)
include_directories(./third_party)

add_library(${TARGET}_shared SHARED
    ./src/target.c
    $<TARGET_OBJECTS:prng>
    $<TARGET_OBJECTS:hash>
)

add_library(${TARGET}_static STATIC
    ./src/target.c
    $<TARGET_OBJECTS:prng>
    $<TARGET_OBJECTS:hash>
)

add_dependencies(${TARGET}_static prng hash)

target_include_directories(${TARGET}_static INTERFACE ./include)
target_include_directories(${TARGET}_shared INTERFACE ./include)

set_target_properties(${TARGET}_static PROPERTIES OUTPUT_NAME ${TARGET} CLEAN_DIRECT_OUTPUT 1)
set_target_properties(${TARGET}_shared PROPERTIES OUTPUT_NAME ${TARGET} CLEAN_DIRECT_OUTPUT 1)

install(TARGETS ${TARGET}_shared DESTINATION "lib")
install(TARGETS ${TARGET}_static DESTINATION "lib")
install(FILES
    ./include/faz_fp_avx2.h
    ./include/faz_ecdh_avx2.h
    ./include/faz_eddsa_avx2.h
    ./include/faz_hash_avx2.h
    DESTINATION "include"
)

add_subdirectory(bench)
add_subdirectory(examples)
add_subdirectory(doc)
enable_testing()
add_subdirectory(tests)
