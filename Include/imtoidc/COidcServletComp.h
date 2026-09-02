// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>

// ACF includes
#include <ilog/TLoggerCompWrap.h>

// ImtCore includes
#include <imtrest/CHttpServletCompBase.h>
#include <imtrest/IProtocolEngine.h>
#include <imtauth/IJwtTokenProvider.h>
#include <imtauth/IOidcScopeMapper.h>
#include <imtauth/ICredentialController.h>
#include <imtauth/IUserInfoProvider.h>
#include <imtbase/IObjectCollection.h>


namespace imtoidc
{


/**
	OIDC servlet component providing OpenID Connect REST endpoints.

	Handles the following endpoints:
	- GET  /.well-known/openid-configuration — Discovery document
	- GET  /oauth/authorize — Authorization endpoint
	- POST /oauth/token — Token endpoint
	- GET  /oauth/userinfo — UserInfo endpoint
	- GET  /oauth/jwks — JWKS endpoint
	- POST /oauth/revoke — Token revocation (RFC 7009)
	- POST /oauth/introspect — Token introspection (RFC 7662)

	\ingroup OIDC
*/
class COidcServletComp: public imtrest::CHttpServletCompBase
{
public:
	typedef imtrest::CHttpServletCompBase BaseClass;

	I_BEGIN_COMPONENT(COidcServletComp);
		I_ASSIGN(m_jwtTokenProviderCompPtr, "JwtTokenProvider", "RS256 JWT token provider", true, "JwtTokenProvider");
		I_ASSIGN(m_scopeMapperCompPtr, "ScopeMapper", "OIDC scope to claims mapper", true, "ScopeMapper");
		I_ASSIGN(m_credentialControllerCompPtr, "CredentialController", "Credential controller for user authentication", true, "CredentialController");
		I_ASSIGN(m_userInfoProviderCompPtr, "UserInfoProvider", "User info provider", true, "UserInfoProvider");
		I_ASSIGN(m_clientCollectionCompPtr, "ClientCollection", "OIDC client collection", true, "OidcClientCollection");
		I_ASSIGN(m_authCodeCollectionCompPtr, "AuthCodeCollection", "OIDC authorization code collection", true, "OidcAuthCodeCollection");
		I_ASSIGN(m_tokenCollectionCompPtr, "TokenCollection", "OIDC token collection", true, "OidcTokenCollection");
		I_ASSIGN(m_issuerUrlAttrPtr, "IssuerUrl", "OIDC issuer URL (e.g. https://example.com)", true, "");
	I_END_COMPONENT;

	// reimplemented (imtrest::IRequestServlet)
	virtual bool IsCommandSupported(const QByteArray& commandId) const override;

protected:
	// reimplemented (imtrest::CHttpServletCompBase)
	virtual ConstResponsePtr OnGet(
				const QByteArray& commandId,
				const imtrest::IRequest::CommandParams& commandParams,
				const HeadersMap& headers,
				const imtrest::CHttpRequest& request) const override;
	virtual ConstResponsePtr OnPost(
				const QByteArray& commandId,
				const imtrest::IRequest::CommandParams& commandParams,
				const HeadersMap& headers,
				const imtrest::CHttpRequest& request) const override;

private:
	// Endpoint handlers
	ConstResponsePtr HandleDiscovery(const imtrest::CHttpRequest& request) const;
	ConstResponsePtr HandleAuthorize(
				const imtrest::IRequest::CommandParams& commandParams,
				const HeadersMap& headers,
				const imtrest::CHttpRequest& request) const;
	ConstResponsePtr HandleToken(
				const HeadersMap& headers,
				const imtrest::CHttpRequest& request) const;
	ConstResponsePtr HandleUserInfo(
				const HeadersMap& headers,
				const imtrest::CHttpRequest& request) const;
	ConstResponsePtr HandleJwks(const imtrest::CHttpRequest& request) const;
	ConstResponsePtr HandleRevoke(
				const HeadersMap& headers,
				const imtrest::CHttpRequest& request) const;
	ConstResponsePtr HandleIntrospect(
				const HeadersMap& headers,
				const imtrest::CHttpRequest& request) const;

	// Helper methods
	ConstResponsePtr CreateJsonResponse(
				const QJsonObject& json,
				imtrest::IProtocolEngine::StatusCode statusCode,
				const imtrest::CHttpRequest& request) const;
	ConstResponsePtr CreateErrorResponse(
				const QString& error,
				const QString& description,
				imtrest::IProtocolEngine::StatusCode statusCode,
				const imtrest::CHttpRequest& request) const;
	QByteArray ExtractBearerToken(const HeadersMap& headers) const;
	QMap<QByteArray, QByteArray> ParseFormUrlEncoded(const QByteArray& body) const;
	bool ValidateClient(const QByteArray& clientId, const QByteArray& clientSecret) const;
	QByteArray ExtractCommandEndpoint(const QByteArray& commandId) const;

	// Token grant handlers
	ConstResponsePtr HandleAuthorizationCodeGrant(
				const QMap<QByteArray, QByteArray>& params,
				const QByteArray& clientId,
				const imtrest::CHttpRequest& request) const;
	ConstResponsePtr HandleClientCredentialsGrant(
				const QMap<QByteArray, QByteArray>& params,
				const QByteArray& clientId,
				const imtrest::CHttpRequest& request) const;
	ConstResponsePtr HandleRefreshTokenGrant(
				const QMap<QByteArray, QByteArray>& params,
				const QByteArray& clientId,
				const imtrest::CHttpRequest& request) const;

private:
	I_REF(imtauth::IJwtTokenProvider, m_jwtTokenProviderCompPtr);
	I_REF(imtauth::IOidcScopeMapper, m_scopeMapperCompPtr);
	I_REF(imtauth::ICredentialController, m_credentialControllerCompPtr);
	I_REF(imtauth::IUserInfoProvider, m_userInfoProviderCompPtr);
	I_REF(imtbase::IObjectCollection, m_clientCollectionCompPtr);
	I_REF(imtbase::IObjectCollection, m_authCodeCollectionCompPtr);
	I_REF(imtbase::IObjectCollection, m_tokenCollectionCompPtr);
	I_ATTR(QByteArray, m_issuerUrlAttrPtr);
};


} // namespace imtoidc


