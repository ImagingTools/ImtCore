#include <imtauthgql/CAuthorizationControllerComp.h>


// ACF includes
#include <iprm/CParamsSet.h>
#include <iprm/CTextParam.h>

// ImtCore includes
#include <imtauth/CUserInfo.h>
#include <imtauth/CSessionInfo.h>


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
	iprm::CParamsSet filterParam;
	iprm::CParamsSet paramsSet;

	iprm::CTextParam userIdParam;
	userIdParam.SetText(login);

	paramsSet.SetEditableParameter("Id", &userIdParam);
	filterParam.SetEditableParameter("ObjectFilter", &paramsSet);

	imtbase::IObjectCollection::Ids userIds = m_userCollectionCompPtr->GetElementIds(0, -1, &filterParam);
	if (userIds.isEmpty()){
		return QByteArray();
	}

	return userIds[0];
}


bool CAuthorizationControllerComp::CheckCredential(const QByteArray& systemId, const QByteArray& login, const QByteArray& password) const
{
	int index = m_systemIdsAttrPtr.FindValue(systemId);
	Q_ASSERT_X(index >= 0, "CAuthorizationControllerComp::CreateInternalResponse", QString("System-ID '%1' cannot found").arg(qPrintable(systemId)).toUtf8());

	const imtauth::ICredentialController* credentialControllerPtr = m_credentialControllersCompPtr[index];
	Q_ASSERT_X(credentialControllerPtr != nullptr, "CAuthorizationControllerComp::CreateInternalResponse", "Invalid credential controller");

	return credentialControllerPtr->CheckCredential(login, password);
}


sdl::imtauth::Authorization::CAuthorizationPayload CAuthorizationControllerComp::CreateInvalidLoginOrPasswordResponse(const QByteArray& login, QString& errorMessage) const
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

	istd::TDelPtr<imtauth::CSessionInfo> sessionInfoPtr;
	sessionInfoPtr.SetPtr(new imtauth::CSessionInfo());

	sessionInfoPtr->SetUserId(objectId);
	sessionInfoPtr->SetToken(tokenValue);

	if (m_sessionCollectionCompPtr.IsValid()){
		m_sessionCollectionCompPtr->InsertNewObject("", "", "", sessionInfoPtr.GetPtr(), tokenValue);
	}

	userInfo.SetLastConnection(QDateTime::currentDateTimeUtc());

	if (!m_userCollectionCompPtr->SetObjectData(objectId, userInfo)){
		errorMessage = QString("Unable to set last connection info for user with login: '%1'").arg(qPrintable(login));
		SendWarningMessage(0, errorMessage, "imtgql::CAuthorizationControllerComp");
	}

	return payload;
}


// reimplemented (sdl::imtauth::Authorization::CGraphQlHandlerCompBase)

sdl::imtauth::Authorization::CAuthorizationPayload CAuthorizationControllerComp::OnAuthorization(
			const sdl::imtauth::Authorization::CAuthorizationGqlRequest& authorizationRequest,
			const imtgql::CGqlRequest& /*gqlRequest*/,
			QString& errorMessage) const
{
	if (!m_userCollectionCompPtr.IsValid()){
		Q_ASSERT_X(false, "Component 'UserCollection' was not set", "CAuthorizationControllerComp");
		return sdl::imtauth::Authorization::CAuthorizationPayload();
	}

	if (!m_sessionCollectionCompPtr.IsValid()){
		Q_ASSERT_X(false, "Component 'SessionCollection' was not set", "CAuthorizationControllerComp");
		return sdl::imtauth::Authorization::CAuthorizationPayload();
	}

	sdl::imtauth::Authorization::CAuthorizationPayload retVal;

	if (authorizationRequest.GetRequestedArguments().input.Version_1_0){
		sdl::imtauth::Authorization::CAuthorizationInput::V1_0 inputArgument = *authorizationRequest.GetRequestedArguments().input.Version_1_0;
		QByteArray login;
		if (inputArgument.Login){
			login = inputArgument.Login->toUtf8();
		}
		QByteArray productId;
		if (inputArgument.ProductId){
			productId = *inputArgument.ProductId;
		}
		QByteArray password;
		if (inputArgument.Password){
			password = inputArgument.Password->toUtf8();
		}

		QByteArray userObjectId = GetUserObjectId(login);
		if (userObjectId.isEmpty()){
			retVal = CreateInvalidLoginOrPasswordResponse(login, errorMessage);

			return retVal;
		}

		imtauth::CUserInfo* userInfoPtr = nullptr;
		imtbase::IObjectCollection::DataPtr dataPtr;
		if (m_userCollectionCompPtr->GetObjectData(userObjectId, dataPtr)){
			userInfoPtr = dynamic_cast<imtauth::CUserInfo*>(dataPtr.GetPtr());
		}

		if (userInfoPtr == nullptr){
			retVal = CreateInvalidLoginOrPasswordResponse(login, errorMessage);

			return retVal;
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
			retVal = CreateInvalidLoginOrPasswordResponse(login, errorMessage);

			return retVal;
		}

		retVal = CreateAuthorizationSuccessfulResponse(*userInfoPtr, activeSystemId, productId, errorMessage);
	}

	return retVal;
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

	if (!m_sessionCollectionCompPtr.IsValid()){
		Q_ASSERT_X(false, "Component 'SessionCollection' was not set", "CAuthorizationControllerComp");
		return sdl::imtauth::Authorization::CAuthorizationPayload();
	}

	sdl::imtauth::Authorization::CAuthorizationPayload retVal;

	if (userTokenRequest.GetRequestedArguments().input.Version_1_0) {
		sdl::imtauth::Authorization::CAuthorizationInput::V1_0 inputArgument = *userTokenRequest.GetRequestedArguments().input.Version_1_0;
		QByteArray login;
		if (inputArgument.Login){
			login = inputArgument.Login->toUtf8();
		}
		QByteArray productId;
		if (inputArgument.ProductId){
			productId = *inputArgument.ProductId;
		}
		QByteArray password;
		if (inputArgument.Password){
			password = inputArgument.Password->toUtf8();
		}

		QByteArray userObjectId = GetUserObjectId(login);
		if (userObjectId.isEmpty()){
			retVal = CreateInvalidLoginOrPasswordResponse(login, errorMessage);

			return retVal;
		}

		imtauth::CUserInfo* userInfoPtr = nullptr;
		imtbase::IObjectCollection::DataPtr dataPtr;
		if (m_userCollectionCompPtr->GetObjectData(userObjectId, dataPtr)){
			userInfoPtr = dynamic_cast<imtauth::CUserInfo*>(dataPtr.GetPtr());
		}

		if (userInfoPtr == nullptr){
			retVal = CreateInvalidLoginOrPasswordResponse(login, errorMessage);

			return retVal;
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
			retVal = CreateInvalidLoginOrPasswordResponse(login, errorMessage);

			return retVal;
		}

		retVal = CreateAuthorizationSuccessfulResponse(*userInfoPtr, activeSystemId, productId, errorMessage);
	}

	return retVal;
}


sdl::imtauth::Authorization::CLogoutPayload CAuthorizationControllerComp::OnLogout(
	const sdl::imtauth::Authorization::CLogoutGqlRequest& /*logoutRequest*/,
	const ::imtgql::CGqlRequest& /*gqlRequest*/,
	QString& /*errorMessage*/) const
{
	return sdl::imtauth::Authorization::CLogoutPayload();
}


} // namespace imtauthgql


