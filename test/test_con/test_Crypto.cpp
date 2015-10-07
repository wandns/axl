#include "pch.h"

#include <string>

namespace test_Crypto { 

//.............................................................................

void run ()
{
	cry::EcKey key (NID_secp112r1);
	key.generateKey ();

	cry::BigNum privateKey = key.getPrivateKey ();
	cry::EcPoint publicKey = key.getPublicKey ();
	printf ("private: %s\n", privateKey.getHexString ().cc ());
	printf ("public:  %s\n", publicKey.getHexString (key.getGroup ()).cc ());

	const char* userName = "Vovkos";

	sl::String productKey = generateEcProductKey (key, userName);
	printf ("user:    %s\n", userName);
	printf ("license: %s\n", productKey.cc ());

	bool result = verifyEcProductKey (key, userName, productKey);
	printf ("verify:  %d\n", result);

	privateKey.detach ();
	publicKey.detach ();

}

//.............................................................................

} // namespace test_Vso
