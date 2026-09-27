// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <icomp/CComponentBase.h>

// ImtCore includes
#include <imtauth/IExternalOidcProvider.h>


namespace imtauth
{


/**
	Apple OIDC provider component.

	Configures Apple as an external OpenID Connect provider with:
	- Discovery: https://appleid.apple.com/.well-known/openid-configuration
	- Scopes: openid name email
	- Note: Apple uses client_secret_jwt with ES256 for token exchange

	\ingroup Authorization
*/
class CAppleOidcProviderComp:
			public icomp::CComponentBase,
			virtual public imtauth::IExternalOidcProvider
{
public:
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_COMPONENT(CAppleOidcProviderComp);
		I_REGISTER_INTERFACE(imtauth::IExternalOidcProvider);
		I_ASSIGN(m_clientIdAttrPtr, "ClientId", "Apple Services ID (com.example.app)", true, "");
		I_ASSIGN(m_clientSecretAttrPtr, "ClientSecret", "Apple client secret (JWT or key data)", true, "");
		I_ASSIGN(m_redirectUriAttrPtr, "RedirectUri", "Callback URL for Apple auth", true, "");
		I_ASSIGN(m_scopesAttrPtr, "Scopes", "Requested scopes", false, "openid name email");
	I_END_COMPONENT;

	// reimplemented (imtauth::IExternalOidcProvider)
	virtual QByteArray GetProviderId() const override;
	virtual QString GetDiscoveryUrl() const override;
	virtual QString GetAuthorizationEndpoint() const override;
	virtual QString GetTokenEndpoint() const override;
	virtual QString GetUserInfoEndpoint() const override;
	virtual QByteArray GetClientId() const override;
	virtual QByteArray GetClientSecret() const override;
	virtual QString GetScopes() const override;
	virtual QString GetRedirectUri() const override;

private:
	I_ATTR(QByteArray, m_clientIdAttrPtr);
	I_ATTR(QByteArray, m_clientSecretAttrPtr);
	I_ATTR(QByteArray, m_redirectUriAttrPtr);
	I_ATTR(QByteArray, m_scopesAttrPtr);
};


} // namespace imtauth
