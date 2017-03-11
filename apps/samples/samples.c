#include <eddsa_avx2.h>
#include <element_1w_h0h8.h>
#include <element_2w_h0h8.h>
#include <element_4w_h0h8.h>

int main()
{
	Ed448_PublicKey public;
	Ed448_PrivateKey private;
	Ed448_Signature signature;
	char mes[20]= "hello";

	EdDSA.Ed448.keygen(public, private);
	EdDSA.Ed448ph.keygen(public, private);

	EdDSA.Ed448.sign(signature,(uint8_t*)mes,5,EDDSA_NOCONTEXT,0,public,private);
	EdDSA.Ed448ph.sign(signature,(uint8_t*)mes,5,EDDSA_NOCONTEXT,0,public,private);

	EdDSA.Ed448.verify((uint8_t*)mes,5,EDDSA_NOCONTEXT,0,public,signature);
	EdDSA.Ed448ph.verify((uint8_t*)mes,5,EDDSA_NOCONTEXT,0,public,signature);

	ECDH_X448_KEY aliceSession,alicePrivate,bobSession,bobPrivate,aliceShared,bobShared;

	ECDHX.X448.keygen(aliceSession,alicePrivate);
	ECDHX.X448.keygen(bobSession,bobPrivate);
	ECDHX.X448.shared(aliceShared,bobSession,alicePrivate);
	ECDHX.X448.shared(bobShared,aliceSession,bobPrivate);

	Element_1w_H0H8 c,a,b;
	Element_2w_H0H8 C,A,B;
	Element_4w_H0H8 CC,AA,BB;
	Fp_Arith._1way.add(c,a,b);
	Fp_Arith._2way.mul(C,A,B);
	Fp_Arith._4way.mul(CC,AA,BB);
	return 0;
}
