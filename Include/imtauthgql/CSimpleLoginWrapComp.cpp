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
	if (!m_loggedUserId.isEmpty()){
		if (m_userInfoPtr.IsValid()){
			istd::TDelPtr<iauth::CUser> userPtr(new iauth::CUser);

			QByteArray passwordHash = m_userInfoPtr->GetPasswordHash();
			userPtr->SetPassword(passwordHash);

			QByteArray username = m_userInfoPtr->GetId();
			userPtr->SetUserName(username);

			return userPtr.PopPtr();
		}
	}

	return nullptr;
}


bool CSimpleLoginWrapComp::Login(const QString& userName, const QString& password)
{
	if (!m_userInfoFactCompPtr.IsValid()){
		return false;
	}

	imtgql::CGqlRequest request(imtgql::CGqlRequest::RT_QUERY, "UserToken");
	imtgql::CGqlObject inputObject("input");
	inputObject.InsertField("Login", userName);
	inputObject.InsertField("Password", password);
	request.AddParam(inputObject);

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


// protected methods

// reimplemented (icomp::CComponentBase)

void CSimpleLoginWrapComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();
}


// private methods

bool CSimpleLoginWrapComp::RetrieveUserInfo(const QByteArray& userObjectId)
{
	if (!m_userInfoFactCompPtr.IsValid()){
		return false;
	}

	imtgql::CGqlRequest userRequest(imtgql::CGqlRequest::RT_QUERY, "UserItem");
	imtgql::CGqlObject userInputObject("input");
	userInputObject.InsertField(QByteArray("ProductId"), QVariant(*m_productIdAttrPtr));
	userInputObject.InsertField(QByteArray("Id"), QVariant(userObjectId));
	userInputObject.InsertField(QByteArray("IsJsonSerialized"), QVariant(true));
	userRequest.AddParam(userInputObject);

	imtgql::CGqlObject queryUserFields("item");
	queryUserFields.InsertField("Id");
	userRequest.AddField(queryUserFields);

	imtbase::CTreeItemModel userResponseModel;
	bool ok = SendModelRequest(userRequest, userResponseModel);
	if (ok){
		imtbase::CTreeItemModel* userDataModelPtr = userResponseModel.GetTreeItemModel("data");
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

		QByteArray userJson = userDataModelPtr->toJSON().toUtf8();
		iser::CJsonMemReadArchive archive(userJson);
		if (!m_userInfoPtr->Serialize(archive)){
			return false;
		}

		m_userPermissionIds = m_userInfoPtr->GetPermissions(*m_productIdAttrPtr);

		return true;
	}

	return false;
}


} // namespace imtauthgql


