// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauthgql/CAuthorizationControllerComp.h>
#include <GeneratedFiles/imtauthsdl/SDL/1.0/CPP/Authorization.h>


// Qt includes
#include <QtCore/QMutableListIterator>

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
	const QByteArray errorMessage = QString("System-ID '%1' cannot found").arg(systemId).toUtf8();
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
	errorMessage = QT_TR_NOOP(QString("Invalid login or password. Login: '%1'").arg(login));
	SendErrorMessage(0, errorMessage, "imtgql::CAuthorizationControllerComp");

	return sdl::V1_0::imtauth::CAuthorizationPayload();
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
		errorMessage = QString("Unable to create authorization request. Error: User with login '%1' does not exists").arg(login);
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

	QByteArray userObjectId = GetUserObjectId(login);
	if (userObjectId.isEmpty()){
		return CreateInvalidLoginOrPasswordResponse(login, errorMessage);
	}

	imtauth::CUserInfo* userInfoPtr = nullptr;
	imtbase::IObjectCollection::DataPtr dataPtr;
	if (m_userCollectionCompPtr->GetObjectData(userObjectId, dataPtr)){
		userInfoPtr = dynamic_cast<imtauth::CUserInfo*>(dataPtr.GetPtr());
	}

	if (userInfoPtr == nullptr){
		return CreateInvalidLoginOrPasswordResponse(login, errorMessage);
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
		return CreateInvalidLoginOrPasswordResponse(login, errorMessage);
	}

	return CreateAuthorizationSuccessfulResponse(*userInfoPtr, activeSystemId, productId, errorMessage);
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

	QByteArray userObjectId = GetUserObjectId(login);
	if (userObjectId.isEmpty()){
		return CreateInvalidLoginOrPasswordResponse(login, errorMessage);
	}

	imtauth::CUserInfo* userInfoPtr = nullptr;
	imtbase::IObjectCollection::DataPtr dataPtr;
	if (m_userCollectionCompPtr->GetObjectData(userObjectId, dataPtr)){
		userInfoPtr = dynamic_cast<imtauth::CUserInfo*>(dataPtr.GetPtr());
	}

	if (userInfoPtr == nullptr){
		return CreateInvalidLoginOrPasswordResponse(login, errorMessage);
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
		return CreateInvalidLoginOrPasswordResponse(login, errorMessage);
	}

	return CreateAuthorizationSuccessfulResponse(*userInfoPtr, activeSystemId, productId, errorMessage);
}


sdl::V1_0::imtauth::CLogoutPayload CAuthorizationControllerComp::OnLogout(
			const sdl::V1_0::imtauth::CLogoutGqlRequest& logoutRequest,
			const ::imtgql::CGqlRequest& /*gqlRequest*/,
			QString& errorMessage) const
{
	sdl::V1_0::imtauth::CLogoutPayload response;
	const imtgql::IGqlContext* gqlContextPtr = logoutRequest.GetRequestContext();
	if (gqlContextPtr == nullptr){
		errorMessage = QString("Unable to logout user with token '%1'. Error: GraphQL context is invalid");
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
	else if (m_jwtSessionControllerCompPtr.IsValid()){
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
			const QSet<QByteArray> scopeSet(tokenScopes.begin(), tokenScopes.end());
			QMutableListIterator permissionsIter(globalPermissions);
			while (permissionsIter.hasNext()){
				if (!scopeSet.contains(permissionsIter.next())){
					permissionsIter.remove();
				}
			}
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


// reimplemented (imtservergql::CPermissibleGqlRequestHandlerComp)

bool CAuthorizationControllerComp::CheckPermissions(const imtgql::CGqlRequest& /*gqlRequest*/, QString& /*errorMessage*/) const
{
	return true;
}


} // namespace imtauthgql
