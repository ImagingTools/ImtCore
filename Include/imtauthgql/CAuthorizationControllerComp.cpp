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

imtbase::CTreeItemModel* CAuthorizationControllerComp::CreateInvalidLoginOrPasswordResponse(const QByteArray& login, QString& errorMessage) const
{
	errorMessage = QT_TR_NOOP(QString("Invalid login or password. Login: '%1'.").arg(login));
	SendErrorMessage(0, errorMessage, "imtgql::CAuthorizationControllerComp");

	return nullptr;
}


// reimplemented (imtgql::CGqlRepresentationControllerCompBase)

imtbase::CTreeItemModel* CAuthorizationControllerComp::CreateInternalResponse(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const
{
	if (!m_userCollectionCompPtr.IsValid()){
		return nullptr;
	}

	if (!m_sessionCollectionCompPtr.IsValid()){
		return nullptr;
	}

	const imtgql::CGqlObject* gqlInputParamPtr = gqlRequest.GetParam("input");
	if (gqlInputParamPtr == nullptr){
		errorMessage = QString("Unable to create response for request with ID: '%1'. Error: GraphQL input params is invalid.").arg(gqlRequest.GetCommandId());
		SendErrorMessage(0, errorMessage, "imtgql::CAuthorizationControllerComp");

		return nullptr;
	}

	QByteArray login = gqlInputParamPtr->GetFieldArgumentValue("Login").toByteArray();
	QByteArray productId = gqlInputParamPtr->GetFieldArgumentValue("ProductId").toByteArray();
	QByteArray password = gqlInputParamPtr->GetFieldArgumentValue("Password").toByteArray();

	iprm::CParamsSet filterParam;
	iprm::CParamsSet paramsSet;

	iprm::CTextParam userIdParam;
	userIdParam.SetText(login);

	paramsSet.SetEditableParameter("Id", &userIdParam);
	filterParam.SetEditableParameter("ObjectFilter", &paramsSet);

	imtbase::IObjectCollection::Ids userIds = m_userCollectionCompPtr->GetElementIds(0, -1, &filterParam);
	if (userIds.isEmpty()){
		return CreateInvalidLoginOrPasswordResponse(login, errorMessage);
	}

	imtauth::CUserInfo* userInfoPtr = nullptr;
	QByteArray userObjectId = userIds[0];
	imtbase::IObjectCollection::DataPtr dataPtr;
	if (m_userCollectionCompPtr->GetObjectData(userObjectId, dataPtr)){
		userInfoPtr = dynamic_cast<imtauth::CUserInfo*>(dataPtr.GetPtr());
	}

	if (userInfoPtr == nullptr){
		return CreateInvalidLoginOrPasswordResponse(login, errorMessage);
	}

	bool ok = false;
	for (const imtauth::IUserInfo::SystemInfo& systemInfo : userInfoPtr->GetSystemInfos()){
		if (systemInfo.enabled){
			int index = m_systemIdsAttrPtr.FindValue(systemInfo.systemId);
			Q_ASSERT_X(index >= 0, "CAuthorizationControllerComp::CreateInternalResponse", QString("System-ID '%1' cannot found").arg(systemInfo.systemId).toUtf8());

			const imtauth::ICredentialController* credentialControllerPtr = m_credentialControllersCompPtr[index];
			Q_ASSERT_X(credentialControllerPtr != nullptr, "CAuthorizationControllerComp::CreateInternalResponse", "Invalid credential controller");

			if (credentialControllerPtr->CheckCredential(login, password)){
				ok = true;
				break;
			}
		}
	}

	if (!ok){
		return CreateInvalidLoginOrPasswordResponse(login, errorMessage);
	}

	istd::TDelPtr<imtbase::CTreeItemModel> rootModelPtr(new imtbase::CTreeItemModel());
	imtbase::CTreeItemModel* dataModelPtr = rootModelPtr->AddTreeModel("data");

	QByteArray tokenValue = QUuid::createUuid().toByteArray();

	dataModelPtr->SetData("Token", tokenValue);
	dataModelPtr->SetData("Login", login);
	dataModelPtr->SetData("UserId", userObjectId);
//	dataModelPtr->SetData("PasswordHash", passwordHash);

	imtauth::IUserInfo::FeatureIds permissionIds = userInfoPtr->GetPermissions(productId);
	dataModelPtr->SetData("Permissions", permissionIds.join(';'));

	istd::TDelPtr<imtauth::CSessionInfo> sessionInfoPtr;
	sessionInfoPtr.SetPtr(new imtauth::CSessionInfo());

	sessionInfoPtr->SetUserId(userObjectId);
	sessionInfoPtr->SetToken(tokenValue);

	if (m_sessionCollectionCompPtr.IsValid()){
		m_sessionCollectionCompPtr->InsertNewObject("", "", "", sessionInfoPtr.PopPtr(), tokenValue);
	}

	userInfoPtr->SetLastConnection(QDateTime::currentDateTimeUtc());

	if (!m_userCollectionCompPtr->SetObjectData(userObjectId, *userInfoPtr)){
		errorMessage = QString("Unable to set last connection info for user with login: '%1'").arg(login);
		SendWarningMessage(0, errorMessage, "imtgql::CAuthorizationControllerComp");
	}

	return rootModelPtr.PopPtr();
}


} // namespace imtauthgql


