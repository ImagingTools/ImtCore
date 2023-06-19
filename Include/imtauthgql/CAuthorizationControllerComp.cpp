#include <imtauthgql/CAuthorizationControllerComp.h>


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
		QByteArray login;
		QByteArray password;

		const QList<imtgql::CGqlObject> paramList = gqlRequest.GetParams();

		if (!paramList.isEmpty()){
			if (paramList.at(0).GetFieldIds().contains("Login")){
				login = paramList.at(0).GetFieldArgumentValue("Login").toByteArray();
			}

			if (paramList.at(0).GetFieldIds().contains("Password")){
				password = paramList.at(0).GetFieldArgumentValue("Password").toByteArray();
			}
		}

		QByteArray passwordHash;
		if (m_hashCalculatorCompPtr.IsValid()){
			passwordHash = m_hashCalculatorCompPtr->GenerateHash(login + password);
		}

		imtbase::IObjectCollection::Ids userIds = m_userCollectionCompPtr->GetElementIds();
		for (const imtbase::IObjectCollection::Id& userId : userIds){
			imtbase::IObjectCollection::DataPtr dataPtr;
			if (m_userCollectionCompPtr->GetObjectData(userId, dataPtr)){
				const imtauth::CUserInfo* userInfoPtr = dynamic_cast<const imtauth::CUserInfo*>(dataPtr.GetPtr());
				if (userInfoPtr != nullptr){
					if (userInfoPtr->GetId() == login){
						QByteArray userPassword = userInfoPtr->GetPasswordHash();
						if (userPassword == passwordHash){
							QByteArray tokenValue = QUuid::createUuid().toByteArray();

							dataModelPtr->SetData("Token", tokenValue);
							dataModelPtr->SetData("Login", login);

							istd::TDelPtr<imtauth::CSessionInfo> sessionInfoPtr = new imtauth::CSessionInfo();

							sessionInfoPtr->SetUserId(userId);
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
	}

	errorMessage = QT_TR_NOOP("Invalid username or password");

	imtbase::CTreeItemModel* errorsItemModelPtr = rootModelPtr->AddTreeModel("errors");
	errorsItemModelPtr->SetData("message", errorMessage);

	return rootModelPtr.PopPtr();
}


} // namespace imtauthgql


