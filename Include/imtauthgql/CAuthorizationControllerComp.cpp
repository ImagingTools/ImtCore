// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauthgql/CAuthorizationControllerComp.h>
#include <GeneratedFiles/imtauthsdl/SDL/1.0/CPP/Authorization.h>


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


sdl::V1_0::imtauth::CAuthorizationPayload CAuthorizationControllerComp::CreateInvalidLoginOrPasswordResponse(
			const QByteArray& login,
			QString& errorMessage) const
{
	errorMessage = QT_TR_NOOP(QString("Invalid login or password. Login: '%1'").arg(qPrintable(login)));
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
		errorMessage = QString("Unable to create authorization request. Error: User with login '%1' does not exists").arg(qPrintable(login));
		return payload;
	}

	QByteArray tokenValue = QUuid::createUuid().toByteArray(QUuid::WithoutBraces);

	payload.token = tokenValue;
	payload.username = login;
	payload.userId = objectId;
	payload.systemId = systemId;

	if (!productId.isEmpty()){
		imtauth::IUserInfo::FeatureIds permissionIds = userInfo.GetPermissions(productId);
		QByteArrayList uniqueList = QSet<QByteArray>(permissionIds.begin(), permissionIds.end()).values();
		std::sort(uniqueList.begin(), uniqueList.end());
		QByteArray permissions = uniqueList.join(';');
		(payload).permissions = permissions;
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

	sdl::V1_0::imtauth::CAuthorizationInput inputArgument = authorizationRequest.GetRequestedArguments().input;
	QByteArray login;
	if (inputArgument.login){
		login = inputArgument.login->toUtf8();
	}

	QByteArray productId;
	if (inputArgument.productId){
		productId = *inputArgument.productId;
	}

	QByteArray password;
	if (inputArgument.password){
		password = inputArgument.password->toUtf8();
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

	sdl::V1_0::imtauth::CAuthorizationInput inputArgument = userTokenRequest.GetRequestedArguments().input;
	QByteArray login;
	if (inputArgument.login){
		login = inputArgument.login->toUtf8();
	}
	
	QByteArray productId;
	if (inputArgument.productId){
		productId = *inputArgument.productId;
	}
	
	QByteArray password;
	if (inputArgument.password){
		password = inputArgument.password->toUtf8();
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

	sdl::V1_0::imtauth::CTokenInput arguments = logoutRequest.GetRequestedArguments().input;

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
			const ::imtgql::CGqlRequest& /*gqlRequest*/,
			QString& /*errorMessage*/) const
{
	sdl::V1_0::imtauth::CPermissionList response;

	if (!m_userCollectionCompPtr.IsValid()){
		Q_ASSERT_X(false, "Component 'UserCollection' was not set", "CAuthorizationControllerComp");
		return response;
	}

	if (!m_jwtSessionControllerCompPtr.IsValid()){
		Q_ASSERT_X(false, "Component 'JwtSessionControlle' was not set", "CAuthorizationControllerComp");
		return response;
	}

	sdl::V1_0::imtauth::CTokenInput arguments = getPermissionsRequest.GetRequestedArguments().input;

	QByteArray token;
	if (arguments.accessToken.HasValue()){
		token = *arguments.accessToken;
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

	QByteArrayList permissions = userInfoPtr->GetPermissions();
	response.permissions.Emplace().FromList(permissions);

	return response;
}


// reimplemented (imtservergql::CPermissibleGqlRequestHandlerComp)

bool CAuthorizationControllerComp::CheckPermissions(const imtgql::CGqlRequest& /*gqlRequest*/, QString& /*errorMessage*/) const
{
	return true;
}


} // namespace imtauthgql


