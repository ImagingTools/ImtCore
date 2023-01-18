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

	if (m_userCollectionCompPtr.IsValid()){
		QByteArray login;
		QByteArray password;

		const QList<imtgql::CGqlObject>* paramList = gqlRequest.GetParams();

		if (paramList->size() > 0){
			if (paramList->at(0).GetFieldIds().contains("Login")){
				login = paramList->at(0).GetFieldArgumentValue("Login").toByteArray();
			}

			if (paramList->at(0).GetFieldIds().contains("Password")){
				password = paramList->at(0).GetFieldArgumentValue("Password").toByteArray();
			}
		}

		QByteArray passwordHash;
		if (m_hashCalculatorCompPtr.IsValid()){
			passwordHash = m_hashCalculatorCompPtr->GenerateHash(login + password);
		}

		imtbase::IObjectCollection::DataPtr dataPtr;
		if (m_userCollectionCompPtr->GetObjectData(login, dataPtr)){
			const imtauth::CUserInfo* userInfoPtr = dynamic_cast<const imtauth::CUserInfo*>(dataPtr.GetPtr());
			if (userInfoPtr != nullptr){
				QByteArray userPassword = userInfoPtr->GetPasswordHash();
				if (userPassword == passwordHash){
					imtbase::CTreeItemModel* dataModel = new imtbase::CTreeItemModel();

					QByteArray tokenValue = QUuid::createUuid().toByteArray();

					dataModel->SetData("Token", tokenValue);
					dataModel->SetData("Login", login);

					istd::TDelPtr<imtauth::CSessionInfo> sessionInfoPtr = new imtauth::CSessionInfo();

					sessionInfoPtr->SetUserId(login);
					sessionInfoPtr->SetToken(tokenValue);

					if (m_sessionCollectionCompPtr.IsValid()){
						m_sessionCollectionCompPtr->InsertNewObject("", "", "", sessionInfoPtr.PopPtr(), tokenValue);
					}

					rootModelPtr->SetExternTreeModel("data", dataModel);

					return rootModelPtr.PopPtr();
				}
			}
		}
	}

	errorMessage = QT_TR_NOOP("Invalid username or password");

	imtbase::CTreeItemModel* errorsItemModelPtr = rootModelPtr->AddTreeModel("errors");
	rootModelPtr->SetExternTreeModel("errors", errorsItemModelPtr);
	errorsItemModelPtr->SetData("message", errorMessage);

	return rootModelPtr.PopPtr();
}


} // namespace imtauthgql


