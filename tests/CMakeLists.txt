include_directories(../include)
include_directories(../third_party)

# Other Tests using GTest suite.
enable_language(CXX)
add_definitions(-mavx2)
set(CMAKE_CXX_STANDARD 14)

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

# download gtest
include(ExternalProject)
include("GNUInstallDirs")
ExternalProject_Add(googletest-download
  GIT_REPOSITORY "https://github.com/google/googletest"
  GIT_TAG        "v1.15.2"
  CMAKE_ARGS     -DCMAKE_INSTALL_PREFIX=${CMAKE_CURRENT_BINARY_DIR}
                 -DCMAKE_POSITION_INDEPENDENT_CODE=ON
                 -DCMAKE_CXX_COMPILER:FILEPATH=${CMAKE_CXX_COMPILER}
  TEST_COMMAND   ""
  UPDATE_COMMAND ""
)
set_target_properties(googletest-download PROPERTIES EXCLUDE_FROM_ALL TRUE)
include_directories(${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_INSTALL_INCLUDEDIR})
link_directories(${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR})

# Locate GMP
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}")
find_package(GMP QUIET)

include_directories(${GTEST_INCLUDE_DIRS})
add_executable(tests EXCLUDE_FROM_ALL
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
add_dependencies(tests ${TARGET}_static cpu_caps googletest-download)
target_link_libraries(tests
    $<TARGET_OBJECTS:cpu_caps>
    $<TARGET_OBJECTS:prng>
    gtest pthread gmp
    ${TARGET}_static)
add_test(NAME test_lib COMMAND tests)
