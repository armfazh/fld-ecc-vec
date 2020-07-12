cmake_minimum_required(VERSION 2.8.4)

include_directories(../include)
include_directories(../third_party)

add_executable(bench
        clocks.h
        clocks.c
        bench_fp.c
        bench_third_party.c
        bench_hash.c
        bench_ecdh.c
        bench_eddsa.c
        bench.c
        bench.h
        )

add_dependencies(bench ${TARGET}_static cpu_caps)
target_link_libraries(bench
        ${TARGET}_static
        $<TARGET_OBJECTS:cpu_caps>
        )
