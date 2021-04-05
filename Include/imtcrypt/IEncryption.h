#pragma once


// ACF includes
#include <istd/IPolymorphic.h>


namespace imtcrypt
{


class IEncryptionKeysProvider;


/**
	Interface describing encryption/decryption of data.
	\ingroup Cryptography
*/
class IEncryption: virtual public istd::IPolymorphic
{
public:
	enum EncryptionAlgorithm
	{
		EA_RSA,
		EA_AES
	};

	virtual bool EncryptData(
				const QByteArray& input,
				EncryptionAlgorithm algorithm,
				const IEncryptionKeysProvider& keys,
				QByteArray& output) const = 0;

	virtual bool DecryptData(
				const QByteArray& input,
				EncryptionAlgorithm algorithm,
				const IEncryptionKeysProvider& keys,
				QByteArray& output) const = 0;
};


} //namespace imtcrypt


