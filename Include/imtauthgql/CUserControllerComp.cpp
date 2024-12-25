#include <imtauthgql/CUserControllerComp.h>


// ACF includes
#include <iprm/CParamsSet.h>
#include <iprm/CTextParam.h>

// ImtCore includes
#include <imtauth/CUserInfo.h>


namespace imtauthgql
{


// reimplemented (sdl::imtauth::Users::V1_0::CGraphQlHandlerCompBase)

sdl::imtbase::ImtCollection::CVisualStatus::V1_0 CUserControllerComp::OnGetObjectVisualStatus(
			const sdl::imtauth::Users::V1_0::CGetObjectVisualStatusGqlRequest& /*getObjectVisualStatusRequest*/,
			const ::imtgql::CGqlRequest& /*gqlRequest*/,
			QString& /*errorMessage*/) const
{
	return sdl::imtbase::ImtCollection::CVisualStatus::V1_0();
}

sdl::imtauth::Users::CUsersListPayload::V1_0 CUserControllerComp::OnUsersList(
			const sdl::imtauth::Users::V1_0::CUsersListGqlRequest& /*usersListRequest*/,
			const imtgql::CGqlRequest& /*gqlRequest*/,
			QString& /*errorMessage*/) const
{
	return sdl::imtauth::Users::CUsersListPayload::V1_0();
}


sdl::imtauth::Users::CUserDataPayload::V1_0 CUserControllerComp::OnUserItem(
			const sdl::imtauth::Users::V1_0::CUserItemGqlRequest& /*userItemRequest*/,
			const imtgql::CGqlRequest& /*gqlRequest*/,
			QString& /*errorMessage*/) const
{
	return sdl::imtauth::Users::CUserDataPayload::V1_0();
}


sdl::imtbase::ImtCollection::CUpdatedNotificationPayload::V1_0 CUserControllerComp::OnUserUpdate(
			const sdl::imtauth::Users::V1_0::CUserUpdateGqlRequest& /*userUpdateRequest*/,
			const imtgql::CGqlRequest& /*gqlRequest*/,
			QString& /*errorMessage*/) const
{
	return sdl::imtbase::ImtCollection::CUpdatedNotificationPayload::V1_0();
}


sdl::imtbase::ImtCollection::CAddedNotificationPayload::V1_0 CUserControllerComp::OnUserAdd(
			const sdl::imtauth::Users::V1_0::CUserAddGqlRequest& /*userAddRequest*/,
			const imtgql::CGqlRequest& /*gqlRequest*/,
			QString& /*errorMessage*/) const
{
	return sdl::imtbase::ImtCollection::CAddedNotificationPayload::V1_0();
}


sdl::imtauth::Users::CChangePasswordPayload::V1_0 CUserControllerComp::OnChangePassword(
			const sdl::imtauth::Users::V1_0::CChangePasswordGqlRequest& changePasswordRequest,
			const imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	sdl::imtauth::Users::CChangePasswordInput::V1_0 inputArgument = changePasswordRequest.GetRequestedArguments().input;
	QByteArray id;
	if (inputArgument.UserId){
		id = *inputArgument.UserId;
	}
	QString oldPassword;
	if (inputArgument.OldPassword){
		oldPassword = *inputArgument.OldPassword;
	}
	QString newPassword;
	if (inputArgument.NewPassword){
		newPassword = *inputArgument.NewPassword;
	}

	imtauth::IUserInfo* userInfoPtr = nullptr;
	imtbase::IObjectCollection::DataPtr dataPtr;
	if (m_userCollectionCompPtr->GetObjectData(id, dataPtr)){
		userInfoPtr = dynamic_cast<imtauth::IUserInfo*>(dataPtr.GetPtr());
	}

	if (userInfoPtr == nullptr){
		errorMessage = QString("Unable to change password for user '%1'. Error: The user does not exist").arg(qPrintable(id));
		return sdl::imtauth::Users::CChangePasswordPayload::V1_0();
	}

	QByteArray login = userInfoPtr->GetId();

	if (userInfoPtr == nullptr){
		errorMessage = QString("Unable to change password for user '%1'. Error: The user does not exist").arg(qPrintable(login));
		SendErrorMessage(0, errorMessage, "CUserControllerComp");

		return sdl::imtauth::Users::CChangePasswordPayload::V1_0();
	}

	imtauth::IUserInfo::SystemInfoList systemInfoList = userInfoPtr->GetSystemInfos();
	for (const imtauth::IUserInfo::SystemInfo& systemInfo : systemInfoList){
		if (systemInfo.enabled && !systemInfo.systemId.isEmpty()){
			errorMessage = QString("Unable to change password for user '%1'. Error: A user from an external system").arg(qPrintable(login));
			SendErrorMessage(0, errorMessage, "CUserControllerComp");

			return sdl::imtauth::Users::CChangePasswordPayload::V1_0();
		}
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
		SendErrorMessage(0, errorMessage, "CUserControllerComp");

		return sdl::imtauth::Users::CChangePasswordPayload::V1_0();
	}

	QByteArray passwordHash = m_hashCalculatorCompPtr->GenerateHash(login + newPassword.toUtf8());
	userInfoPtr->SetPasswordHash(passwordHash);

	if (!m_userCollectionCompPtr->SetObjectData(id, *userInfoPtr)){
		errorMessage = QString("Unable to change password for user '%1'").arg(qPrintable(login));
		SendErrorMessage(0, errorMessage, "CUserControllerComp");

		return sdl::imtauth::Users::CChangePasswordPayload::V1_0();
	}

	sdl::imtauth::Users::CChangePasswordPayload::V1_0 payload;
	payload.UserId = QByteArray(id);

	return payload;
}


} // namespace imtauthgql


