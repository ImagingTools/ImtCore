// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauthgql/CGqlJwtSessionControllerComp.h>


// ACF includes
#include <iser/CMemoryWriteArchive.h>

// ImtCore includes
#include <imtauth/ITenantInfo.h>
#include <imtgql/IGqlContext.h>


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
		response.Version_1_0->userSession = CreateUserSessionData(userSession);
	}

	return response;
}


sdl::imtauth::Sessions::CCreateNewSessionPayload CGqlJwtSessionControllerComp::OnCreateNewSession(
	const sdl::imtauth::Sessions::CCreateNewSessionGqlRequest& createNewSessionRequest,
	const ::imtgql::CGqlRequest& gqlRequest,
	QString& /*errorMessage*/) const
{
	sdl::imtauth::Sessions::CCreateNewSessionPayload response;

	if (!m_jwtSessionControllerCompPtr.IsValid()){
		Q_ASSERT_X(false, "Attribute 'JwtSessionController' was not set", "CGqlJwtSessionControllerComp");
		return response;
	}

	response.Version_1_0.emplace();

	QByteArray userId;
	QByteArray tenantId;
	sdl::imtauth::Sessions::CreateNewSessionRequestArguments arguments = createNewSessionRequest.GetRequestedArguments();
	if (arguments.input.Version_1_0->userId){
		userId = *arguments.input.Version_1_0->userId;
	}
	if (arguments.input.Version_1_0->tenantId){
		tenantId = *arguments.input.Version_1_0->tenantId;
	}

	if (!tenantId.isEmpty()){
		QByteArray authenticatedUserId = GetAuthenticatedUserId(gqlRequest);
		if (authenticatedUserId.isEmpty()){
			response.Version_1_0->ok = false;
			response.Version_1_0->errorMessage = QStringLiteral("Authenticated user is required to select tenant");
			return response;
		}

		if (authenticatedUserId != userId){
			response.Version_1_0->ok = false;
			response.Version_1_0->errorMessage = QStringLiteral("Cannot create tenant session for another user");
			return response;
		}

		QString tenantError;
		if (!CanUseTenant(userId, tenantId, tenantError)){
			response.Version_1_0->ok = false;
			response.Version_1_0->errorMessage = tenantError;
			return response;
		}
	}

	imtauth::IJwtSessionController::UserSession userSession;
	response.Version_1_0->ok = m_jwtSessionControllerCompPtr->CreateNewSession(userId, tenantId, userSession);
	if (response.Version_1_0->ok){
		response.Version_1_0->userSession = CreateUserSessionData(userSession);
	}
	else{
		response.Version_1_0->errorMessage = QStringLiteral("Failed to create session");
	}

	return response;
}


