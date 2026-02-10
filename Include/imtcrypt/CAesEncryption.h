// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// OpenSSL includes
#include <openssl/ossl_typ.h>

// ImtCore includes
#include <imtcrypt/CAesKey.h>


namespace imtcrypt
{


class CAesEncryption
{
public:
	CAesEncryption();
	~CAesEncryption();

	virtual bool Encrypt(const QByteArray& inputData, const CAesKey& key, QByteArray& outputData) const;
	virtual bool Decrypt(const QByteArray& inputData, const CAesKey& key, QByteArray& outputData) const;

protected:
	virtual void InitContext() const;
	virtual void DestroyContext() const;

protected:
	mutable EVP_CIPHER_CTX* m_aesEncryptContext;
	mutable EVP_CIPHER_CTX* m_aesDecryptContext;
	CAesKey m_aesKey;
};


} //namespace imtcrypt


