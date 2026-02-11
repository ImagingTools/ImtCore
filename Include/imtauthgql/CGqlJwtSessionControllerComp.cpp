// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauthgql/CGqlJwtSessionControllerComp.h>


// ACF includes
#include <iser/CMemoryWriteArchive.h>


namespace imtauthgql
{


// protected methods

// reimplemented (sdl::imtauth::Sessions::CGraphQlHandlerCompBase)

sdl::imtauth::Sessions::CValidateSessionPayload CGqlJwtSessionControllerComp::OnValidateSession(
	const sdl::imtauth::Sessions::CValidateSessionGqlRequest& validateSessionRequest,
	const ::imtgql::CGqlRequest& /*gqlRequest*/,
	QString& /*errorMessage*/) const
{
	sdl::imtauth::Sessions::CValidateSessionPayload response;

	if (!m_jwtSessionControllerCompPtr.IsValid()){
		Q_ASSERT_X(false, "Attribute 'JwtSessionController' was not set", "CGqlJwtSessionControllerComp");
		return response;
	}

	response.Version_1_0.emplace();

	QByteArray sessionId;
	sdl::imtauth::Sessions::ValidateSessionRequestArguments arguments = validateSessionRequest.GetRequestedArguments();
	if (arguments.input.Version_1_0->sessionId){
		sessionId = *arguments.input.Version_1_0->sessionId;
	}

	response.Version_1_0->isValid = m_jwtSessionControllerCompPtr->ValidateSession(sessionId);

	return response;
}


sdl::imtauth::Sessions::CValidateJwtPayload CGqlJwtSessionControllerComp::OnValidateJwt(
	const sdl::imtauth::Sessions::CValidateJwtGqlRequest& validateJwtRequest,
	const ::imtgql::CGqlRequest& /*gqlRequest*/,
	QString& /*errorMessage*/) const
{
	sdl::imtauth::Sessions::CValidateJwtPayload response;

	if (!m_jwtSessionControllerCompPtr.IsValid()){
		Q_ASSERT_X(false, "Attribute 'JwtSessionController' was not set", "CGqlJwtSessionControllerComp");
		return response;
	}

	QString jwt;
	sdl::imtauth::Sessions::ValidateJwtRequestArguments arguments = validateJwtRequest.GetRequestedArguments();
	if (arguments.input.Version_1_0->jwt){
		jwt = *arguments.input.Version_1_0->jwt;
	}

	response.Version_1_0.emplace();

	imtauth::IJwtSessionController::JwtState state = m_jwtSessionControllerCompPtr->ValidateJwt(jwt.toUtf8());

	response.Version_1_0->state = sdl::imtauth::Sessions::JwtState::NONE;

	if (state == imtauth::IJwtSessionController::JS_EXPIRED){
		response.Version_1_0->state = sdl::imtauth::Sessions::JwtState::EXPIRED;
	}
	else if (state == imtauth::IJwtSessionController::JS_INVALID){
		response.Version_1_0->state = sdl::imtauth::Sessions::JwtState::INVALID;
	}
	else if (state == imtauth::IJwtSessionController::JS_OK){
		response.Version_1_0->state = sdl::imtauth::Sessions::JwtState::OK;
	}

	return response;
}


sdl::imtauth::Sessions::CGetSessionPayload CGqlJwtSessionControllerComp::OnGetSession(
	const sdl::imtauth::Sessions::CGetSessionGqlRequest& getSessionRequest,
	const ::imtgql::CGqlRequest& /*gqlRequest*/,
	QString& /*errorMessage*/) const
{
	sdl::imtauth::Sessions::CGetSessionPayload response;

	if (!m_jwtSessionControllerCompPtr.IsValid()){
		Q_ASSERT_X(false, "Attribute 'JwtSessionController' was not set", "CGqlJwtSessionControllerComp");
		return response;
	}

	response.Version_1_0.emplace();

	QByteArray sessionId;
	sdl::imtauth::Sessions::GetSessionRequestArguments arguments = getSessionRequest.GetRequestedArguments();
	if (arguments.input.Version_1_0->sessionId){
		sessionId = *arguments.input.Version_1_0->sessionId;
	}

	imtauth::ISessionSharedPtr sessionInfoPtr = m_jwtSessionControllerCompPtr->GetSession(sessionId);

	istd::TDelPtr<iser::CMemoryWriteArchive> archivePtr;
	if (m_versionInfoCompPtr.IsValid()){
		archivePtr.SetPtr(new iser::CMemoryWriteArchive(m_versionInfoCompPtr.GetPtr()));
	}
	else{
		archivePtr.SetPtr(new iser::CMemoryWriteArchive());
	}

	if (!sessionInfoPtr->Serialize(*archivePtr.GetPtr())){
		SendErrorMessage(0, QString("Unable to serialize object. Error: Serialization failed"));
		return response;
	}

	QByteArray objectData = QByteArray((char*)archivePtr->GetBuffer(), archivePtr->GetBufferSize());

	response.Version_1_0->sessionData = objectData;

	return response;
}


sdl::imtauth::Sessions::CRefreshTokenPayload CGqlJwtSessionControllerComp::OnRefreshToken(
	const sdl::imtauth::Sessions::CRefreshTokenGqlRequest& refreshTokenRequest,
	const ::imtgql::CGqlRequest& /*gqlRequest*/,
	QString& /*errorMessage*/) const
{
	sdl::imtauth::Sessions::CRefreshTokenPayload response;

	if (!m_jwtSessionControllerCompPtr.IsValid()){
		Q_ASSERT_X(false, "Attribute 'JwtSessionController' was not set", "CGqlJwtSessionControllerComp");
		return response;
	}

	response.Version_1_0.emplace();

	QByteArray refreshToken;
	sdl::imtauth::Sessions::RefreshTokenRequestArguments arguments = refreshTokenRequest.GetRequestedArguments();
	if (arguments.input.Version_1_0->refreshToken){
		refreshToken = *arguments.input.Version_1_0->refreshToken;
	}

	imtauth::IJwtSessionController::UserSession userSession;
	response.Version_1_0->ok = m_jwtSessionControllerCompPtr->RefreshToken(refreshToken, userSession);
	if (response.Version_1_0->ok){
		sdl::imtauth::Sessions::CUserSession::V1_0 userData;
		userData.userId = userSession.userId;
		userData.accessToken = userSession.accessToken;
		userData.refreshToken = userSession.refreshToken;

		response.Version_1_0->userSession = userData;
	}

	return response;
}


sdl::imtauth::Sessions::CCreateNewSessionPayload CGqlJwtSessionControllerComp::OnCreateNewSession(
	const sdl::imtauth::Sessions::CCreateNewSessionGqlRequest& createNewSessionRequest,
	const ::imtgql::CGqlRequest& /*gqlRequest*/,
	QString& /*errorMessage*/) const
{
	sdl::imtauth::Sessions::CCreateNewSessionPayload response;

	if (!m_jwtSessionControllerCompPtr.IsValid()){
		Q_ASSERT_X(false, "Attribute 'JwtSessionController' was not set", "CGqlJwtSessionControllerComp");
		return response;
	}

	response.Version_1_0.emplace();

	QByteArray userId;
	sdl::imtauth::Sessions::CreateNewSessionRequestArguments arguments = createNewSessionRequest.GetRequestedArguments();
	if (arguments.input.Version_1_0->userId){
		userId = *arguments.input.Version_1_0->userId;
	}

	imtauth::IJwtSessionController::UserSession userSession;
	response.Version_1_0->ok = m_jwtSessionControllerCompPtr->CreateNewSession(userId, userSession);
	if (response.Version_1_0->ok){
		sdl::imtauth::Sessions::CUserSession::V1_0 userData;
		userData.userId = userSession.userId;
		userData.accessToken = userSession.accessToken;
		userData.refreshToken = userSession.refreshToken;

		response.Version_1_0->userSession = userData;
	}

	return response;
}


sdl::imtauth::Sessions::CRemoveSessionPayload CGqlJwtSessionControllerComp::OnRemoveSession(
	const sdl::imtauth::Sessions::CRemoveSessionGqlRequest& removeSessionRequest,
	const ::imtgql::CGqlRequest& /*gqlRequest*/,
	QString& /*errorMessage*/) const
{
	sdl::imtauth::Sessions::CRemoveSessionPayload response;

	if (!m_jwtSessionControllerCompPtr.IsValid()){
		Q_ASSERT_X(false, "Attribute 'JwtSessionController' was not set", "CGqlJwtSessionControllerComp");
		return response;
	}

	response.Version_1_0.emplace();

	QByteArray sessionId;
	sdl::imtauth::Sessions::RemoveSessionRequestArguments arguments = removeSessionRequest.GetRequestedArguments();
	if (arguments.input.Version_1_0->sessionId){
		sessionId = *arguments.input.Version_1_0->sessionId;
	}

	response.Version_1_0->ok = m_jwtSessionControllerCompPtr->RemoveSession(sessionId);

	return response;
}


sdl::imtauth::Sessions::CGetUserFromJwtPayload CGqlJwtSessionControllerComp::OnGetUserFromJwt(
	const sdl::imtauth::Sessions::CGetUserFromJwtGqlRequest& getUserFromJwtRequest,
	const ::imtgql::CGqlRequest& /*gqlRequest*/,
	QString& /*errorMessage*/) const
{
	sdl::imtauth::Sessions::CGetUserFromJwtPayload::V1_0 response;

	if (!m_jwtSessionControllerCompPtr.IsValid()){
		Q_ASSERT_X(false, "Attribute 'JwtSessionController' was not set", "CGqlJwtSessionControllerComp");
		return sdl::imtauth::Sessions::CGetUserFromJwtPayload();
	}

	QString jwt;
	sdl::imtauth::Sessions::GetUserFromJwtRequestArguments arguments = getUserFromJwtRequest.GetRequestedArguments();
	if (arguments.input.Version_1_0->jwt){
		jwt = *arguments.input.Version_1_0->jwt;
	}

	response.userId = m_jwtSessionControllerCompPtr->GetUserFromJwt(jwt.toUtf8());

	sdl::imtauth::Sessions::CGetUserFromJwtPayload retVal;
	retVal.Version_1_0 = std::move(response);

	return retVal;
}


} // namespace imtauthgql
