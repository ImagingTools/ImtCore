// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtauth/CExternalOidcProviderCompBase.h>


namespace imtauth
{


/**
	Any OpenID Connect or OAuth 2.0 provider (Keycloak, Microsoft Entra ID, GitLab, ...)
	described completely by its attributes.
*/
class CGenericOidcProviderComp: public CExternalOidcProviderCompBase
{
public:
	typedef CExternalOidcProviderCompBase BaseClass;

	I_BEGIN_COMPONENT(CGenericOidcProviderComp);
		I_ASSIGN(m_authorizationEndpointAttrPtr, "AuthorizationEndpoint", "Authorization endpoint URL", true, "");
		I_ASSIGN(m_tokenEndpointAttrPtr, "TokenEndpoint", "Token endpoint URL", true, "");
		I_ASSIGN(m_userInfoEndpointAttrPtr, "UserInfoEndpoint", "User info endpoint URL, claims are taken from the ID token if not set", false, "");
		I_ASSIGN_MULTI_0(m_issuersAttrPtr, "Issuers", "Accepted 'iss' claim values of the ID token", false);
		I_ASSIGN(m_subjectClaimAttrPtr, "SubjectClaim", "Claim holding the account identifier", false, "sub");
		I_ASSIGN(m_isOpenIdConnectAttrPtr, "OpenIdConnect", "Provider issues an ID token (OpenID Connect)", true, true);
		I_ASSIGN(m_isPkceSupportedAttrPtr, "PkceSupported", "Provider supports PKCE (RFC 7636)", true, true);
		I_ASSIGN(m_isEmailVerifiedByProviderAttrPtr, "EmailVerifiedByProvider", "Every e-mail returned by the provider is verified", true, false);
		I_ASSIGN(m_providerNameParamCompPtr, "ProviderNameParam", "Provider name from the settings, used instead of ProviderName if not empty", false, "ProviderNameParam");
		I_ASSIGN(m_authorizationEndpointParamCompPtr, "AuthorizationEndpointParam", "Authorization endpoint from the settings, used instead of AuthorizationEndpoint if not empty", false, "AuthorizationEndpointParam");
		I_ASSIGN(m_tokenEndpointParamCompPtr, "TokenEndpointParam", "Token endpoint from the settings, used instead of TokenEndpoint if not empty", false, "TokenEndpointParam");
		I_ASSIGN(m_userInfoEndpointParamCompPtr, "UserInfoEndpointParam", "User info endpoint from the settings, used instead of UserInfoEndpoint if not empty", false, "UserInfoEndpointParam");
		I_ASSIGN(m_issuerParamCompPtr, "IssuerParam", "Issuer from the settings, used instead of Issuers if not empty", false, "IssuerParam");
	I_END_COMPONENT;

	// reimplemented (imtauth::IExternalOidcProvider)
	virtual QString GetProviderName() const override;
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
	static QString GetParamText(const iprm::ITextParam* paramPtr);

private:
	I_REF(iprm::ITextParam, m_providerNameParamCompPtr);
	I_REF(iprm::ITextParam, m_authorizationEndpointParamCompPtr);
	I_REF(iprm::ITextParam, m_tokenEndpointParamCompPtr);
	I_REF(iprm::ITextParam, m_userInfoEndpointParamCompPtr);
	I_REF(iprm::ITextParam, m_issuerParamCompPtr);
	I_ATTR(QString, m_authorizationEndpointAttrPtr);
	I_ATTR(QString, m_tokenEndpointAttrPtr);
	I_ATTR(QString, m_userInfoEndpointAttrPtr);
	I_MULTIATTR(QString, m_issuersAttrPtr);
	I_ATTR(QString, m_subjectClaimAttrPtr);
	I_ATTR(bool, m_isOpenIdConnectAttrPtr);
	I_ATTR(bool, m_isPkceSupportedAttrPtr);
	I_ATTR(bool, m_isEmailVerifiedByProviderAttrPtr);
};


} // namespace imtauth


