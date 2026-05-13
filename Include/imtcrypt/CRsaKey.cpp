// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtcrypt/CRsaKey.h>


// OpenSSL includes
#include <openssl/rsa.h>
#include <openssl/pem.h>


namespace imtcrypt
{


void CRsaKey::GenerateRsaKeys()
{
	RSA* keypair = RSA_new();
	BIGNUM *e = BN_new();
	BN_set_word(e, RSA_F4);
	constexpr int keyLength = 512;
	if (RSA_generate_key_ex(keypair, keyLength, e, nullptr) != 0){
		RSA* privKey;
		RSA* pubKey;
		privKey = RSAPrivateKey_dup(keypair);
		pubKey = RSAPublicKey_dup(keypair);
		if (privKey != nullptr){
			RSA_free(privKey);
		}
		if (pubKey != nullptr){
			RSA_free(pubKey);
		}

		BIO *bio = BIO_new(BIO_s_mem());
		i2d_RSAPrivateKey_bio(bio, privKey);

		int bioLength = BIO_pending(bio);
		m_privateKey.resize(bioLength);

		BIO_read(bio, m_privateKey.data(), bioLength);

		i2d_RSAPublicKey_bio(bio, pubKey);

		bioLength = BIO_pending(bio);
		m_publicKey.resize(bioLength);

		BIO_read(bio, m_publicKey.data(), bioLength);

		BIO_free_all(bio);
	}
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


