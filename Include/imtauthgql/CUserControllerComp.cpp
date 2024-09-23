#include <imtauthgql/CUserControllerComp.h>


// ACF includes
#include <iprm/CParamsSet.h>
#include <iprm/CTextParam.h>

// ImtCore includes
#include <imtauth/CUserInfo.h>


namespace imtauthgql
{


sdl::imtauth::Users::V1_0::CUsersListPayload CUserControllerComp::OnUsersList(
			const sdl::imtauth::Users::V1_0::CUsersListGqlRequest& usersListRequest,
			const imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	return sdl::imtauth::Users::V1_0::CUsersListPayload();
}


sdl::imtauth::Users::V1_0::CUserDataPayload CUserControllerComp::OnUserItem(
			const sdl::imtauth::Users::V1_0::CUserItemGqlRequest& userItemRequest,
			const imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	return sdl::imtauth::Users::V1_0::CUserDataPayload();
}


sdl::imtauth::Users::V1_0::CUpdatedNotificationPayload CUserControllerComp::OnUserUpdate(
			const sdl::imtauth::Users::V1_0::CUserUpdateGqlRequest& userUpdateRequest,
			const imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	return sdl::imtauth::Users::V1_0::CUpdatedNotificationPayload();
}


sdl::imtauth::Users::V1_0::CAddedNotificationPayload CUserControllerComp::OnUserAdd(
			const sdl::imtauth::Users::V1_0::CUserAddGqlRequest& userAddRequest,
			const imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	return sdl::imtauth::Users::V1_0::CAddedNotificationPayload();
}


sdl::imtauth::Users::V1_0::CChangePasswordPayload CUserControllerComp::OnChangePassword(
			const sdl::imtauth::Users::V1_0::CChangePasswordGqlRequest& changePasswordRequest,
			const imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	QByteArray id = changePasswordRequest.GetRequestedArguments().input.GetUserId();
	QString oldPassword = changePasswordRequest.GetRequestedArguments().input.GetOldPassword();
	QString newPassword = changePasswordRequest.GetRequestedArguments().input.GetNewPassword();

	imtauth::IUserInfo* userInfoPtr = nullptr;
	imtbase::IObjectCollection::DataPtr dataPtr;
	if (m_userCollectionCompPtr->GetObjectData(id, dataPtr)){
		userInfoPtr = dynamic_cast<imtauth::IUserInfo*>(dataPtr.GetPtr());
	}

	if (userInfoPtr == nullptr){
		errorMessage = QString("Unable to change password for user '%1'. Error: The user does not exist").arg(qPrintable(id));
		return sdl::imtauth::Users::V1_0::CChangePasswordPayload();
	}

	QByteArray login = userInfoPtr->GetId();

	if (userInfoPtr == nullptr){
		errorMessage = QString("Unable to change password for user '%1'. Error: The user does not exist").arg(qPrintable(login));
		SendWarningMessage(0, errorMessage, "CUserControllerComp");

		return sdl::imtauth::Users::V1_0::CChangePasswordPayload();
	}

	bool ok = false;

	const imtgql::IGqlContext* gqlContextPtr = gqlRequest.GetRequestContext();
	if (gqlContextPtr != nullptr){
		imtauth::IUserInfo* contextUserInfoPtr = gqlContextPtr->GetUserInfo();
		if (contextUserInfoPtr != nullptr){
			ok = contextUserInfoPtr->IsAdmin();
		}
	}

	if (!ok && m_credentialControllerCompPtr.IsValid()){
		ok = m_credentialControllerCompPtr->CheckCredential(login, oldPassword.toUtf8());
	}

	if (!ok){
		errorMessage = QString("Unable to change password for user '%1'. Error: Invalid login or password.").arg(qPrintable(login));
		SendWarningMessage(0, errorMessage, "CUserControllerComp");

		return sdl::imtauth::Users::V1_0::CChangePasswordPayload();
	}

	QByteArray passwordHash = m_hashCalculatorCompPtr->GenerateHash(login + newPassword.toUtf8());
	userInfoPtr->SetPasswordHash(passwordHash);

	if (!m_userCollectionCompPtr->SetObjectData(id, *userInfoPtr)){
		errorMessage = QString("Unable to change password for user '%1'").arg(qPrintable(login));
		SendWarningMessage(0, errorMessage, "CUserControllerComp");

		return sdl::imtauth::Users::V1_0::CChangePasswordPayload();
	}

	sdl::imtauth::Users::V1_0::CChangePasswordPayload payload;
	payload.SetUserId(id);

	return payload;
}


} // namespace imtauthgql


