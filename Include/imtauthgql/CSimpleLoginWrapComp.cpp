#include <imtauthgql/CSimpleLoginWrapComp.h>


// ACF includes
#include <iser/CJsonMemReadArchive.h>

// ImtCore includes
#include <imtgql/CGqlRequest.h>
#include <imtqml/CGqlModel.h>
#include <imtauth/CUserInfo.h>
#include <GeneratedFiles/imtauthsdl/SDL/1.0/CPP/Authorization.h>
#include <GeneratedFiles/imtauthsdl/SDL/1.0/CPP/Users.h>


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
	imtgql::CGqlRequest request(imtgql::CGqlRequest::RT_QUERY,
								sdl::imtauth::Authorization::V1_0::CAuthorizationGqlRequest::GetCommandId());

	imtgql::CGqlObject inputObject;
	inputObject.InsertField(sdl::imtauth::Authorization::V1_0::CAuthorizationInput::AuthorizationInputFields::Login.toUtf8(), userName);
	inputObject.InsertField(sdl::imtauth::Authorization::V1_0::CAuthorizationInput::AuthorizationInputFields::Password.toUtf8(), password);
	inputObject.InsertField(sdl::imtauth::Authorization::V1_0::CAuthorizationInput::AuthorizationInputFields::ProductId.toUtf8(), qPrintable(*m_productIdAttrPtr));
	request.AddParam("input", inputObject);

	imtbase::CTreeItemModel responseModel;
	bool retVal = SendModelRequest(request, responseModel);
	if (retVal){
		imtbase::CTreeItemModel* dataModelPtr = responseModel.GetTreeItemModel("data");
		if (dataModelPtr == nullptr){
			return false;
		}

		QByteArray objectId;
		if (dataModelPtr->ContainsKey(sdl::imtauth::Authorization::V1_0::CAuthorizationPayload::AuthorizationPayloadFields::UserId.toUtf8())){
			objectId = dataModelPtr->GetData(sdl::imtauth::Authorization::V1_0::CAuthorizationPayload::AuthorizationPayloadFields::UserId.toUtf8()).toByteArray();
		}

		if (objectId.isEmpty()){
			return false;
		}

		RetrieveUserInfo(objectId);

		if (dataModelPtr->ContainsKey(sdl::imtauth::Authorization::V1_0::CAuthorizationPayload::AuthorizationPayloadFields::Token.toUtf8())){
			m_loggedUserToken = dataModelPtr->GetData(sdl::imtauth::Authorization::V1_0::CAuthorizationPayload::AuthorizationPayloadFields::Token.toUtf8()).toByteArray();
			imtqml::CGqlModel::SetGlobalAccessToken(m_loggedUserToken);
		}

		if (dataModelPtr->ContainsKey(sdl::imtauth::Authorization::V1_0::CAuthorizationPayload::AuthorizationPayloadFields::Username.toUtf8())){
			istd::CChangeNotifier notifier(this);
			Q_UNUSED(notifier);

			m_loggedUserId = dataModelPtr->GetData(sdl::imtauth::Authorization::V1_0::CAuthorizationPayload::AuthorizationPayloadFields::Username.toUtf8()).toByteArray();
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

	imtgql::CGqlRequest userRequest(imtgql::CGqlRequest::RT_QUERY,
									sdl::imtauth::Users::V1_0::CUserItemGqlRequest::GetCommandId());
	imtgql::CGqlObject userInputObject;
	userInputObject.InsertField(QByteArray(sdl::imtauth::Users::V1_0::CUserItemInput::UserItemInputFields::ProductId.toUtf8()), QVariant(*m_productIdAttrPtr));
	userInputObject.InsertField(QByteArray(sdl::imtauth::Users::V1_0::CUserItemInput::UserItemInputFields::Id.toUtf8()), QVariant(userObjectId));
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

		imtbase::CTreeItemModel* userDataModelPtr = dataModelPtr->GetTreeItemModel(
			sdl::imtauth::Users::V1_0::CUserDataPayload::UserDataPayloadFields::UserData.toUtf8());
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

		if (userDataModelPtr->ContainsKey(sdl::imtauth::Users::V1_0::CUserData::UserDataFields::Id.toUtf8())){
			QByteArray login = userDataModelPtr->GetData(sdl::imtauth::Users::V1_0::CUserData::UserDataFields::Username.toUtf8()).toByteArray();
			m_userInfoPtr->SetId(login);
		}

		if (userDataModelPtr->ContainsKey(sdl::imtauth::Users::V1_0::CUserData::UserDataFields::Password.toUtf8())){
			QByteArray password = userDataModelPtr->GetData(sdl::imtauth::Users::V1_0::CUserData::UserDataFields::Password.toUtf8()).toByteArray();
			m_userInfoPtr->SetPasswordHash(password);
		}

		m_userPermissionIds = m_userInfoPtr->GetPermissions(*m_productIdAttrPtr);

		return true;
	}

	return false;
}


} // namespace imtauthgql


