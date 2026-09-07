// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauth/CRs256JwtTokenProviderComp.h>


// Qt includes
#include <QtCore/QFile>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QJsonArray>
#include <QtCore/QDateTime>

// OpenSSL includes
#include <openssl/evp.h>
#include <openssl/pem.h>
#include <openssl/err.h>
#include <openssl/bn.h>
#include <openssl/rsa.h>


namespace imtauth
{


// reimplemented (icomp::CComponentBase)

void CRs256JwtTokenProviderComp::OnComponentCreated()
{
	LoadPrivateKey();
}


// protected methods

// reimplemented (imtauth::IJwtTokenProvider)

QByteArray CRs256JwtTokenProviderComp::GenerateToken(const QJsonObject& claims) const
{
	if (m_privateKeyPem.isEmpty()){
		SendErrorMessage(0, "Private key not loaded", "CRs256JwtTokenProviderComp");
		return QByteArray();
	}

	// Build JWT header
	QJsonObject header;
	header["alg"] = "RS256";
	header["typ"] = "JWT";
	header["kid"] = QString::fromUtf8(*m_keyIdAttrPtr);

	QByteArray headerJson = QJsonDocument(header).toJson(QJsonDocument::Compact);
	QByteArray headerBase64 = Base64UrlEncode(headerJson);

	// Add standard claims if not present
	QJsonObject fullClaims = claims;
	if (!fullClaims.contains("iat")){
		fullClaims["iat"] = QDateTime::currentSecsSinceEpoch();
	}
	if (!fullClaims.contains("exp")){
		fullClaims["exp"] = QDateTime::currentSecsSinceEpoch() + *m_tokenLifetimeAttrPtr;
	}
	if (!fullClaims.contains("iss") && m_issuerCompPtr.IsValid()){
		fullClaims["iss"] = m_issuerCompPtr->GetName();
	}

	QByteArray payloadJson = QJsonDocument(fullClaims).toJson(QJsonDocument::Compact);
	QByteArray payloadBase64 = Base64UrlEncode(payloadJson);

	QByteArray signingInput = headerBase64 + "." + payloadBase64;
	QByteArray signature = SignWithRs256(signingInput);
	if (signature.isEmpty()){
		SendErrorMessage(0, "Failed to sign JWT", "CRs256JwtTokenProviderComp");
		return QByteArray();
	}

	return signingInput + "." + Base64UrlEncode(signature);
}


bool CRs256JwtTokenProviderComp::ValidateToken(const QByteArray& jwt, QJsonObject& claims) const
{
	QByteArrayList parts = jwt.split('.');
	if (parts.size() != 3){
		return false;
	}

	QByteArray headerBase64 = parts[0];
	QByteArray payloadBase64 = parts[1];
	QByteArray signatureBase64 = parts[2];

	// Verify signature
	QByteArray signingInput = headerBase64 + "." + payloadBase64;
	QByteArray signature = Base64UrlDecode(signatureBase64);

	if (!VerifyWithRs256(signingInput, signature)){
		return false;
	}

	// Decode payload
	QByteArray payloadJson = Base64UrlDecode(payloadBase64);
	QJsonDocument payloadDoc = QJsonDocument::fromJson(payloadJson);
	if (!payloadDoc.isObject()){
		return false;
	}

	claims = payloadDoc.object();

	// Check expiration
	if (claims.contains("exp")){
		qint64 exp = static_cast<qint64>(claims["exp"].toDouble());
		if (exp < QDateTime::currentSecsSinceEpoch()){
			return false;
		}
	}

	return true;
}


QByteArray CRs256JwtTokenProviderComp::GetJwks() const
{
	if (!m_cachedJwks.isEmpty()){
		return m_cachedJwks;
	}

	QByteArray modulus;
	QByteArray exponent;
	if (!ExtractPublicKeyComponents(modulus, exponent)){
		SendErrorMessage(0, "Failed to extract public key components for JWKS", "CRs256JwtTokenProviderComp");
		return QByteArray();
	}

	QJsonObject jwk;
	jwk["kty"] = "RSA";
	jwk["use"] = "sig";
	jwk["alg"] = "RS256";
	jwk["kid"] = QString::fromUtf8(*m_keyIdAttrPtr);
	jwk["n"] = QString::fromUtf8(Base64UrlEncode(modulus));
	jwk["e"] = QString::fromUtf8(Base64UrlEncode(exponent));

	QJsonObject jwks;
	QJsonArray keys;
	keys.append(jwk);
	jwks["keys"] = keys;

	m_cachedJwks = QJsonDocument(jwks).toJson(QJsonDocument::Compact);
	return m_cachedJwks;
}


QByteArray CRs256JwtTokenProviderComp::GetKeyId() const
{
	return *m_keyIdAttrPtr;
}


// private methods

QByteArray CRs256JwtTokenProviderComp::Base64UrlEncode(const QByteArray& data) const
{
	return data.toBase64(QByteArray::Base64UrlEncoding | QByteArray::OmitTrailingEquals);
}


QByteArray CRs256JwtTokenProviderComp::Base64UrlDecode(const QByteArray& data) const
{
	return QByteArray::fromBase64(data, QByteArray::Base64UrlEncoding | QByteArray::OmitTrailingEquals);
}


QByteArray CRs256JwtTokenProviderComp::SignWithRs256(const QByteArray& data) const
{
	if (m_privateKeyPem.isEmpty()){
		return QByteArray();
	}

	BIO* bio = BIO_new_mem_buf(m_privateKeyPem.constData(), m_privateKeyPem.size());
	if (bio == nullptr){
		return QByteArray();
	}

	EVP_PKEY* pkey = PEM_read_bio_PrivateKey(bio, nullptr, nullptr, nullptr);
	BIO_free(bio);

	if (pkey == nullptr){
		return QByteArray();
	}

	EVP_MD_CTX* mdCtx = EVP_MD_CTX_new();
	if (mdCtx == nullptr){
		EVP_PKEY_free(pkey);
		return QByteArray();
	}

	QByteArray signature;

	if (EVP_DigestSignInit(mdCtx, nullptr, EVP_sha256(), nullptr, pkey) == 1){
		if (EVP_DigestSignUpdate(mdCtx, data.constData(), static_cast<size_t>(data.size())) == 1){
			size_t sigLen = 0;
			if (EVP_DigestSignFinal(mdCtx, nullptr, &sigLen) == 1){
				signature.resize(static_cast<int>(sigLen));
				if (EVP_DigestSignFinal(mdCtx, reinterpret_cast<unsigned char*>(signature.data()), &sigLen) == 1){
					signature.resize(static_cast<int>(sigLen));
				}
				else{
					signature.clear();
				}
			}
		}
	}

	EVP_MD_CTX_free(mdCtx);
	EVP_PKEY_free(pkey);

	return signature;
}


bool CRs256JwtTokenProviderComp::VerifyWithRs256(const QByteArray& data, const QByteArray& signature) const
{
	if (m_privateKeyPem.isEmpty()){
		return false;
	}

	// Extract public key from private key
	BIO* bio = BIO_new_mem_buf(m_privateKeyPem.constData(), m_privateKeyPem.size());
	if (bio == nullptr){
		return false;
	}

	EVP_PKEY* pkey = PEM_read_bio_PrivateKey(bio, nullptr, nullptr, nullptr);
	BIO_free(bio);

	if (pkey == nullptr){
		return false;
	}

	EVP_MD_CTX* mdCtx = EVP_MD_CTX_new();
	if (mdCtx == nullptr){
		EVP_PKEY_free(pkey);
		return false;
	}

	bool result = false;

	if (EVP_DigestVerifyInit(mdCtx, nullptr, EVP_sha256(), nullptr, pkey) == 1){
		if (EVP_DigestVerifyUpdate(mdCtx, data.constData(), static_cast<size_t>(data.size())) == 1){
			int verifyResult = EVP_DigestVerifyFinal(
				mdCtx,
				reinterpret_cast<const unsigned char*>(signature.constData()),
				static_cast<size_t>(signature.size()));
			result = (verifyResult == 1);
		}
	}

	EVP_MD_CTX_free(mdCtx);
	EVP_PKEY_free(pkey);

	return result;
}


bool CRs256JwtTokenProviderComp::LoadPrivateKey()
{
	if (!m_privateKeyPathCompPtr.IsValid()){
		SendErrorMessage(0, "PrivateKeyPath not configured", "CRs256JwtTokenProviderComp");
		return false;
	}

	QString keyPath = m_privateKeyPathCompPtr->GetPath();
	QFile keyFile(keyPath);
	if (!keyFile.open(QFile::ReadOnly)){
		SendErrorMessage(0, QString("Cannot open private key file: %1").arg(keyPath), "CRs256JwtTokenProviderComp");
		return false;
	}

	m_privateKeyPem = keyFile.readAll();
	keyFile.close();

	if (m_privateKeyPem.isEmpty()){
		SendErrorMessage(0, "Private key file is empty", "CRs256JwtTokenProviderComp");
		return false;
	}

	// Validate the key can be loaded
	BIO* bio = BIO_new_mem_buf(m_privateKeyPem.constData(), m_privateKeyPem.size());
	if (bio == nullptr){
		SendErrorMessage(0, "Failed to create BIO for private key", "CRs256JwtTokenProviderComp");
		m_privateKeyPem.clear();
		return false;
	}

	EVP_PKEY* pkey = PEM_read_bio_PrivateKey(bio, nullptr, nullptr, nullptr);
	BIO_free(bio);

	if (pkey == nullptr){
		SendErrorMessage(0, "Failed to parse private key PEM", "CRs256JwtTokenProviderComp");
		m_privateKeyPem.clear();
		return false;
	}

	EVP_PKEY_free(pkey);

	SendInfoMessage(0, "RS256 private key loaded successfully", "CRs256JwtTokenProviderComp");
	return true;
}


bool CRs256JwtTokenProviderComp::ExtractPublicKeyComponents(QByteArray& modulus, QByteArray& exponent) const
{
	if (m_privateKeyPem.isEmpty()){
		return false;
	}

	BIO* bio = BIO_new_mem_buf(m_privateKeyPem.constData(), m_privateKeyPem.size());
	if (bio == nullptr){
		return false;
	}

	EVP_PKEY* pkey = PEM_read_bio_PrivateKey(bio, nullptr, nullptr, nullptr);
	BIO_free(bio);

	if (pkey == nullptr){
		return false;
	}

	BIGNUM* n = nullptr;
	BIGNUM* e = nullptr;

#if OPENSSL_VERSION_NUMBER >= 0x30000000L
	// OpenSSL 3.x
	EVP_PKEY_get_bn_param(pkey, "n", &n);
	EVP_PKEY_get_bn_param(pkey, "e", &e);
#else
	// OpenSSL 1.x
	const RSA* rsa = EVP_PKEY_get0_RSA(pkey);
	if (rsa != nullptr){
		const BIGNUM* nConst = nullptr;
		const BIGNUM* eConst = nullptr;
		RSA_get0_key(rsa, &nConst, &eConst, nullptr);
		if (nConst != nullptr){
			n = BN_dup(nConst);
		}
		if (eConst != nullptr){
			e = BN_dup(eConst);
		}
	}
#endif

	bool result = false;
	if (n != nullptr && e != nullptr){
		int nLen = BN_num_bytes(n);
		modulus.resize(nLen);
		BN_bn2bin(n, reinterpret_cast<unsigned char*>(modulus.data()));

		int eLen = BN_num_bytes(e);
		exponent.resize(eLen);
		BN_bn2bin(e, reinterpret_cast<unsigned char*>(exponent.data()));

		result = true;
	}

	if (n != nullptr){
		BN_free(n);
	}
	if (e != nullptr){
		BN_free(e);
	}
	EVP_PKEY_free(pkey);

	return result;
}


} // namespace imtauth
