enable_language(C)

include_directories(../include)
include_directories(../third_party)

add_library(${TARGET}_shared SHARED
        target.c
#        f.c
        $<TARGET_OBJECTS:prng>
        $<TARGET_OBJECTS:hash>
        )

add_library(${TARGET}_static STATIC
        target.c
#                f.c
        $<TARGET_OBJECTS:prng>
        $<TARGET_OBJECTS:hash>
        )

add_dependencies(${TARGET}_static prng hash)

set_target_properties(${TARGET}_static PROPERTIES OUTPUT_NAME ${TARGET} CLEAN_DIRECT_OUTPUT 1)
set_target_properties(${TARGET}_shared PROPERTIES OUTPUT_NAME ${TARGET} CLEAN_DIRECT_OUTPUT 1)

install(TARGETS ${TARGET}_shared DESTINATION "lib")
install(TARGETS ${TARGET}_static DESTINATION "lib")
install(FILES
        ../include/faz_fp_avx2.h
        ../include/faz_ecdh_avx2.h
        ../include/faz_eddsa_avx2.h
        ../include/faz_hash_avx2.h
        DESTINATION "include"
        )
