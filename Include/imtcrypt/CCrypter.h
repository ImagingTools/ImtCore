#pragma once


// OpenSSL includes
#include <openssl/rsa.h>

// ImtCore includes
#include <imtcrypt/ICrypter.h>
#include <imtcrypt/CAesKey.h>
#include <imtcrypt/CRsaKey.h>


namespace imtcrypt
{


class CCrypter: virtual public ICrypter
{
public:
	CCrypter();
	virtual ~CCrypter();

	// reimplemented (ICrypter)
	virtual bool EncryptAes(QByteArray &data, const IAesKey &key) override;
	virtual bool DecryptAes(QByteArray &data, const IAesKey &key) override;
	virtual bool EncryptRsa(QByteArray &data, const IRsaKey &key) override;
	virtual bool DecryptRsa(QByteArray &data, const IRsaKey &key) override;

protected:
	bool InitAesContext();
	QByteArray GetRsaPrivateKey();
	QByteArray GetRsaPublicKey();
	bool SetRsaPrivateKey(const QByteArray &key);
	bool SetRsaPublicKey(const QByteArray &key);

private:
	EVP_CIPHER_CTX* m_aesEncryptContext;
	EVP_CIPHER_CTX* m_aesDecryptContext;
	RSA* m_rsaPublicKey;
	RSA* m_rsaPrivateKey;
	CAesKey m_aesKey;
	CRsaKey m_rsaKey;
};


} //namespace imtcrypt

