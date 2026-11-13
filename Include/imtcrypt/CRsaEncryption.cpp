// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtcrypt/CRsaEncryption.h>


// STL includes
#include <memory>

// OpenSSL includes
#include <openssl/evp.h>
#include <openssl/rsa.h>

// ImtCore includes
#include <imtbase/imtbase.h>


namespace imtcrypt
{


namespace
{


using KeyPtr = std::unique_ptr<EVP_PKEY, decltype(&EVP_PKEY_free)>;


// Keys are PKCS#1 DER (RSAPrivateKey and RSAPublicKey), as written by CRsaKey
template <typename DecodeFunction>
KeyPtr DecodeKey(const QByteArray& key, DecodeFunction decode)
{
	const uchar* data = reinterpret_cast<const uchar*>(key.constData());

	return KeyPtr(decode(EVP_PKEY_RSA, nullptr, &data, imtbase::narrow_cast<long>(key.length())), &EVP_PKEY_free);
}


template <typename InitFunction, typename CryptFunction>
bool Crypt(EVP_PKEY* key, InitFunction init, CryptFunction crypt, const QByteArray& inputData, QByteArray& outputData)
{
	if (key == nullptr){
		return false;
	}

	std::unique_ptr<EVP_PKEY_CTX, decltype(&EVP_PKEY_CTX_free)> context(EVP_PKEY_CTX_new(key, nullptr), &EVP_PKEY_CTX_free);
	if (!context || init(context.get()) <= 0 || EVP_PKEY_CTX_set_rsa_padding(context.get(), RSA_PKCS1_PADDING) <= 0){
		return false;
	}

	const uchar* input = reinterpret_cast<const uchar*>(inputData.constData());
	const size_t inputLength = size_t(inputData.length());

	size_t outputLength = 0;
	if (crypt(context.get(), nullptr, &outputLength, input, inputLength) <= 0){
		return false;
	}

	QByteArray result(imtbase::narrow_cast<qsizetype>(outputLength), '\0');
	if (crypt(context.get(), reinterpret_cast<uchar*>(result.data()), &outputLength, input, inputLength) <= 0){
		return false;
	}

	result.truncate(imtbase::narrow_cast<qsizetype>(outputLength));
	outputData = result;

	return true;
}


} // namespace


bool CRsaEncryption::Encrypt(const QByteArray& inputData, const CRsaKey& key, QByteArray& outputData) const
{
	outputData.clear();

	KeyPtr publicKey = DecodeKey(key.GetPublicKey(), d2i_PublicKey);

	return Crypt(publicKey.get(), EVP_PKEY_encrypt_init, EVP_PKEY_encrypt, inputData, outputData);
}


bool CRsaEncryption::Decrypt(const QByteArray& inputData, const CRsaKey& key, QByteArray& outputData) const
{
	outputData.clear();

	KeyPtr privateKey = DecodeKey(key.GetPrivateKey(), d2i_PrivateKey);

	return Crypt(privateKey.get(), EVP_PKEY_decrypt_init, EVP_PKEY_decrypt, inputData, outputData);
}


} //namespace imtcrypt


