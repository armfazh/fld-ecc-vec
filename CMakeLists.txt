cmake_minimum_required(VERSION 2.8.4)
project(faz_eddsa_avx2 C)

include_directories(./include)
link_directories(./lib)
set(EXECUTABLE_OUTPUT_PATH ./bin)
set(LIBRARY_OUTPUT_PATH ./lib)
set(TARGET faz_crypto_avx2)
set(SRC ./src)
enable_language(ASM-ATT)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -O3 -std=c99 -pedantic -mavx2 -march=native -mtune=native")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wall -Wextra -O3 -std=c99 -pedantic -mavx2 -march=native -mtune=native")

add_library(${TARGET} STATIC ${SRC}/${TARGET}.c)

add_executable(bench   apps/bench/bench.c apps/bench/clocks.c )
add_executable(tests   apps/tests/tests.c apps/tests/vectors.h )
add_executable(samples apps/samples/samples.c )
add_executable(cpu_id  apps/samples/cpu_id.c )

target_link_libraries(
        bench
        ${TARGET}
)

target_link_libraries(
		tests
		${TARGET}
)

target_link_libraries(
		samples
		${TARGET}
)

INSTALL(TARGETS ${TARGET}
		DESTINATION "lib"
)

INSTALL(FILES include/faz_eddsa_avx2.h include/faz_ecdh_avx2.h include/faz_fp_avx2.h
		DESTINATION "include"
)
