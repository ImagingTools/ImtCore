// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <ilog/TLoggerCompWrap.h>
#include <ifile/IFileNameParam.h>
#include <iprm/INameParam.h>

// ImtCore includes
#include <imtauth/IJwtTokenProvider.h>


namespace imtauth
{


class CRs256JwtTokenProviderComp:
			public ilog::CLoggerComponentBase,
			virtual public imtauth::IJwtTokenProvider
{
public:
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_COMPONENT(CRs256JwtTokenProviderComp);
		I_REGISTER_INTERFACE(imtauth::IJwtTokenProvider);
		I_ASSIGN(m_privateKeyPathCompPtr, "PrivateKeyPath", "Path to RSA private key PEM file", true, "PrivateKeyPath");
		I_ASSIGN(m_keyIdAttrPtr, "KeyId", "Key identifier for JWKS", false, "imtcore-oidc-key-1");
		I_ASSIGN(m_tokenLifetimeAttrPtr, "TokenLifetime", "Token lifetime in seconds", false, 3600);
		I_ASSIGN(m_issuerCompPtr, "Issuer", "OIDC token issuer URL", true, "Issuer");
	I_END_COMPONENT;

	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;

protected:
	// reimplemented (imtauth::IJwtTokenProvider)
	virtual QByteArray GenerateToken(const QJsonObject& claims) const override;
	virtual bool ValidateToken(const QByteArray& jwt, QJsonObject& claims) const override;
	virtual QByteArray GetJwks() const override;
	virtual QByteArray GetKeyId() const override;

private:
	QByteArray Base64UrlEncode(const QByteArray& data) const;
	QByteArray Base64UrlDecode(const QByteArray& data) const;
	QByteArray SignWithRs256(const QByteArray& data) const;
	bool VerifyWithRs256(const QByteArray& data, const QByteArray& signature) const;
	bool LoadPrivateKey();
	bool ExtractPublicKeyComponents(QByteArray& modulus, QByteArray& exponent) const;

private:
	I_REF(ifile::IFileNameParam, m_privateKeyPathCompPtr);
	I_ATTR(QByteArray, m_keyIdAttrPtr);
	I_ATTR(int, m_tokenLifetimeAttrPtr);
	I_REF(iprm::INameParam, m_issuerCompPtr);

	mutable QByteArray m_privateKeyPem;
	mutable QByteArray m_cachedJwks;
};


} // namespace imtauth
