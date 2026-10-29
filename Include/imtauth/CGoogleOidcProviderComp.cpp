// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauth/CGoogleOidcProviderComp.h>


namespace imtauth
{


// public methods

// reimplemented (imtauth::IExternalOidcProvider)

QByteArray CGoogleOidcProviderComp::GetProviderType() const
{
	return QByteArrayLiteral("Google");
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
	// The ID token carries 'email' and 'email_verified' for the 'email' scope.
	return QString();
}


QStringList CGoogleOidcProviderComp::GetIssuers() const
{
	return QStringList() << QStringLiteral("https://accounts.google.com") << QStringLiteral("accounts.google.com");
}


QString CGoogleOidcProviderComp::GetSubjectClaim() const
{
	return QStringLiteral("sub");
}


bool CGoogleOidcProviderComp::IsOpenIdConnect() const
{
	return true;
}


bool CGoogleOidcProviderComp::IsPkceSupported() const
{
	return true;
}


bool CGoogleOidcProviderComp::IsEmailVerifiedByProvider() const
{
	return false;
}


// protected methods

// reimplemented (CExternalOidcProviderCompBase)

QByteArray CGoogleOidcProviderComp::GetDefaultProviderId() const
{
	return QByteArrayLiteral("google");
}


QString CGoogleOidcProviderComp::GetDefaultProviderName() const
{
	return QStringLiteral("Google");
}


QString CGoogleOidcProviderComp::GetDefaultScopes() const
{
	return QStringLiteral("openid email profile");
}


} // namespace imtauth


