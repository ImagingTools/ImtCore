#include <imtauthgql/CSimpleLoginWrapComp.h>


// ACF includes
#include <iser/CJsonMemReadArchive.h>

// ImtCore includes
#include <imtgql/CGqlRequest.h>
#include <imtqml/CGqlModel.h>
#include <imtauth/CUserInfo.h>


namespace imtauthgql
{


// public methods

CSimpleLoginWrapComp::CSimpleLoginWrapComp()
{
}


// reimplemented (iauth::ILogin)

iauth::CUser* CSimpleLoginWrapComp::GetLoggedUser() const
{
	if (!m_loggedUserId.isEmpty() && m_userInfoPtr.IsValid()){
		istd::TDelPtr<iauth::CUser> userPtr(new iauth::CUser);
		userPtr->SetUserName(m_loggedUserId);
		userPtr->SetPassword(m_loggedUserPassword);

		return userPtr.PopPtr();
	}

	return nullptr;
}


bool CSimpleLoginWrapComp::Login(const QString& userName, const QString& password)
{
	imtgql::CGqlRequest request(imtgql::CGqlRequest::RT_QUERY, "UserToken");
	imtgql::CGqlObject inputObject;
	inputObject.InsertField("Login", userName);
	inputObject.InsertField("Password", password);
	request.AddParam("input", inputObject);

	imtbase::CTreeItemModel responseModel;
	bool retVal = SendModelRequest(request, responseModel);
	if (retVal){
		imtbase::CTreeItemModel* dataModelPtr = responseModel.GetTreeItemModel("data");
		if (dataModelPtr == nullptr){
			return false;
		}

		QByteArray objectId;
		if (dataModelPtr->ContainsKey("UserId")){
			objectId = dataModelPtr->GetData("UserId").toByteArray();
		}

		if (objectId.isEmpty()){
			return false;
		}

		RetrieveUserInfo(objectId);

		if (dataModelPtr->ContainsKey("Token")){
			m_loggedUserToken = dataModelPtr->GetData("Token").toByteArray();
			imtqml::CGqlModel::SetGlobalAccessToken(m_loggedUserToken);
		}

		if (dataModelPtr->ContainsKey("Login")){
			istd::CChangeNotifier notifier(this);
			Q_UNUSED(notifier);

			m_loggedUserId = dataModelPtr->GetData("Login").toByteArray();
		}

		m_loggedUserPassword = password.toUtf8();

		return true;
	}

	return false;
}


bool CSimpleLoginWrapComp::Logout()
{
	if (!m_loggedUserId.isEmpty()){
		istd::CChangeNotifier notifier(this);
		Q_UNUSED(notifier);

		m_loggedUserId.clear();

		return true;
	}

	return false;
}


// reimplemented (iauth::IRightsProvider)

bool CSimpleLoginWrapComp::HasRight(
		const QByteArray& operationId,
		bool /*beQuiet*/) const
{
	if (m_userInfoPtr.IsValid()){
		bool isAdmin = m_userInfoPtr->IsAdmin();
		if (isAdmin){
			return true;
		}
	}

	if (!m_checkPermissionCompPtr.IsValid()){
		return false;
	}

	bool retVal = m_checkPermissionCompPtr->CheckPermission(m_userPermissionIds, QByteArrayList() << operationId);

	return retVal;
}


// reimplemented (imtauth::IAccessTokenProvider)

QByteArray CSimpleLoginWrapComp::GetToken(const QByteArray& /*userId*/) const
{
	return m_loggedUserToken;
}


// private methods

bool CSimpleLoginWrapComp::RetrieveUserInfo(const QByteArray& userObjectId)
{
	if (!m_userInfoFactCompPtr.IsValid()){
		return false;
	}

	imtgql::CGqlRequest userRequest(imtgql::CGqlRequest::RT_QUERY, "UserItem");
	imtgql::CGqlObject userInputObject;
	userInputObject.InsertField(QByteArray("ProductId"), QVariant(*m_productIdAttrPtr));
	userInputObject.InsertField(QByteArray("Id"), QVariant(userObjectId));
	userRequest.AddParam("input", userInputObject);

	imtgql::CGqlObject queryUserFields;
	queryUserFields.InsertField("Id");
	userRequest.AddField("item", queryUserFields);

	imtbase::CTreeItemModel userResponseModel;
	bool ok = SendModelRequest(userRequest, userResponseModel);
	if (ok){
		imtbase::CTreeItemModel* dataModelPtr = userResponseModel.GetTreeItemModel("data");
		if (dataModelPtr == nullptr){
			return false;
		}

		imtbase::CTreeItemModel* userDataModelPtr = dataModelPtr->GetTreeItemModel("UserData");
		if (userDataModelPtr == nullptr){
			return false;
		}

		istd::TDelPtr<imtauth::IUserInfo> userInfoPtr = m_userInfoFactCompPtr.CreateInstance();
		if (!userInfoPtr.IsValid()){
			return false;
		}

		m_userInfoPtr.SetPtr(userInfoPtr.PopPtr());
		if (!m_userInfoPtr.IsValid()){
			return false;
		}

		if (userDataModelPtr->ContainsKey("Id")){
			QByteArray login = userDataModelPtr->GetData("Id").toByteArray();
			m_userInfoPtr->SetId(login);
		}

		if (userDataModelPtr->ContainsKey("Password")){
			QByteArray password = userDataModelPtr->GetData("Password").toByteArray();
			m_userInfoPtr->SetPasswordHash(password);
		}

		m_userPermissionIds = m_userInfoPtr->GetPermissions(*m_productIdAttrPtr);

		return true;
	}

	return false;
}


} // namespace imtauthgql


