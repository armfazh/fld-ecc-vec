include_directories(.)

add_library(fips202-opt64 OBJECT
    KeccakHash.c
    KeccakP-1600-opt64.c
    KeccakSponge.c
    SimpleFIPS202.c
)