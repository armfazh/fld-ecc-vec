#include <eddsa_avx2.h>
#include <fp.h>

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

	argElement_1w a1 = Fp.fp448._1way.new();
	argElement_1w b1 = Fp.fp448._1way.new();
	argElement_1w c1 = Fp.fp448._1way.new();

	argElement_2w a2 = Fp.fp448._2way.new();
	argElement_2w b2 = Fp.fp448._2way.new();
	argElement_2w c2 = Fp.fp448._2way.new();

	argElement_4w a4 = Fp.fp448._4way.new();
	argElement_4w b4 = Fp.fp448._4way.new();
	argElement_4w c4 = Fp.fp448._4way.new();

	Fp.fp448._1way.mul(c1,a1,b1);
	Fp.fp448._2way.mul(c2,a2,b2);
	Fp.fp448._4way.mul(c4,a4,b4);

	Fp.fp448._1way.add(c1,a1,b1);

	Fp.fp448._1way.clean(a1);	Fp.fp448._1way.clean(b1);	Fp.fp448._1way.clean(c1);
	Fp.fp448._2way.clean(a2);	Fp.fp448._2way.clean(b2);	Fp.fp448._2way.clean(c2);
	Fp.fp448._4way.clean(a4);	Fp.fp448._4way.clean(b4);	Fp.fp448._4way.clean(c4);

	return 0;
}
