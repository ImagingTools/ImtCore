// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauthgql/CAuthorizationControllerComp.h>


// ACF includes
#include <iprm/CParamsSet.h>

// ImtCore includes
#include <imtbase/CComplexCollectionFilter.h>
#include <imtauth/CUserInfo.h>
#include <imtauth/CUserConnectionInfo.h>


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
	Q_ASSERT_X(index >= 0, "CAuthorizationControllerComp::CreateInternalResponse", QString("System-ID '%1' cannot found").arg(qPrintable(systemId)).toUtf8());

	const imtauth::ICredentialController* credentialControllerPtr = m_credentialControllersCompPtr[index];
	Q_ASSERT_X(credentialControllerPtr != nullptr, "CAuthorizationControllerComp::CreateInternalResponse", "Invalid credential controller");

	return credentialControllerPtr->CheckCredential(login, password);
}


sdl::imtauth::Authorization::CAuthorizationPayload CAuthorizationControllerComp::CreateInvalidLoginOrPasswordResponse(
			const QByteArray& login,
			QString& errorMessage) const
{
	errorMessage = QT_TR_NOOP(QString("Invalid login or password. Login: '%1'").arg(qPrintable(login)));
	SendErrorMessage(0, errorMessage, "imtgql::CAuthorizationControllerComp");

	return sdl::imtauth::Authorization::CAuthorizationPayload();
}


sdl::imtauth::Authorization::CAuthorizationPayload CAuthorizationControllerComp::CreateAuthorizationSuccessfulResponse(
			imtauth::CUserInfo& userInfo,
			const QByteArray& systemId,
			const QByteArray& productId,
			QString& errorMessage) const
{
	sdl::imtauth::Authorization::CAuthorizationPayload payload;

	QByteArray login = userInfo.GetId();
	QByteArray objectId = GetUserObjectId(login);
	Q_ASSERT(!objectId.isEmpty());
	if (objectId.isEmpty()){
		errorMessage = QString("Unable to create authorization request. Error: User with login '%1' does not exists").arg(qPrintable(login));
		return payload;
	}

	QByteArray tokenValue = QUuid::createUuid().toByteArray();

	payload.Version_1_0.emplace();

	payload.Version_1_0->token = tokenValue;
	payload.Version_1_0->username = login;
	payload.Version_1_0->userId = objectId;
	payload.Version_1_0->systemId = systemId;

	if (!productId.isEmpty()){
		imtauth::IUserInfo::FeatureIds permissionIds = userInfo.GetPermissions(productId);
		QByteArrayList uniqueList = QSet<QByteArray>(permissionIds.begin(), permissionIds.end()).values();
		std::sort(uniqueList.begin(), uniqueList.end());
		QByteArray permissions = uniqueList.join(';');
		(*payload.Version_1_0).permissions = permissions;
	}

	if (m_jwtSessionControllerCompPtr.IsValid()){
		imtauth::IJwtSessionController::UserSession userSession;
		if (m_jwtSessionControllerCompPtr->CreateNewSession(objectId, userSession)){
			payload.Version_1_0->refreshToken = userSession.refreshToken;
			payload.Version_1_0->token = userSession.accessToken;
			payload.Version_1_0->userId = userSession.userId;
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
				errorMessage = QString("Unable to insert last connection info for user with login: '%1'").arg(qPrintable(login));
				SendWarningMessage(0, errorMessage, "imtgql::CAuthorizationControllerComp");
			}
		}
		else{
			if (!m_userConnectionCollectionCompPtr->SetObjectData(objectId, *userConnectionInfoPtr.GetPtr())){
				errorMessage = QString("Unable to set last connection info for user with login: '%1'").arg(qPrintable(login));
				SendWarningMessage(0, errorMessage, "imtgql::CAuthorizationControllerComp");
			}
		}
	}

	return payload;
}


// reimplemented (sdl::imtauth::Authorization::V1_0::CGraphQlHandlerCompBase)

