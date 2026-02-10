// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
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
		KT_INIT_VECTOR
	};

	/**
		Get the key data according to specified type.
		\sa KeyType
	*/
	virtual QByteArray GetEncryptionKey(KeyType type) const = 0;
};


} //namespace imtcrypt


