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

// reimplemented (imtgql::CGqlRepresentationControllerCompBase)

imtbase::CTreeItemModel* CAuthorizationControllerComp::CreateInternalResponse(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const
{
	istd::TDelPtr<imtbase::CTreeItemModel> rootModelPtr(new imtbase::CTreeItemModel());
	imtbase::CTreeItemModel* dataModelPtr = rootModelPtr->AddTreeModel("data");

	if (m_userCollectionCompPtr.IsValid()){
		const imtgql::CGqlObject* gqlInputParamPtr = gqlRequest.GetParam("input");
		if (gqlInputParamPtr == nullptr){
			SendErrorMessage(0, QString("Invalid input parameters"), "imtgql::CAuthorizationControllerComp");
			errorMessage = QString("Invalid input parameters");

			return nullptr;
		}

		QByteArray productId = gqlInputParamPtr->GetFieldArgumentValue("ProductId").toByteArray();

		QByteArray login = gqlInputParamPtr->GetFieldArgumentValue("Login").toByteArray();
		QByteArray passwordHash = gqlInputParamPtr->GetFieldArgumentValue("Password").toByteArray();

		iprm::CParamsSet filterParam;
		iprm::CParamsSet paramsSet;

		iprm::CTextParam userId;
		userId.SetText(login);

		paramsSet.SetEditableParameter("Id", &userId);
		filterParam.SetEditableParameter("ObjectFilter", &paramsSet);

		imtbase::IObjectCollection::Ids userIds = m_userCollectionCompPtr->GetElementIds(0, -1, &filterParam);
		if (!userIds.isEmpty()){
			QByteArray userObjectId = userIds[0];
			imtbase::IObjectCollection::DataPtr dataPtr;
			if (m_userCollectionCompPtr->GetObjectData(userObjectId, dataPtr)){
				const imtauth::CUserInfo* userInfoPtr = dynamic_cast<const imtauth::CUserInfo*>(dataPtr.GetPtr());
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

						istd::TDelPtr<imtauth::CSessionInfo> sessionInfoPtr = new imtauth::CSessionInfo();

						sessionInfoPtr->SetUserId(userObjectId);
						sessionInfoPtr->SetToken(tokenValue);

						if (m_sessionCollectionCompPtr.IsValid()){
							m_sessionCollectionCompPtr->InsertNewObject("", "", "", sessionInfoPtr.PopPtr(), tokenValue);
						}

						return rootModelPtr.PopPtr();
					}
				}
			}
		}
	}

	SendWarningMessage(0, QString("Invalid login or password"), "imtgql::CAuthorizationControllerComp");
	errorMessage = QT_TR_NOOP("Invalid login or password");

	imtbase::CTreeItemModel* errorsItemModelPtr = rootModelPtr->AddTreeModel("errors");
	errorsItemModelPtr->SetData("message", errorMessage);

	return rootModelPtr.PopPtr();
}


} // namespace imtauthgql


