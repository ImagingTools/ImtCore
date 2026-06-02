// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauthgql/CGqlJwtSessionControllerComp.h>
#include <GeneratedFiles/imtauthsdl/SDL/1.0/CPP/Sessions.h>


// ACF includes
#include <iser/CMemoryWriteArchive.h>

// ImtCore includes
#include <imtauth/ITenantInfo.h>
#include <imtgql/IGqlContext.h>


namespace imtauthgql
{


// protected methods

// reimplemented (sdl::V1_0::imtauth::CSessionsGqlHandlerCompBase)

sdl::V1_0::imtauth::CValidateSessionPayload CGqlJwtSessionControllerComp::OnValidateSession(
	const sdl::V1_0::imtauth::CValidateSessionGqlRequest& validateSessionRequest,
	const ::imtgql::CGqlRequest& /*gqlRequest*/,
	QString& /*errorMessage*/) const
{
	sdl::V1_0::imtauth::CValidateSessionPayload response;

	if (!m_jwtSessionControllerCompPtr.IsValid()){
		Q_ASSERT_X(false, "Attribute 'JwtSessionController' was not set", "CGqlJwtSessionControllerComp");
		return response;
	}

	QByteArray sessionId;
	sdl::V1_0::imtauth::ValidateSessionRequestArguments arguments = validateSessionRequest.GetRequestedArguments();
	if (!arguments.input.has_value()){
		Q_ASSERT(false);
		return response;
	}

	if (arguments.input->sessionId){
		sessionId = *arguments.input->sessionId;
	}

	response.isValid = m_jwtSessionControllerCompPtr->ValidateSession(sessionId);

	return response;
}


sdl::V1_0::imtauth::CValidateJwtPayload CGqlJwtSessionControllerComp::OnValidateJwt(
	const sdl::V1_0::imtauth::CValidateJwtGqlRequest& validateJwtRequest,
	const ::imtgql::CGqlRequest& /*gqlRequest*/,
	QString& /*errorMessage*/) const
{
	sdl::V1_0::imtauth::CValidateJwtPayload response;

	if (!m_jwtSessionControllerCompPtr.IsValid()){
		Q_ASSERT_X(false, "Attribute 'JwtSessionController' was not set", "CGqlJwtSessionControllerComp");
		return response;
	}

	QString jwt;
	sdl::V1_0::imtauth::ValidateJwtRequestArguments arguments = validateJwtRequest.GetRequestedArguments();
	if (!arguments.input.has_value()){
		Q_ASSERT(false);
		return response;
	}

	if (arguments.input->jwt){
		jwt = *arguments.input->jwt;
	}

	imtauth::IJwtSessionController::JwtState state = m_jwtSessionControllerCompPtr->ValidateJwt(jwt.toUtf8());

	response.state = sdl::V1_0::imtauth::JwtState::NONE;

	if (state == imtauth::IJwtSessionController::JS_EXPIRED){
		response.state = sdl::V1_0::imtauth::JwtState::EXPIRED;
	}
	else if (state == imtauth::IJwtSessionController::JS_INVALID){
		response.state = sdl::V1_0::imtauth::JwtState::INVALID;
	}
	else if (state == imtauth::IJwtSessionController::JS_OK){
		response.state = sdl::V1_0::imtauth::JwtState::OK;
	}

	return response;
}


sdl::V1_0::imtauth::CGetSessionPayload CGqlJwtSessionControllerComp::OnGetSession(
	const sdl::V1_0::imtauth::CGetSessionGqlRequest& getSessionRequest,
	const ::imtgql::CGqlRequest& /*gqlRequest*/,
	QString& /*errorMessage*/) const
{
	sdl::V1_0::imtauth::CGetSessionPayload response;

	if (!m_jwtSessionControllerCompPtr.IsValid()){
		Q_ASSERT_X(false, "Attribute 'JwtSessionController' was not set", "CGqlJwtSessionControllerComp");

		return response;
	}

	sdl::V1_0::imtauth::GetSessionRequestArguments arguments = getSessionRequest.GetRequestedArguments();
	if (!arguments.input.has_value()){
		Q_ASSERT(false);

		return response;
	}

	QByteArray sessionId;
	if (arguments.input->sessionId){
		sessionId = *arguments.input->sessionId;
	}

	imtauth::ISessionSharedPtr sessionInfoPtr = m_jwtSessionControllerCompPtr->GetSession(sessionId);
	if (!sessionInfoPtr.IsValid()){
		SendErrorMessage(0, QStringLiteral("Unable to get session '%1'. Error: Session not found").arg(sessionId));

		return response;
	}

	istd::TDelPtr<iser::CMemoryWriteArchive> archivePtr;
	if (m_versionInfoCompPtr.IsValid()){
		archivePtr.SetPtr(new iser::CMemoryWriteArchive(m_versionInfoCompPtr.GetPtr()));
	}
	else{
		archivePtr.SetPtr(new iser::CMemoryWriteArchive());
	}

	if (!sessionInfoPtr->Serialize(*archivePtr.GetPtr())){
		SendErrorMessage(0, QStringLiteral("Unable to serialize object. Error: Serialization failed"));

		return response;
	}

	QByteArray objectData = QByteArray((char*)archivePtr->GetBuffer(), archivePtr->GetBufferSize());

	response.sessionData = objectData;

	return response;
}


sdl::V1_0::imtauth::CRefreshTokenPayload CGqlJwtSessionControllerComp::OnRefreshToken(
	const sdl::V1_0::imtauth::CRefreshTokenGqlRequest& refreshTokenRequest,
	const ::imtgql::CGqlRequest& /*gqlRequest*/,
	QString& /*errorMessage*/) const
{
	sdl::V1_0::imtauth::CRefreshTokenPayload response;

	if (!m_jwtSessionControllerCompPtr.IsValid()){
		Q_ASSERT_X(false, "Attribute 'JwtSessionController' was not set", "CGqlJwtSessionControllerComp");
		return response;
	}

	QByteArray refreshToken;
	sdl::V1_0::imtauth::RefreshTokenRequestArguments arguments = refreshTokenRequest.GetRequestedArguments();
	if (!arguments.input.has_value()){
		Q_ASSERT(false);
		return response;
	}

	if (arguments.input->refreshToken){
		refreshToken = *arguments.input->refreshToken;
	}

	imtauth::IJwtSessionController::UserSession userSession;
	response.ok = m_jwtSessionControllerCompPtr->RefreshToken(refreshToken, userSession);
	if (response.ok){
		response.userSession = CreateUserSessionData(userSession);
	}

	return response;
}


sdl::V1_0::imtauth::CCreateNewSessionPayload CGqlJwtSessionControllerComp::OnCreateNewSession(
	const sdl::V1_0::imtauth::CCreateNewSessionGqlRequest& createNewSessionRequest,
	const ::imtgql::CGqlRequest& gqlRequest,
	QString& /*errorMessage*/) const
{
	sdl::V1_0::imtauth::CCreateNewSessionPayload response;

	if (!m_jwtSessionControllerCompPtr.IsValid()){
		Q_ASSERT_X(false, "Attribute 'JwtSessionController' was not set", "CGqlJwtSessionControllerComp");
		return response;
	}

	QByteArray userId;
	QByteArray tenantId;
	sdl::V1_0::imtauth::CreateNewSessionRequestArguments arguments = createNewSessionRequest.GetRequestedArguments();
	if (!arguments.input.has_value()){
		Q_ASSERT(false);
		return response;
	}

	if (arguments.input->userId){
		userId = *arguments.input->userId;
	}
	if (arguments.input->tenantId){
		tenantId = *arguments.input->tenantId;
	}

	if (!tenantId.isEmpty()){
		QByteArray authenticatedUserId = GetAuthenticatedUserId(gqlRequest);
		if (authenticatedUserId.isEmpty()){
			response.ok = false;
			response.errorMessage = QStringLiteral("Authenticated user is required to select tenant");
			return response;
		}

		if (authenticatedUserId != userId){
			response.ok = false;
			response.errorMessage = QStringLiteral("Cannot create tenant session for another user");
			return response;
		}

		QString tenantError;
		if (!CanUseTenant(userId, tenantId, tenantError)){
			response.ok = false;
			response.errorMessage = tenantError;
			return response;
		}
	}

	imtauth::IJwtSessionController::UserSession userSession;
	response.ok = m_jwtSessionControllerCompPtr->CreateNewSession(userId, tenantId, userSession);
	if (response.ok){
		response.userSession = CreateUserSessionData(userSession);
	}
	else{
		response.errorMessage = QStringLiteral("Failed to create session");
	}

	return response;
}


sdl::V1_0::imtauth::CSelectTenantPayload CGqlJwtSessionControllerComp::OnSelectTenant(
	const sdl::V1_0::imtauth::CSelectTenantGqlRequest& selectTenantRequest,
	const ::imtgql::CGqlRequest& gqlRequest,
	QString& /*errorMessage*/) const
{
	sdl::V1_0::imtauth::CSelectTenantPayload response;

	if (!m_jwtSessionControllerCompPtr.IsValid()){
		Q_ASSERT_X(false, "Attribute 'JwtSessionController' was not set", "CGqlJwtSessionControllerComp");
		return response;
	}

	QByteArray userId = GetAuthenticatedUserId(gqlRequest);
	if (userId.isEmpty()){
		response.ok = false;
		response.errorMessage = QStringLiteral("Authenticated user is required to select tenant");
		return response;
	}

	QByteArray tenantId;
	sdl::V1_0::imtauth::SelectTenantRequestArguments arguments = selectTenantRequest.GetRequestedArguments();
	if (!arguments.input.has_value()){
		Q_ASSERT(false);
		return response;
	}
	if (arguments.input->tenantId){
		tenantId = *arguments.input->tenantId;
	}

	if (!tenantId.isEmpty()){
		QString tenantError;
		if (!CanUseTenant(userId, tenantId, tenantError)){
			response.ok = false;
			response.errorMessage = tenantError;
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
	response.ok = m_jwtSessionControllerCompPtr->CreateNewSession(userId, tenantId, userSession);
	if (!response.ok){
		response.errorMessage = QStringLiteral("Failed to create tenant session");
		return response;
	}

	response.userSession = CreateUserSessionData(userSession);

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


sdl::V1_0::imtauth::CRemoveSessionPayload CGqlJwtSessionControllerComp::OnRemoveSession(
	const sdl::V1_0::imtauth::CRemoveSessionGqlRequest& removeSessionRequest,
	const ::imtgql::CGqlRequest& /*gqlRequest*/,
	QString& /*errorMessage*/) const
{
	sdl::V1_0::imtauth::CRemoveSessionPayload response;

	if (!m_jwtSessionControllerCompPtr.IsValid()){
		Q_ASSERT_X(false, "Attribute 'JwtSessionController' was not set", "CGqlJwtSessionControllerComp");
		return response;
	}

	QByteArray sessionId;
	sdl::V1_0::imtauth::RemoveSessionRequestArguments arguments = removeSessionRequest.GetRequestedArguments();
	if (!arguments.input.has_value()){
		Q_ASSERT(false);
		return response;
	}
	if (arguments.input->sessionId){
		sessionId = *arguments.input->sessionId;
	}

	response.ok = m_jwtSessionControllerCompPtr->RemoveSession(sessionId);

	return response;
}


sdl::V1_0::imtauth::CGetUserFromJwtPayload CGqlJwtSessionControllerComp::OnGetUserFromJwt(
	const sdl::V1_0::imtauth::CGetUserFromJwtGqlRequest& getUserFromJwtRequest,
	const ::imtgql::CGqlRequest& /*gqlRequest*/,
	QString& /*errorMessage*/) const
{
	sdl::V1_0::imtauth::CGetUserFromJwtPayload response;

	if (!m_jwtSessionControllerCompPtr.IsValid()){
		Q_ASSERT_X(false, "Attribute 'JwtSessionController' was not set", "CGqlJwtSessionControllerComp");
		return sdl::V1_0::imtauth::CGetUserFromJwtPayload();
	}

	QString jwt;
	sdl::V1_0::imtauth::GetUserFromJwtRequestArguments arguments = getUserFromJwtRequest.GetRequestedArguments();
	if (!arguments.input.has_value()){
		Q_ASSERT(false);
		return response;
	}
	if (arguments.input->jwt){
		jwt = *arguments.input->jwt;
	}

	response.userId = m_jwtSessionControllerCompPtr->GetUserFromJwt(jwt.toUtf8());

	return response;
}


sdl::V1_0::imtauth::CGetTenantFromJwtPayload CGqlJwtSessionControllerComp::OnGetTenantFromJwt(
	const sdl::V1_0::imtauth::CGetTenantFromJwtGqlRequest& getTenantFromJwtRequest,
	const ::imtgql::CGqlRequest& /*gqlRequest*/,
	QString& /*errorMessage*/) const
{
	sdl::V1_0::imtauth::CGetTenantFromJwtPayload response;

	if (!m_jwtSessionControllerCompPtr.IsValid()){
		Q_ASSERT_X(false, "Attribute 'JwtSessionController' was not set", "CGqlJwtSessionControllerComp");
		return sdl::V1_0::imtauth::CGetTenantFromJwtPayload();
	}

	QString jwt;
	sdl::V1_0::imtauth::GetTenantFromJwtRequestArguments arguments = getTenantFromJwtRequest.GetRequestedArguments();
	if (!arguments.input.has_value()){
		Q_ASSERT(false);
		return response;
	}
	if (arguments.input->jwt){
		jwt = *arguments.input->jwt;
	}

	response.tenantId = m_jwtSessionControllerCompPtr->GetTenantFromJwt(jwt.toUtf8());

	return response;
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


sdl::V1_0::imtauth::CUserSession CGqlJwtSessionControllerComp::CreateUserSessionData(
			const imtauth::IJwtSessionController::UserSession& userSession) const
{
	sdl::V1_0::imtauth::CUserSession userData;
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
