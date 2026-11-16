// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauthgql/CGqlJwtSessionControllerComp.h>

// ACF includes
#include <iser/CMemoryWriteArchive.h>
#include <iser/CJsonMemWriteArchive.h>

// ImtCore includes
#include <GeneratedFiles/imtauthsdl/SDL/1.0/CPP/Sessions.h>
#include <imtauth/ITenantInfo.h>
#include <imtauth/IUserInfo.h>
#include <imtbase/imtbase.h>
#include <imtgql/IGqlContext.h>
#include <imtauthgql/imtauthgql.h>


namespace imtauthgql
{


namespace
{


// A session is read and ended by its own user, or by the superuser
bool IsSessionOfRequest(const imtauth::ISession& session, const imtgql::CGqlRequest& gqlRequest)
{
	if (IsSuperuserRequest(gqlRequest)){
		return true;
	}

	const imtgql::IGqlContext* gqlContextPtr = gqlRequest.GetRequestContext();
	const QByteArray requestUserId = gqlContextPtr != nullptr ? gqlContextPtr->GetUserId() : QByteArray();

	return !requestUserId.isEmpty() && requestUserId == session.GetUserId();
}


} // anonymous namespace


// reimplemented (imtauth::ISessionAuthorizationProvider)

bool CGqlJwtSessionControllerComp::IsAvailable() const
{
	return true;
}


imtauth::ISessionAuthorizationProvider::SessionAuthorization CGqlJwtSessionControllerComp::GetSessionAuthorization(
			const QByteArray& accessToken,
			const QByteArray& productId,
			bool isEstablishedSubscription) const
{
	if (productId.isEmpty()){
		return {SAS_REFUSED, RR_FORBIDDEN, QStringLiteral("The product of the session is not known.")};
	}
	if (!m_authorizationSessionControllerCompPtr.IsValid()){
		return {SAS_UNCHECKED, RR_NONE, QStringLiteral("The session authorization source is not configured.")};
	}

	imtauth::SessionTokenClaims claims;
	QString message;
	switch (imtauth::CheckSessionToken(*m_authorizationSessionControllerCompPtr, accessToken, isEstablishedSubscription, claims, message)){
	case imtauth::STC_VALID:
		break;
	case imtauth::STC_REAUTHENTICATE:
		return {SAS_REFUSED, RR_REAUTHENTICATE, message};
	case imtauth::STC_INVALID:
		return {SAS_REFUSED, RR_FORBIDDEN, message};
	default:
		return {SAS_UNCHECKED, RR_NONE, message};
	}

	const imtauth::ISessionSharedPtr sessionPtr = m_authorizationSessionControllerCompPtr->GetSession(claims.sessionId);
	if (!sessionPtr.IsValid()){
		return {SAS_UNCHECKED, RR_NONE, QStringLiteral("The session could not be read.")};
	}
	if (sessionPtr->GetUserId() != claims.userId || sessionPtr->GetTenantId() != claims.tenantId || sessionPtr->GetProductId() != claims.productId){
		return {SAS_REFUSED, RR_FORBIDDEN, QStringLiteral("The token does not match its session.")};
	}
	if (claims.productId.isEmpty()){
		return {SAS_REFUSED, RR_FORBIDDEN, QStringLiteral("The session was opened at the authorization server and works only there.")};
	}

	SessionAuthorization retVal;
	retVal.userId = claims.userId;
	retVal.tenantId = claims.tenantId;
	retVal.sessionExpiresAt = sessionPtr->GetExpirationDate();
	if (!claims.tenantId.isEmpty()){
		if (!m_tenantManagerCompPtr.IsValid()){
			return {SAS_UNCHECKED, RR_NONE, QStringLiteral("The organization manager is not configured.")};
		}
		bool isRead = false;
		const imtauth::ITenantInfoUniquePtr tenantPtr = m_tenantManagerCompPtr->GetTenant(claims.tenantId, &isRead);
		if (!isRead){
			return {SAS_UNCHECKED, RR_NONE, QStringLiteral("The organization of the session could not be read.")};
		}
		if (!tenantPtr.IsValid()){
			return {SAS_REFUSED, RR_FORBIDDEN, QStringLiteral("The organization of the session does not exist.")};
		}

		retVal.tenantOwnerId = tenantPtr->GetOwnerId();
		retVal.tenantCreatorId = tenantPtr->GetCreatorId();
		retVal.tenantPermissions = tenantPtr->GetTenantPermissions();
	}

	const TenantUserSources sources{
		m_userCollectionCompPtr.GetPtr(),
		m_roleCollectionCompPtr.GetPtr(),
		m_userGroupCollectionCompPtr.GetPtr(),
		m_bindingManagerCompPtr.GetPtr(),
		m_delegatedAccessCompPtr.GetPtr(),
		m_membershipManagerCompPtr.GetPtr()
	};
	istd::IChangeableUniquePtr userPtr;
	switch (ComputeTenantUser(sources, claims.userId, claims.tenantId, productId, userPtr, message)){
	case imtservergql::CAuthorizationSnapshot::RS_READ:
		break;
	case imtservergql::CAuthorizationSnapshot::RS_ABSENT:
		return {SAS_REFUSED, RR_FORBIDDEN, QStringLiteral("User account is not available.")};
	case imtservergql::CAuthorizationSnapshot::RS_DENIED:
		return {SAS_REFUSED, RR_FORBIDDEN, message};
	default:
		return {SAS_UNCHECKED, RR_NONE, message};
	}

	const imtauth::IUserInfo* userInfoPtr = dynamic_cast<const imtauth::IUserInfo*>(userPtr.GetPtr());
	if (userInfoPtr == nullptr || !userInfoPtr->IsEnabled()){
		return {SAS_REFUSED, RR_FORBIDDEN, QStringLiteral("User account is disabled.")};
	}

	retVal.state = SAS_COMPUTED;
	retVal.userPtr.FromUnique(std::move(userPtr));
	return retVal;
}


// protected methods

// reimplemented (sdl::V1_0::imtauth::CSessionsGqlHandlerCompBase)

sdl::V1_0::imtauth::CGetSessionAuthorizationPayload CGqlJwtSessionControllerComp::OnGetSessionAuthorization(
			const sdl::V1_0::imtauth::CGetSessionAuthorizationGqlRequest& getSessionAuthorizationRequest,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& /*errorMessage*/) const
{
	namespace sessionsdl = sdl::V1_0::imtauth;
	sessionsdl::CGetSessionAuthorizationPayload response;
	QByteArray productId;
	QString message;
	SessionAuthorization authorization;
	authorization.state = CheckAuthorizationServer(gqlRequest, productId, message);
	authorization.refusalReason = RR_FORBIDDEN;
	authorization.message = message;
	if (authorization.state == SAS_COMPUTED){
		const sessionsdl::GetSessionAuthorizationRequestArguments arguments = getSessionAuthorizationRequest.GetRequestedArguments();
		const QByteArray accessToken = (arguments.input.HasValue() && arguments.input->accessToken.HasValue()) ? *arguments.input->accessToken : QByteArray();
		const bool isEstablished = arguments.input.HasValue() && arguments.input->isEstablishedSubscription.HasValue() && *arguments.input->isEstablishedSubscription;
		authorization = GetSessionAuthorization(accessToken, productId, isEstablished);
	}

	switch (authorization.state){
	case SAS_COMPUTED:
		response.state = sessionsdl::SessionAuthorizationState::COMPUTED;
		break;
	case SAS_REFUSED:
		response.state = sessionsdl::SessionAuthorizationState::REFUSED;
		response.refusalReason = (authorization.refusalReason == RR_REAUTHENTICATE)
					? sessionsdl::SessionRefusalReason::REAUTHENTICATE
					: sessionsdl::SessionRefusalReason::FORBIDDEN;
		break;
	default:
		response.state = sessionsdl::SessionAuthorizationState::UNCHECKED;
		break;
	}
	response.message = authorization.message;
	if (authorization.state != SAS_COMPUTED){
		return response;
	}

	iser::ISerializable* serializablePtr = dynamic_cast<iser::ISerializable*>(authorization.userPtr.GetPtr());
	iser::CJsonMemWriteArchive archive(m_versionInfoCompPtr.GetPtr());
	if (serializablePtr == nullptr || !serializablePtr->Serialize(archive)){
		response.state = sessionsdl::SessionAuthorizationState::UNCHECKED;
		response.message = QStringLiteral("The user could not be serialized.");
		return response;
	}

	response.refusalReason = sessionsdl::SessionRefusalReason::NONE;
	response.userId = authorization.userId;
	response.tenantId = authorization.tenantId;
	response.userData = QString::fromUtf8(archive.GetData());
	response.tenantOwnerId = authorization.tenantOwnerId;
	response.tenantCreatorId = authorization.tenantCreatorId;
	if (authorization.sessionExpiresAt.isValid()){
		response.sessionExpiresAt = authorization.sessionExpiresAt.toUTC().toString(Qt::ISODateWithMs);
	}
	response.tenantPermissions.Emplace();
	response.tenantPermissions->FromList(authorization.tenantPermissions);
	return response;
}


sdl::V1_0::imtauth::CValidateSessionPayload CGqlJwtSessionControllerComp::OnValidateSession(
	const sdl::V1_0::imtauth::CValidateSessionGqlRequest& validateSessionRequest,
	const ::imtgql::CGqlRequest& /*gqlRequest*/,
	QString& errorMessage) const
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

