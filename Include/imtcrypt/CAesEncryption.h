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

	bool Encrypt(const QByteArray& inputData, const CAesKey& key, QByteArray& outputData) const;
	bool Decrypt(const QByteArray& inputData, const CAesKey& key, QByteArray& outputData) const;

protected:
	void InitContext() const;
	void DestroyContext() const;

private:
	mutable EVP_CIPHER_CTX* m_aesEncryptContext;
	mutable EVP_CIPHER_CTX* m_aesDecryptContext;
	CAesKey m_aesKey;
};


} //namespace imtcrypt


