#include "CCrypter.h"
#include <QCryptographicHash>
#include <QDebug>

#include <openssl/evp.h>
#include <openssl/pem.h>
#include <openssl/aes.h>
#include <openssl/err.h>
#include <openssl/rand.h>

namespace imtcrypter
{

	CCrypter::CCrypter()
	{
		m_rsaPrivKey = nullptr;
		m_rsaPubKey = nullptr;
		m_aesEncryptContext = nullptr;
		m_aesDecryptContext = nullptr;
	}

	CCrypter::~CCrypter()
	{
		EVP_CIPHER_CTX_free(m_aesDecryptContext);
		EVP_CIPHER_CTX_free(m_aesEncryptContext);
	}

	bool CCrypter::EncryptAES(QByteArray &data, const IAESKey &key)
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
		if (!EVP_EncryptInit_ex(m_aesEncryptContext, EVP_aes_256_cbc(), NULL, (uchar *)aesKey.data(), (uchar*)aesIV.data())) {
			return false;
		}

		if (!EVP_EncryptUpdate(m_aesEncryptContext, encryptedMessage, (int*)&blockLength, (unsigned char*)data.data(), data.length())) {
			return false;
		}
		encryptedMessageLength += blockLength;

		if (!EVP_EncryptFinal_ex(m_aesEncryptContext, encryptedMessage + encryptedMessageLength, (int*)&blockLength)) {
			return false;
		}

		data = QByteArray((char*)encryptedMessage, encryptedMessageLength + blockLength);

		//delete encryptedMessage;
		return true;
	}

	bool CCrypter::DecryptAES(QByteArray &data, const IAESKey &key)
	{
		size_t decryptedMessageLength = 0;
		size_t blockLength = 0;

		QByteArray baRes;
		baRes.resize((data.length()));
		uchar* decryptedMessage = (uchar*)baRes.data();

		QByteArray aesKey = key.GetKey();
		QByteArray aesIV = key.GetIV();

		// Decrypt it!
		if (!EVP_DecryptInit_ex(m_aesDecryptContext, EVP_aes_256_cbc(), NULL, (uchar*)aesKey.data(), (uchar*)aesIV.data())) {
			return false;
		}

		if (!EVP_DecryptUpdate(m_aesDecryptContext, decryptedMessage, (int*)&blockLength, (uchar*)data.data(), (int)data.length())) {
			return false;
		}
		decryptedMessageLength += blockLength;

		if (!EVP_DecryptFinal_ex(m_aesDecryptContext, decryptedMessage + decryptedMessageLength, (int*)&blockLength)) {
			return false;
		}
		decryptedMessageLength += blockLength;

		data = QByteArray((char*)decryptedMessage, decryptedMessageLength);

		return true;

	}

	bool CCrypter::EncryptRSA(QByteArray &data, const IRSAKey &key)
	{
		setRSAPrivateKey(key.GetPrivateKey());
		setRSAPublicKey(key.GetPublicKey());

		if (!m_rsaPubKey)
		{
			return false;
		}

		QByteArray baRes;
		baRes.resize(RSA_size(m_rsaPubKey) + 1);
		uchar *encryptedMessage = (uchar*)baRes.data();
		//    uchar res_chr[565];
		//    char message[40] = "Batuhan AVLAYAN - OpenSSL_RSA demo";
		//    int len = RSA_public_encrypt(35, (uchar*)message, res_chr, m_pubKey, RSA_PKCS1_PADDING);
		int len = RSA_public_encrypt(data.length(), (uchar*)data.data(), encryptedMessage, m_rsaPubKey, RSA_PKCS1_PADDING);
		if (len < 0) {
//			RSA_free(m_rsaPubKey);
			return false;
		}

		data = QByteArray((char*)encryptedMessage, len);

//		if (!extPubKey.isEmpty())
//			RSA_free(pubKey);

		return true;

	}

	bool CCrypter::DecryptRSA(QByteArray &data, const IRSAKey &key)
	{
		setRSAPrivateKey(key.GetPrivateKey());
		setRSAPublicKey(key.GetPublicKey());

		if (!m_rsaPrivKey)
			return false;

		QByteArray baRes;

		baRes.resize(RSA_size(m_rsaPubKey) + 1);
		uchar *decryptedMessage = (uchar*)baRes.data();


		int len = RSA_private_decrypt(data.length(), (uchar*)data.data(), decryptedMessage, m_rsaPrivKey, RSA_PKCS1_PADDING);
		if (len < 0)
			return false;

		data = QByteArray((char*)decryptedMessage, len);

		return true;

	}



	QByteArray CCrypter::getRSAPrivateKey()
	{
		BIO *bio = BIO_new(BIO_s_mem());
		i2d_RSAPrivateKey_bio(bio, m_rsaPrivKey);

		size_t bioLength = BIO_pending(bio);
		QByteArray baRes;
		baRes.resize(bioLength);

		BIO_read(bio, baRes.data(), bioLength);

		BIO_free_all(bio);

		return  baRes;
	}

	QByteArray CCrypter::getRSAPublicKey()
	{
		BIO *bio = BIO_new(BIO_s_mem());
		i2d_RSAPublicKey_bio(bio, m_rsaPubKey);

		size_t bioLength = BIO_pending(bio);
		QByteArray baRes;
		baRes.resize(bioLength);

		BIO_read(bio, baRes.data(), bioLength);

		BIO_free_all(bio);

		return  baRes;
	}

	bool CCrypter::setRSAPrivateKey(const QByteArray &key)
	{
		if (m_rsaPrivKey)
		{
			RSA_free(m_rsaPrivKey);
			m_rsaPrivKey = nullptr;
		}

		BIO *bio = BIO_new(BIO_s_mem());
		if (BIO_write(bio, key.data(), key.length()) != (int)key.length())
		{
			return false;
		}


		m_rsaPrivKey = d2i_RSAPrivateKey_bio(bio, &m_rsaPrivKey);

		BIO_free_all(bio);

		return  true;
	}

	bool CCrypter::setRSAPublicKey(const QByteArray &key)
	{
		if (m_rsaPubKey)
		{
			RSA_free(m_rsaPubKey);
			m_rsaPubKey = nullptr;
		}

		BIO *bio = BIO_new(BIO_s_mem());
		if(BIO_write(bio, key.data(), key.length()) != (int)key.length())
		{
			return false;
		}


		m_rsaPubKey = d2i_RSAPublicKey_bio(bio, &m_rsaPubKey);

		BIO_free_all(bio);

//		m_pubKey = readPublicKey(key);
		if (!m_rsaPubKey)
			return false;

		return  true;
	}


	bool CCrypter::initAESContext()
	{
		// Initalize contexts
		m_aesEncryptContext = EVP_CIPHER_CTX_new();
		m_aesDecryptContext = EVP_CIPHER_CTX_new();

		return true;
	}



} //namespace imtcrypter

