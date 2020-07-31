enable_language(C)
include_directories(.)

add_library(prng OBJECT prng.c prng.h)
add_library(hash OBJECT sha512.c sha512.h sha3.c sha3.h sha512_2way.c multi_hash.h)
add_library(cpu_caps OBJECT cpu_caps.c cpu_caps.h cpu_caps.h)
