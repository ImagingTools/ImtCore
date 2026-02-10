// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
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
		EA_RSA = 0,
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


