#include "CRSAKey.h"
#include <QCryptographicHash>
#include <QDebug>

#include "openssl/pem.h"

namespace imtcrypter
{



void CRSAKey::GenerateRSAKeys()
	{
		RSA *keypair = RSA_new();
		BIGNUM *e = BN_new();
		BN_set_word(e, RSA_F4);  //65537
		if (RSA_generate_key_ex(keypair, 512, e, NULL))
		{
			RSA *m_privKey;
			RSA *m_pubKey;
			m_privKey = RSAPrivateKey_dup(keypair);
			m_pubKey = RSAPublicKey_dup(keypair);
			if (m_privKey)
				RSA_free(m_privKey);
			if (m_pubKey)
				RSA_free(m_pubKey);

			BIO *bio = BIO_new(BIO_s_mem());
			i2d_RSAPrivateKey_bio(bio, m_privKey);

			size_t bioLength = BIO_pending(bio);
			m_privateKey.resize(bioLength);

			BIO_read(bio, m_privateKey.data(), bioLength);

			i2d_RSAPublicKey_bio(bio, m_pubKey);

			bioLength = BIO_pending(bio);
			m_publicKey.resize(bioLength);

			BIO_read(bio, m_publicKey.data(), bioLength);

			BIO_free_all(bio);

		}
	}

	QByteArray CRSAKey::GetPrivateKey() const
	{
		return m_privateKey;
	}

	QByteArray CRSAKey::GetPublicKey() const
	{
		return m_publicKey;
	}

	void CRSAKey::SetPrivateKey(const QByteArray &key)
	{
		m_privateKey = key;
	}

	void CRSAKey::SetPublicKey(const QByteArray &key)
	{
		m_publicKey = key;
	}


} //namespace imtcrypter

