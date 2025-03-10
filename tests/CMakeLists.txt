enable_language(CXX)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(ENABLE_AVX512)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mavx512f -mavx512bw")
    add_compile_definitions(ENABLED_AVX512)
endif(ENABLE_AVX512)

# Some Tests
add_executable(test_mine EXCLUDE_FROM_ALL
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

# Building GoogleTest
set(GOOGLETEST_VERSION 1.16.0)
include(FetchContent)
FetchContent_Declare(googletest-${GOOGLETEST_VERSION}
    EXCLUDE_FROM_ALL true
    URL https://github.com/google/googletest/releases/download/v${GOOGLETEST_VERSION}/googletest-${GOOGLETEST_VERSION}.tar.gz
    URL_HASH SHA256=78c676fc63881529bf97bf9d45948d905a66833fbfa5318ea2cd7478cb98f399
    DOWNLOAD_EXTRACT_TIMESTAMP true
)
FetchContent_MakeAvailable(googletest-${GOOGLETEST_VERSION})

# Locate GMP
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}")
find_package(GMP QUIET)

add_executable(unit_tests
    EXCLUDE_FROM_ALL
    runTests.cpp
    test_fp25519_x64.cpp
    test_fp25519_avx2.cpp
    test_fp448_x64.cpp
    vectors.h
    vectors.c
    test_x25519.cpp
    test_ed25519.cpp
    test_hash255.cpp
)
add_dependencies(unit_tests ${TARGET}_static cpu_caps)
target_link_libraries(unit_tests
    $<TARGET_OBJECTS:cpu_caps>
    $<TARGET_OBJECTS:prng>
    GTest::gtest_main gmp
    ${TARGET}_static)

include(GoogleTest)
gtest_discover_tests(unit_tests)
