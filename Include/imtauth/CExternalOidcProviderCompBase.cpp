// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauth/CExternalOidcProviderCompBase.h>


namespace imtauth
{


// public methods

// reimplemented (imtauth::IExternalOidcProvider)

QByteArray CExternalOidcProviderCompBase::GetProviderId() const
{
	if (m_providerIdAttrPtr.IsValid() && !(*m_providerIdAttrPtr).isEmpty()){
		return *m_providerIdAttrPtr;
	}

	return GetDefaultProviderId();
}


QString CExternalOidcProviderCompBase::GetProviderName() const
{
	if (m_providerNameAttrPtr.IsValid() && !(*m_providerNameAttrPtr).isEmpty()){
		return *m_providerNameAttrPtr;
	}

	return GetDefaultProviderName();
}


QByteArray CExternalOidcProviderCompBase::GetClientId() const
{
	if (m_clientIdParamCompPtr.IsValid()){
		const QString clientId = m_clientIdParamCompPtr->GetText().trimmed();
		if (!clientId.isEmpty()){
			return clientId.toUtf8();
		}
	}

	return m_clientIdAttrPtr.IsValid() ? *m_clientIdAttrPtr : QByteArray();
}


QByteArray CExternalOidcProviderCompBase::GetClientSecret() const
{
	if (m_clientSecretParamCompPtr.IsValid()){
		const QString clientSecret = m_clientSecretParamCompPtr->GetText().trimmed();
		if (!clientSecret.isEmpty()){
			return clientSecret.toUtf8();
		}
	}

	return m_clientSecretAttrPtr.IsValid() ? (*m_clientSecretAttrPtr).toUtf8() : QByteArray();
}


QString CExternalOidcProviderCompBase::GetScopes() const
{
	if (m_scopesAttrPtr.IsValid() && !(*m_scopesAttrPtr).isEmpty()){
		return *m_scopesAttrPtr;
	}

	return GetDefaultScopes();
}


IExternalOidcProvider::QueryItems CExternalOidcProviderCompBase::GetAuthorizationParams() const
{
	return QueryItems();
}


} // namespace imtauth


