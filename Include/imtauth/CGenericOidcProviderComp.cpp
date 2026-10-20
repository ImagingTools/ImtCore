// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauth/CGenericOidcProviderComp.h>


namespace imtauth
{


// public methods

// reimplemented (imtauth::IExternalOidcProvider)

QString CGenericOidcProviderComp::GetProviderName() const
{
	const QString providerName = GetParamText(m_providerNameParamCompPtr.GetPtr());
	if (!providerName.isEmpty()){
		return providerName;
	}

	return BaseClass::GetProviderName();
}


QByteArray CGenericOidcProviderComp::GetProviderType() const
{
	return QByteArrayLiteral("Generic");
}


QString CGenericOidcProviderComp::GetAuthorizationEndpoint() const
{
	const QString endpoint = GetParamText(m_authorizationEndpointParamCompPtr.GetPtr());

	return endpoint.isEmpty() ? *m_authorizationEndpointAttrPtr : endpoint;
}


QString CGenericOidcProviderComp::GetTokenEndpoint() const
{
	const QString endpoint = GetParamText(m_tokenEndpointParamCompPtr.GetPtr());

	return endpoint.isEmpty() ? *m_tokenEndpointAttrPtr : endpoint;
}


QString CGenericOidcProviderComp::GetUserInfoEndpoint() const
{
	const QString endpoint = GetParamText(m_userInfoEndpointParamCompPtr.GetPtr());
	if (!endpoint.isEmpty()){
		return endpoint;
	}

	return m_userInfoEndpointAttrPtr.IsValid() ? *m_userInfoEndpointAttrPtr : QString();
}


QStringList CGenericOidcProviderComp::GetIssuers() const
{
	const QString issuer = GetParamText(m_issuerParamCompPtr.GetPtr());
	if (!issuer.isEmpty()){
		return QStringList() << issuer;
	}

	QStringList retVal;
	for (int i = 0; i < m_issuersAttrPtr.GetCount(); ++i){
		retVal.append(m_issuersAttrPtr[i]);
	}

	return retVal;
}


QString CGenericOidcProviderComp::GetSubjectClaim() const
{
	if (m_subjectClaimAttrPtr.IsValid() && !(*m_subjectClaimAttrPtr).isEmpty()){
		return *m_subjectClaimAttrPtr;
	}

	return QStringLiteral("sub");
}


bool CGenericOidcProviderComp::IsOpenIdConnect() const
{
	return *m_isOpenIdConnectAttrPtr;
}


bool CGenericOidcProviderComp::IsPkceSupported() const
{
	return *m_isPkceSupportedAttrPtr;
}


bool CGenericOidcProviderComp::IsEmailVerifiedByProvider() const
{
	return *m_isEmailVerifiedByProviderAttrPtr;
}


// protected methods

// reimplemented (CExternalOidcProviderCompBase)

QByteArray CGenericOidcProviderComp::GetDefaultProviderId() const
{
	return QByteArrayLiteral("oidc");
}


QString CGenericOidcProviderComp::GetDefaultProviderName() const
{
	return QStringLiteral("OpenID Connect");
}


QString CGenericOidcProviderComp::GetDefaultScopes() const
{
	return QStringLiteral("openid email profile");
}


// private static methods

QString CGenericOidcProviderComp::GetParamText(const iprm::ITextParam* paramPtr)
{
	return (paramPtr != nullptr) ? paramPtr->GetText().trimmed() : QString();
}


} // namespace imtauth