sdl::imtauth::Sessions::CSelectTenantPayload CGqlJwtSessionControllerComp::OnSelectTenant(
	const sdl::imtauth::Sessions::CSelectTenantGqlRequest& selectTenantRequest,
	const ::imtgql::CGqlRequest& gqlRequest,
	QString& /*errorMessage*/) const
{
	sdl::imtauth::Sessions::CSelectTenantPayload response;

	if (!m_jwtSessionControllerCompPtr.IsValid()){
		Q_ASSERT_X(false, "Attribute 'JwtSessionController' was not set", "CGqlJwtSessionControllerComp");
		return response;
	}

	response.Version_1_0.emplace();

	QByteArray userId = GetAuthenticatedUserId(gqlRequest);
	if (userId.isEmpty()){
		response.Version_1_0->ok = false;
		response.Version_1_0->errorMessage = QStringLiteral("Authenticated user is required to select tenant");
		return response;
	}

	QByteArray tenantId;
	sdl::imtauth::Sessions::SelectTenantRequestArguments arguments = selectTenantRequest.GetRequestedArguments();
	if (arguments.input.Version_1_0->tenantId){
		tenantId = *arguments.input.Version_1_0->tenantId;
	}

	if (!tenantId.isEmpty()){
		QString tenantError;
		if (!CanUseTenant(userId, tenantId, tenantError)){
			response.Version_1_0->ok = false;
			response.Version_1_0->errorMessage = tenantError;
			return response;
		}
	}

	QByteArray oldSessionId;
	QByteArray oldToken;
	const imtgql::IGqlContext* gqlContextPtr = gqlRequest.GetRequestContext();
	if (gqlContextPtr != nullptr){
		oldToken = gqlContextPtr->GetToken();
		oldSessionId = m_jwtSessionControllerCompPtr->GetSessionFromJwt(oldToken);
	}

	imtauth::IJwtSessionController::UserSession userSession;
	response.Version_1_0->ok = m_jwtSessionControllerCompPtr->CreateNewSession(userId, tenantId, userSession);
	if (!response.Version_1_0->ok){
		response.Version_1_0->errorMessage = QStringLiteral("Failed to create tenant session");
		return response;
	}

	response.Version_1_0->userSession = CreateUserSessionData(userSession);

	QByteArray newSessionId = m_jwtSessionControllerCompPtr->GetSessionFromJwt(userSession.accessToken);
	if (!oldSessionId.isEmpty() && oldSessionId != newSessionId){
		m_jwtSessionControllerCompPtr->RemoveSession(oldSessionId);
	}

	// Invalidate any cached state for the old token so that requests still
	// referencing it do not resolve to stale (e.g. empty-tenant) data.
	if (!oldToken.isEmpty() && oldToken != userSession.accessToken){
		m_jwtSessionControllerCompPtr->InvalidateToken(oldToken);
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


sdl::imtauth::Sessions::CGetTenantFromJwtPayload CGqlJwtSessionControllerComp::OnGetTenantFromJwt(
	const sdl::imtauth::Sessions::CGetTenantFromJwtGqlRequest& getTenantFromJwtRequest,
	const ::imtgql::CGqlRequest& /*gqlRequest*/,
	QString& /*errorMessage*/) const
{
	sdl::imtauth::Sessions::CGetTenantFromJwtPayload::V1_0 response;

	if (!m_jwtSessionControllerCompPtr.IsValid()){
		Q_ASSERT_X(false, "Attribute 'JwtSessionController' was not set", "CGqlJwtSessionControllerComp");
		return sdl::imtauth::Sessions::CGetTenantFromJwtPayload();
	}

	QString jwt;
	sdl::imtauth::Sessions::GetTenantFromJwtRequestArguments arguments = getTenantFromJwtRequest.GetRequestedArguments();
	if (arguments.input.Version_1_0->jwt){
		jwt = *arguments.input.Version_1_0->jwt;
	}

	response.tenantId = m_jwtSessionControllerCompPtr->GetTenantFromJwt(jwt.toUtf8());

	sdl::imtauth::Sessions::CGetTenantFromJwtPayload retVal;
	retVal.Version_1_0 = std::move(response);

	return retVal;
}


// private methods

QByteArray CGqlJwtSessionControllerComp::GetAuthenticatedUserId(const ::imtgql::CGqlRequest& gqlRequest) const
{
	const imtgql::IGqlContext* gqlContextPtr = gqlRequest.GetRequestContext();
	if (gqlContextPtr == nullptr){
		return QByteArray();
	}

	return gqlContextPtr->GetUserId();
}


bool CGqlJwtSessionControllerComp::CanUseTenant(
			const QByteArray& userId,
			const QByteArray& tenantId,
			QString& errorMessage) const
{
	if (tenantId.isEmpty()){
		errorMessage = QStringLiteral("Tenant id is empty");
		return false;
	}

	if (!m_tenantManagerCompPtr.IsValid()){
		errorMessage = QStringLiteral("Tenant manager is not configured");
		return false;
	}

	imtauth::ITenantInfoUniquePtr tenantPtr = m_tenantManagerCompPtr->GetTenant(tenantId);
	if (!tenantPtr.IsValid()){
		errorMessage = QStringLiteral("Tenant not found");
		return false;
	}

	if (!tenantPtr->IsActive()){
		errorMessage = QStringLiteral("Tenant is not active");
		return false;
	}

	if (tenantPtr->GetOwnerId() == userId){
		return true;
	}

	if (!m_membershipManagerCompPtr.IsValid()){
		errorMessage = QStringLiteral("Tenant membership manager is not configured");
		return false;
	}

	imtauth::ITenantMembershipUniquePtr membershipPtr = m_membershipManagerCompPtr->FindMembership(userId, tenantId);
	if (membershipPtr.IsValid() && membershipPtr->IsActive()){
		return true;
	}

	errorMessage = QStringLiteral("User is not an active member of tenant");
	return false;
}


sdl::imtauth::Sessions::CUserSession::V1_0 CGqlJwtSessionControllerComp::CreateUserSessionData(
			const imtauth::IJwtSessionController::UserSession& userSession) const
{
	sdl::imtauth::Sessions::CUserSession::V1_0 userData;
	userData.userId = userSession.userId;
	userData.tenantId = userSession.tenantId;
	userData.accessToken = userSession.accessToken;
	userData.refreshToken = userSession.refreshToken;

	// Resolve tenant name if tenant is set
	if (!userSession.tenantId.isEmpty() && m_tenantManagerCompPtr.IsValid()){
		imtauth::ITenantInfoUniquePtr tenantPtr = m_tenantManagerCompPtr->GetTenant(userSession.tenantId);
		if (tenantPtr.IsValid()){
			userData.tenantName = tenantPtr->GetTenantName();
		}
	}

	return userData;
}


} // namespace imtauthgql
