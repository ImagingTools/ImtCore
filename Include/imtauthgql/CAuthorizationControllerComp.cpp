// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauthgql/CAuthorizationControllerComp.h>
#include <GeneratedFiles/imtauthsdl/SDL/1.0/CPP/Authorization.h>


// ACF includes
#include <iprm/CParamsSet.h>

// ImtCore includes
#include <imtbase/CComplexCollectionFilter.h>
#include <imtauth/CUserInfo.h>
#include <imtauth/CUserConnectionInfo.h>
#include <imtauth/ITenantInfo.h>
#include <imtauthgql/imtauthgql.h>


namespace imtauthgql
{


// protected methods

bool CAuthorizationControllerComp::ParseDataFromGqlRequest(
			const imtgql::CGqlRequest& gqlRequest,
			QByteArray& login,
			QByteArray& password,
			QByteArray& productId) const
{
	const imtgql::CGqlParamObject* gqlInputParamPtr = gqlRequest.GetParamObject("input");
	if (gqlInputParamPtr == nullptr){
		return false;
	}

	login = gqlInputParamPtr->GetParamArgumentValue("login").toByteArray();
	productId = gqlInputParamPtr->GetParamArgumentValue("productId").toByteArray();
	password = gqlInputParamPtr->GetParamArgumentValue("password").toByteArray();

	return true;
}


QByteArray CAuthorizationControllerComp::GetUserObjectId(const QByteArray& login) const
{
	imtbase::IComplexCollectionFilter::FieldFilter fieldFilter;
	fieldFilter.fieldId = "Id";
	fieldFilter.filterValue = login;

	imtbase::IComplexCollectionFilter::FilterExpression groupFilter;
	groupFilter.fieldFilters << fieldFilter;

	imtbase::CComplexCollectionFilter complexFilter;
	complexFilter.SetFilterExpression(groupFilter);

	iprm::CParamsSet filterParam;
	filterParam.SetEditableParameter("ComplexFilter", &complexFilter);

	imtbase::IObjectCollection::Ids userIds = m_userCollectionCompPtr->GetElementIds(0, -1, &filterParam);
	if (userIds.isEmpty()){
		return QByteArray();
	}

	return userIds[0];
}


bool CAuthorizationControllerComp::CheckCredential(
			const QByteArray& systemId,
			const QByteArray& login,
			const QByteArray& password) const
{
	int index = m_systemIdsAttrPtr.FindValue(systemId);
	const QByteArray errorMessage = QStringLiteral("System-ID '%1' cannot found").arg(systemId).toUtf8();
	Q_ASSERT_X(index >= 0, "CAuthorizationControllerComp::CreateInternalResponse", errorMessage.constData());

	const imtauth::ICredentialController* credentialControllerPtr = m_credentialControllersCompPtr[index];
	Q_ASSERT_X(credentialControllerPtr != nullptr, "CAuthorizationControllerComp::CreateInternalResponse", "Invalid credential controller");

	return credentialControllerPtr->CheckCredential(login, password);
}


QByteArrayList CAuthorizationControllerComp::CalculateGlobalPermissions(
			const imtauth::IUserInfo& userInfo,
			const QByteArray& userId,
			const QByteArray& productId) const
{
	QSet<QByteArray> userPermissionsSet;
	for (const QByteArray& permissionId : userInfo.GetPermissions(productId)){
		if (!permissionId.isEmpty()){
			userPermissionsSet.insert(permissionId);
		}
	}

	QSet<QByteArray> tenantPermissionsSet;
	if (m_tenantMembershipManagerCompPtr.IsValid() && m_tenantManagerCompPtr.IsValid()){
		QSet<QByteArray> tenantIds;
		const QByteArrayList membershipIds = m_tenantMembershipManagerCompPtr->GetMembershipsByUser(userId);
		for (const QByteArray& membershipId : membershipIds){
			imtauth::ITenantMembershipUniquePtr membershipPtr = m_tenantMembershipManagerCompPtr->GetMembership(membershipId);
			if (membershipPtr.IsValid() && membershipPtr->IsActive() && !membershipPtr->GetTenantId().isEmpty()){
				tenantIds.insert(membershipPtr->GetTenantId());
			}
		}

		for (const QByteArray& tenantId : std::as_const(tenantIds)){
			for (const QByteArray& permissionId : m_tenantManagerCompPtr->GetTenantPermissions(tenantId)){
				if (!permissionId.isEmpty()){
					tenantPermissionsSet.insert(permissionId);
				}
			}
		}
	}

	QByteArrayList globalPermissions;
	for (const QByteArray& permissionId : std::as_const(userPermissionsSet)){
		if (!tenantPermissionsSet.contains(permissionId)){
			globalPermissions.push_back(permissionId);
		}
	}

	std::sort(globalPermissions.begin(), globalPermissions.end());
	return globalPermissions;
}


sdl::V1_0::imtauth::CAuthorizationPayload CAuthorizationControllerComp::CreateInvalidLoginOrPasswordResponse(
			const QByteArray& login,
			QString& errorMessage) const
{
	errorMessage = QT_TR_NOOP(QStringLiteral("Invalid login or password. Login: '%1'").arg(login));
	SendErrorMessage(0, errorMessage, "imtgql::CAuthorizationControllerComp");

	return sdl::V1_0::imtauth::CAuthorizationPayload();
}


sdl::V1_0::imtauth::CAuthorizationPayload CAuthorizationControllerComp::CreateAccountDisabledResponse(
			const QByteArray& login) const
{
	// Only reached once the credentials were verified, so naming the account state
	// discloses nothing the caller has not already proven. Reported through the
	// payload, not through errorMessage: a non-empty errorMessage makes the
	// generated handler drop the payload, and with it the flag.
	SendWarningMessage(0,
					QStringLiteral("Authorization denied for disabled account. Login: '%1'").arg(login),
					"imtgql::CAuthorizationControllerComp");

	sdl::V1_0::imtauth::CAuthorizationPayload payload;
	payload.accountDisabled = true;

	return payload;
}


sdl::V1_0::imtauth::CAuthorizationPayload CAuthorizationControllerComp::CreateAccountLockedResponse(
			const QByteArray& login,
			qint64 remainingLockoutSecs,
			QString& errorMessage) const
{
	// The user reads this message, so it says when to try again rather than only that the account is locked.
	QString retryText;
	if (remainingLockoutSecs < 0){
		retryText = QStringLiteral("It stays locked until an administrator unlocks it.");
	}
	else if (remainingLockoutSecs < 60){
		retryText = (remainingLockoutSecs <= 1) ? QStringLiteral("Try again in 1 second.") : QStringLiteral("Try again in %1 seconds.").arg(remainingLockoutSecs);
	}
	else{
		qint64 minutes = (remainingLockoutSecs + 59) / 60;
		if (minutes < 120){
			retryText = (minutes == 1) ? QStringLiteral("Try again in 1 minute.") : QStringLiteral("Try again in %1 minutes.").arg(minutes);
		}
		else{
			retryText = QStringLiteral("Try again in %1 hours.").arg((minutes + 59) / 60);
		}
	}

	errorMessage = QStringLiteral("Too many invalid sign-in attempts. The account is locked. %1").arg(retryText);
	SendErrorMessage(0, QStringLiteral("%1 Login: '%2'").arg(errorMessage, QString::fromUtf8(login)), "imtgql::CAuthorizationControllerComp");

	return sdl::V1_0::imtauth::CAuthorizationPayload();
}


sdl::V1_0::imtauth::CAuthorizationPayload CAuthorizationControllerComp::CreateFailedAttemptResponse(
			const QByteArray& login,
			QString& errorMessage) const
{
	RegisterAccessAttempt(login, false);

	qint64 remainingLockoutSecs = 0;
	if (IsAccountLocked(login, &remainingLockoutSecs)){
		return CreateAccountLockedResponse(login, remainingLockoutSecs, errorMessage);
	}

	return CreateInvalidLoginOrPasswordResponse(login, errorMessage);
}


bool CAuthorizationControllerComp::IsAccountLocked(const QByteArray& login, qint64* remainingLockoutSecsPtr) const
{
	if (!m_accountLockoutControllerCompPtr.IsValid()){
		return false;
	}

	return m_accountLockoutControllerCompPtr->IsAccountLocked(login, remainingLockoutSecsPtr);
}


void CAuthorizationControllerComp::RegisterAccessAttempt(const QByteArray& login, bool successful) const
{
	if (!m_accountLockoutControllerCompPtr.IsValid()){
		return;
	}

	if (successful){
		m_accountLockoutControllerCompPtr->RegisterSuccessfulAttempt(login);
	}
	else{
		m_accountLockoutControllerCompPtr->RegisterFailedAttempt(login);
	}
}


sdl::V1_0::imtauth::CAuthorizationPayload CAuthorizationControllerComp::CreateAuthorizationSuccessfulResponse(
			imtauth::CUserInfo& userInfo,
			const QByteArray& systemId,
			const QByteArray& productId,
			QString& errorMessage) const
{
	sdl::V1_0::imtauth::CAuthorizationPayload payload;

	QByteArray login = userInfo.GetId();
	QByteArray objectId = GetUserObjectId(login);
	Q_ASSERT(!objectId.isEmpty());
	if (objectId.isEmpty()){
		errorMessage = QStringLiteral("Unable to create authorization request. Error: User with login '%1' does not exists").arg(login);
		return payload;
	}

	QByteArray tokenValue = QUuid::createUuid().toByteArray(QUuid::WithoutBraces);

	payload.token = tokenValue;
	payload.username = login;
	payload.userId = objectId;
	payload.systemId = systemId;

	// During initial Authorization/UserToken flow tenant is not selected yet.
	// Return only non-tenant (global) permissions here; tenant-scoped permissions
	// are returned by GetPermissions after tenant selection.
	// Global permissions are calculated as all user permissions minus the union
	// of permissions from all tenants доступных пользователю.
	if (!productId.isEmpty()){
		QByteArrayList globalPermissions = CalculateGlobalPermissions(userInfo, objectId, productId);
		payload.permissions = globalPermissions.join(';');
	}

	if (m_jwtSessionControllerCompPtr.IsValid()){
		imtauth::IJwtSessionController::UserSession userSession;
		if (m_jwtSessionControllerCompPtr->CreateNewSession(objectId, QByteArray(), userSession)){
			payload.refreshToken = userSession.refreshToken;
			payload.token = userSession.accessToken;
			payload.userId = userSession.userId;
		}
	}

	if (m_userConnectionCollectionCompPtr.IsValid()){
		istd::TDelPtr<imtauth::CUserConnectionInfo> userConnectionInfoPtr;
		userConnectionInfoPtr.SetPtr(new imtauth::CUserConnectionInfo);

		userConnectionInfoPtr->SetUserId(objectId);
		userConnectionInfoPtr->SetLastConnection(QDateTime::currentDateTimeUtc());

		imtbase::IComplexCollectionFilter::FieldFilter fieldFilter;
		fieldFilter.fieldId = "DocumentId";
		fieldFilter.filterValue = objectId;

		imtbase::IComplexCollectionFilter::FilterExpression groupFilter;
		groupFilter.fieldFilters << fieldFilter;

		imtbase::CComplexCollectionFilter complexFilter;
		complexFilter.SetFilterExpression(groupFilter);

		iprm::CParamsSet filterParam;
		filterParam.SetEditableParameter("ComplexFilter", &complexFilter);

		imtbase::ICollectionInfo::Ids ids = m_userConnectionCollectionCompPtr->GetElementIds(0, -1, &filterParam);
		if (ids.isEmpty()){
			QByteArray typeId = userConnectionInfoPtr->GetFactoryId();
			QByteArray result = m_userConnectionCollectionCompPtr->InsertNewObject(typeId, "", "", userConnectionInfoPtr.GetPtr(), objectId);
			if (result.isEmpty()){
				SendWarningMessage(0, errorMessage, "imtgql::CAuthorizationControllerComp");
			}
		}
		else{
			if (!m_userConnectionCollectionCompPtr->SetObjectData(objectId, *userConnectionInfoPtr.GetPtr())){
				SendWarningMessage(0, errorMessage, "imtgql::CAuthorizationControllerComp");
			}
		}
	}

	return payload;
}


sdl::V1_0::imtauth::CAuthorizationPayload CAuthorizationControllerComp::CreateAuthorizationResponseWithLifetimeCheck(
			imtauth::CUserInfo& userInfo,
			const QByteArray& systemId,
			const QByteArray& productId,
			QString& errorMessage) const
{
	int daysUntilExpiration = -1;
	imtauth::IPasswordPolicy::LifetimeStatus lifetimeStatus = imtauth::IPasswordPolicy::LS_OK;

	// Password lifetime is only evaluated for password-based accounts of the internal system;
	// externally managed users (e.g. LDAP) are governed by their own system.
	if (m_passwordPolicyCompPtr.IsValid() && systemId.isEmpty()){
		lifetimeStatus = m_passwordPolicyCompPtr->GetPasswordLifetimeStatus(userInfo, daysUntilExpiration);
	}

	if (lifetimeStatus == imtauth::IPasswordPolicy::LS_EXPIRED || lifetimeStatus == imtauth::IPasswordPolicy::LS_CHANGE_REQUIRED){
		// Reported through the payload, not through errorMessage: a non-empty
		// errorMessage makes the generated handler drop the payload, and with it the flag.
		SendErrorMessage(0,
						QStringLiteral("The password has expired and must be changed. Login: '%1'").arg(userInfo.GetId()),
						"imtgql::CAuthorizationControllerComp");

		sdl::V1_0::imtauth::CAuthorizationPayload payload;
		payload.passwordExpired = true;

		return payload;
	}

	sdl::V1_0::imtauth::CAuthorizationPayload payload = CreateAuthorizationSuccessfulResponse(userInfo, systemId, productId, errorMessage);

	if (lifetimeStatus == imtauth::IPasswordPolicy::LS_EXPIRES_SOON){
		payload.passwordExpired = false;
		payload.passwordExpiresInDays = daysUntilExpiration;
	}

	return payload;
}


// reimplemented (sdl::V1_0::imtauth::CAuthorizationGqlHandlerCompBase)

sdl::V1_0::imtauth::CAuthorizationPayload CAuthorizationControllerComp::OnAuthorization(
			const sdl::V1_0::imtauth::CAuthorizationGqlRequest& authorizationRequest,
			const imtgql::CGqlRequest& /*gqlRequest*/,
			QString& errorMessage) const
{
	if (!m_userCollectionCompPtr.IsValid()){
		Q_ASSERT_X(false, "Component 'UserCollection' was not set", "CAuthorizationControllerComp");
		return sdl::V1_0::imtauth::CAuthorizationPayload();
	}

	istd::TNullableValue<sdl::V1_0::imtauth::CAuthorizationInput> inputArgument = *authorizationRequest.GetRequestedArguments().input;
	if (!inputArgument.has_value()){
		Q_ASSERT(false);
		return sdl::V1_0::imtauth::CAuthorizationPayload();
	}

	QByteArray login;
	if (inputArgument->login){
		login = inputArgument->login->toUtf8();
	}

	QByteArray productId;
	if (inputArgument->productId){
		productId = *inputArgument->productId;
	}

	QByteArray password;
	if (inputArgument->password){
		password = inputArgument->password->toUtf8();
	}

	qint64 remainingLockoutSecs = 0;
	if (IsAccountLocked(login, &remainingLockoutSecs)){
		return CreateAccountLockedResponse(login, remainingLockoutSecs, errorMessage);
	}

	QByteArray userObjectId = GetUserObjectId(login);
	if (userObjectId.isEmpty()){
		return CreateFailedAttemptResponse(login, errorMessage);
	}

	imtauth::CUserInfo* userInfoPtr = nullptr;
	imtbase::IObjectCollection::DataPtr dataPtr;
	if (m_userCollectionCompPtr->GetObjectData(userObjectId, dataPtr)){
		userInfoPtr = dynamic_cast<imtauth::CUserInfo*>(dataPtr.GetPtr());
	}

	if (userInfoPtr == nullptr){
		return CreateFailedAttemptResponse(login, errorMessage);
	}

	QByteArray activeSystemId;
	bool ok = false;
	for (const imtauth::IUserInfo::SystemInfo& systemInfo : userInfoPtr->GetSystemInfos()){
		if (systemInfo.enabled){
			if (CheckCredential(systemInfo.systemId, login, password)){
				ok = true;
				activeSystemId = systemInfo.systemId;
				break;
			}
		}
	}

	if (!ok){
		return CreateFailedAttemptResponse(login, errorMessage);
	}

	RegisterAccessAttempt(login, true);

	// Checked after the credentials, so a wrong password cannot be used to probe account states.
	if (!userInfoPtr->IsEnabled()){
		return CreateAccountDisabledResponse(login);
	}

	return CreateAuthorizationResponseWithLifetimeCheck(*userInfoPtr, activeSystemId, productId, errorMessage);
}


sdl::V1_0::imtauth::CAuthorizationPayload CAuthorizationControllerComp::OnUserToken(
			const sdl::V1_0::imtauth::CUserTokenGqlRequest& userTokenRequest,
			const ::imtgql::CGqlRequest& /*gqlRequest*/,
			QString& errorMessage) const
{
	if (!m_userCollectionCompPtr.IsValid()){
		Q_ASSERT_X(false, "Component 'UserCollection' was not set", "CAuthorizationControllerComp");
		return sdl::V1_0::imtauth::CAuthorizationPayload();
	}

	istd::TNullableValue<sdl::V1_0::imtauth::CAuthorizationInput> inputArgument = userTokenRequest.GetRequestedArguments().input;
	if (!inputArgument.has_value()){
		Q_ASSERT(false);
		return sdl::V1_0::imtauth::CAuthorizationPayload();
	}

	QByteArray login;
	if (inputArgument->login){
		login = inputArgument->login->toUtf8();
	}
	
	QByteArray productId;
	if (inputArgument->productId){
		productId = *inputArgument->productId;
	}
	
	QByteArray password;
	if (inputArgument->password){
		password = inputArgument->password->toUtf8();
	}

	qint64 remainingLockoutSecs = 0;
	if (IsAccountLocked(login, &remainingLockoutSecs)){
		return CreateAccountLockedResponse(login, remainingLockoutSecs, errorMessage);
	}

	QByteArray userObjectId = GetUserObjectId(login);
	if (userObjectId.isEmpty()){
		return CreateFailedAttemptResponse(login, errorMessage);
	}

	imtauth::CUserInfo* userInfoPtr = nullptr;
	imtbase::IObjectCollection::DataPtr dataPtr;
	if (m_userCollectionCompPtr->GetObjectData(userObjectId, dataPtr)){
		userInfoPtr = dynamic_cast<imtauth::CUserInfo*>(dataPtr.GetPtr());
	}

	if (userInfoPtr == nullptr){
		return CreateFailedAttemptResponse(login, errorMessage);
	}

	QByteArray activeSystemId;
	bool ok = false;
	for (const imtauth::IUserInfo::SystemInfo& systemInfo : userInfoPtr->GetSystemInfos()){
		if (systemInfo.enabled){
			if (CheckCredential(systemInfo.systemId, login, password)){
				ok = true;
				activeSystemId = systemInfo.systemId;
				break;
			}
		}
	}

	if (!ok){
		return CreateFailedAttemptResponse(login, errorMessage);
	}

	RegisterAccessAttempt(login, true);

	// Checked after the credentials, so a wrong password cannot be used to probe account states.
	if (!userInfoPtr->IsEnabled()){
		return CreateAccountDisabledResponse(login);
	}

	return CreateAuthorizationResponseWithLifetimeCheck(*userInfoPtr, activeSystemId, productId, errorMessage);
}


sdl::V1_0::imtauth::CLogoutPayload CAuthorizationControllerComp::OnLogout(
			const sdl::V1_0::imtauth::CLogoutGqlRequest& logoutRequest,
			const ::imtgql::CGqlRequest& /*gqlRequest*/,
			QString& errorMessage) const
{
	sdl::V1_0::imtauth::CLogoutPayload response;
	const imtgql::IGqlContext* gqlContextPtr = logoutRequest.GetRequestContext();
	if (gqlContextPtr == nullptr){
		errorMessage = QStringLiteral("Unable to logout user with token '%1'. Error: GraphQL context is invalid");
		SendErrorMessage(0, errorMessage, "CAuthorizationControllerComp");
		return response;
	}

	QByteArray accessToken = gqlContextPtr->GetToken();
	if (m_jwtSessionControllerCompPtr.IsValid()){
		QByteArray sessionId = m_jwtSessionControllerCompPtr->GetSessionFromJwt(accessToken);
		if (!m_jwtSessionControllerCompPtr->RemoveSession(sessionId)){
			response.ok = false;
			return response;
		}
	}

	response.ok = true;

	return response;
}


sdl::V1_0::imtauth::CPermissionList CAuthorizationControllerComp::OnGetPermissions(
			const sdl::V1_0::imtauth::CGetPermissionsGqlRequest& getPermissionsRequest,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& /*errorMessage*/) const
{
	sdl::V1_0::imtauth::CPermissionList response;
	response.permissions.emplace();

	if (!m_userCollectionCompPtr.IsValid()){
		Q_ASSERT_X(false, "Component 'UserCollection' was not set", "CAuthorizationControllerComp");
		return response;
	}

	istd::TNullableValue<sdl::V1_0::imtauth::CTokenInput> arguments = getPermissionsRequest.GetRequestedArguments().input;
	if (!arguments.HasValue()){
		SendWarningMessage(0, "GetPermissions called without input arguments", "imtauthgql::CAuthorizationControllerComp");
		return response;
	}

	const imtgql::IGqlContext* gqlContextPtr = gqlRequest.GetRequestContext();

	QByteArray token;
	if (arguments->accessToken.HasValue()){
		token = *arguments->accessToken;
	}
	if (token.isEmpty() && gqlContextPtr != nullptr){
		token = gqlContextPtr->GetToken();
	}

	QByteArray userId;
	QByteArray tenantId;
	QByteArray tokenId;
	QByteArrayList tokenScopes;
	bool isPat = false;

	const QByteArray patPrefix = m_patPrefixAttrPtr.IsValid() ? *m_patPrefixAttrPtr : QByteArrayLiteral("imt_pat_");
	if (!patPrefix.isEmpty() && token.size() > patPrefix.size() && token.startsWith(patPrefix)){
		isPat = true;
		if (!m_personalAccessTokenManagerCompPtr.IsValid() ||
			!m_personalAccessTokenManagerCompPtr->ValidateToken(token, userId, tokenId, tokenScopes)){
			SendWarningMessage(0, "GetPermissions called with invalid or expired personal access token", "imtauthgql::CAuthorizationControllerComp");
			return response;
		}

		m_personalAccessTokenManagerCompPtr->UpdateLastUsedAt(tokenId);
	}
	else if (m_jwtSessionControllerCompPtr.IsValid() && m_jwtSessionControllerCompPtr->ValidateJwt(token) == imtauth::IJwtSessionController::JS_OK){
		userId = m_jwtSessionControllerCompPtr->GetUserFromJwt(token);
		tenantId = m_jwtSessionControllerCompPtr->GetTenantFromJwt(token);
	}

	if (userId.isEmpty()){
		SendWarningMessage(0, "GetPermissions called with invalid or expired token", "imtauthgql::CAuthorizationControllerComp");
		return response;
	}

	QByteArray productId;
	if (gqlContextPtr != nullptr){
		productId = gqlContextPtr->GetProductId();
	}
	if (isPat){
		imtauth::IPersonalAccessTokenSharedPtr tokenPtr = m_personalAccessTokenManagerCompPtr->GetToken(tokenId);
		if (!tokenPtr.IsValid()){
			SendWarningMessage(0, "GetPermissions cannot resolve personal access token", "imtauthgql::CAuthorizationControllerComp");
			return response;
		}

		const QByteArray tokenProductId = tokenPtr->GetProductId();
		if (!tokenProductId.isEmpty()){
			if (!productId.isEmpty() && productId != tokenProductId){
				SendWarningMessage(0, "GetPermissions called with personal access token for a different product", "imtauthgql::CAuthorizationControllerComp");
				return response;
			}
			productId = tokenProductId;
		}
	}

	const imtauth::IUserInfo* userInfoPtr = nullptr;
	imtbase::IObjectCollection::DataPtr dataPtr;
	if (m_userCollectionCompPtr->GetObjectData(userId, dataPtr)){
		userInfoPtr = dynamic_cast<const imtauth::IUserInfo*>(dataPtr.GetPtr());
	}

	if (userInfoPtr == nullptr){
		SendWarningMessage(0, "GetPermissions cannot resolve user for provided token", "imtauthgql::CAuthorizationControllerComp");
		return response;
	}

	QByteArrayList userPermissions = tenantId.isEmpty()
		? (productId.isEmpty() ? userInfoPtr->GetPermissions() : userInfoPtr->GetPermissions(productId))
		: GetEffectiveUserPermissions(
					userId,
					*userInfoPtr,
					tenantId,
					productId,
					m_bindingManagerCompPtr.IsValid() ? m_bindingManagerCompPtr.GetPtr() : nullptr,
					m_delegatedAccessCompPtr.IsValid() ? m_delegatedAccessCompPtr.GetPtr() : nullptr,
					m_tenantMembershipManagerCompPtr.IsValid() ? m_tenantMembershipManagerCompPtr.GetPtr() : nullptr,
					m_roleInfoProviderCompPtr.IsValid() ? m_roleInfoProviderCompPtr.GetPtr() : nullptr);

	// Empty tenantId means global (non-tenant) scope.
	if (tenantId.isEmpty()){
		QByteArrayList globalPermissions = CalculateGlobalPermissions(*userInfoPtr, userId, productId);
		if (isPat){
			globalPermissions.removeIf([&tokenScopes](const QByteArray& permission){
				return !imtauth::HasPermission(tokenScopes, permission, false);
			});
		}
		response.permissions.Emplace().FromList(globalPermissions);
		return response;
	}

	QByteArrayList tenantPermissions;
	if (m_tenantManagerCompPtr.IsValid()){
		tenantPermissions = m_tenantManagerCompPtr->GetTenantPermissions(tenantId);
	}

	QByteArrayList permissions;
	if (tenantPermissions.isEmpty()){
		permissions = userPermissions;
	}
	else{
		QSet<QByteArray> tenantSet(tenantPermissions.begin(), tenantPermissions.end());
		for (const QByteArray& permissionId : userPermissions){
			if (tenantSet.contains(permissionId)){
				permissions.push_back(permissionId);
			}
		}
	}

	permissions = QSet<QByteArray>(permissions.begin(), permissions.end()).values();
	std::sort(permissions.begin(), permissions.end());
	response.permissions.Emplace().FromList(permissions);

	return response;
}


sdl::V1_0::imtauth::CExternalLoginProviderList CAuthorizationControllerComp::OnGetExternalLoginProviders(
			const sdl::V1_0::imtauth::CGetExternalLoginProvidersGqlRequest& /*getExternalLoginProvidersRequest*/,
			const ::imtgql::CGqlRequest& /*gqlRequest*/,
			QString& /*errorMessage*/) const
{
	QList<sdl::V1_0::imtauth::CExternalLoginProvider> providers;

	if (m_externalLoginControllerCompPtr.IsValid()){
		const imtauth::IExternalLoginController::Providers providerInfos = m_externalLoginControllerCompPtr->GetProviders();
		for (const imtauth::IExternalLoginController::ProviderInfo& providerInfo : providerInfos){
			sdl::V1_0::imtauth::CExternalLoginProvider provider;
			provider.id = providerInfo.id;
			provider.name = providerInfo.name;
			provider.providerType = providerInfo.type;

			providers.append(provider);
		}
	}

	sdl::V1_0::imtauth::CExternalLoginProviderList response;
	response.providers.Emplace().FromList(providers);

	return response;
}


sdl::V1_0::imtauth::CExternalIdentityList CAuthorizationControllerComp::OnGetExternalIdentities(
			const sdl::V1_0::imtauth::CGetExternalIdentitiesGqlRequest& /*getExternalIdentitiesRequest*/,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	sdl::V1_0::imtauth::CExternalIdentityList response;

	const QByteArray userId = GetSessionUserId(gqlRequest);
	if (userId.isEmpty()){
		errorMessage = QStringLiteral("The linked accounts are only available for a signed-in user");

		return response;
	}

	QList<sdl::V1_0::imtauth::CExternalIdentity> identities;
	if (m_externalLoginControllerCompPtr.IsValid()){
		QMap<QByteArray, imtauth::IExternalLoginController::ProviderInfo> providerInfos;
		for (const imtauth::IExternalLoginController::ProviderInfo& providerInfo : m_externalLoginControllerCompPtr->GetProviders()){
			providerInfos.insert(providerInfo.id, providerInfo);
		}

		const imtauth::IExternalLoginController::LinkedAccounts accounts = m_externalLoginControllerCompPtr->GetLinkedAccounts(userId);
		for (const imtauth::IExternalLoginController::LinkedAccount& account : accounts){
			const imtauth::IExternalLoginController::ProviderInfo providerInfo = providerInfos.value(account.providerId);

			sdl::V1_0::imtauth::CExternalIdentity identity;
			identity.providerId = account.providerId;
			identity.providerName = providerInfo.name.isEmpty() ? QString::fromUtf8(account.providerId) : providerInfo.name;
			identity.providerType = providerInfo.type;
			identity.email = account.email;
			identity.linkedAt = account.linkedAt.toString(Qt::ISODate);
			identity.lastSignInAt = account.lastSignInAt.toString(Qt::ISODate);

			identities.append(identity);
		}
	}

	response.identities.Emplace().FromList(identities);

	imtbase::IObjectCollection::DataPtr dataPtr;
	if (m_userCollectionCompPtr.IsValid() && m_userCollectionCompPtr->GetObjectData(userId, dataPtr)){
		const imtauth::IUserInfo* userInfoPtr = dynamic_cast<const imtauth::IUserInfo*>(dataPtr.GetPtr());
		response.hasPassword = (userInfoPtr != nullptr) && !userInfoPtr->GetSystemInfos().isEmpty();
	}

	return response;
}


sdl::V1_0::imtauth::CBeginExternalLoginPayload CAuthorizationControllerComp::OnBeginExternalLogin(
			const sdl::V1_0::imtauth::CBeginExternalLoginGqlRequest& beginExternalLoginRequest,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& /*errorMessage*/) const
{
	sdl::V1_0::imtauth::CBeginExternalLoginPayload response;

	if (!m_externalLoginControllerCompPtr.IsValid()){
		response.message = QStringLiteral("Sign-in with external accounts is not configured");

		return response;
	}

	const istd::TNullableValue<sdl::V1_0::imtauth::CBeginExternalLoginInput> input = beginExternalLoginRequest.GetRequestedArguments().input;
	if (!input.HasValue() || !input->providerId.HasValue()){
		response.message = QStringLiteral("No sign-in provider is given");

		return response;
	}

	const QByteArray productId = input->productId.HasValue() ? *input->productId : QByteArray();

	QByteArray linkUserId;
	if (input->link.HasValue() && *input->link){
		linkUserId = GetSessionUserId(gqlRequest);
		if (linkUserId.isEmpty()){
			response.message = QStringLiteral("Sign in before linking an external account");

			return response;
		}
	}

	QByteArray loginId;
	QString authorizationUrl;
	QString beginErrorMessage;
	if (!m_externalLoginControllerCompPtr->BeginLogin(*input->providerId, productId, linkUserId, loginId, authorizationUrl, beginErrorMessage)){
		response.message = beginErrorMessage;

		return response;
	}

	response.loginId = loginId;
	response.authorizationUrl = authorizationUrl;

	return response;
}


sdl::V1_0::imtauth::CExternalLoginPayload CAuthorizationControllerComp::OnCompleteExternalLogin(
			const sdl::V1_0::imtauth::CCompleteExternalLoginGqlRequest& completeExternalLoginRequest,
			const ::imtgql::CGqlRequest& /*gqlRequest*/,
			QString& /*errorMessage*/) const
{
	sdl::V1_0::imtauth::CExternalLoginPayload response;

	const istd::TNullableValue<sdl::V1_0::imtauth::CExternalLoginInput> input = completeExternalLoginRequest.GetRequestedArguments().input;
	if (!m_externalLoginControllerCompPtr.IsValid() || !input.HasValue() || !input->loginId.HasValue()){
		response.status = QByteArrayLiteral("Failed");
		response.message = QStringLiteral("Sign-in with external accounts is not configured");

		return response;
	}

	const imtauth::IExternalLoginController::LoginResult result = m_externalLoginControllerCompPtr->TakeLoginResult(*input->loginId);
	switch (result.status){
	case imtauth::IExternalLoginController::LS_PENDING:
		response.status = QByteArrayLiteral("Pending");
		return response;

	case imtauth::IExternalLoginController::LS_LINKED:
		response.status = QByteArrayLiteral("Linked");
		return response;

	case imtauth::IExternalLoginController::LS_FAILED:
		response.status = QByteArrayLiteral("Failed");
		response.message = result.message;
		return response;

	case imtauth::IExternalLoginController::LS_SIGNED_IN:
		break;

	default:
		response.status = QByteArrayLiteral("Failed");
		response.message = QStringLiteral("The sign-in request has expired. Please start the sign-in again.");
		return response;
	}

	imtauth::CUserInfo* userInfoPtr = nullptr;
	imtbase::IObjectCollection::DataPtr dataPtr;
	if (m_userCollectionCompPtr.IsValid() && m_userCollectionCompPtr->GetObjectData(result.userId, dataPtr)){
		userInfoPtr = dynamic_cast<imtauth::CUserInfo*>(dataPtr.GetPtr());
	}

	response.status = QByteArrayLiteral("Failed");

	if (userInfoPtr == nullptr){
		response.message = QStringLiteral("The user does not exist.");

		return response;
	}

	if (!userInfoPtr->IsEnabled()){
		response.message = QStringLiteral("This account has been deactivated. Please contact your administrator.");

		return response;
	}

	QString sessionErrorMessage;
	sdl::V1_0::imtauth::CAuthorizationPayload authorization = CreateAuthorizationSuccessfulResponse(*userInfoPtr, QByteArray(), result.productId, sessionErrorMessage);
	if (!sessionErrorMessage.isEmpty()){
		response.message = sessionErrorMessage;

		return response;
	}

	response.status = QByteArrayLiteral("SignedIn");
	response.authorization = authorization;

	return response;
}


sdl::V1_0::imtauth::CExternalLoginCallbackPayload CAuthorizationControllerComp::OnHandleExternalLoginCallback(
			const sdl::V1_0::imtauth::CHandleExternalLoginCallbackGqlRequest& handleExternalLoginCallbackRequest,
			const ::imtgql::CGqlRequest& /*gqlRequest*/,
			QString& /*errorMessage*/) const
{
	sdl::V1_0::imtauth::CExternalLoginCallbackPayload response;
	response.ok = false;

	const istd::TNullableValue<sdl::V1_0::imtauth::CExternalLoginCallbackInput> input = handleExternalLoginCallbackRequest.GetRequestedArguments().input;
	if (!m_externalLoginControllerCompPtr.IsValid() || !input.HasValue() || !input->state.HasValue()){
		response.message = QStringLiteral("Sign-in with external accounts is not configured");

		return response;
	}

	const QByteArray code = input->code.HasValue() ? input->code->toUtf8() : QByteArray();
	const QString providerError = input->error.HasValue() ? *input->error : QString();

	QString message;
	response.ok = m_externalLoginControllerCompPtr->HandleCallback(*input->state, code, providerError, message);
	response.message = message;

	return response;
}


sdl::V1_0::imtauth::CUnlinkExternalIdentityPayload CAuthorizationControllerComp::OnUnlinkExternalIdentity(
			const sdl::V1_0::imtauth::CUnlinkExternalIdentityGqlRequest& unlinkExternalIdentityRequest,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	sdl::V1_0::imtauth::CUnlinkExternalIdentityPayload response;
	response.ok = false;

	const QByteArray userId = GetSessionUserId(gqlRequest);
	if (userId.isEmpty()){
		errorMessage = QStringLiteral("The linked accounts are only available for a signed-in user");

		return response;
	}

	const istd::TNullableValue<sdl::V1_0::imtauth::CUnlinkExternalIdentityInput> input = unlinkExternalIdentityRequest.GetRequestedArguments().input;
	if (!m_externalLoginControllerCompPtr.IsValid() || !input.HasValue() || !input->providerId.HasValue()){
		response.message = QStringLiteral("No linked account is given");

		return response;
	}

	QString unlinkErrorMessage;
	response.ok = m_externalLoginControllerCompPtr->UnlinkAccount(userId, *input->providerId, unlinkErrorMessage);
	response.message = unlinkErrorMessage;

	return response;
}


QByteArray CAuthorizationControllerComp::GetSessionUserId(const imtgql::CGqlRequest& gqlRequest) const
{
	const imtgql::IGqlContext* gqlContextPtr = gqlRequest.GetRequestContext();
	if (gqlContextPtr == nullptr){
		return QByteArray();
	}

	const QByteArray token = gqlContextPtr->GetToken();

	// A personal access token must not manage the sign-in methods of its owner.
	const QByteArray patPrefix = m_patPrefixAttrPtr.IsValid() ? *m_patPrefixAttrPtr : QByteArrayLiteral("imt_pat_");
	if (token.isEmpty() || (!patPrefix.isEmpty() && token.startsWith(patPrefix))){
		return QByteArray();
	}

	QByteArray userId = gqlContextPtr->GetUserId();
	if (userId.isEmpty() && m_jwtSessionControllerCompPtr.IsValid() && m_jwtSessionControllerCompPtr->ValidateJwt(token) == imtauth::IJwtSessionController::JS_OK){
		userId = m_jwtSessionControllerCompPtr->GetUserFromJwt(token);
	}

	return userId;
}


// reimplemented (imtservergql::CPermissibleGqlRequestHandlerComp)

bool CAuthorizationControllerComp::CheckPermissions(const imtgql::CGqlRequest& /*gqlRequest*/, QString& /*errorMessage*/) const
{
	return true;
}


} // namespace imtauthgql
