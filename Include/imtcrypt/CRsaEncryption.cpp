// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtcrypt/CRsaEncryption.h>


// OpenSSL includes
#include <openssl/evp.h>
#include <openssl/pem.h>
#include <openssl/aes.h>
#include <openssl/err.h>
#include <openssl/rand.h>

// ImtCore includes
#include <imtbase/imtbase.h>


namespace imtcrypt
{



CRsaEncryption::CRsaEncryption()
{
	m_rsaPrivateKey = nullptr;
	m_rsaPublicKey = nullptr;
}


bool CRsaEncryption::Encrypt(const QByteArray& inputData, const CRsaKey& key, QByteArray& outputData) const
{
	SetRsaPrivateKey(key.GetPrivateKey());
	SetRsaPublicKey(key.GetPublicKey());

	outputData.clear();

	if (m_rsaPublicKey == nullptr){
		return false;
	}

	QByteArray baRes;
	baRes.resize(RSA_size(m_rsaPublicKey) + 1);
	uchar *encryptedMessage = (uchar*)baRes.data();
	int len = RSA_public_encrypt(imtbase::narrow_cast<int>(inputData.length()), (uchar*)inputData.data(), encryptedMessage, m_rsaPublicKey, RSA_PKCS1_PADDING);
	if (len < 0){
		return false;
	}

	outputData = QByteArray((char*)encryptedMessage, len);

	return true;

}

bool CRsaEncryption::Decrypt(const QByteArray& inputData, const CRsaKey& key, QByteArray& outputData) const
{
	SetRsaPrivateKey(key.GetPrivateKey());
	SetRsaPublicKey(key.GetPublicKey());

	outputData.clear();

	if (m_rsaPrivateKey == nullptr){
		return false;
	}

	QByteArray baRes;

	baRes.resize(RSA_size(m_rsaPublicKey) + 1);
	uchar *decryptedMessage = (uchar*)baRes.data();

	int len = RSA_private_decrypt(imtbase::narrow_cast<int>(inputData.length()), (uchar*)inputData.data(), decryptedMessage, m_rsaPrivateKey, RSA_PKCS1_PADDING);
	if (len < 0){
		return false;
	}

	outputData = QByteArray((char*)decryptedMessage, len);

	return true;

}


QByteArray CRsaEncryption::GetRsaPrivateKey() const
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


QByteArray CRsaEncryption::GetRsaPublicKey() const
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


bool CRsaEncryption::SetRsaPrivateKey(const QByteArray &key) const
{
	if (m_rsaPrivateKey != nullptr){
		RSA_free(m_rsaPrivateKey);
		m_rsaPrivateKey = nullptr;
	}

	BIO *bio = BIO_new(BIO_s_mem());
	const int keyLength = imtbase::narrow_cast<int>(key.length());
	if (BIO_write(bio, key.data(), keyLength) != keyLength){
		return false;
	}

	m_rsaPrivateKey = d2i_RSAPrivateKey_bio(bio, &m_rsaPrivateKey);

	BIO_free_all(bio);

	return  true;
}


bool CRsaEncryption::SetRsaPublicKey(const QByteArray &key) const
{
	if (m_rsaPublicKey != nullptr){
		RSA_free(m_rsaPublicKey);
		m_rsaPublicKey = nullptr;
	}

	BIO *bio = BIO_new(BIO_s_mem());
	int keyLength = imtbase::narrow_cast<int>(key.length());
	if (BIO_write(bio, key.data(), keyLength) != keyLength){
		return false;
	}


	m_rsaPublicKey = d2i_RSAPublicKey_bio(bio, &m_rsaPublicKey);

	BIO_free_all(bio);

	return m_rsaPublicKey != nullptr;
}


} //namespace imtcrypt

