#include <imtauthgql/CUserControllerComp.h>


// ACF includes
#include <iprm/CParamsSet.h>
#include <iprm/CTextParam.h>
#include <iprm/CEnableableParam.h>

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


sdl::imtauth::Users::CRegisterUserPayload::V1_0 CUserControllerComp::OnRegisterUser(
			const sdl::imtauth::Users::V1_0::CRegisterUserGqlRequest& registerUserRequest,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	sdl::imtauth::Users::CRegisterUserPayload::V1_0 response;
	if (!m_userCollectionCompPtr.IsValid()){
		Q_ASSERT_X(false, "Attribute 'UserCollection' was not set", "CUserControllerComp");
		return response;
	}

	if (!m_userFactoryCompPtr.IsValid()){
		Q_ASSERT_X(false, "Attribute 'UserFactory' was not set", "CUserControllerComp");
		return response;
	}

	if (!m_hashCalculatorCompPtr.IsValid()){
		Q_ASSERT_X(false, "Attribute 'HashCalculator' was not set", "CUserControllerComp");
		return response;
	}

	sdl::imtauth::Users::V1_0::RegisterUserRequestArguments arguments = registerUserRequest.GetRequestedArguments();
	if (!arguments.input.UserData){
		errorMessage = QString("Unable to register user. Error: User data is invalid");
		return response;
	}

	QByteArray productId;
	if (!arguments.input.ProductId){
		productId = *arguments.input.ProductId;
	}

	istd::TDelPtr<imtauth::IUserInfo> userInfoPtr = m_userFactoryCompPtr.CreateInstance();
	sdl::imtauth::Users::CUserData::V1_0 userData = *arguments.input.UserData;

	QByteArray userId;
	if (!m_userRepresentationController.FillUserInfoFromRepresentation(userData, *userInfoPtr, *m_userCollectionCompPtr, userId, errorMessage)){
		errorMessage = QString("Unable to register user. Error: '%1'").arg(errorMessage);
		return response;
	}

	QString password;
	if (userData.Password){
		password = *userData.Password;
	}

	if (password.isEmpty()) {
		errorMessage = QString("Unable to register user. Error: Password cannot be empty");
		return response;
	}

	if (!userData.Username){
		errorMessage = QString("Unable to register user. Error: Invalid username");
		return response;
	}

	password = m_hashCalculatorCompPtr->GenerateHash(*userData.Username + password.toUtf8());

	userInfoPtr->SetPasswordHash(password.toUtf8());

	response.Id = userId;

	iprm::CParamsSet filterParam;
	iprm::CParamsSet paramsSet;

	iprm::CEnableableParam enableableParam;
	enableableParam.SetEnabled(true);

	paramsSet.SetEditableParameter("IsDefault", &enableableParam);

	iprm::CTextParam productIdParam;
	productIdParam.SetText(productId);

	paramsSet.SetEditableParameter("ProductId", &productIdParam);

	filterParam.SetEditableParameter("ObjectFilter", &paramsSet);

	imtbase::IObjectCollection::Ids roleIds = m_roleCollectionCompPtr->GetElementIds(0, -1, &filterParam);
	if (!roleIds.isEmpty()){
		userInfoPtr->AddRole(productId, roleIds[0]);
	}

	m_userCollectionCompPtr->InsertNewObject("", "", "", userInfoPtr.GetPtr(), userId);

	return response;
}


sdl::imtauth::Users::CCheckEmailPayload::V1_0 CUserControllerComp::OnCheckEmail(
			const sdl::imtauth::Users::V1_0::CCheckEmailGqlRequest& checkEmailRequest,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	sdl::imtauth::Users::CCheckEmailPayload::V1_0 response;
	if (!m_userCollectionCompPtr.IsValid()){
		Q_ASSERT_X(false, "Attribute 'UserCollection' was not set", "CUserControllerComp");
		return response;
	}

	sdl::imtauth::Users::V1_0::CheckEmailRequestArguments arguments = checkEmailRequest.GetRequestedArguments();

	QByteArray login;
	if (arguments.input.Login){
		login = *arguments.input.Login;
	}

	if (login.isEmpty()){
		errorMessage = QString("Unable to check email. Error: Login is empty");
		return response;
	}

	QString email;
	if (arguments.input.Email){
		email = *arguments.input.Email;
	}

	if (email.isEmpty()){
		errorMessage = QString("Unable to check email. Error: Email is empty");
		return response;
	}

	iprm::CParamsSet filterParam;
	iprm::CParamsSet paramsSet;

	iprm::CTextParam userIdParam;
	userIdParam.SetText(login);

	paramsSet.SetEditableParameter("Id", &userIdParam);
	filterParam.SetEditableParameter("ObjectFilter", &paramsSet);

	imtbase::IObjectCollection::DataPtr dataPtr;
	const imtauth::CUserInfo* userInfoPtr = nullptr;
	imtbase::IObjectCollection::Ids userIds = m_userCollectionCompPtr->GetElementIds(0, -1, &filterParam);
	if (!userIds.isEmpty()){
		QByteArray userObjectId = userIds[0];
		if (m_userCollectionCompPtr->GetObjectData(userObjectId, dataPtr)){
			userInfoPtr = dynamic_cast<const imtauth::CUserInfo*>(dataPtr.GetPtr());
		}
	}

	if (userInfoPtr == nullptr){
		errorMessage = QString("The user with username '%1' does not exist").arg(qPrintable(login));
		return response;
	}

	response.CorrectEmail = false;

	QString userMail = userInfoPtr->GetMail();
	if (userMail == email){
		response.CorrectEmail = true;
	}

	return response;
}


sdl::imtauth::Users::CCheckEmailPayload::V1_0 CUserControllerComp::OnSendEmailCode(
	const sdl::imtauth::Users::V1_0::CSendEmailCodeGqlRequest& /*sendEmailCodeRequest*/,
	const ::imtgql::CGqlRequest& /*gqlRequest*/,
	QString& /*errorMessage*/) const
{
	sdl::imtauth::Users::CCheckEmailPayload::V1_0 response;

	return response;
}


} // namespace imtauthgql
