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


sdl::imtauth::Authorization::V1_0::CAuthorizationPayload CAuthorizationControllerComp::CreateInvalidLoginOrPasswordResponse(const QByteArray& login, QString& errorMessage) const
{
	errorMessage = QT_TR_NOOP(QString("Invalid login or password. Login: '%1'").arg(qPrintable(login)));
	SendErrorMessage(0, errorMessage, "imtgql::CAuthorizationControllerComp");

	return sdl::imtauth::Authorization::V1_0::CAuthorizationPayload();
}


sdl::imtauth::Authorization::V1_0::CAuthorizationPayload CAuthorizationControllerComp::CreateAuthorizationSuccessfulResponse(
			imtauth::CUserInfo& userInfo,
			const QByteArray& systemId,
			const QByteArray& productId,
			QString& errorMessage) const
{
	sdl::imtauth::Authorization::V1_0::CAuthorizationPayload payload;

	QByteArray login = userInfo.GetId();
	QByteArray objectId = GetUserObjectId(login);
	QByteArray tokenValue = QUuid::createUuid().toByteArray();

	payload.SetToken(tokenValue);
	payload.SetUsername(login);
	payload.SetUserId(objectId);
	payload.SetSystemId(systemId);

	if (!productId.isEmpty()){
		imtauth::IUserInfo::FeatureIds permissionIds = userInfo.GetPermissions(productId);
		payload.SetPermissions(permissionIds.join(';'));
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


// reimplemented (sdl::imtauth::Authorization::V1_0::CGraphQlHandlerCompBase)

sdl::imtauth::Authorization::V1_0::CAuthorizationPayload CAuthorizationControllerComp::OnAuthorization(
			const sdl::imtauth::Authorization::V1_0::CAuthorizationGqlRequest& authorizationRequest,
			const imtgql::CGqlRequest& /*gqlRequest*/,
			QString& errorMessage) const
{
	if (!m_userCollectionCompPtr.IsValid()){
		Q_ASSERT_X(false, "Component 'UserCollection' was not set", "CAuthorizationControllerComp");
		return sdl::imtauth::Authorization::V1_0::CAuthorizationPayload();
	}

	if (!m_sessionCollectionCompPtr.IsValid()){
		Q_ASSERT_X(false, "Component 'SessionCollection' was not set", "CAuthorizationControllerComp");
		return sdl::imtauth::Authorization::V1_0::CAuthorizationPayload();
	}

	QByteArray login = authorizationRequest.GetRequestedArguments().input.GetLogin().toUtf8();
	QByteArray productId = authorizationRequest.GetRequestedArguments().input.GetProductId();
	QByteArray password = authorizationRequest.GetRequestedArguments().input.GetPassword().toUtf8();

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


sdl::imtauth::Authorization::V1_0::CUserManagementPayload CAuthorizationControllerComp::OnGetUserMode(
			const sdl::imtauth::Authorization::V1_0::CGetUserModeGqlRequest& /*getUserModeRequest*/,
			const imtgql::CGqlRequest& /*gqlRequest*/,
			QString& /*errorMessage*/) const
{
	sdl::imtauth::Authorization::V1_0::CUserManagementPayload payload;
	return payload;
}


} // namespace imtauthgql


