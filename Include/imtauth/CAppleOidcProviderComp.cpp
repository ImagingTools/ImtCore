// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauth/CAppleOidcProviderComp.h>


namespace imtauth
{


// public methods

// reimplemented (imtauth::IExternalOidcProvider)

QByteArray CAppleOidcProviderComp::GetProviderType() const
{
	return QByteArrayLiteral("Apple");
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
	// Apple has no user info endpoint, the claims are in the ID token.
	return QString();
}


QStringList CAppleOidcProviderComp::GetIssuers() const
{
	return QStringList() << QStringLiteral("https://appleid.apple.com");
}


IExternalOidcProvider::QueryItems CAppleOidcProviderComp::GetAuthorizationParams() const
{
	QueryItems retVal;
	retVal.append(qMakePair(QStringLiteral("response_mode"), QStringLiteral("form_post")));

	return retVal;
}


QString CAppleOidcProviderComp::GetSubjectClaim() const
{
	return QStringLiteral("sub");
}


bool CAppleOidcProviderComp::IsOpenIdConnect() const
{
	return true;
}


bool CAppleOidcProviderComp::IsPkceSupported() const
{
	return false;
}


bool CAppleOidcProviderComp::IsEmailVerifiedByProvider() const
{
	return false;
}


// protected methods

// reimplemented (CExternalOidcProviderCompBase)

QByteArray CAppleOidcProviderComp::GetDefaultProviderId() const
{
	return QByteArrayLiteral("apple");
}


QString CAppleOidcProviderComp::GetDefaultProviderName() const
{
	return QStringLiteral("Apple");
}


QString CAppleOidcProviderComp::GetDefaultScopes() const
{
	return QStringLiteral("openid email name");
}


} // namespace imtauth


