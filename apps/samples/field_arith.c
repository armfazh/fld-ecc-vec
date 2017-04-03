#include<faz_fp_avx2.h>
#include<stdio.h>

void identity(const struct _struct_Fp_1way * fp)
{
    argElement_1w a      = fp->init();    argElement_1w b      = fp->init();
    argElement_1w add_ab = fp->init();    argElement_1w sub_ab = fp->init();
    argElement_1w left   = fp->init();    argElement_1w right  = fp->init();
    argElement_1w prime  = fp->get_modulus();

    printf("Testing the identity: (a+b)*(a-b) = (a^2-b^2) \n");
    printf("Prime: "); fp->print(prime);

    fp->rand(a); printf("a: "); fp->print(a);
    fp->rand(b); printf("b: "); fp->print(b);

    fp->add(add_ab,a,b);          fp->cred(add_ab);
    fp->sub(sub_ab,a,b);          fp->cred(sub_ab);
    fp->mul(left,add_ab,sub_ab);  fp->cred(left);
    printf("left: ");             fp->print(left);

    fp->sqr(a);                   fp->cred(a);
    fp->sqr(b);                   fp->cred(b);
    fp->sub(right,a,b);           fp->cred(right);
    printf("right: ");            fp->print(right);

    if( fp->cmp(left,right) == 0 )
    {
        printf("Test passed.\n\n");
    }
    else
    {
        printf("Test failed.\n\n");
    }
    fp->clear(a     );    fp->clear(b     );
    fp->clear(add_ab);    fp->clear(sub_ab);
    fp->clear(left  );    fp->clear(right );
    fp->clear(prime );
}

int main(void)
{
    identity(&Fp.fp25519._1way_x64);
    identity(&Fp.fp25519._1way);
    identity(&Fp.fp448._1way);
    return 0;
}


