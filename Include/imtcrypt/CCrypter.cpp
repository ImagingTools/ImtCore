#include <imtcrypt/CCrypter.h>


// Qt includes
#include <QtCore/QCryptographicHash>
#include <QtCore/QDebug>

// OpenSSL includes
#include <openssl/evp.h>
#include <openssl/pem.h>
#include <openssl/aes.h>
#include <openssl/err.h>
#include <openssl/rand.h>


namespace imtcrypt
{


CCrypter::CCrypter()
{
	m_rsaPrivateKey = nullptr;
	m_rsaPublicKey = nullptr;
	m_aesEncryptContext = nullptr;
	m_aesDecryptContext = nullptr;
}


CCrypter::~CCrypter()
{
	EVP_CIPHER_CTX_free(m_aesDecryptContext);
	EVP_CIPHER_CTX_free(m_aesEncryptContext);
}


// reimplemented (ICrypter)

bool CCrypter::EncryptAes(QByteArray& data, const IAesKey& key)
{
	// Allocate memory for everything
	size_t blockLength = 0;
	size_t encryptedMessageLength = 0;

	QByteArray baRes;
	baRes.resize(data.length() + AES_BLOCK_SIZE);
	uchar *encryptedMessage = (uchar*)baRes.data();

	QByteArray aesKey = key.GetKey();
	QByteArray aesIV = key.GetIV();

	// Encrypt it!
	if (!EVP_EncryptInit_ex(m_aesEncryptContext, EVP_aes_256_cbc(), NULL, (uchar *)aesKey.data(), (uchar*)aesIV.data())){
		return false;
	}

	if (!EVP_EncryptUpdate(m_aesEncryptContext, encryptedMessage, (int*)&blockLength, (unsigned char*)data.data(), data.length())){
		return false;
	}

	encryptedMessageLength += blockLength;

	if (!EVP_EncryptFinal_ex(m_aesEncryptContext, encryptedMessage + encryptedMessageLength, (int*)&blockLength)){
		return false;
	}

	data = QByteArray((char*)encryptedMessage, int(encryptedMessageLength + blockLength));

	return true;
}


bool CCrypter::DecryptAes(QByteArray &data, const IAesKey &key)
{
	size_t decryptedMessageLength = 0;
	size_t blockLength = 0;

	QByteArray baRes;
	baRes.resize((data.length()));
	uchar* decryptedMessage = (uchar*)baRes.data();

	QByteArray aesKey = key.GetKey();
	QByteArray aesIV = key.GetIV();

	// Decrypt it!
	if (!EVP_DecryptInit_ex(m_aesDecryptContext, EVP_aes_256_cbc(), NULL, (uchar*)aesKey.data(), (uchar*)aesIV.data())){
		return false;
	}

	if (!EVP_DecryptUpdate(m_aesDecryptContext, decryptedMessage, (int*)&blockLength, (uchar*)data.data(), (int)data.length())){
		return false;
	}
	decryptedMessageLength += blockLength;

	if (!EVP_DecryptFinal_ex(m_aesDecryptContext, decryptedMessage + decryptedMessageLength, (int*)&blockLength)){
		return false;
	}
	decryptedMessageLength += blockLength;

	data = QByteArray((char*)decryptedMessage, int(decryptedMessageLength));

	return true;

}

bool CCrypter::EncryptRsa(QByteArray &data, const IRsaKey &key)
{
	SetRsaPrivateKey(key.GetPrivateKey());
	SetRsaPublicKey(key.GetPublicKey());

	if (!m_rsaPublicKey){
		return false;
	}

	QByteArray baRes;
	baRes.resize(RSA_size(m_rsaPublicKey) + 1);
	uchar *encryptedMessage = (uchar*)baRes.data();
	int len = RSA_public_encrypt(data.length(), (uchar*)data.data(), encryptedMessage, m_rsaPublicKey, RSA_PKCS1_PADDING);
	if (len < 0){
		return false;
	}

	data = QByteArray((char*)encryptedMessage, len);

	return true;

}

bool CCrypter::DecryptRsa(QByteArray& data, const IRsaKey& key)
{
	SetRsaPrivateKey(key.GetPrivateKey());
	SetRsaPublicKey(key.GetPublicKey());

	if (!m_rsaPrivateKey){
		return false;
	}

	QByteArray baRes;

	baRes.resize(RSA_size(m_rsaPublicKey) + 1);
	uchar *decryptedMessage = (uchar*)baRes.data();

	int len = RSA_private_decrypt(data.length(), (uchar*)data.data(), decryptedMessage, m_rsaPrivateKey, RSA_PKCS1_PADDING);
	if (len < 0){
		return false;
	}

	data = QByteArray((char*)decryptedMessage, len);

	return true;

}



QByteArray CCrypter::GetRsaPrivateKey()
{
	BIO *bio = BIO_new(BIO_s_mem());
	d2i_RSAPrivateKey_bio(bio, &m_rsaPrivateKey);

	size_t bioLength = BIO_pending(bio);
	QByteArray baRes;
	baRes.resize(int(bioLength));

	BIO_read(bio, baRes.data(), int(bioLength));

	BIO_free_all(bio);

	return  baRes;
}

QByteArray CCrypter::GetRsaPublicKey()
{
	BIO *bio = BIO_new(BIO_s_mem());
	d2i_RSAPublicKey_bio(bio, &m_rsaPublicKey);

	size_t bioLength = BIO_pending(bio);
	QByteArray baRes;
	baRes.resize(int(bioLength));

	BIO_read(bio, baRes.data(), int(bioLength));

	BIO_free_all(bio);

	return  baRes;
}

bool CCrypter::SetRsaPrivateKey(const QByteArray &key)
{
	if (m_rsaPrivateKey){
		RSA_free(m_rsaPrivateKey);
		m_rsaPrivateKey = nullptr;
	}

	BIO *bio = BIO_new(BIO_s_mem());
	if (BIO_write(bio, key.data(), key.length()) != (int)key.length()){
		return false;
	}


	m_rsaPrivateKey = d2i_RSAPrivateKey_bio(bio, &m_rsaPrivateKey);

	BIO_free_all(bio);

	return  true;
}

bool CCrypter::SetRsaPublicKey(const QByteArray &key)
{
	if (m_rsaPublicKey){
		RSA_free(m_rsaPublicKey);
		m_rsaPublicKey = nullptr;
	}

	BIO *bio = BIO_new(BIO_s_mem());
	if (BIO_write(bio, key.data(), key.length()) != (int)key.length()){
		return false;
	}


	m_rsaPublicKey = d2i_RSAPublicKey_bio(bio, &m_rsaPublicKey);

	BIO_free_all(bio);

	//		m_pubKey = readPublicKey(key);
	if (!m_rsaPublicKey)
		return false;

	return  true;
}


bool CCrypter::InitAesContext()
{
	// Initalize contexts
	m_aesEncryptContext = EVP_CIPHER_CTX_new();
	m_aesDecryptContext = EVP_CIPHER_CTX_new();

	return true;
}


} //namespace imtcrypt

