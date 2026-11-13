// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtauth/CExternalOidcProviderCompBase.h>


namespace imtauth
{


/**
	"Continue with Facebook" (OAuth 2.0 with the Graph API, Facebook issues no ID token in the web flow).
	ClientId is the App ID, ClientSecret the App Secret.
*/
class CFacebookOidcProviderComp: public CExternalOidcProviderCompBase
{
public:
	typedef CExternalOidcProviderCompBase BaseClass;

	I_BEGIN_COMPONENT(CFacebookOidcProviderComp);
		I_ASSIGN(m_graphApiVersionAttrPtr, "GraphApiVersion", "Graph API version, e.g. v22.0", false, "v22.0");
	I_END_COMPONENT;

	// reimplemented (imtauth::IExternalOidcProvider)
	virtual QByteArray GetProviderType() const override;
	virtual QString GetAuthorizationEndpoint() const override;
	virtual QString GetTokenEndpoint() const override;
	virtual QString GetUserInfoEndpoint() const override;
	virtual QStringList GetIssuers() const override;
	virtual QString GetSubjectClaim() const override;
	virtual bool IsOpenIdConnect() const override;
	virtual bool IsPkceSupported() const override;
	virtual bool IsEmailVerifiedByProvider() const override;

protected:
	// reimplemented (CExternalOidcProviderCompBase)
	virtual QByteArray GetDefaultProviderId() const override;
	virtual QString GetDefaultProviderName() const override;
	virtual QString GetDefaultScopes() const override;

private:
	QString GetGraphApiVersion() const;

private:
	I_ATTR(QByteArray, m_graphApiVersionAttrPtr);
};


} // namespace imtauth


