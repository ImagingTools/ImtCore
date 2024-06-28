#include <imtauthgql/CAuthorizationControllerComp.h>


// ACF includes
#include <iprm/CParamsSet.h>
#include <iprm/CIdParam.h>

// ImtCore includes
#include <imtauth/CUserInfo.h>
#include <imtauth/CSessionInfo.h>


namespace imtauthgql
{


// protected methods

// reimplemented (imtgql::CGqlRepresentationControllerCompBase)

imtbase::CTreeItemModel* CAuthorizationControllerComp::CreateInternalResponse(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const
{
	istd::TDelPtr<imtbase::CTreeItemModel> rootModelPtr(new imtbase::CTreeItemModel());
	imtbase::CTreeItemModel* dataModelPtr = rootModelPtr->AddTreeModel("data");

	QByteArray login;

	if (m_userCollectionCompPtr.IsValid()){
		const imtgql::CGqlObject* gqlInputParamPtr = gqlRequest.GetParam("input");
		if (gqlInputParamPtr == nullptr){
			SendErrorMessage(0, QString("Invalid input parameters"), "imtgql::CAuthorizationControllerComp");
			errorMessage = QString("Invalid input parameters");

			return nullptr;
		}

		QByteArray productId = gqlInputParamPtr->GetFieldArgumentValue("ProductId").toByteArray();

		login = gqlInputParamPtr->GetFieldArgumentValue("Login").toByteArray();
		QByteArray password = gqlInputParamPtr->GetFieldArgumentValue("Password").toByteArray();

		QByteArray passwordHash;
		if (m_hashCalculatorCompPtr.IsValid()){
			passwordHash = m_hashCalculatorCompPtr->GenerateHash(login + password);
		}

		iprm::CParamsSet filterParam;
		iprm::CParamsSet paramsSet;

		iprm::CIdParam userId;
		userId.SetId(login);

		paramsSet.SetEditableParameter("Id", &userId);
		filterParam.SetEditableParameter("ObjectFilter", &paramsSet);

		imtbase::IObjectCollection::Ids userIds = m_userCollectionCompPtr->GetElementIds(0, -1, &filterParam);
		if (!userIds.isEmpty()){
			QByteArray userObjectId = userIds[0];
			imtbase::IObjectCollection::DataPtr dataPtr;
			if (m_userCollectionCompPtr->GetObjectData(userObjectId, dataPtr)){
				imtauth::CUserInfo* userInfoPtr = dynamic_cast<imtauth::CUserInfo*>(dataPtr.GetPtr());
				if (userInfoPtr != nullptr){
					QByteArray userPassword = userInfoPtr->GetPasswordHash();
					if (userPassword == passwordHash){
						QByteArray tokenValue = QUuid::createUuid().toByteArray();

						dataModelPtr->SetData("Token", tokenValue);
						dataModelPtr->SetData("Login", login);
						dataModelPtr->SetData("UserId", userObjectId);
						dataModelPtr->SetData("PasswordHash", passwordHash);

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
							SendWarningMessage(0, QString("Unable to set last connection info for user with login: '%1'").arg(login), "imtgql::CAuthorizationControllerComp");
						}

						return rootModelPtr.PopPtr();
					}
				}
			}
		}
	}

	errorMessage = QT_TR_NOOP(QString("Invalid login or password. Login: '%1'.").arg(login));
	SendWarningMessage(0, errorMessage, "imtgql::CAuthorizationControllerComp");

	imtbase::CTreeItemModel* errorsItemModelPtr = rootModelPtr->AddTreeModel("errors");
	errorsItemModelPtr->SetData("message", errorMessage);

	return rootModelPtr.PopPtr();
}


} // namespace imtauthgql


