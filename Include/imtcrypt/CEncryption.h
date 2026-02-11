// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtcrypt/IEncryption.h>


namespace imtcrypt
{


class CEncryption: virtual public IEncryption
{
public:
	// reimplemented (IEncryption)
	virtual bool EncryptData(
				const QByteArray& input,
				EncryptionAlgorithm algorithm,
				const IEncryptionKeysProvider& keys,
				QByteArray& output) const override;
	virtual bool DecryptData(
				const QByteArray& input,
				EncryptionAlgorithm algorithm,
				const IEncryptionKeysProvider& keys,
				QByteArray& output) const  override;
};


} //namespace imtcrypt


