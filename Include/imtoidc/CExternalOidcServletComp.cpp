// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtoidc/CExternalOidcServletComp.h>

// Qt includes
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QJsonArray>
#include <QtCore/QUuid>
#include <QtCore/QUrl>
#include <QtCore/QUrlQuery>
#include <QtCore/QCryptographicHash>

// ImtCore includes
#include <imtrest/CHttpResponse.h>
#include <imtrest/IProtocolEngine.h>


namespace imtoidc
{


// public methods

// reimplemented (imtrest::IRequestServlet)

bool CExternalOidcServletComp::IsCommandSupported(const QByteArray& commandId) const
{
	QByteArray endpoint = ExtractCommandEndpoint(commandId);

	return endpoint == "providers"
		|| endpoint == "authorize"
		|| endpoint == "callback"
		|| endpoint == "unlink"
		|| endpoint == "identities";
}


// protected methods

// reimplemented (imtrest::CHttpServletCompBase)

CExternalOidcServletComp::ConstResponsePtr CExternalOidcServletComp::OnGet(
			const QByteArray& commandId,
			const imtrest::IRequest::CommandParams& commandParams,
			const HeadersMap& headers,
			const imtrest::CHttpRequest& request) const
{
	QByteArray endpoint = ExtractCommandEndpoint(commandId);

	if (endpoint == "providers"){
		return HandleListProviders(request);
	}

	if (endpoint == "authorize"){
		return HandleAuthorize(commandParams, request);
	}

	if (endpoint == "callback"){
		return HandleCallback(commandParams, request);
	}

	if (endpoint == "identities"){
		return HandleListIdentities(headers, request);
	}

	return CreateErrorResponse(QStringLiteral("invalid_request"), QStringLiteral("Unknown endpoint"),
				imtrest::IProtocolEngine::SC_NOT_FOUND, request);
}


CExternalOidcServletComp::ConstResponsePtr CExternalOidcServletComp::OnPost(
			const QByteArray& commandId,
			const imtrest::IRequest::CommandParams& commandParams,
			const HeadersMap& headers,
			const imtrest::CHttpRequest& request) const
{
	QByteArray endpoint = ExtractCommandEndpoint(commandId);

	if (endpoint == "unlink"){
		return HandleUnlink(commandParams, headers, request);
	}

	return CreateErrorResponse(QStringLiteral("invalid_request"), QStringLiteral("Unknown endpoint"),
				imtrest::IProtocolEngine::SC_NOT_FOUND, request);
}


// private methods - Endpoint handlers

CExternalOidcServletComp::ConstResponsePtr CExternalOidcServletComp::HandleListProviders(
			const imtrest::CHttpRequest& request) const
{
	if (!m_authControllerCompPtr.IsValid()){
		return CreateErrorResponse(QStringLiteral("server_error"), QStringLiteral("Auth controller not available"),
					imtrest::IProtocolEngine::SC_INTERNAL_ERROR, request);
	}

	QByteArrayList providerIds = m_authControllerCompPtr->GetAvailableProviders();

	QJsonArray providersArray;
	for (const QByteArray& id : ::std::as_const(providerIds)){
		QJsonObject providerObj;
		providerObj[QStringLiteral("id")] = QString::fromUtf8(id);
		providersArray.append(providerObj);
	}

	QJsonObject response;
	response[QStringLiteral("providers")] = providersArray;

	return CreateJsonResponse(response, imtrest::IProtocolEngine::SC_OK, request);
}


CExternalOidcServletComp::ConstResponsePtr CExternalOidcServletComp::HandleAuthorize(
			const imtrest::IRequest::CommandParams& commandParams,
			const imtrest::CHttpRequest& request) const
{
	if (!m_authControllerCompPtr.IsValid()){
		return CreateErrorResponse(QStringLiteral("server_error"), QStringLiteral("Auth controller not available"),
					imtrest::IProtocolEngine::SC_INTERNAL_ERROR, request);
	}

	QByteArray providerId = commandParams.value("provider");
	if (providerId.isEmpty()){
		return CreateErrorResponse(QStringLiteral("invalid_request"), QStringLiteral("Missing 'provider' parameter"),
					imtrest::IProtocolEngine::SC_BAD_REQUEST, request);
	}

	// Generate CSRF state and nonce
	QByteArray state = QCryptographicHash::hash(
		QUuid::createUuid().toByteArray(), QCryptographicHash::Sha256).toHex();
	QByteArray nonce = QCryptographicHash::hash(
		QUuid::createUuid().toByteArray(), QCryptographicHash::Sha256).toHex();

	QString authUrl = m_authControllerCompPtr->GetAuthorizationUrl(providerId, state, nonce);
	if (authUrl.isEmpty()){
		return CreateErrorResponse(QStringLiteral("invalid_request"), QStringLiteral("Unknown provider or configuration error"),
					imtrest::IProtocolEngine::SC_BAD_REQUEST, request);
	}

	QJsonObject response;
	response[QStringLiteral("authorization_url")] = authUrl;
	response[QStringLiteral("state")] = QString::fromUtf8(state);

	return CreateJsonResponse(response, imtrest::IProtocolEngine::SC_OK, request);
}


CExternalOidcServletComp::ConstResponsePtr CExternalOidcServletComp::HandleCallback(
			const imtrest::IRequest::CommandParams& commandParams,
			const imtrest::CHttpRequest& request) const
{
	if (!m_authControllerCompPtr.IsValid()){
		return CreateErrorResponse(QStringLiteral("server_error"), QStringLiteral("Auth controller not available"),
					imtrest::IProtocolEngine::SC_INTERNAL_ERROR, request);
	}

	QByteArray code = commandParams.value("code");
	QByteArray state = commandParams.value("state");
	QByteArray providerId = commandParams.value("provider");

	if (code.isEmpty() || providerId.isEmpty()){
		// Check for error response from provider
		QByteArray error = commandParams.value("error");
		if (!error.isEmpty()){
			QByteArray errorDesc = commandParams.value("error_description");
			return CreateErrorResponse(QString::fromUtf8(error), QString::fromUtf8(errorDesc),
						imtrest::IProtocolEngine::SC_BAD_REQUEST, request);
		}

		return CreateErrorResponse(QStringLiteral("invalid_request"), QStringLiteral("Missing 'code' or 'provider' parameter"),
					imtrest::IProtocolEngine::SC_BAD_REQUEST, request);
	}

	imtauth::IExternalOidcAuthController::AuthResult authResult =
		m_authControllerCompPtr->HandleCallback(providerId, code, state);

	if (!authResult.success){
		return CreateErrorResponse(QStringLiteral("authentication_failed"), authResult.errorMessage,
					imtrest::IProtocolEngine::SC_UNAUTHORIZED, request);
	}

	QJsonObject response;
	response[QStringLiteral("access_token")] = QString::fromUtf8(authResult.session.accessToken);
	response[QStringLiteral("refresh_token")] = QString::fromUtf8(authResult.session.refreshToken);
	response[QStringLiteral("user_id")] = QString::fromUtf8(authResult.userId);
	response[QStringLiteral("token_type")] = QStringLiteral("Bearer");

	return CreateJsonResponse(response, imtrest::IProtocolEngine::SC_OK, request);
}


CExternalOidcServletComp::ConstResponsePtr CExternalOidcServletComp::HandleUnlink(
			const imtrest::IRequest::CommandParams& commandParams,
			const HeadersMap& headers,
			const imtrest::CHttpRequest& request) const
{
	if (!m_authControllerCompPtr.IsValid() || !m_sessionControllerCompPtr.IsValid()){
		return CreateErrorResponse(QStringLiteral("server_error"), QStringLiteral("Controller not available"),
					imtrest::IProtocolEngine::SC_INTERNAL_ERROR, request);
	}

	QByteArray token = ExtractBearerToken(headers);
	if (token.isEmpty()){
		return CreateErrorResponse(QStringLiteral("unauthorized"), QStringLiteral("Missing or invalid bearer token"),
					imtrest::IProtocolEngine::SC_UNAUTHORIZED, request);
	}

	QByteArray userId = m_sessionControllerCompPtr->GetUserFromJwt(token);
	if (userId.isEmpty()){
		return CreateErrorResponse(QStringLiteral("unauthorized"), QStringLiteral("Invalid token"),
					imtrest::IProtocolEngine::SC_UNAUTHORIZED, request);
	}

	QByteArray providerId = commandParams.value("provider");
	if (providerId.isEmpty()){
		return CreateErrorResponse(QStringLiteral("invalid_request"), QStringLiteral("Missing 'provider' parameter"),
					imtrest::IProtocolEngine::SC_BAD_REQUEST, request);
	}

	bool success = m_authControllerCompPtr->UnlinkProvider(userId, providerId);
	if (!success){
		return CreateErrorResponse(QStringLiteral("not_found"), QStringLiteral("Provider link not found"),
					imtrest::IProtocolEngine::SC_NOT_FOUND, request);
	}

	QJsonObject response;
	response[QStringLiteral("status")] = QStringLiteral("unlinked");
	response[QStringLiteral("provider")] = QString::fromUtf8(providerId);

	return CreateJsonResponse(response, imtrest::IProtocolEngine::SC_OK, request);
}


CExternalOidcServletComp::ConstResponsePtr CExternalOidcServletComp::HandleListIdentities(
			const HeadersMap& headers,
			const imtrest::CHttpRequest& request) const
{
	if (!m_authControllerCompPtr.IsValid() || !m_sessionControllerCompPtr.IsValid()){
		return CreateErrorResponse(QStringLiteral("server_error"), QStringLiteral("Controller not available"),
					imtrest::IProtocolEngine::SC_INTERNAL_ERROR, request);
	}

	QByteArray token = ExtractBearerToken(headers);
	if (token.isEmpty()){
		return CreateErrorResponse(QStringLiteral("unauthorized"), QStringLiteral("Missing or invalid bearer token"),
					imtrest::IProtocolEngine::SC_UNAUTHORIZED, request);
	}

	QByteArray userId = m_sessionControllerCompPtr->GetUserFromJwt(token);
	if (userId.isEmpty()){
		return CreateErrorResponse(QStringLiteral("unauthorized"), QStringLiteral("Invalid token"),
					imtrest::IProtocolEngine::SC_UNAUTHORIZED, request);
	}

	QList<imtauth::IExternalOidcAuthController::LinkedProvider> linkedProviders =
		m_authControllerCompPtr->GetLinkedProviders(userId);

	QJsonArray identitiesArray;
	for (const auto& linked : ::std::as_const(linkedProviders)){
		QJsonObject identityObj;
		identityObj[QStringLiteral("provider")] = QString::fromUtf8(linked.provider);
		identityObj[QStringLiteral("external_subject")] = linked.externalSubject;
		identityObj[QStringLiteral("external_email")] = linked.externalEmail;
		identityObj[QStringLiteral("linked_at")] = linked.linkedAt.toString(Qt::ISODate);
		if (linked.lastAuthAt.isValid()){
			identityObj[QStringLiteral("last_auth_at")] = linked.lastAuthAt.toString(Qt::ISODate);
		}
		identitiesArray.append(identityObj);
	}

	QJsonObject response;
	response[QStringLiteral("identities")] = identitiesArray;

	return CreateJsonResponse(response, imtrest::IProtocolEngine::SC_OK, request);
}


// private methods - Helper methods

CExternalOidcServletComp::ConstResponsePtr CExternalOidcServletComp::CreateJsonResponse(
			const QJsonObject& json,
			imtrest::IProtocolEngine::StatusCode statusCode,
			const imtrest::CHttpRequest& request) const
{
	Q_UNUSED(request);

	QJsonDocument doc(json);
	QByteArray body = doc.toJson(QJsonDocument::Compact);

	imtrest::CHttpResponse* responsePtr = new imtrest::CHttpResponse();
	responsePtr->SetStatusCode(statusCode);
	responsePtr->SetContentType(QByteArrayLiteral("application/json"));
	responsePtr->SetBody(body);

	return ConstResponsePtr(responsePtr);
}


CExternalOidcServletComp::ConstResponsePtr CExternalOidcServletComp::CreateErrorResponse(
			const QString& error,
			const QString& description,
			imtrest::IProtocolEngine::StatusCode statusCode,
			const imtrest::CHttpRequest& request) const
{
	QJsonObject errorObj;
	errorObj[QStringLiteral("error")] = error;
	if (!description.isEmpty()){
		errorObj[QStringLiteral("error_description")] = description;
	}

	return CreateJsonResponse(errorObj, statusCode, request);
}


QByteArray CExternalOidcServletComp::ExtractBearerToken(const HeadersMap& headers) const
{
	QByteArray authHeader = headers.value("Authorization");
	if (authHeader.isEmpty()){
		authHeader = headers.value("authorization");
	}

	if (authHeader.startsWith("Bearer ") || authHeader.startsWith("bearer ")){
		return authHeader.mid(7);
	}

	return QByteArray();
}


QByteArray CExternalOidcServletComp::ExtractCommandEndpoint(const QByteArray& commandId) const
{
	// Extract last segment from command path (e.g. "/oauth/external/providers" -> "providers")
	int lastSlash = commandId.lastIndexOf('/');
	if (lastSlash >= 0){
		return commandId.mid(lastSlash + 1);
	}

	return commandId;
}


} // namespace imtoidc
