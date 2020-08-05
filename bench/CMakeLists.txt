cmake_minimum_required(VERSION 2.8.4)

# Other Tests using GTest suite.
enable_language(CXX)
add_definitions(-mavx2)
set(CMAKE_CXX_STANDARD 11)

include(ExternalProject)
include("GNUInstallDirs")
set(GBENCH_VERSION "v1.5.1")
ExternalProject_Add(benchmark-download
  URL            "https://api.github.com/repos/google/benchmark/tarball/${GBENCH_VERSION}"
  URL_HASH       "SHA256=440ded04a56a63043d9112da1b787be550f59d0df3c13eb0f9d40e6b3e805cc6"
  CMAKE_ARGS     -DCMAKE_INSTALL_PREFIX=${CMAKE_CURRENT_BINARY_DIR}
                 -DCMAKE_CXX_COMPILER:FILEPATH=${CMAKE_CXX_COMPILER}
                 -DCMAKE_POSITION_INDEPENDENT_CODE=ON
                 -DCMAKE_BUILD_TYPE=Release
                 -DBENCHMARK_ENABLE_TESTING=OFF
                 -DBENCHMARK_ENABLE_GTEST_TESTS=OFF
  TEST_COMMAND   ""
  UPDATE_COMMAND ""
)
set_target_properties(benchmark-download PROPERTIES EXCLUDE_FROM_ALL TRUE)
include_directories(${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_INSTALL_INCLUDEDIR})
link_directories(${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR})

include_directories(../include)
include_directories(../third_party)

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
