// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtcrypt/CRsaKey.h>


// STL includes
#include <memory>

// OpenSSL includes
#include <openssl/evp.h>
#include <openssl/rsa.h>


namespace imtcrypt
{


namespace
{


// For RSA keys i2d_PrivateKey and i2d_PublicKey write PKCS#1 DER (RSAPrivateKey and RSAPublicKey)
template <typename EncodeFunction>
QByteArray EncodeKey(EVP_PKEY* key, EncodeFunction encode)
{
	const int length = encode(key, nullptr);
	if (length <= 0){
		return QByteArray();
	}

	QByteArray result(length, '\0');
	uchar* data = reinterpret_cast<uchar*>(result.data());
	encode(key, &data);

	return result;
}


} // namespace


void CRsaKey::GenerateRsaKeys()
{
	constexpr int keyLength = 2048;

	std::unique_ptr<EVP_PKEY_CTX, decltype(&EVP_PKEY_CTX_free)> context(EVP_PKEY_CTX_new_id(EVP_PKEY_RSA, nullptr), &EVP_PKEY_CTX_free);
	if (!context || EVP_PKEY_keygen_init(context.get()) <= 0 || EVP_PKEY_CTX_set_rsa_keygen_bits(context.get(), keyLength) <= 0){
		return;
	}

	EVP_PKEY* keyPairPtr = nullptr;
	if (EVP_PKEY_keygen(context.get(), &keyPairPtr) <= 0){
		return;
	}

	std::unique_ptr<EVP_PKEY, decltype(&EVP_PKEY_free)> keyPair(keyPairPtr, &EVP_PKEY_free);

	m_privateKey = EncodeKey(keyPair.get(), i2d_PrivateKey);
	m_publicKey = EncodeKey(keyPair.get(), i2d_PublicKey);
}


QByteArray CRsaKey::GetPrivateKey() const
{
	return m_privateKey;
}


QByteArray CRsaKey::GetPublicKey() const
{
	return m_publicKey;
}


void CRsaKey::SetPrivateKey(const QByteArray &key)
{
	m_privateKey = key;
}


void CRsaKey::SetPublicKey(const QByteArray &key)
{
	m_publicKey = key;
}


} //namespace imtcrypt


