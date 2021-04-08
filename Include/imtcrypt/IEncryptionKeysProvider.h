#pragma once


// Qt includes
#include <QtCore/QByteArray>

// ACF includes
#include <istd/IChangeable.h>


namespace imtcrypt
{


/**
	Interface for a cryptopgraphic keys provider.
	\ingroup Cryptography
*/
class IEncryptionKeysProvider: virtual public istd::IChangeable
{
public:
	enum KeyType
	{
		KT_PUBLIC,
		KT_PRIVATE,
		KT_PASSWORD,
		KT_VECTOR
	};

	/**
		Get the key data according to specified type.
		\sa KeyType
	*/
	virtual QByteArray GetEncryptionKey(KeyType type) const = 0;
};


} //namespace imtcrypt


