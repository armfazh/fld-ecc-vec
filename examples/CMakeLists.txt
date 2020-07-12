cmake_minimum_required(VERSION 2.8.4)

include_directories(../include)

add_executable(example_field field.c)
add_dependencies(example_field ${TARGET}_static)
target_link_libraries(example_field ${TARGET}_static)

add_executable(example_ecdh ecdh.c)
add_dependencies(example_ecdh ${TARGET}_static)
target_link_libraries(example_ecdh ${TARGET}_static)

add_executable(example_eddsa eddsa.c)
add_dependencies(example_eddsa ${TARGET}_static)
target_link_libraries(example_eddsa ${TARGET}_static)

add_executable(example_hash hash.c)
add_dependencies(example_hash ${TARGET}_static)
target_link_libraries(example_hash ${TARGET}_static)
