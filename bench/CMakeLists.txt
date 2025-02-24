# Other Tests using GTest suite.
enable_language(CXX)
set(CMAKE_CXX_STANDARD 11)

include(ExternalProject)
include("GNUInstallDirs")
ExternalProject_Add(benchmark-download
    GIT_REPOSITORY "https://github.com/google/benchmark"
    GIT_TAG "v1.8.5"
    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${CMAKE_CURRENT_BINARY_DIR}
               -DCMAKE_CXX_COMPILER:FILEPATH=${CMAKE_CXX_COMPILER}
               -DCMAKE_POSITION_INDEPENDENT_CODE=ON
               -DCMAKE_BUILD_TYPE=Release
               -DBENCHMARK_ENABLE_TESTING=OFF
               -DBENCHMARK_ENABLE_GTEST_TESTS=OFF
    TEST_COMMAND ""
    UPDATE_COMMAND ""
)
set_target_properties(benchmark-download PROPERTIES EXCLUDE_FROM_ALL TRUE)
include_directories(${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_INSTALL_INCLUDEDIR})
link_directories(${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR})

add_executable(bench EXCLUDE_FROM_ALL
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

add_executable(gbench EXCLUDE_FROM_ALL gbench.cpp)
add_dependencies(gbench ${TARGET}_static prng benchmark-download)
target_link_libraries(gbench
    $<TARGET_OBJECTS:prng>
    benchmark pthread
    ${TARGET}_static
)
