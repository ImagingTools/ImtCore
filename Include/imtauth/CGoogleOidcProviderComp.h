// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtauth/CExternalOidcProviderCompBase.h>


namespace imtauth
{


/**
	"Sign in with Google" (OpenID Connect). The client is registered in the Google Cloud Console
	as a "Web application" with the server callback URL as authorized redirect URI.
*/
class CGoogleOidcProviderComp: public CExternalOidcProviderCompBase
{
public:
	typedef CExternalOidcProviderCompBase BaseClass;

	I_BEGIN_COMPONENT(CGoogleOidcProviderComp);
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
};


} // namespace imtauth


