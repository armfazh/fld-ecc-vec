include_directories(.)

add_library(prng OBJECT prng.c prng.h)
add_library(sha512 OBJECT sha512.c sha512.h sha512_2way.c multi_hash.h)
add_library(cpu_caps OBJECT cpu_caps.c cpu_caps.h cpu_caps.h)

add_subdirectory(xkcp)