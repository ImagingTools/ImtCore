// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtservergql/CHttpGraphQLServletComp.h>

// Qt includes
#include <QtCore/QScopeGuard>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QJsonArray>

// ACF includes
#include <iprm/TParamsPtr.h>
#include <iprm/ISelectionParam.h>
#include <iprm/IOptionsList.h>

// ImtCore includes
#include <imtbase/imtbase.h>
#include <imtgql/CGqlRequest.h>
#include <imtgql/CGqlContext.h>
#include <imtgql/CGqlRequestContextManager.h>
#include <imtrest/IProtocolEngine.h>


namespace imtservergql
{


namespace
{

QString MaskToken(const QByteArray& token)
{
	if (token.isEmpty()){
		return QStringLiteral("<empty>");
	}

	return QStringLiteral("***") + QString::fromUtf8(token.right(4));
}

} // namespace


// protected methods

// reimplemented (IRequestHandler)

bool CHttpGraphQLServletComp::IsCommandSupported(const QByteArray& commandId) const
{
	if (m_applicationInfoCompPtr.IsValid()){
		QString appId = m_applicationInfoCompPtr->GetApplicationAttribute(ibase::IApplicationInfo::AA_APPLICATION_ID);
		if (commandId == appId + QStringLiteral("/graphql")){
			return true;
		}
	}

	return BaseClass::IsCommandSupported(commandId);
}


// reimplemented (imtrest::IRequestServlet)

imtrest::ConstResponsePtr CHttpGraphQLServletComp::OnPost(
	const QByteArray& /*commandId*/,
	const imtrest::IRequest::CommandParams& /*commandParams*/,
	const HeadersMap& headers,
	const imtrest::CHttpRequest& request) const
{
	// Ensure Clear() is called on every return path
	auto cleanup = qScopeGuard([]() {
		imtgql::CGqlRequestContextManager::Clear();
	});

	m_lastRequest.ResetData();

	qsizetype errorPosition = -1;
	const QByteArray requestBody = request.GetBody();
	if (!m_lastRequest.ParseQuery(requestBody, errorPosition)){
		qCritical() << __FILE__ << __LINE__ << QStringLiteral("Error when parsing request: '%1'; Error position: '%2'")
														.arg(request.GetBody(),
															 QString::number(errorPosition));

		return GenerateError(StatusCode::SC_BAD_REQUEST, QStringLiteral("Request is incorrect"), request);
	}

	const QByteArray gqlCommand = m_lastRequest.GetCommandId();

	// Set protocol version - RFC 2616: 4.2 case-insensitive header comparison
	for (HeadersMap::const_iterator headerIter = headers.cbegin(); headerIter != headers.cend(); ++headerIter) {
		// find header. compare with lowercase. RFC 2616: 4.2
		if (headerIter.key().toLower() == imtbase::s_protocolVersionHeaderId.toLower()) {
			const QByteArray& protocolVersion = *headerIter;
			m_lastRequest.SetProtocolVersion(protocolVersion);

			break;
		}
	}

	if (m_gqlContextCreatorCompPtr.IsValid()){
		imtgql::IGqlContextCreator::ContextCreationError contextError;
		imtgql::IGqlContextUniquePtr gqlContextPtr = m_gqlContextCreatorCompPtr->CreateGqlContext(headers, contextError);
		if (!gqlContextPtr.IsValid()){
			const QString maskedToken = MaskToken(headers.value(imtbase::s_authenticationTokenHeaderId));
			if (contextError.status == imtgql::IGqlContextCreator::CCS_UNAUTHORIZED){
				// Client should refresh the access token; log detail for diagnostics.
				SendWarningMessage(
							0,
							QStringLiteral("HTTP GraphQL auth rejected (401 Unauthorized) for Command-ID '%1', token %2: %3")
									.arg(gqlCommand, maskedToken, contextError.message),
							QStringLiteral("GraphQL - servlet"));
				return CreateResponse(StatusCode::SC_UNAUTHORIZED, QByteArray(), request);
			}
			if (contextError.status == imtgql::IGqlContextCreator::CCS_FORBIDDEN){
				SendWarningMessage(
							0,
							QStringLiteral("HTTP GraphQL auth rejected (403 Forbidden) for Command-ID '%1', token %2: %3")
									.arg(gqlCommand, maskedToken, contextError.message),
							QStringLiteral("GraphQL - servlet"));
				return CreateResponse(StatusCode::SC_FORBIDDEN, QByteArray(), request);
			}

			SendCriticalMessage(
						0,
						QStringLiteral("Unable to create a GraphQL context for the access token '%1' for Command-ID: '%2'. Error: '%3'")
											.arg(maskedToken, QString(gqlCommand), contextError.message),
						QStringLiteral("GraphQL - servlet"));
			return GenerateError(StatusCode::SC_INTERNAL_SERVER_ERROR, QStringLiteral("Request context is invalid"), request);
		}

		m_lastRequest.SetGqlContext(std::move(gqlContextPtr));
	}
	else{
		// No GqlContextCreator wired for this servlet's assembly: every request is
		// processed with no context attached, so any downstream permission check
		// sees an anonymous caller regardless of the token supplied. This is a
		// static misconfiguration (a missing "AuthenticationManager"-style
		// attribute wire-up somewhere in the .acc chain), not a per-request
		// condition, so log it loudly instead of silently degrading to anonymous
		// access - see Docs/GQL_CONTEXT_THREAD_PROPAGATION_BUG.md.
		SendCriticalMessage(
					0,
					QStringLiteral("GqlContextCreator is not configured for this servlet - Command-ID: '%1' will be processed with no request context (caller will appear anonymous to every permission check).")
										.arg(gqlCommand),
					QStringLiteral("GraphQL - servlet"));
	}

	QByteArray responseData;
	bool isSuccessful = false;

	int dataControllersCount = m_gqlRequestHandlerCompPtr.GetCount();
	for (int index = 0; index < dataControllersCount; index++){
		const imtgql::IGqlRequestHandler* requestHandlerPtr = m_gqlRequestHandlerCompPtr[index];
		if (requestHandlerPtr == nullptr){
			continue;
		}

		if (!requestHandlerPtr->IsRequestSupported(m_lastRequest)) {
			// unsupported request
			continue;
		}

		QString errorMessage;
		QString errorType = QStringLiteral("Warning");
		QJsonObject sourceItemObj = requestHandlerPtr->CreateResponse(m_lastRequest, errorMessage);

		bool isError = sourceItemObj.isEmpty();

		// Detect if result model itself contains an "errors" child
		if (!isError) {
			if (sourceItemObj.contains(QStringLiteral("errors"))) {
				isError = true;
				QJsonObject errorsObj = sourceItemObj.value(QStringLiteral("errors")).toObject();
				if (errorsObj.contains(QStringLiteral("message"))) {
					errorMessage = errorsObj.value(QStringLiteral("message")).toString();
				}

				if (errorsObj.contains(QStringLiteral("type"))) {
					errorType = errorsObj.value(QStringLiteral("type")).toString();
				}
			}
		}

		if (isError) {
			// Permission handler reports unauthenticated callers as type Unauthorized.
			// Map to HTTP 401 so clients run the same reactive refresh path as expired JWT.
			if (errorType.compare(QStringLiteral("Unauthorized"), Qt::CaseInsensitive) == 0){
				SendWarningMessage(
							0,
							QStringLiteral("HTTP GraphQL auth rejected (401 Unauthorized) for Command-ID '%1': %2")
									.arg(gqlCommand, errorMessage),
							QStringLiteral("GraphQL - servlet"));
				return CreateResponse(StatusCode::SC_UNAUTHORIZED, QByteArray(), request);
			}

			responseData = BuildGqlErrorJson(gqlCommand, errorMessage, errorType);
			isSuccessful = !responseData.isEmpty();
		}
		else {
			// Success path: wrap source data under "data" → commandId
			QJsonObject rootObj;
			QJsonObject dataWrapperObj;
			QJsonValue sourceData = sourceItemObj.contains(QStringLiteral("data"))
				? sourceItemObj.value(QStringLiteral("data"))
				: QJsonValue(sourceItemObj);

			dataWrapperObj.insert(QString::fromUtf8(gqlCommand), sourceData);
			rootObj.insert(QStringLiteral("data"), dataWrapperObj);

			QJsonDocument doc(rootObj);
			responseData = doc.toJson(QJsonDocument::Compact);
			isSuccessful = true;
		}

		break;
	}

	// If we have a GraphQL JSON response (Success OR Warning/Execution Error)
	// We return 200 OK so the client can read the { data: null, errors: [...] } structure.
	if (!responseData.isEmpty()){
		return CreateResponse(
					StatusCode::SC_OK,
					responseData,
					request,
					QByteArrayLiteral("application/json; charset=utf-8"));;
	}

	// If no handler supported the request (isSuccessful is still false)
	// or if the loop never ran/serialization failed.
	if (!isSuccessful){
		QString invalidCommandMessage;
		if (gqlCommand.isEmpty()){
			invalidCommandMessage = QStringLiteral("Invalid GraphQL command");
		}
		else{
			invalidCommandMessage = QStringLiteral("Invalid command request: '%1'").arg(gqlCommand);
		}

		SendErrorMessage(0, invalidCommandMessage, QStringLiteral("GraphQL - servlet"));

		// Return 400 because this is a transport/routing failure, not a business logic error.
		QByteArray errorJson = BuildGqlErrorJson(gqlCommand, invalidCommandMessage, QStringLiteral("Error"));
		return CreateResponse(
					StatusCode::SC_BAD_REQUEST,
					errorJson,
					request,
					QByteArrayLiteral("application/json; charset=utf-8"));
	}

	// Fallback for unexpected internal state
	SendErrorMessage(0, QStringLiteral("Internal server error for command '%1'").arg(gqlCommand), QStringLiteral("GraphQL - servlet"));
	return GenerateError(StatusCode::SC_INTERNAL_SERVER_ERROR, QStringLiteral("Request is incorrect"), request);
}


// reimplemented (imtgql::IGqlRequestProvider)

const imtgql::IGqlRequest* CHttpGraphQLServletComp::GetGqlRequest() const
{
	return &m_lastRequest;
}


// private methods

imtrest::ConstResponsePtr CHttpGraphQLServletComp::CreateResponse(
	const StatusCode& statusCode,
	const QByteArray& payload,
	const imtrest::IRequest& request,
	const QByteArray& contentTypeId) const
{
	return imtrest::ConstResponsePtr(request.GetProtocolEngine().CreateResponse(request, statusCode, payload, contentTypeId).PopInterfacePtr());
}


imtrest::ConstResponsePtr CHttpGraphQLServletComp::GenerateError(
	const StatusCode& errorCode,
	const QString& errorString,
	const imtrest::CHttpRequest& request) const
{
	// Default type to "Error" for 400/500 codes.
	QByteArray responseJson = BuildGqlErrorJson(QByteArray(), errorString, QStringLiteral("Error"));

	return imtrest::ConstResponsePtr(
		request.GetProtocolEngine().CreateResponse(
									   request,
									   errorCode,
									   responseJson,
									   QByteArray("application/json;charset=utf-8")).PopInterfacePtr());
}


QByteArray CHttpGraphQLServletComp::BuildGqlErrorJson(
	const QByteArray& gqlCommand,
	const QString& message,
	const QString& type) const
{
	QJsonObject rootObj;

	QJsonArray errorsArr;
	QJsonObject errorObj;
	errorObj.insert(QStringLiteral("message"), message);

	// path: [ "CommandName" ]
	if (!gqlCommand.isEmpty()) {
		QJsonArray pathArr;
		pathArr.append(QString::fromUtf8(gqlCommand));
		errorObj.insert(QStringLiteral("path"), pathArr);
	}

	// extensions: { "type": "Warning/Error" }
	QJsonObject extObj;
	extObj.insert(QStringLiteral("type"), type);
	errorObj.insert(QStringLiteral("extensions"), extObj);

	errorsArr.append(errorObj);
	rootObj.insert(QStringLiteral("errors"), errorsArr);

	QJsonDocument doc(rootObj);
	return doc.toJson(QJsonDocument::Compact);
}


} // namespace imtservergql
