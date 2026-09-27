// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauth/CGoogleOidcProviderComp.h>


namespace imtauth
{


// reimplemented (imtauth::IExternalOidcProvider)

QByteArray CGoogleOidcProviderComp::GetProviderId() const
{
	return QByteArrayLiteral("google");
}


QString CGoogleOidcProviderComp::GetDiscoveryUrl() const
{
	return QStringLiteral("https://accounts.google.com/.well-known/openid-configuration");
}


QString CGoogleOidcProviderComp::GetAuthorizationEndpoint() const
{
	return QStringLiteral("https://accounts.google.com/o/oauth2/v2/auth");
}


QString CGoogleOidcProviderComp::GetTokenEndpoint() const
{
	return QStringLiteral("https://oauth2.googleapis.com/token");
}


QString CGoogleOidcProviderComp::GetUserInfoEndpoint() const
{
	return QStringLiteral("https://openidconnect.googleapis.com/v1/userinfo");
}


QByteArray CGoogleOidcProviderComp::GetClientId() const
{
	return *m_clientIdAttrPtr;
}


QByteArray CGoogleOidcProviderComp::GetClientSecret() const
{
	return *m_clientSecretAttrPtr;
}


QString CGoogleOidcProviderComp::GetScopes() const
{
	return QString::fromUtf8(*m_scopesAttrPtr);
}


QString CGoogleOidcProviderComp::GetRedirectUri() const
{
	return QString::fromUtf8(*m_redirectUriAttrPtr);
}


} // namespace imtauth
