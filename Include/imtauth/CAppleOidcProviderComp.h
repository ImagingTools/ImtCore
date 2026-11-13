// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtauth/CExternalOidcProviderCompBase.h>


namespace imtauth
{


/**
	"Sign in with Apple" (OpenID Connect). ClientId is the Services ID; ClientSecret is the
	ES256-signed client secret JWT generated from the Sign in with Apple key (valid for at most
	6 months, it has to be renewed before it expires).
	Apple posts the callback as a form (response_mode=form_post) when name or e-mail is requested.
*/
class CAppleOidcProviderComp: public CExternalOidcProviderCompBase
{
public:
	typedef CExternalOidcProviderCompBase BaseClass;

	I_BEGIN_COMPONENT(CAppleOidcProviderComp);
	I_END_COMPONENT;

	// reimplemented (imtauth::IExternalOidcProvider)
	virtual QByteArray GetProviderType() const override;
	virtual QString GetAuthorizationEndpoint() const override;
	virtual QString GetTokenEndpoint() const override;
	virtual QString GetUserInfoEndpoint() const override;
	virtual QStringList GetIssuers() const override;
	virtual QueryItems GetAuthorizationParams() const override;
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


