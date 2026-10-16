// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <icomp/CComponentBase.h>

// ImtCore includes
#include <imtauth/IExternalOidcProvider.h>


namespace imtauth
{


/**
	Facebook OIDC provider component.

	Configures Facebook as an external OAuth/OIDC provider.
	Facebook does not follow standard OIDC discovery — uses the Graph API:
	- Token endpoint: https://graph.facebook.com/v18.0/oauth/access_token
	- UserInfo: https://graph.facebook.com/me?fields=id,name,email

	\ingroup Authorization
*/
class CFacebookOidcProviderComp:
			public icomp::CComponentBase,
			virtual public imtauth::IExternalOidcProvider
{
public:
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_COMPONENT(CFacebookOidcProviderComp);
		I_REGISTER_INTERFACE(imtauth::IExternalOidcProvider);
		I_ASSIGN(m_clientIdAttrPtr, "ClientId", "Facebook App ID", true, "");
		I_ASSIGN(m_clientSecretAttrPtr, "ClientSecret", "Facebook App Secret", true, "");
		I_ASSIGN(m_redirectUriAttrPtr, "RedirectUri", "Callback URL for Facebook auth", true, "");
		I_ASSIGN(m_scopesAttrPtr, "Scopes", "Requested scopes", false, "openid email public_profile");
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
