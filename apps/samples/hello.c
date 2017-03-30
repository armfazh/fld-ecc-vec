#include<faz_eddsa_avx2.h>
#include<stdio.h>

int main(void)
{
    printf("Size of an Ed25519 key: %d bytes.\n",
           ED25519_KEY_SIZE_BYTES_PARAM);
    return 0;
}
