#pragma once


// OpenSSL includes
#include <openssl/rsa.h>

// ImtCore includes
#include <imtcrypt/CRsaKey.h>


namespace imtcrypt
{


class CRsaEncryption
{
public:
	CRsaEncryption();

	bool Encrypt(const QByteArray& inputData, const CRsaKey& key, QByteArray& outputData) const;
	bool Decrypt(const QByteArray& inputData, const CRsaKey& key, QByteArray& outputData) const;

protected:
	QByteArray GetRsaPrivateKey() const;
	QByteArray GetRsaPublicKey() const;
	bool SetRsaPrivateKey(const QByteArray &key) const;
	bool SetRsaPublicKey(const QByteArray &key) const;

private:
	mutable RSA* m_rsaPublicKey;
	mutable RSA* m_rsaPrivateKey;
	mutable CRsaKey m_rsaKey;
};


} //namespace imtcrypt

