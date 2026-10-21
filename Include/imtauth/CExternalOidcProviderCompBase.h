// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <icomp/CComponentBase.h>
#include <iprm/ITextParam.h>

// ImtCore includes
#include <imtauth/IExternalOidcProvider.h>


namespace imtauth
{


/**
	Common part of the external provider components: client registration and the attributes
	every provider has. Brand components only describe the endpoints of their provider.
*/
class CExternalOidcProviderCompBase:
			public icomp::CComponentBase,
			virtual public IExternalOidcProvider
{
public:
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_BASE_COMPONENT(CExternalOidcProviderCompBase);
		I_REGISTER_INTERFACE(IExternalOidcProvider);
		I_ASSIGN(m_providerIdAttrPtr, "ProviderId", "Provider ID used in the API and stored with linked accounts, the brand default if not set", false, "");
		I_ASSIGN(m_providerNameAttrPtr, "ProviderName", "Name shown on the sign-in button, the brand name if not set", false, "");
		I_ASSIGN(m_clientIdAttrPtr, "ClientId", "Client ID registered at the provider, the provider is not offered without it", false, "");
		I_ASSIGN(m_clientSecretAttrPtr, "ClientSecret", "Client secret registered at the provider", false, "");
		I_ASSIGN(m_clientIdParamCompPtr, "ClientIdParam", "Client ID from the settings, used instead of ClientId if not empty", false, "ClientIdParam");
		I_ASSIGN(m_clientSecretParamCompPtr, "ClientSecretParam", "Client secret from the settings, used instead of ClientSecret if not empty", false, "ClientSecretParam");
		I_ASSIGN(m_scopesAttrPtr, "Scopes", "Space-separated scopes, the brand default if not set", false, "");
	I_END_COMPONENT;

	// reimplemented (imtauth::IExternalOidcProvider)
	virtual QByteArray GetProviderId() const override;
	virtual QString GetProviderName() const override;
	virtual QByteArray GetClientId() const override;
	virtual QByteArray GetClientSecret() const override;
	virtual QString GetScopes() const override;
	virtual QueryItems GetAuthorizationParams() const override;

protected:
	virtual QByteArray GetDefaultProviderId() const = 0;
	virtual QString GetDefaultProviderName() const = 0;
	virtual QString GetDefaultScopes() const = 0;

private:
	I_ATTR(QByteArray, m_providerIdAttrPtr);
	I_TEXTATTR(m_providerNameAttrPtr);
	I_ATTR(QByteArray, m_clientIdAttrPtr);
	I_ATTR(QString, m_clientSecretAttrPtr);
	I_REF(iprm::ITextParam, m_clientIdParamCompPtr);
	I_REF(iprm::ITextParam, m_clientSecretParamCompPtr);
	I_ATTR(QString, m_scopesAttrPtr);
};


} // namespace imtauth


