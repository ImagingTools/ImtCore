#include <imtauthgql/CAuthorizationControllerComp.h>


// ImtCore includes
#include <imtauth/CUserInfo.h>
#include <imtauth/CSessionInfo.h>


namespace imtauthgql
{


imtbase::CTreeItemModel* CAuthorizationControllerComp::CreateResponse(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const
{
	const QList<imtgql::CGqlObject>* paramList = gqlRequest.GetParams();

	imtbase::CTreeItemModel* rootModelPtr = new imtbase::CTreeItemModel();
	imtbase::CTreeItemModel* dataModel = nullptr;

	QByteArray login;
	QByteArray password;

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
		passwordHash = m_hashCalculatorCompPtr->Calculate(login + password);
	}

	imtbase::IObjectCollection::DataPtr dataPtr;
	if (m_userCollectionCompPtr->GetObjectData(login, dataPtr)){
		const imtauth::CUserInfo* userInfoPtr = dynamic_cast<const imtauth::CUserInfo*>(dataPtr.GetPtr());
		if (userInfoPtr != nullptr){
			QByteArray userPassword = userInfoPtr->GetPasswordHash();
			if (userPassword == passwordHash){
				dataModel = new imtbase::CTreeItemModel();

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

				return rootModelPtr;
			}
		}
	}

	errorMessage = QT_TR_NOOP("Invalid username or password");

	imtbase::CTreeItemModel* errorsItemModelPtr = rootModelPtr->AddTreeModel("errors");
	rootModelPtr->SetExternTreeModel("errors", errorsItemModelPtr);
	errorsItemModelPtr->SetData("message", errorMessage);

	return rootModelPtr;
}


bool CAuthorizationControllerComp::LoginMatch(const QByteArray& login) const
{
	imtbase::IObjectCollection::DataPtr dataPtr;
	return m_userCollectionCompPtr->GetObjectData(login, dataPtr);
}


int CAuthorizationControllerComp::GetUserId(const QString& verificationLogin) const
{
	return 0;
}


bool CAuthorizationControllerComp::ChangePassword(const QByteArray& userId, const QString& oldPassword, const QString& newPassword)
{
	return false;
}


} // namespace imtauthgql


