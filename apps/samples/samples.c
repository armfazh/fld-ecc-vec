#include <faz_eddsa_avx2.h>
#include <faz_ecdh_avx2.h>
#include <fp.h>
#include <random.h>
#include <stdio.h>
#include <string.h>


void x25519()
{
	const DiffieHellmanXFunction *ecdh = &ECDHX.X25519;

	argECDHX_Key alice_private_key = ecdh->newKey();
	argECDHX_Key alice_session_key = ecdh->newKey();
	argECDHX_Key alice_shared_key  = ecdh->newKey();
	argECDHX_Key bob_private_key   = ecdh->newKey();
	argECDHX_Key bob_session_key   = ecdh->newKey();
	argECDHX_Key bob_shared_key    = ecdh->newKey();

	random_bytes(alice_private_key, ecdh->key_size);
	random_bytes(bob_private_key, ecdh->key_size);

	ecdh->keygen(alice_session_key, alice_private_key);
	ecdh->keygen(bob_session_key, bob_private_key);

	ecdh->shared(alice_shared_key, bob_session_key, alice_private_key);
	ecdh->shared(bob_shared_key, alice_session_key, bob_private_key);
//	memcmp(alice_shared_key, bob_shared_key, ecdh->key_size);

	ecdh->cleanKey(alice_private_key);
	ecdh->cleanKey(alice_session_key);
	ecdh->cleanKey(alice_shared_key);
	ecdh->cleanKey(bob_private_key);
	ecdh->cleanKey(bob_session_key);
	ecdh->cleanKey(bob_shared_key);

}

void x448()
{
	const DiffieHellmanXFunction *ecdh = &ECDHX.X448;

	argECDHX_Key alice_private_key = ecdh->newKey();
	argECDHX_Key alice_session_key = ecdh->newKey();
	argECDHX_Key alice_shared_key  = ecdh->newKey();
	argECDHX_Key bob_private_key   = ecdh->newKey();
	argECDHX_Key bob_session_key   = ecdh->newKey();
	argECDHX_Key bob_shared_key    = ecdh->newKey();

	random_bytes(alice_private_key, ecdh->key_size);
	random_bytes(bob_private_key, ecdh->key_size);

	ecdh->keygen(alice_session_key, alice_private_key);
	ecdh->keygen(bob_session_key, bob_private_key);

	ecdh->shared(alice_shared_key, bob_session_key, alice_private_key);
	ecdh->shared(bob_shared_key, alice_session_key, bob_private_key);
//	memcmp(alice_shared_key, bob_shared_key, ecdh->key_size);

	ecdh->cleanKey(alice_private_key);
	ecdh->cleanKey(alice_session_key);
	ecdh->cleanKey(alice_shared_key);
	ecdh->cleanKey(bob_private_key);
	ecdh->cleanKey(bob_session_key);
	ecdh->cleanKey(bob_shared_key);

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
}

void fp25519()
{
	argElement_1w a = Fp.fp25519._1way_x64.new();
	argElement_1w b = Fp.fp25519._1way_x64.new();
	argElement_1w c = Fp.fp25519._1way_x64.new();

	argElement_1w a1 = Fp.fp25519._1way.new();
	argElement_1w b1 = Fp.fp25519._1way.new();
	argElement_1w c1 = Fp.fp25519._1way.new();

	argElement_2w a2 = Fp.fp25519._2way.new();
	argElement_2w b2 = Fp.fp25519._2way.new();
	argElement_2w c2 = Fp.fp25519._2way.new();

	argElement_4w a4 = Fp.fp25519._4way.new();
	argElement_4w b4 = Fp.fp25519._4way.new();
	argElement_4w c4 = Fp.fp25519._4way.new();

	Fp.fp25519._1way_x64.mul(c,a,b);
	Fp.fp25519._1way.mul(c1,a1,b1);
	Fp.fp25519._2way.mul(c2,a2,b2);
	Fp.fp25519._4way.mul(c4,a4,b4);

	Fp.fp25519._1way_x64.clean(a);	Fp.fp25519._1way_x64.clean(b);	Fp.fp25519._1way_x64.clean(c);
	Fp.fp25519._1way.clean(a1);	Fp.fp25519._1way.clean(b1);	Fp.fp25519._1way.clean(c1);
	Fp.fp25519._2way.clean(a2);	Fp.fp25519._2way.clean(b2);	Fp.fp25519._2way.clean(c2);
	Fp.fp25519._4way.clean(a4);	Fp.fp25519._4way.clean(b4);	Fp.fp25519._4way.clean(c4);
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
