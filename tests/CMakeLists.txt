cmake_minimum_required(VERSION 2.8.4)

include_directories(../include)
include_directories(../third_party)

# Some Tests
add_executable(test_mine
        tests.h
        tests.c
        test_fp.c
        test_ecdh.c
        vectors.h
        vectors.c
        test_eddsa.c
        )
add_dependencies(test_mine ${TARGET}_static cpu_caps)
target_link_libraries(test_mine
        $<TARGET_OBJECTS:cpu_caps>
        ${TARGET}_static
        )

# Other Tests using GTest suite.
enable_language(CXX)
add_definitions(-mavx2)
set(CMAKE_CXX_STANDARD 11)

# Locate GMP
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}")
find_package(GMP QUIET)
# Locate GTest
find_package(GTest QUIET)
link_directories(${GTEST_LIBRARY_DIRS})

if (GTest_FOUND)
    include_directories(${GTEST_INCLUDE_DIRS})
    add_executable(tests
        runTests.cpp
        test_fp25519_x64.cpp
        test_fp25519_avx2.cpp
        test_fp448_x64.cpp
        vectors.h
        vectors.c
        test_x25519.cpp
        test_ed25519.cpp
    )
    add_dependencies(tests ${TARGET}_static cpu_caps)
    target_link_libraries(tests
        $<TARGET_OBJECTS:cpu_caps>
        ${GTEST_LIBRARIES}
        gmp
        ${TARGET}_static)
    add_test(NAME test_lib COMMAND tests)
endif()

