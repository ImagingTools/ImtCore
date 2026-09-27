// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauth/CAppleOidcProviderComp.h>


namespace imtauth
{


// reimplemented (imtauth::IExternalOidcProvider)

QByteArray CAppleOidcProviderComp::GetProviderId() const
{
	return QByteArrayLiteral("apple");
}


QString CAppleOidcProviderComp::GetDiscoveryUrl() const
{
	return QStringLiteral("https://appleid.apple.com/.well-known/openid-configuration");
}


QString CAppleOidcProviderComp::GetAuthorizationEndpoint() const
{
	return QStringLiteral("https://appleid.apple.com/auth/authorize");
}


QString CAppleOidcProviderComp::GetTokenEndpoint() const
{
	return QStringLiteral("https://appleid.apple.com/auth/token");
}


QString CAppleOidcProviderComp::GetUserInfoEndpoint() const
{
	// Apple does not provide a standard userinfo endpoint;
	// user data is embedded in the id_token
	return QString();
}


QByteArray CAppleOidcProviderComp::GetClientId() const
{
	return *m_clientIdAttrPtr;
}


QByteArray CAppleOidcProviderComp::GetClientSecret() const
{
	return *m_clientSecretAttrPtr;
}


QString CAppleOidcProviderComp::GetScopes() const
{
	return QString::fromUtf8(*m_scopesAttrPtr);
}


QString CAppleOidcProviderComp::GetRedirectUri() const
{
	return QString::fromUtf8(*m_redirectUriAttrPtr);
}


} // namespace imtauth
