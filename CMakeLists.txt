cmake_minimum_required(VERSION 2.8.4)
set(TARGET faz_crypto_avx2)
set(CMAKE_BUILD_TYPE Release)
project(${TARGET} C)
enable_language(ASM-ATT)

include_directories(./include)
link_directories(./lib)
set(EXECUTABLE_OUTPUT_PATH ./bin)
set(LIBRARY_OUTPUT_PATH ./lib)
set(SRC ./src)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC -Wall -Wextra -O3 -std=c99 -pedantic -mavx2 -march=native -mtune=native")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wall -Wextra -O3 -std=c99 -pedantic -mavx2 -march=native -mtune=native")

add_library(${TARGET}_shared SHARED ${SRC}/${TARGET}.c)
add_library(${TARGET}_static STATIC ${SRC}/${TARGET}.c)

add_executable(bench       apps/bench/bench.c apps/bench/clocks.c)
add_executable(tests       apps/tests/tests.c apps/tests/vectors.h)
add_executable(hello       apps/samples/hello.c)
add_executable(cpu_id      apps/samples/cpu_id.c)
add_executable(field_arith apps/samples/field_arith.c)
add_executable(ecdh        apps/samples/ecdh.c)
add_executable(eddsa       apps/samples/eddsa.c)

target_link_libraries(bench       ${TARGET}_static)
target_link_libraries(tests       ${TARGET}_static)
target_link_libraries(field_arith ${TARGET}_static)
target_link_libraries(hello	      ${TARGET}_static)
target_link_libraries(cpu_id      ${TARGET}_static)
target_link_libraries(ecdh        ${TARGET}_static)
target_link_libraries(eddsa       ${TARGET}_static)

INSTALL(TARGETS ${TARGET}_static ${TARGET}_shared DESTINATION "lib")
INSTALL(FILES
		include/faz_eddsa_avx2.h
		include/faz_ecdh_avx2.h
		include/faz_fp_avx2.h
		DESTINATION "include"
)

