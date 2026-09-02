// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauth/CFacebookOidcProviderComp.h>


namespace imtauth
{


// reimplemented (imtauth::IExternalOidcProvider)

QByteArray CFacebookOidcProviderComp::GetProviderId() const
{
	return QByteArrayLiteral("facebook");
}


QString CFacebookOidcProviderComp::GetDiscoveryUrl() const
{
	// Facebook does not support standard OIDC discovery
	return QString();
}


QString CFacebookOidcProviderComp::GetAuthorizationEndpoint() const
{
	return QStringLiteral("https://www.facebook.com/v18.0/dialog/oauth");
}


QString CFacebookOidcProviderComp::GetTokenEndpoint() const
{
	return QStringLiteral("https://graph.facebook.com/v18.0/oauth/access_token");
}


QString CFacebookOidcProviderComp::GetUserInfoEndpoint() const
{
	return QStringLiteral("https://graph.facebook.com/me?fields=id,name,email");
}


QByteArray CFacebookOidcProviderComp::GetClientId() const
{
	return *m_clientIdAttrPtr;
}


QByteArray CFacebookOidcProviderComp::GetClientSecret() const
{
	return *m_clientSecretAttrPtr;
}


QString CFacebookOidcProviderComp::GetScopes() const
{
	return QString::fromUtf8(*m_scopesAttrPtr);
}


QString CFacebookOidcProviderComp::GetRedirectUri() const
{
	return QString::fromUtf8(*m_redirectUriAttrPtr);
}


} // namespace imtauth
