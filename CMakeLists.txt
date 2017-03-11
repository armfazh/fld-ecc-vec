cmake_minimum_required(VERSION 2.8.4)
project(eddsa_avx2)

include_directories(./include)
link_directories(./lib)
set(EXECUTABLE_OUTPUT_PATH ./bin)
set(LIBRARY_OUTPUT_PATH ./lib)
set(TARGET eddsa_avx2)
set(SRC ./src)
enable_language(ASM-ATT)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -O3 -march=native -mtune=native -std=c99 -pedantic")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wall -Wextra -O3 -march=native -mtune=native -std=c99 -pedantic")

add_library(${TARGET} STATIC ${SRC}/${TARGET}.c )

add_executable(bench apps/bench/bench.c
	${SRC}/random.c
	apps/bench/clocks.c
)

add_executable(tests apps/tests/tests.c
	${SRC}/random.c
	apps/tests/vectors.h
)

add_executable(samples apps/samples/samples.c
	${SRC}/random.c
)

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

INSTALL(FILES include/${TARGET}.h
		DESTINATION "include"
)