	// A session that could not be checked is an error of the request: a caller would take "not valid" for a closed session
	bool isChecked = false;
	const bool isValid = m_jwtSessionControllerCompPtr->ValidateSession(sessionId, &isChecked);
	if (!isChecked){
		errorMessage = QStringLiteral("The session could not be checked.");

		return response;
	}

	response.isValid = isValid;

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
	const ::imtgql::CGqlRequest& gqlRequest,
	QString& errorMessage) const
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

	// A session of another user is answered like a missing one
	imtauth::ISessionSharedPtr sessionInfoPtr = m_jwtSessionControllerCompPtr->GetSession(sessionId);
	if (!sessionInfoPtr.IsValid()){
		SendErrorMessage(0, QStringLiteral("Unable to get session '%1'. Error: Session not found").arg(sessionId));
		errorMessage = QStringLiteral("Unable to get session. Error: Session not found");

		return response;
	}

	if (!IsSessionOfRequest(*sessionInfoPtr, gqlRequest)){
		SendWarningMessage(0, QStringLiteral("Session '%1' was requested by another user").arg(sessionId));
		errorMessage = QStringLiteral("Unable to get session. Error: Session not found");

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
	const ::imtgql::CGqlRequest& gqlRequest,
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
	userSession.productId = GetRequestServerProductId(gqlRequest);
	response.ok = m_jwtSessionControllerCompPtr->RefreshToken(refreshToken, userSession);
	if (!response.ok){
		return response;
	}

	// A refresh is no new login, but the password lifetime ends the session like any other
	if (IsPasswordChangeRequired(userSession.userId)){
		SendWarningMessage(0, QStringLiteral("Token refresh refused: the password of user '%1' has expired or must be changed").arg(QString::fromUtf8(userSession.userId)));

		m_jwtSessionControllerCompPtr->RemoveSession(m_jwtSessionControllerCompPtr->GetSessionFromJwt(userSession.accessToken));
		response.ok = false;

		return response;
	}

	response.userSession = CreateUserSessionData(userSession);

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

	// Only the authenticated user themselves or the superuser may open a session for a user.
	const QByteArray authenticatedUserId = GetAuthenticatedUserId(gqlRequest);
	if (authenticatedUserId.isEmpty()){
		response.ok = false;
		response.errorMessage = QStringLiteral("Authenticated user is required to create a session");
		return response;
	}

	if (userId.isEmpty() || (authenticatedUserId != userId && !IsSuperuserRequest(gqlRequest))){
		response.ok = false;
		response.errorMessage = QStringLiteral("Cannot create a session for another user");
		return response;
	}

	if (!tenantId.isEmpty()){
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
	userSession.productId = GetRequestServerProductId(gqlRequest);
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
	userSession.productId = GetRequestServerProductId(gqlRequest);
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
	const ::imtgql::CGqlRequest& gqlRequest,
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

	imtauth::ISessionSharedPtr sessionInfoPtr = m_jwtSessionControllerCompPtr->GetSession(sessionId);
	if (!sessionInfoPtr.IsValid() || !IsSessionOfRequest(*sessionInfoPtr, gqlRequest)){
		response.ok = false;

		return response;
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

bool CGqlJwtSessionControllerComp::IsPasswordChangeRequired(const QByteArray& userId) const
{
	if (!m_passwordPolicyCompPtr.IsValid() || !m_userCollectionCompPtr.IsValid()){
		return false;
	}

	// Fails closed: a user that cannot be read gets no new token
	imtbase::IObjectCollection::DataPtr dataPtr;
	if (!m_userCollectionCompPtr->GetObjectData(userId, dataPtr)){
		return true;
	}

	const imtauth::IUserInfo* userInfoPtr = dynamic_cast<const imtauth::IUserInfo*>(dataPtr.GetPtr());
	if (userInfoPtr == nullptr){
		return true;
	}

	// The session does not tell which system signed the user in, so a user with an external account
	// (e.g. LDAP) is left to that system, as its login skips the lifetime check as well
	bool hasInternalPassword = false;
	for (const imtauth::IUserInfo::SystemInfo& systemInfo : userInfoPtr->GetSystemInfos()){
		if (!systemInfo.enabled){
			continue;
		}

		if (!systemInfo.systemId.isEmpty()){
			return false;
		}

		hasInternalPassword = true;
	}

	if (!hasInternalPassword){
		return false;
	}

	int daysUntilExpiration = -1;
	const imtauth::IPasswordPolicy::LifetimeStatus lifetimeStatus = m_passwordPolicyCompPtr->GetPasswordLifetimeStatus(*userInfoPtr, daysUntilExpiration);

	return lifetimeStatus == imtauth::IPasswordPolicy::LS_EXPIRED || lifetimeStatus == imtauth::IPasswordPolicy::LS_CHANGE_REQUIRED;
}


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
		errorMessage = QStringLiteral("Organization not found");
		return false;
	}

	if (!tenantPtr->IsActive()){
		errorMessage = QStringLiteral("The organization is not active");
		return false;
	}

	// The creator manages the permission scope of the organization, which is edited only from inside it
	if (tenantPtr->GetOwnerId() == userId || tenantPtr->GetCreatorId() == userId){
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

	// Check delegated access via cross-org grants
	if (m_delegatedAccessCompPtr.IsValid()){
		// Resolve user's home tenants from direct memberships
		const imtauth::ITenantMembershipManager::MembershipIds membershipIds = m_membershipManagerCompPtr->GetMembershipsByUser(userId);
		for (const QByteArray& membershipId : membershipIds){
			imtauth::ITenantMembershipUniquePtr homeMembershipPtr = m_membershipManagerCompPtr->GetMembership(membershipId);
			if (!homeMembershipPtr.IsValid() || !homeMembershipPtr->IsActive()){
				continue;
			}
			QByteArray homeTenantId = homeMembershipPtr->GetTenantId();
			if (homeTenantId.isEmpty()){
				continue;
			}
			if (m_delegatedAccessCompPtr->IsDelegatedAccess(userId, homeTenantId, tenantId)){
				return true;
			}
		}
	}

	errorMessage = QStringLiteral("You are not an active member of this organization");
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


imtauth::ISessionAuthorizationProvider::AuthorizationState CGqlJwtSessionControllerComp::CheckAuthorizationServer(
			const ::imtgql::CGqlRequest& gqlRequest,
			QByteArray& productId,
			QString& message) const
{
	// The session token is proof in the body; only the product's own server token authenticates this operation.
	const imtgql::IGqlContext* gqlContextPtr = gqlRequest.GetRequestContext();
	const QByteArray senderProductId = (gqlContextPtr != nullptr) ? gqlContextPtr->GetServerProductId() : QByteArray();
	const QByteArray serverToken = (gqlContextPtr != nullptr) ? gqlContextPtr->GetHeaders().value(imtbase::s_serverTokenHeaderId) : QByteArray();
	if (senderProductId.isEmpty() || !imtauth::IsServerToken(serverToken) || !gqlContextPtr->GetUserId().isEmpty()
				|| !gqlContextPtr->GetToken().isEmpty() || gqlContextPtr->GetProductId() != senderProductId){
		message = QStringLiteral("The authorization of a session is given to the server of a product only.");
		return SAS_REFUSED;
	}

	imtauth::IPersonalAccessTokenManager* tokenManagerPtr = m_serverTokenReceiverCompPtr.IsValid() ? m_serverTokenReceiverCompPtr->GetTokenManager() : nullptr;
	if (tokenManagerPtr == nullptr){
		message = QStringLiteral("The server tokens are not configured.");
		return SAS_UNCHECKED;
	}

	QByteArray tokenUserId;
	QByteArray tokenId;
	QByteArrayList scopes;
	QByteArray tokenProductId;
	QByteArray clientProductId;
	bool isCheckFailed = false;
	if (!tokenManagerPtr->ValidateToken(serverToken, tokenUserId, tokenId, scopes, &tokenProductId, &clientProductId, &isCheckFailed)){
		message = isCheckFailed ? QStringLiteral("The token of the server could not be checked.") : QStringLiteral("The token of the server is revoked, expired or unknown.");
		return isCheckFailed ? SAS_UNCHECKED : SAS_REFUSED;
	}
	if (tokenProductId != m_serverTokenReceiverCompPtr->GetServerProductId() || clientProductId.isEmpty() || clientProductId != senderProductId){
		message = QStringLiteral("The token of the server is not issued for this server and the product of the request.");
		return SAS_REFUSED;
	}

	productId = clientProductId;
	return SAS_COMPUTED;
}


} // namespace imtauthgql