sdl::imtauth::Authorization::CAuthorizationPayload CAuthorizationControllerComp::OnAuthorization(
			const sdl::imtauth::Authorization::CAuthorizationGqlRequest& authorizationRequest,
			const imtgql::CGqlRequest& /*gqlRequest*/,
			QString& errorMessage) const
{
	if (!m_userCollectionCompPtr.IsValid()){
		Q_ASSERT_X(false, "Component 'UserCollection' was not set", "CAuthorizationControllerComp");
		return sdl::imtauth::Authorization::CAuthorizationPayload();
	}

	sdl::imtauth::Authorization::CAuthorizationInput inputArgument = authorizationRequest.GetRequestedArguments().input;
	if (!inputArgument.Version_1_0.has_value()){
		Q_ASSERT(false);
		return sdl::imtauth::Authorization::CAuthorizationPayload();
	}

	QByteArray login;
	if (inputArgument.Version_1_0->login){
		login = inputArgument.Version_1_0->login->toUtf8();
	}

	QByteArray productId;
	if (inputArgument.Version_1_0->productId){
		productId = *inputArgument.Version_1_0->productId;
	}

	QByteArray password;
	if (inputArgument.Version_1_0->password){
		password = inputArgument.Version_1_0->password->toUtf8();
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


sdl::imtauth::Authorization::CAuthorizationPayload CAuthorizationControllerComp::OnUserToken(
			const sdl::imtauth::Authorization::CUserTokenGqlRequest& userTokenRequest,
			const ::imtgql::CGqlRequest& /*gqlRequest*/,
			QString& errorMessage) const
{
	if (!m_userCollectionCompPtr.IsValid()){
		Q_ASSERT_X(false, "Component 'UserCollection' was not set", "CAuthorizationControllerComp");
		return sdl::imtauth::Authorization::CAuthorizationPayload();
	}

	sdl::imtauth::Authorization::CAuthorizationInput inputArgument = userTokenRequest.GetRequestedArguments().input;
	if (!inputArgument.Version_1_0.has_value()){
		Q_ASSERT(false);
		return sdl::imtauth::Authorization::CAuthorizationPayload();
	}
	
	QByteArray login;
	if (inputArgument.Version_1_0->login){
		login = inputArgument.Version_1_0->login->toUtf8();
	}
	
	QByteArray productId;
	if (inputArgument.Version_1_0->productId){
		productId = *inputArgument.Version_1_0->productId;
	}
	
	QByteArray password;
	if (inputArgument.Version_1_0->password){
		password = inputArgument.Version_1_0->password->toUtf8();
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


sdl::imtauth::Authorization::CLogoutPayload CAuthorizationControllerComp::OnLogout(
			const sdl::imtauth::Authorization::CLogoutGqlRequest& logoutRequest,
			const ::imtgql::CGqlRequest& /*gqlRequest*/,
			QString& errorMessage) const
{
	sdl::imtauth::Authorization::CLogoutPayload response;
	const imtgql::IGqlContext* gqlContextPtr = logoutRequest.GetRequestContext();
	if (gqlContextPtr == nullptr){
		errorMessage = QString("Unable to logout user with token '%1'. Error: GraphQL context is invalid");
		SendErrorMessage(0, errorMessage, "CAuthorizationControllerComp");
		return response;
	}

	response.Version_1_0.emplace();

	sdl::imtauth::Authorization::CTokenInput arguments = logoutRequest.GetRequestedArguments().input;

	QByteArray accessToken = gqlContextPtr->GetToken();
	if (m_jwtSessionControllerCompPtr.IsValid()){
		QByteArray sessionId = m_jwtSessionControllerCompPtr->GetSessionFromJwt(accessToken);
		if (!m_jwtSessionControllerCompPtr->RemoveSession(sessionId)){
			response.Version_1_0->ok = false;
			return response;
		}
	}

	response.Version_1_0->ok = true;

	return response;
}


sdl::imtauth::Authorization::CPermissionList CAuthorizationControllerComp::OnGetPermissions(
			const sdl::imtauth::Authorization::CGetPermissionsGqlRequest& getPermissionsRequest,
			const ::imtgql::CGqlRequest& /*gqlRequest*/,
			QString& /*errorMessage*/) const
{
	sdl::imtauth::Authorization::CPermissionList response;

	if (!m_userCollectionCompPtr.IsValid()){
		Q_ASSERT_X(false, "Component 'UserCollection' was not set", "CAuthorizationControllerComp");
		return response;
	}

	if (!m_jwtSessionControllerCompPtr.IsValid()){
		Q_ASSERT_X(false, "Component 'JwtSessionControlle' was not set", "CAuthorizationControllerComp");
		return response;
	}

	sdl::imtauth::Authorization::CTokenInput arguments = getPermissionsRequest.GetRequestedArguments().input;
	if (!arguments.Version_1_0.HasValue()){
		Q_ASSERT(false);
		return response;
	}

	QByteArray token;
	if (arguments.Version_1_0->accessToken.HasValue()){
		token = *arguments.Version_1_0->accessToken;
	}

	QByteArray userId = m_jwtSessionControllerCompPtr->GetUserFromJwt(token);

	const imtauth::IUserInfo* userInfoPtr = nullptr;
	imtbase::IObjectCollection::DataPtr dataPtr;
	if (m_userCollectionCompPtr->GetObjectData(userId, dataPtr)){
		userInfoPtr = dynamic_cast<const imtauth::IUserInfo*>(dataPtr.GetPtr());
	}

	if (userInfoPtr == nullptr){
		Q_ASSERT(false);
		return response;
	}

	response.Version_1_0.Emplace();
	QByteArrayList permissions = userInfoPtr->GetPermissions();
	response.Version_1_0->permissions.Emplace().FromList(permissions);

	return response;
}


// reimplemented (imtservergql::CPermissibleGqlRequestHandlerComp)

bool CAuthorizationControllerComp::CheckPermissions(const imtgql::CGqlRequest& /*gqlRequest*/, QString& /*errorMessage*/) const
{
	return true;
}


} // namespace imtauthgql


