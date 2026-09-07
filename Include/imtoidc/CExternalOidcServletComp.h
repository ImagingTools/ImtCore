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
#include <imtauth/IExternalOidcAuthController.h>
#include <imtauth/IJwtSessionController.h>


namespace imtoidc
{


/**
	External OIDC servlet component providing REST endpoints for external provider authentication.

	Handles the following endpoints:
	- GET  /oauth/external/providers — List available external providers
	- GET  /oauth/external/authorize/{provider} — Redirect to external provider
	- GET  /oauth/external/callback — Callback handler (code exchange + session creation)
	- POST /oauth/external/unlink/{provider} — Unlink external provider
	- GET  /oauth/external/identities — List linked providers for current user

	\ingroup OIDC
*/
class CExternalOidcServletComp: public imtrest::CHttpServletCompBase
{
public:
	typedef imtrest::CHttpServletCompBase BaseClass;

	I_BEGIN_COMPONENT(CExternalOidcServletComp);
		I_ASSIGN(m_authControllerCompPtr, "AuthController", "External OIDC auth controller", true, "ExternalOidcAuthController");
		I_ASSIGN(m_sessionControllerCompPtr, "SessionController", "JWT session controller for token validation", true, "JwtSessionController");
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
	ConstResponsePtr HandleListProviders(const imtrest::CHttpRequest& request) const;
	ConstResponsePtr HandleAuthorize(
				const imtrest::IRequest::CommandParams& commandParams,
				const imtrest::CHttpRequest& request) const;
	ConstResponsePtr HandleCallback(
				const imtrest::IRequest::CommandParams& commandParams,
				const imtrest::CHttpRequest& request) const;
	ConstResponsePtr HandleUnlink(
				const imtrest::IRequest::CommandParams& commandParams,
				const HeadersMap& headers,
				const imtrest::CHttpRequest& request) const;
	ConstResponsePtr HandleListIdentities(
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
	QByteArray ExtractCommandEndpoint(const QByteArray& commandId) const;

private:
	I_REF(imtauth::IExternalOidcAuthController, m_authControllerCompPtr);
	I_REF(imtauth::IJwtSessionController, m_sessionControllerCompPtr);
};


} // namespace imtoidc
