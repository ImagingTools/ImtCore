#include <imtauthgql/CGqlJwtSessionControllerComp.h>


// ACF includes
#include <iser/CMemoryWriteArchive.h>


namespace imtauthgql
{


// protected methods

// reimplemented (sdl::imtauth::Session::CGraphQlHandlerCompBase)

sdl::imtauth::Session::CValidateSessionPayload CGqlJwtSessionControllerComp::OnValidateSession(
	const sdl::imtauth::Session::CValidateSessionGqlRequest& validateSessionRequest,
	const ::imtgql::CGqlRequest& /*gqlRequest*/,
	QString& /*errorMessage*/) const
{
	sdl::imtauth::Session::CValidateSessionPayload response;

	if (!m_jwtSessionControllerCompPtr.IsValid()){
		Q_ASSERT_X(false, "Attribute 'JwtSessionController' was not set", "CGqlJwtSessionControllerComp");
		return response;
	}

	response.Version_1_0.emplace();

	QByteArray sessionId;
	sdl::imtauth::Session::ValidateSessionRequestArguments arguments = validateSessionRequest.GetRequestedArguments();
	if (arguments.input.Version_1_0->sessionId){
		sessionId = *arguments.input.Version_1_0->sessionId;
	}

	response.Version_1_0->isValid = m_jwtSessionControllerCompPtr->ValidateSession(sessionId);

	return response;
}


sdl::imtauth::Session::CValidateJwtPayload CGqlJwtSessionControllerComp::OnValidateJwt(
	const sdl::imtauth::Session::CValidateJwtGqlRequest& validateJwtRequest,
	const ::imtgql::CGqlRequest& /*gqlRequest*/,
	QString& /*errorMessage*/) const
{
	sdl::imtauth::Session::CValidateJwtPayload response;

	if (!m_jwtSessionControllerCompPtr.IsValid()){
		Q_ASSERT_X(false, "Attribute 'JwtSessionController' was not set", "CGqlJwtSessionControllerComp");
		return response;
	}

	QByteArray jwt;
	sdl::imtauth::Session::ValidateJwtRequestArguments arguments = validateJwtRequest.GetRequestedArguments();
	if (arguments.input.Version_1_0->jwt){
		jwt = *arguments.input.Version_1_0->jwt;
	}

	response.Version_1_0.emplace();

	imtauth::IJwtSessionController::JwtState state = m_jwtSessionControllerCompPtr->ValidateJwt(jwt);

	response.Version_1_0->state = sdl::imtauth::Session::JwtState::NONE;

	if (state == imtauth::IJwtSessionController::JS_EXPIRED){
		response.Version_1_0->state = sdl::imtauth::Session::JwtState::EXPIRED;
	}
	else if (state == imtauth::IJwtSessionController::JS_INVALID){
		response.Version_1_0->state = sdl::imtauth::Session::JwtState::INVALID;
	}
	else if (state == imtauth::IJwtSessionController::JS_OK){
		response.Version_1_0->state = sdl::imtauth::Session::JwtState::OK;
	}

	return response;
}


sdl::imtauth::Session::CGetSessionPayload CGqlJwtSessionControllerComp::OnGetSession(
	const sdl::imtauth::Session::CGetSessionGqlRequest& getSessionRequest,
	const ::imtgql::CGqlRequest& /*gqlRequest*/,
	QString& /*errorMessage*/) const
{
	sdl::imtauth::Session::CGetSessionPayload response;

	if (!m_jwtSessionControllerCompPtr.IsValid()){
		Q_ASSERT_X(false, "Attribute 'JwtSessionController' was not set", "CGqlJwtSessionControllerComp");
		return response;
	}

	response.Version_1_0.emplace();

	QByteArray sessionId;
	sdl::imtauth::Session::GetSessionRequestArguments arguments = getSessionRequest.GetRequestedArguments();
	if (arguments.input.Version_1_0->sessionId){
		sessionId = *arguments.input.Version_1_0->sessionId;
	}

	istd::TDelPtr<const imtauth::ISession> sessionInfoPtr = m_jwtSessionControllerCompPtr->GetSession(sessionId);

	istd::TDelPtr<iser::CMemoryWriteArchive> archivePtr;
	if (m_versionInfoCompPtr.IsValid()){
		archivePtr.SetPtr(new iser::CMemoryWriteArchive(m_versionInfoCompPtr.GetPtr()));
	}
	else{
		archivePtr.SetPtr(new iser::CMemoryWriteArchive());
	}

	imtauth::ISession* sessionInfoPtrNotConst = const_cast<imtauth::ISession*>(sessionInfoPtr.GetPtr());
	Q_ASSERT(sessionInfoPtrNotConst != nullptr);

	if (!sessionInfoPtrNotConst->Serialize(*archivePtr.GetPtr())){
		SendErrorMessage(0, QString("Unable to serialize object. Error: Serialization failed"));
		return response;
	}

	QByteArray objectData = QByteArray((char*)archivePtr->GetBuffer(), archivePtr->GetBufferSize());

	response.Version_1_0->sessionData = objectData;

	return response;
}


sdl::imtauth::Session::CRefreshTokenPayload CGqlJwtSessionControllerComp::OnRefreshToken(
	const sdl::imtauth::Session::CRefreshTokenGqlRequest& refreshTokenRequest,
	const ::imtgql::CGqlRequest& /*gqlRequest*/,
	QString& /*errorMessage*/) const
{
	sdl::imtauth::Session::CRefreshTokenPayload response;

	if (!m_jwtSessionControllerCompPtr.IsValid()){
		Q_ASSERT_X(false, "Attribute 'JwtSessionController' was not set", "CGqlJwtSessionControllerComp");
		return response;
	}

	response.Version_1_0.emplace();

	QByteArray refreshToken;
	sdl::imtauth::Session::RefreshTokenRequestArguments arguments = refreshTokenRequest.GetRequestedArguments();
	if (arguments.input.Version_1_0->refreshToken){
		refreshToken = *arguments.input.Version_1_0->refreshToken;
	}

	imtauth::IJwtSessionController::UserSession userSession;
	response.Version_1_0->ok = m_jwtSessionControllerCompPtr->RefreshToken(refreshToken, userSession);
	if (response.Version_1_0->ok){
		sdl::imtauth::Session::CUserSession::V1_0 userData;
		userData.userId = userSession.userId;
		userData.accessToken = userSession.accessToken;
		userData.refreshToken = userSession.refreshToken;

		response.Version_1_0->userSession = userData;
	}

	return response;
}


sdl::imtauth::Session::CCreateNewSessionPayload CGqlJwtSessionControllerComp::OnCreateNewSession(
	const sdl::imtauth::Session::CCreateNewSessionGqlRequest& createNewSessionRequest,
	const ::imtgql::CGqlRequest& /*gqlRequest*/,
	QString& /*errorMessage*/) const
{
	sdl::imtauth::Session::CCreateNewSessionPayload response;

	if (!m_jwtSessionControllerCompPtr.IsValid()){
		Q_ASSERT_X(false, "Attribute 'JwtSessionController' was not set", "CGqlJwtSessionControllerComp");
		return response;
	}

	response.Version_1_0.emplace();

	QByteArray userId;
	sdl::imtauth::Session::CreateNewSessionRequestArguments arguments = createNewSessionRequest.GetRequestedArguments();
	if (arguments.input.Version_1_0->userId){
		userId = *arguments.input.Version_1_0->userId;
	}

	imtauth::IJwtSessionController::UserSession userSession;
	response.Version_1_0->ok = m_jwtSessionControllerCompPtr->CreateNewSession(userId, userSession);
	if (response.Version_1_0->ok){
		sdl::imtauth::Session::CUserSession::V1_0 userData;
		userData.userId = userSession.userId;
		userData.accessToken = userSession.accessToken;
		userData.refreshToken = userSession.refreshToken;

		response.Version_1_0->userSession = userData;
	}

	return response;
}


sdl::imtauth::Session::CRemoveSessionPayload CGqlJwtSessionControllerComp::OnRemoveSession(
	const sdl::imtauth::Session::CRemoveSessionGqlRequest& removeSessionRequest,
	const ::imtgql::CGqlRequest& /*gqlRequest*/,
	QString& /*errorMessage*/) const
{
	sdl::imtauth::Session::CRemoveSessionPayload response;

	if (!m_jwtSessionControllerCompPtr.IsValid()){
		Q_ASSERT_X(false, "Attribute 'JwtSessionController' was not set", "CGqlJwtSessionControllerComp");
		return response;
	}

	response.Version_1_0.emplace();

	QByteArray sessionId;
	sdl::imtauth::Session::RemoveSessionRequestArguments arguments = removeSessionRequest.GetRequestedArguments();
	if (arguments.input.Version_1_0->sessionId){
		sessionId = *arguments.input.Version_1_0->sessionId;
	}

	response.Version_1_0->ok = m_jwtSessionControllerCompPtr->RemoveSession(sessionId);

	return response;
}


sdl::imtauth::Session::CGetUserFromJwtPayload CGqlJwtSessionControllerComp::OnGetUserFromJwt(
	const sdl::imtauth::Session::CGetUserFromJwtGqlRequest& getUserFromJwtRequest,
	const ::imtgql::CGqlRequest& /*gqlRequest*/,
	QString& /*errorMessage*/) const
{
	sdl::imtauth::Session::CGetUserFromJwtPayload::V1_0 response;

	if (!m_jwtSessionControllerCompPtr.IsValid()){
		Q_ASSERT_X(false, "Attribute 'JwtSessionController' was not set", "CGqlJwtSessionControllerComp");
		return sdl::imtauth::Session::CGetUserFromJwtPayload();
	}

	QByteArray jwt;
	sdl::imtauth::Session::GetUserFromJwtRequestArguments arguments = getUserFromJwtRequest.GetRequestedArguments();
	if (arguments.input.Version_1_0->jwt){
		jwt = *arguments.input.Version_1_0->jwt;
	}

	response.userId = m_jwtSessionControllerCompPtr->GetUserFromJwt(jwt);

	sdl::imtauth::Session::CGetUserFromJwtPayload retVal;
	retVal.Version_1_0 = std::make_optional(response);

	return retVal;
}


} // namespace imtauthgql
