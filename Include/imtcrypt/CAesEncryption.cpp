#include <imtcrypt/CAesEncryption.h>


// Qt includes
#include <QtCore/QCryptographicHash>

// OpenSSL includes
#include <openssl/evp.h>
#include <openssl/pem.h>
#include <openssl/aes.h>
#include <openssl/err.h>
#include <openssl/rand.h>


namespace imtcrypt
{


CAesEncryption::CAesEncryption()
{
	m_aesEncryptContext = nullptr;
	m_aesDecryptContext = nullptr;
}


CAesEncryption::~CAesEncryption()
{
	EVP_CIPHER_CTX_free(m_aesDecryptContext);
	EVP_CIPHER_CTX_free(m_aesEncryptContext);
}


bool CAesEncryption::Encrypt(const QByteArray& inputData, const CAesKey& key, QByteArray& outputData) const
{
	InitContext();

	outputData.clear();

	// Allocate memory for everything
	size_t blockLength = 0;
	size_t encryptedMessageLength = 0;

	QByteArray tempOutput;
	tempOutput.resize(inputData.length() + AES_BLOCK_SIZE);
	uchar *encryptedMessage = (uchar*)tempOutput.data();

	QByteArray aesKey = key.GetKey();
	QByteArray aesIV = key.GetIV();

	// Encrypt it!
	if (!EVP_EncryptInit_ex(m_aesEncryptContext, EVP_aes_256_cbc(), NULL, (uchar *)aesKey.data(), (uchar*)aesIV.data())){
		return false;
	}

	if (!EVP_EncryptUpdate(m_aesEncryptContext, encryptedMessage, (int*)&blockLength, (unsigned char*)inputData.data(), inputData.length())){
		return false;
	}

	encryptedMessageLength += blockLength;

	if (!EVP_EncryptFinal_ex(m_aesEncryptContext, encryptedMessage + encryptedMessageLength, (int*)&blockLength)){
		return false;
	}

	outputData = QByteArray((char*)encryptedMessage, int(encryptedMessageLength + blockLength));

	return true;
}


bool CAesEncryption::Decrypt(const QByteArray& inputData, const CAesKey& key, QByteArray& outputData) const 
{
	InitContext();

	outputData.clear();

	size_t decryptedMessageLength = 0;
	size_t blockLength = 0;

	QByteArray tempOutput;
	tempOutput.resize((inputData.length()));
	uchar* decryptedMessage = (uchar*)tempOutput.data();

	QByteArray aesKey = key.GetKey();
	QByteArray aesIV = key.GetIV();

	// Decrypt it!
	if (!EVP_DecryptInit_ex(m_aesDecryptContext, EVP_aes_256_cbc(), NULL, (uchar*)aesKey.data(), (uchar*)aesIV.data())){
		return false;
	}

	if (!EVP_DecryptUpdate(m_aesDecryptContext, decryptedMessage, (int*)&blockLength, (uchar*)inputData.data(), (int)inputData.length())){
		return false;
	}

	decryptedMessageLength += blockLength;

	if (!EVP_DecryptFinal_ex(m_aesDecryptContext, decryptedMessage + decryptedMessageLength, (int*)&blockLength)){
		return false;
	}

	decryptedMessageLength += blockLength;

	outputData = QByteArray((char*)decryptedMessage, int(decryptedMessageLength));

	return true;

}


// protected methods

void CAesEncryption::InitContext() const
{
	DestroyContext();

	m_aesEncryptContext = EVP_CIPHER_CTX_new();
	m_aesDecryptContext = EVP_CIPHER_CTX_new();
}


void CAesEncryption::DestroyContext() const
{
	if (m_aesEncryptContext != nullptr){
		EVP_CIPHER_CTX_free(m_aesEncryptContext);
	}

	if (m_aesDecryptContext != nullptr){
		EVP_CIPHER_CTX_free(m_aesDecryptContext);
	}
}

} //namespace imtcrypt

