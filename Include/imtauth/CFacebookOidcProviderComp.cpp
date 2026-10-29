// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauth/CFacebookOidcProviderComp.h>


namespace imtauth
{


// public methods

// reimplemented (imtauth::IExternalOidcProvider)

QByteArray CFacebookOidcProviderComp::GetProviderType() const
{
	return QByteArrayLiteral("Facebook");
}


QString CFacebookOidcProviderComp::GetAuthorizationEndpoint() const
{
	return QStringLiteral("https://www.facebook.com/%1/dialog/oauth").arg(GetGraphApiVersion());
}


QString CFacebookOidcProviderComp::GetTokenEndpoint() const
{
	return QStringLiteral("https://graph.facebook.com/%1/oauth/access_token").arg(GetGraphApiVersion());
}


QString CFacebookOidcProviderComp::GetUserInfoEndpoint() const
{
	return QStringLiteral("https://graph.facebook.com/%1/me?fields=id,name,email").arg(GetGraphApiVersion());
}


QStringList CFacebookOidcProviderComp::GetIssuers() const
{
	return QStringList();
}


QString CFacebookOidcProviderComp::GetSubjectClaim() const
{
	return QStringLiteral("id");
}


bool CFacebookOidcProviderComp::IsOpenIdConnect() const
{
	return false;
}


bool CFacebookOidcProviderComp::IsPkceSupported() const
{
	return false;
}


bool CFacebookOidcProviderComp::IsEmailVerifiedByProvider() const
{
	// The Graph API returns only confirmed e-mail addresses.
	return true;
}


// protected methods

// reimplemented (CExternalOidcProviderCompBase)

QByteArray CFacebookOidcProviderComp::GetDefaultProviderId() const
{
	return QByteArrayLiteral("facebook");
}


QString CFacebookOidcProviderComp::GetDefaultProviderName() const
{
	return QStringLiteral("Facebook");
}


QString CFacebookOidcProviderComp::GetDefaultScopes() const
{
	return QStringLiteral("email public_profile");
}


// private methods

QString CFacebookOidcProviderComp::GetGraphApiVersion() const
{
	if (m_graphApiVersionAttrPtr.IsValid() && !(*m_graphApiVersionAttrPtr).isEmpty()){
		return QString::fromUtf8(*m_graphApiVersionAttrPtr);
	}

	return QStringLiteral("v22.0");
}


} // namespace imtauth


