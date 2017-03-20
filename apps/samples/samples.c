#include <faz_eddsa_avx2.h>
#include <faz_ecdh_avx2.h>
#include <fp.h>
#include <random.h>
#include <stdio.h>
#include <string.h>


void x25519()
{
	const DiffieHellmanXFunction *ecdh = &ECDHX.X25519;

	argECDHX_Key alice_private_key = ecdh->initKey();
	argECDHX_Key alice_session_key = ecdh->initKey();
	argECDHX_Key alice_shared_key  = ecdh->initKey();
	argECDHX_Key bob_private_key   = ecdh->initKey();
	argECDHX_Key bob_session_key   = ecdh->initKey();
	argECDHX_Key bob_shared_key    = ecdh->initKey();

	random_bytes(alice_private_key, ecdh->key_size);
	random_bytes(bob_private_key, ecdh->key_size);

	ecdh->keygen(alice_session_key, alice_private_key);
	ecdh->keygen(bob_session_key, bob_private_key);

	ecdh->shared(alice_shared_key, bob_session_key, alice_private_key);
	ecdh->shared(bob_shared_key, alice_session_key, bob_private_key);
//	memcmp(alice_shared_key, bob_shared_key, ecdh->key_size);

	ecdh->clearKey(alice_private_key);
	ecdh->clearKey(alice_session_key);
	ecdh->clearKey(alice_shared_key);
	ecdh->clearKey(bob_private_key);
	ecdh->clearKey(bob_session_key);
	ecdh->clearKey(bob_shared_key);

}

void x448()
{
	const DiffieHellmanXFunction *ecdh = &ECDHX.X448;

	argECDHX_Key alice_private_key = ecdh->initKey();
	argECDHX_Key alice_session_key = ecdh->initKey();
	argECDHX_Key alice_shared_key  = ecdh->initKey();
	argECDHX_Key bob_private_key   = ecdh->initKey();
	argECDHX_Key bob_session_key   = ecdh->initKey();
	argECDHX_Key bob_shared_key    = ecdh->initKey();

	random_bytes(alice_private_key, ecdh->key_size);
	random_bytes(bob_private_key, ecdh->key_size);

	ecdh->keygen(alice_session_key, alice_private_key);
	ecdh->keygen(bob_session_key, bob_private_key);

	ecdh->shared(alice_shared_key, bob_session_key, alice_private_key);
	ecdh->shared(bob_shared_key, alice_session_key, bob_private_key);
//	memcmp(alice_shared_key, bob_shared_key, ecdh->key_size);

	ecdh->clearKey(alice_private_key);
	ecdh->clearKey(alice_session_key);
	ecdh->clearKey(alice_shared_key);
	ecdh->clearKey(bob_private_key);
	ecdh->clearKey(bob_session_key);
	ecdh->clearKey(bob_shared_key);

}

void ed448()
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
}

void ed25519()
{
	Ed25519_PublicKey public;
	Ed25519_PrivateKey private;
	Ed25519_Signature signature;
	char mes[20]= "hello";

	EdDSA.Ed25519.keygen(public, private);
	EdDSA.Ed25519ph.keygen(public, private);

	EdDSA.Ed25519.sign(signature,(uint8_t*)mes,5,public,private);
	EdDSA.Ed25519ph.sign(signature,(uint8_t*)mes,5,EDDSA_NOCONTEXT,0,public,private);

	EdDSA.Ed25519.verify((uint8_t*)mes,5,public,signature);
	EdDSA.Ed25519ph.verify((uint8_t*)mes,5,EDDSA_NOCONTEXT,0,public,signature);

	ECDH_X25519_KEY aliceSession,alicePrivate,bobSession,bobPrivate,aliceShared,bobShared;

	ECDHX.X25519.keygen(aliceSession,alicePrivate);
	ECDHX.X25519.keygen(bobSession,bobPrivate);
	ECDHX.X25519.shared(aliceShared,bobSession,alicePrivate);
	ECDHX.X25519.shared(bobShared,aliceSession,bobPrivate);

}

void fp448()
{
	argElement_1w a1 = Fp.fp448._1way.init();
	argElement_1w b1 = Fp.fp448._1way.init();
	argElement_1w c1 = Fp.fp448._1way.init();

	argElement_2w a2 = Fp.fp448._2way.init();
	argElement_2w b2 = Fp.fp448._2way.init();
	argElement_2w c2 = Fp.fp448._2way.init();

	argElement_4w a4 = Fp.fp448._4way.init();
	argElement_4w b4 = Fp.fp448._4way.init();
	argElement_4w c4 = Fp.fp448._4way.init();

	Fp.fp448._1way.mul(c1,a1,b1);
	Fp.fp448._2way.mul(c2,a2,b2);
	Fp.fp448._4way.mul(c4,a4,b4);

	Fp.fp448._1way.add(c1,a1,b1);

	Fp.fp448._1way.clear(a1);	Fp.fp448._1way.clear(b1);	Fp.fp448._1way.clear(c1);
	Fp.fp448._2way.clear(a2);	Fp.fp448._2way.clear(b2);	Fp.fp448._2way.clear(c2);
	Fp.fp448._4way.clear(a4);	Fp.fp448._4way.clear(b4);	Fp.fp448._4way.clear(c4);
}

void fp25519()
{
	argElement_1w a = Fp.fp25519._1way_x64.init();
	argElement_1w b = Fp.fp25519._1way_x64.init();
	argElement_1w c = Fp.fp25519._1way_x64.init();

	argElement_1w a1 = Fp.fp25519._1way.init();
	argElement_1w b1 = Fp.fp25519._1way.init();
	argElement_1w c1 = Fp.fp25519._1way.init();

	argElement_2w a2 = Fp.fp25519._2way.init();
	argElement_2w b2 = Fp.fp25519._2way.init();
	argElement_2w c2 = Fp.fp25519._2way.init();

	argElement_4w a4 = Fp.fp25519._4way.init();
	argElement_4w b4 = Fp.fp25519._4way.init();
	argElement_4w c4 = Fp.fp25519._4way.init();

	Fp.fp25519._1way_x64.mul(c,a,b);
	Fp.fp25519._1way.mul(c1,a1,b1);
	Fp.fp25519._2way.mul(c2,a2,b2);
	Fp.fp25519._4way.mul(c4,a4,b4);

	Fp.fp25519._1way_x64.clear(a);	Fp.fp25519._1way_x64.clear(b);	Fp.fp25519._1way_x64.clear(c);
	Fp.fp25519._1way.clear(a1);	Fp.fp25519._1way.clear(b1);	Fp.fp25519._1way.clear(c1);
	Fp.fp25519._2way.clear(a2);	Fp.fp25519._2way.clear(b2);	Fp.fp25519._2way.clear(c2);
	Fp.fp25519._4way.clear(a4);	Fp.fp25519._4way.clear(b4);	Fp.fp25519._4way.clear(c4);
}

int main()
{
	ed25519();
	fp25519();
	x25519();

	ed448();
	fp448();
	x448();
	return 0;
}
