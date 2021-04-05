#pragma once


#include <openssl/rsa.h>

//#include "secret/qaesencryption.h"
//#include "secret/qrsaencryption.h"


//#define RSA_KEYLEN 2048
//#define RSA_KEYLEN 512
//#define AES_ROUNDS 6

//#define PSEUDO_CLIENT

//#define USE_PBKDF

//#define SUCCESS 0
//#define FAILURE -1

//#define KEY_SERVER_PRI 0
//#define KEY_SERVER_PUB 1
//#define KEY_CLIENT_PUB 2
//#define KEY_AES        3
//#define KEY_AES_IV     4




// ACF includes
#include <imtcrypter/ICrypter.h>
#include <imtcrypter/CAESKey.h>
#include <imtcrypter/CRSAKey.h>


namespace imtcrypter
{


class CCrypter: virtual public ICrypter
{
public:
	explicit CCrypter();

	~CCrypter();

	// reimplemented (ICrypter)
	virtual bool EncryptAES(QByteArray &data, const IAESKey &key) override;
	virtual bool DecryptAES(QByteArray &data, const IAESKey &key) override;

	virtual bool EncryptRSA(QByteArray &data, const IRSAKey &key) override;
	virtual bool DecryptRSA(QByteArray &data, const IRSAKey &key) override;

protected:
	bool initAESContext();
	QByteArray getRSAPrivateKey();
	QByteArray getRSAPublicKey();
	bool setRSAPrivateKey(const QByteArray &key);
	bool setRSAPublicKey(const QByteArray &key);

private:
	EVP_CIPHER_CTX* m_aesEncryptContext;
	EVP_CIPHER_CTX* m_aesDecryptContext;
	RSA* m_rsaPubKey;
	RSA* m_rsaPrivKey;
	CAESKey m_aesKey;
	CRSAKey m_rsaKey;
};


} //namespace imtcrypter

