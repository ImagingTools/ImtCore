#include "ImtCryptPck.h"


// ACF includes
#include <icomp/export.h>


namespace ImtCryptPck
{


I_EXPORT_PACKAGE(
			"ImtCryptPck",
			"Cryptography component package",
			IM_PROJECT("\"ImagingTools Core Framework\"") IM_COMPANY("ImagingTools"));

I_EXPORT_COMPONENT(
			Encryption,
			"Data encryption",
			"Cryptography Encrypt Decrypt");

I_EXPORT_COMPONENT(
			EncryptionBasePersistence,
			"Persistence based on encrypted data",
			"Persistence Encryption Decryption")

I_EXPORT_COMPONENT(
			StaticEncryptionKeyProvider,
			"Static provider of the encryption keys",
			"Keys AES RSA Encryption Decryption");

I_EXPORT_COMPONENT(
			HashCalculator,
			"MD5 Hash Calculator",
			"MD5 Hash Calculator");


} // namespace ImtCryptPck


