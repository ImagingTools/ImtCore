// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauthgql/CRemoteJwtSessionControllerComp.h>


// Qt includes
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>

// ACF includes
#include <iser/CMemoryReadArchive.h>
#include <iser/CJsonMemReadArchive.h>

// ImtCore includes
#include <imtauth/ISession.h>
#include <imtauth/CUserBaseInfo.h>
#include <imtauth/imtauth.h>
#include <imtbase/IIdentifiable.h>
#include <imtcore/Version.h>
#include <imtservergql/CAuthorizationSnapshot.h>
#include <imtgql/CGqlContext.h>
#include <imtgql/CGqlRequestContextManager.h>
#include <GeneratedFiles/imtauthsdl/SDL/1.0/CPP/Sessions.h>


namespace imtauthgql
{


namespace
{


// Puma gives a session only to its own user, so the request goes as the user the server works for
void AttachCurrentContext(imtgql::CGqlRequest& gqlRequest)
{
	imtgql::IGqlContext* gqlContextPtr = imtgql::CGqlRequestContextManager::GetContext();
	if (gqlContextPtr == nullptr){
		return;
	}

	istd::IChangeableUniquePtr clonedPtr = gqlContextPtr->CloneMe();
	imtgql::IGqlContextUniquePtr castedPtr;
	castedPtr.MoveCastedPtr(std::move(clonedPtr));
	gqlRequest.SetGqlContext(imtgql::IGqlContextSharedPtr::CreateFromUnique(std::move(castedPtr)));
}


} // anonymous namespace


// reimplemented (imtauth::ISessionAuthorizationProvider)

bool CRemoteJwtSessionControllerComp::IsAvailable() const
{
	return !m_serverCredentialCompPtr.IsValid() || !m_serverCredentialCompPtr->GetServerToken().isEmpty();
}


imtauth::ISessionAuthorizationProvider::SessionAuthorization CRemoteJwtSessionControllerComp::GetSessionAuthorization(
			const QByteArray& accessToken,
			const QByteArray& productId,
			bool isEstablishedSubscription) const
{
	namespace sessionsdl = sdl::V1_0::imtauth;
	SessionAuthorization retVal;
	sessionsdl::GetSessionAuthorizationRequestArguments arguments;
	arguments.input.Emplace();
	arguments.input->accessToken = accessToken;
	arguments.input->isEstablishedSubscription = isEstablishedSubscription;

	imtgql::CGqlRequest gqlRequest;
	if (!sessionsdl::CGetSessionAuthorizationGqlRequest::SetupGqlRequest(gqlRequest, arguments)){
		retVal.message = QStringLiteral("The request of the session authorization could not be built.");
		return retVal;
	}

	// Do not attach the current user context: the server authenticates with its own token only.
	imtgql::IGqlContextUniquePtr contextPtr(new imtgql::CGqlContext);
	contextPtr->SetProductId(productId);
	gqlRequest.SetGqlContext(imtgql::IGqlContextSharedPtr::CreateFromUnique(std::move(contextPtr)));

	const auto result = SendModelRequest<sessionsdl::CGetSessionAuthorizationPayload>(gqlRequest);
	if (result.errorCategory == imtclientgql::IGqlClient::EC_ACCESS_DENIED){
		retVal.state = SAS_REFUSED;
		retVal.refusalReason = RR_FORBIDDEN;
		retVal.message = QStringLiteral("The authorization server refused the server credentials.");
		return retVal;
	}
	const auto& response = result.model;
	if (!result || !response.state.HasValue()){
		retVal.message = result.errorMessage.isEmpty() ? QStringLiteral("The authorization server gave no answer.") : result.errorMessage;
		return retVal;
	}

	retVal.message = response.message.HasValue() ? *response.message : QString();
	if (*response.state == sessionsdl::SessionAuthorizationState::REFUSED){
		const bool hasReason = response.refusalReason.HasValue()
					&& (*response.refusalReason == sessionsdl::SessionRefusalReason::REAUTHENTICATE || *response.refusalReason == sessionsdl::SessionRefusalReason::FORBIDDEN);
		if (hasReason){
			retVal.state = SAS_REFUSED;
			retVal.refusalReason = (*response.refusalReason == sessionsdl::SessionRefusalReason::REAUTHENTICATE) ? RR_REAUTHENTICATE : RR_FORBIDDEN;
		}
		else{
			retVal.message = QStringLiteral("The authorization server gave no valid refusal reason.");
		}
		return retVal;
	}
	if (*response.state != sessionsdl::SessionAuthorizationState::COMPUTED){
		if (retVal.message.isEmpty()){
			retVal.message = QStringLiteral("The session authorization could not be checked.");
		}
		return retVal;
	}
	if (!response.refusalReason.HasValue() || *response.refusalReason != sessionsdl::SessionRefusalReason::NONE
				|| !response.tenantId.HasValue() || !response.tenantOwnerId.HasValue() || !response.tenantCreatorId.HasValue()
				|| !response.tenantPermissions.HasValue()){
		retVal.message = QStringLiteral("The authorization server gave an incomplete answer.");
		return retVal;
	}

	const QByteArray userId = response.userId.HasValue() ? *response.userId : QByteArray();
	const QByteArray userData = response.userData.HasValue() ? response.userData->toUtf8() : QByteArray();
	istd::IChangeableUniquePtr userPtr(m_userFactCompPtr.CreateInstance());
	imtauth::IUserInfo* userInfoPtr = dynamic_cast<imtauth::IUserInfo*>(userPtr.GetPtr());
	const imtbase::IIdentifiable* identifiablePtr = dynamic_cast<const imtbase::IIdentifiable*>(userPtr.GetPtr());
	const imtauth::CUserBaseInfo* userBasePtr = dynamic_cast<const imtauth::CUserBaseInfo*>(userPtr.GetPtr());
	iser::ISerializable* serializablePtr = dynamic_cast<iser::ISerializable*>(userPtr.GetPtr());
	iser::CJsonMemReadArchive archive(userData);
	quint32 version = 0;
	imtauth::SessionTokenClaims claims;
	if (userId.isEmpty() || userData.isEmpty() || userInfoPtr == nullptr || serializablePtr == nullptr
				|| userBasePtr == nullptr || identifiablePtr == nullptr
				|| !archive.GetVersionInfo().GetVersionNumber(imtcore::VI_IMTCORE, version) || version < 6940
				|| !serializablePtr->Serialize(archive) || identifiablePtr->GetObjectUuid() != userId || !userInfoPtr->IsEnabled()
				|| !userBasePtr->HasLocalPermissions(productId)
				|| !imtauth::ParseSessionTokenClaims(accessToken, claims) || claims.userId != userId || claims.tenantId != *response.tenantId){
		retVal.message = QStringLiteral("The answer of the authorization server holds no valid user.");
		return retVal;
	}

	retVal.state = SAS_COMPUTED;
	retVal.userId = userId;
	retVal.tenantId = *response.tenantId;
	retVal.tenantOwnerId = *response.tenantOwnerId;
	retVal.tenantCreatorId = *response.tenantCreatorId;
	retVal.tenantPermissions = response.tenantPermissions->ToList();
	// An older authorization server does not tell it: unknown, not ended.
	if (response.sessionExpiresAt.HasValue()){
		retVal.sessionExpiresAt = QDateTime::fromString(*response.sessionExpiresAt, Qt::ISODateWithMs);
	}
	imtservergql::CAuthorizationSnapshot::DetachUser(*userPtr);
	retVal.userPtr.FromUnique(std::move(userPtr));
	return retVal;
}


// protected methods

// reimplemented (imtauth::IJwtSessionController)

bool CRemoteJwtSessionControllerComp::ValidateSession(const QByteArray& sessionId, bool* isCheckedPtr) const
{
	namespace sessionsdl = sdl::V1_0::imtauth;

	if (isCheckedPtr != nullptr){
		*isCheckedPtr = false;
	}

	sessionsdl::ValidateSessionRequestArguments arguments;
	arguments.input.Emplace();
	arguments.input->sessionId = sessionId;

	imtgql::CGqlRequest gqlRequest;
	if (!sessionsdl::CValidateSessionGqlRequest::SetupGqlRequest(gqlRequest, arguments)){
		return false;
	}

	typedef sdl::V1_0::imtauth::CValidateSessionPayload Response;

	QString errorMessage;
	Response response = SendModelRequest<Response>(gqlRequest, errorMessage);
	if (!errorMessage.isEmpty() || !response.isValid.has_value()){
		return false;
	}

	if (isCheckedPtr != nullptr){
		*isCheckedPtr = true;
	}

	return *response.isValid;
}


imtauth::IJwtSessionController::JwtState CRemoteJwtSessionControllerComp::ValidateJwt(const QByteArray& token) const
{
	namespace sessionsdl = sdl::V1_0::imtauth;

	const QString maskedToken = token.isEmpty()
				? QStringLiteral("<empty>")
				: (QStringLiteral("***") + QString::fromUtf8(token.right(4)));

	sessionsdl::ValidateJwtRequestArguments arguments;
	arguments.input.Emplace();
	arguments.input->jwt = token;

	imtgql::CGqlRequest gqlRequest;
	if (!sessionsdl::CValidateJwtGqlRequest::SetupGqlRequest(gqlRequest, arguments)){
		SendErrorMessage(
					0,
					QStringLiteral("Remote ValidateJwt: failed to build GQL request for token %1")
							.arg(maskedToken),
					QStringLiteral("CRemoteJwtSessionControllerComp"));
		// JS_NONE = validation could not be completed (transport / setup), not a
		// permanent credential rejection. Callers must map this to 5xx, not 403.
		return imtauth::IJwtSessionController::JS_NONE;
	}

	typedef sdl::V1_0::imtauth::CValidateJwtPayload Response;

	QString errorMessage;
	Response response = SendModelRequest<Response>(gqlRequest, errorMessage);
	if (!errorMessage.isEmpty()){
		SendErrorMessage(
					0,
					QStringLiteral("Remote ValidateJwt: request failed for token %1: %2")
							.arg(maskedToken, errorMessage),
					QStringLiteral("CRemoteJwtSessionControllerComp"));
		return imtauth::IJwtSessionController::JS_NONE;
	}

	if (response.state.has_value()){
		sessionsdl::JwtState state = *response.state;

		if (state == sessionsdl::JwtState::EXPIRED){
			return imtauth::IJwtSessionController::JS_EXPIRED;
		}
		if (state == sessionsdl::JwtState::INVALID){
			return imtauth::IJwtSessionController::JS_INVALID;
		}
		if (state == sessionsdl::JwtState::OK){
			return imtauth::IJwtSessionController::JS_OK;
		}

		SendWarningMessage(
					0,
					QStringLiteral("Remote ValidateJwt: unexpected JwtState enum value for token %1")
							.arg(maskedToken),
					QStringLiteral("CRemoteJwtSessionControllerComp"));
	}
	else{
		SendErrorMessage(
					0,
					QStringLiteral("Remote ValidateJwt: response missing 'state' for token %1")
							.arg(maskedToken),
					QStringLiteral("CRemoteJwtSessionControllerComp"));
	}

	return imtauth::IJwtSessionController::JS_NONE;
}


bool CRemoteJwtSessionControllerComp::RefreshToken(
			const QByteArray& refreshToken,
			imtauth::IJwtSessionController::UserSession& userSession) const
{
	namespace sessionsdl = sdl::V1_0::imtauth;

	sessionsdl::RefreshTokenRequestArguments arguments;
	arguments.input.Emplace();
	arguments.input->refreshToken = refreshToken;

	imtgql::CGqlRequest gqlRequest;
	if (!sessionsdl::CRefreshTokenGqlRequest::SetupGqlRequest(gqlRequest, arguments)){
		return false;
	}

	imtgql::IGqlContextSharedPtr refreshContextPtr = new imtgql::CGqlContext;
	imtgql::IGqlContext* currentContextPtr = imtgql::CGqlRequestContextManager::GetContext();
	if (currentContextPtr != nullptr){
		refreshContextPtr->SetProductId(currentContextPtr->GetProductId());
	}
	gqlRequest.SetGqlContext(refreshContextPtr);

	typedef sdl::V1_0::imtauth::CRefreshTokenPayload Response;

	QString errorMessage;
	Response response = SendModelRequest<Response>(gqlRequest, errorMessage);
	if (!errorMessage.isEmpty()){
		return false;
	}

	if (response.ok.has_value() && *response.ok){
		if (response.userSession.has_value()){
			if (response.userSession->userId.has_value()){
				userSession.userId = *response.userSession->userId;
			}

			if (response.userSession->tenantId.has_value()){
				userSession.tenantId = *response.userSession->tenantId;
			}

			if (response.userSession->accessToken.has_value()){
				userSession.accessToken = *response.userSession->accessToken;
			}

			if (response.userSession->refreshToken.has_value()){
				userSession.refreshToken = *response.userSession->refreshToken;
			}

			return true;
		}
	}

	return false;
}


bool CRemoteJwtSessionControllerComp::CreateNewSession(
			const QByteArray& userId,
			const QByteArray& tenantId,
			imtauth::IJwtSessionController::UserSession& userSession) const
{
	namespace sessionsdl = sdl::V1_0::imtauth;

	sessionsdl::CreateNewSessionRequestArguments arguments;
	arguments.input.Emplace();
	arguments.input->userId = userId;
	if (!tenantId.isEmpty()){
		arguments.input->tenantId = tenantId;
	}

	imtgql::CGqlRequest gqlRequest;
	if (!sessionsdl::CCreateNewSessionGqlRequest::SetupGqlRequest(gqlRequest, arguments)){
		return false;
	}

	typedef sdl::V1_0::imtauth::CCreateNewSessionPayload Response;

	QString errorMessage;
	Response response = SendModelRequest<Response>(gqlRequest, errorMessage);
	if (!errorMessage.isEmpty()){
		return false;
	}

	if (response.ok.has_value() && *response.ok){
		if (response.userSession.has_value()){
			if (response.userSession->userId.has_value()){
				userSession.userId = *response.userSession->userId;
			}

			if (response.userSession->tenantId.has_value()){
				userSession.tenantId = *response.userSession->tenantId;
			}

			if (response.userSession->accessToken.has_value()){
				userSession.accessToken = *response.userSession->accessToken;
			}

			if (response.userSession->refreshToken.has_value()){
				userSession.refreshToken = *response.userSession->refreshToken;
			}

			return true;
		}
	}

	return false;
}


imtauth::ISessionSharedPtr CRemoteJwtSessionControllerComp::GetSession(const QByteArray& sessionId) const
{
	namespace sessionsdl = sdl::V1_0::imtauth;

	sessionsdl::GetSessionRequestArguments arguments;
	arguments.input.Emplace();
	arguments.input->sessionId = sessionId;

	imtgql::CGqlRequest gqlRequest;
	if (!sessionsdl::CGetSessionGqlRequest::SetupGqlRequest(gqlRequest, arguments)){
		return nullptr;
	}
	AttachCurrentContext(gqlRequest);

	typedef sdl::V1_0::imtauth::CGetSessionPayload Response;

	QString errorMessage;
	Response response = SendModelRequest<Response>(gqlRequest, errorMessage);
	if (!errorMessage.isEmpty()){
		return nullptr;
	}

	if (response.sessionData.has_value() && m_sessionFactCompPtr.IsValid()){
		QByteArray sessionData = *response.sessionData;

		imtauth::ISessionUniquePtr sessionInfoPtr = m_sessionFactCompPtr.CreateInstance();

		iser::CMemoryReadArchive archive(sessionData.data(), sessionData.length());
		if (!sessionInfoPtr->Serialize(archive)){
			SendErrorMessage(0, QStringLiteral("Unable to deserialize session info. Error: Deserialization failed"));
			return nullptr;
		}

		return imtauth::ISessionSharedPtr::CreateFromUnique(std::move(sessionInfoPtr));
	}

	return nullptr;
}


bool CRemoteJwtSessionControllerComp::RemoveSession(const QByteArray& sessionId) const
{
	namespace sessionsdl = sdl::V1_0::imtauth;

	sessionsdl::RemoveSessionRequestArguments arguments;
	arguments.input.Emplace();
	arguments.input->sessionId = sessionId;

	imtgql::CGqlRequest gqlRequest;
	if (!sessionsdl::CRemoveSessionGqlRequest::SetupGqlRequest(gqlRequest, arguments)){
		return false;
	}
	AttachCurrentContext(gqlRequest);

	typedef sdl::V1_0::imtauth::CRemoveSessionPayload Response;

	QString errorMessage;
	Response response = SendModelRequest<Response>(gqlRequest, errorMessage);
	if (!errorMessage.isEmpty()){
		return false;
	}

	if (response.ok.has_value()){
		return *response.ok;
	}

	return false;
}


namespace
{

QJsonObject ParseJwtPayloadObject(const QByteArray& jwt)
{
	const QByteArrayList parts = jwt.split('.');
	if (parts.size() != 3){
		return {};
	}

	const QByteArray json = QByteArray::fromBase64(
				parts[1],
				QByteArray::Base64UrlEncoding | QByteArray::OmitTrailingEquals);
	return QJsonDocument::fromJson(json).object();
}

} // namespace


QByteArray CRemoteJwtSessionControllerComp::GetUserFromJwt(const QByteArray& jwt) const
{
	// Claims are embedded in the JWT payload; decode locally to avoid a remote
	// round-trip on every cache miss (was 3 remote calls per ValidateJwt path).
	const QJsonObject payloadObj = ParseJwtPayloadObject(jwt);
	if (payloadObj.contains(QStringLiteral("userId"))){
		return payloadObj.value(QStringLiteral("userId")).toString().toUtf8();
	}

	namespace sessionsdl = sdl::V1_0::imtauth;

	sessionsdl::GetUserFromJwtRequestArguments arguments;
	arguments.input.Emplace();
	arguments.input->jwt = jwt;

	imtgql::CGqlRequest gqlRequest;
	if (!sessionsdl::CGetUserFromJwtGqlRequest::SetupGqlRequest(gqlRequest, arguments)){
		return QByteArray();
	}

	typedef sdl::V1_0::imtauth::CGetUserFromJwtPayload Response;

	QString errorMessage;
	Response response = SendModelRequest<Response>(gqlRequest, errorMessage);
	if (!errorMessage.isEmpty()){
		SendWarningMessage(
					0,
					QStringLiteral("Remote GetUserFromJwt failed: %1").arg(errorMessage),
					QStringLiteral("CRemoteJwtSessionControllerComp"));
		return QByteArray();
	}

	if (response.userId.has_value()){
		return *response.userId;
	}

	return QByteArray();
}


QByteArray CRemoteJwtSessionControllerComp::GetSessionFromJwt(const QByteArray& jwt) const
{
	const QJsonObject payloadObj = ParseJwtPayloadObject(jwt);
	if (payloadObj.contains(QStringLiteral("sessionId"))){
		return payloadObj.value(QStringLiteral("sessionId")).toString().toUtf8();
	}

	return QByteArray();
}


QByteArray CRemoteJwtSessionControllerComp::GetTenantFromJwt(const QByteArray& jwt) const
{
	const QJsonObject payloadObj = ParseJwtPayloadObject(jwt);
	if (payloadObj.contains(QStringLiteral("tenantId"))){
		return payloadObj.value(QStringLiteral("tenantId")).toString().toUtf8();
	}

	namespace sessionsdl = sdl::V1_0::imtauth;

	sessionsdl::GetTenantFromJwtRequestArguments arguments;
	arguments.input.Emplace();
	arguments.input->jwt = jwt;

	imtgql::CGqlRequest gqlRequest;
	if (!sessionsdl::CGetTenantFromJwtGqlRequest::SetupGqlRequest(gqlRequest, arguments)){
		return QByteArray();
	}

	typedef sdl::V1_0::imtauth::CGetTenantFromJwtPayload Response;

	QString errorMessage;
	Response response = SendModelRequest<Response>(gqlRequest, errorMessage);
	if (!errorMessage.isEmpty()){
		SendWarningMessage(
					0,
					QStringLiteral("Remote GetTenantFromJwt failed: %1").arg(errorMessage),
					QStringLiteral("CRemoteJwtSessionControllerComp"));
		return QByteArray();
	}

	if (response.tenantId.has_value()){
		return *response.tenantId;
	}

	return QByteArray();
}


} // namespace imtauthgql
