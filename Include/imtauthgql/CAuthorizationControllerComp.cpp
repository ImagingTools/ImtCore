#include <imtauthgql/CAuthorizationControllerComp.h>


// ACF includes
#include <iprm/CParamsSet.h>

// ImtCore includes
#include <imtbase/CComplexCollectionFilter.h>
#include <imtauth/CUserInfo.h>


namespace imtauthgql
{


// protected methods

bool CAuthorizationControllerComp::ParseDataFromGqlRequest(
	const imtgql::CGqlRequest& gqlRequest,
	QByteArray& login,
	QByteArray& password,
	QByteArray& productId) const
{
	const imtgql::CGqlObject* gqlInputParamPtr = gqlRequest.GetParamObject("input");
	if (gqlInputParamPtr == nullptr){
		return false;
	}

	login = gqlInputParamPtr->GetFieldArgumentValue("Login").toByteArray();
	productId = gqlInputParamPtr->GetFieldArgumentValue("ProductId").toByteArray();
	password = gqlInputParamPtr->GetFieldArgumentValue("Password").toByteArray();

	return true;
}


QByteArray CAuthorizationControllerComp::GetUserObjectId(const QByteArray& login) const
{
	imtbase::IComplexCollectionFilter::FieldFilter fieldFilter;
	fieldFilter.fieldId = "UserId";
	fieldFilter.filterValue = login;
	
	imtbase::IComplexCollectionFilter::GroupFilter groupFilter;
	groupFilter.fieldFilters << fieldFilter;
	
	imtbase::CComplexCollectionFilter complexFilter;
	complexFilter.SetFieldsFilter(groupFilter);
	
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
	QByteArray tokenValue = QUuid::createUuid().toByteArray();

	payload.Version_1_0.emplace();

	payload.Version_1_0->Token = tokenValue;
	payload.Version_1_0->Username = login;
	payload.Version_1_0->UserId = objectId;
	payload.Version_1_0->SystemId = systemId;

	if (!productId.isEmpty()){
		imtauth::IUserInfo::FeatureIds permissionIds = userInfo.GetPermissions(productId);
		QByteArrayList uniqueList = QSet<QByteArray>(permissionIds.begin(), permissionIds.end()).values();
		std::sort(uniqueList.begin(), uniqueList.end());
		QByteArray permissions = uniqueList.join(';');
		(*payload.Version_1_0).Permissions = permissions;
	}

	if (m_jwtSessionControllerCompPtr.IsValid()){
		imtauth::IJwtSessionController::UserSession userSession;
		if (m_jwtSessionControllerCompPtr->CreateNewSession(objectId, userSession)){
			payload.Version_1_0->RefreshToken = userSession.refreshToken;
			payload.Version_1_0->Token = userSession.accessToken;
			payload.Version_1_0->UserId = userSession.userId;
		}
	}

	userInfo.SetLastConnection(QDateTime::currentDateTimeUtc());

	if (!m_userCollectionCompPtr->SetObjectData(objectId, userInfo)){
		errorMessage = QString("Unable to set last connection info for user with login: '%1'").arg(qPrintable(login));
		SendWarningMessage(0, errorMessage, "imtgql::CAuthorizationControllerComp");
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
	if (inputArgument.Version_1_0->Login){
		login = inputArgument.Version_1_0->Login->toUtf8();
	}
	
	QByteArray productId;
	if (inputArgument.Version_1_0->ProductId){
		productId = *inputArgument.Version_1_0->ProductId;
	}
	
	QByteArray password;
	if (inputArgument.Version_1_0->Password){
		password = inputArgument.Version_1_0->Password->toUtf8();
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
	if (inputArgument.Version_1_0->Login) {
		login = inputArgument.Version_1_0->Login->toUtf8();
	}
	
	QByteArray productId;
	if (inputArgument.Version_1_0->ProductId) {
		productId = *inputArgument.Version_1_0->ProductId;
	}
	
	QByteArray password;
	if (inputArgument.Version_1_0->Password) {
		password = inputArgument.Version_1_0->Password->toUtf8();
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

	sdl::imtauth::Authorization::CLogoutInput arguments = logoutRequest.GetRequestedArguments().input;

	QByteArray accessToken = gqlContextPtr->GetToken();
	if (m_jwtSessionControllerCompPtr.IsValid()){
		QByteArray sessionId = m_jwtSessionControllerCompPtr->GetSessionFromJwt(accessToken);
		if (!m_jwtSessionControllerCompPtr->RemoveSession(sessionId)){
			response.Version_1_0->Ok = false;
			return response;
		}
	}

	response.Version_1_0->Ok = true;

	return response;
}


} // namespace imtauthgql


