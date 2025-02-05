#include <imtauthgql/CUserControllerComp.h>


// ACF includes
#include <iprm/CParamsSet.h>
#include <iprm/CTextParam.h>
#include <iprm/CEnableableParam.h>

// ImtCore includes
#include <imtauth/CUserInfo.h>
#include <imtauth/CUserVerification.h>
#include <imtmail/CSmtpMessage.h>


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


sdl::imtauth::Users::CChangePasswordPayload::V1_0 CUserControllerComp::OnChangePassword(
	const sdl::imtauth::Users::V1_0::CChangePasswordGqlRequest& changePasswordRequest,
	const imtgql::CGqlRequest& gqlRequest,
	QString& errorMessage) const
{
	sdl::imtauth::Users::CChangePasswordPayload::V1_0 payload;
	payload.Success = false;

	if (!m_userCollectionCompPtr.IsValid()){
		Q_ASSERT_X(false, "Attribute 'UserCollection' was not set", "CUserControllerComp");
		return payload;
	}

	if (!m_hashCalculatorCompPtr.IsValid()){
		Q_ASSERT_X(false, "Attribute 'HashCalculator' was not set", "CUserControllerComp");
		return payload;
	}

	sdl::imtauth::Users::CChangePasswordInput::V1_0 inputArgument = changePasswordRequest.GetRequestedArguments().input;
	QByteArray login;
	if (inputArgument.Login){
		login = *inputArgument.Login;
	}
	QString oldPassword;
	if (inputArgument.OldPassword){
		oldPassword = *inputArgument.OldPassword;
	}
	QString newPassword;
	if (inputArgument.NewPassword){
		newPassword = *inputArgument.NewPassword;
	}

	QByteArray userId = GetUserIdByLogin(login);

	imtauth::IUserInfo* userInfoPtr = nullptr;
	imtbase::IObjectCollection::DataPtr dataPtr;
	if (m_userCollectionCompPtr->GetObjectData(userId, dataPtr)){
		userInfoPtr = dynamic_cast<imtauth::IUserInfo*>(dataPtr.GetPtr());
	}

	if (userInfoPtr == nullptr){
		errorMessage = QString("Unable to change password for user '%1'. Error: The user does not exist").arg(qPrintable(login));
		return payload;
	}

	if (userInfoPtr == nullptr){
		errorMessage = QString("Unable to change password for user '%1'. Error: The user does not exist").arg(qPrintable(login));
		SendErrorMessage(0, errorMessage, "CUserControllerComp");

		return payload;
	}

	imtauth::IUserInfo::SystemInfoList systemInfoList = userInfoPtr->GetSystemInfos();
	for (const imtauth::IUserInfo::SystemInfo& systemInfo : systemInfoList){
		if (systemInfo.enabled && !systemInfo.systemId.isEmpty()){
			errorMessage = QString("Unable to change password for user '%1'. Error: A user from an external system").arg(qPrintable(login));
			SendErrorMessage(0, errorMessage, "CUserControllerComp");

			return payload;
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

	if (m_userVerificationControllerCompPtr.IsValid()){
		istd::TDelPtr<const imtauth::IUserVerification> verificationInfoPtr = m_userVerificationControllerCompPtr->GetVerificationData(userId);
		if (verificationInfoPtr.IsValid() && !verificationInfoPtr->IsExpired()){
			ok = true;
		}
	}

	if (!ok){
		errorMessage = QString("Unable to change password for user '%1'. Error: Invalid login or password.").arg(qPrintable(login));
		SendErrorMessage(0, errorMessage, "CUserControllerComp");

		return payload;
	}

	QByteArray passwordHash = m_hashCalculatorCompPtr->GenerateHash(login + newPassword.toUtf8());
	userInfoPtr->SetPasswordHash(passwordHash);

	istd::TDelPtr<imtbase::IOperationContext> operationContextPtr =  nullptr;
	if (m_userOperationContextControllerCompPtr.IsValid()){
		operationContextPtr.SetPtr(m_userOperationContextControllerCompPtr->CreateOperationContext("ChangePassword", userId, *userInfoPtr));
	}

	if (!m_userCollectionCompPtr->SetObjectData(userId, *userInfoPtr, istd::IChangeable::CM_WITHOUT_REFS, operationContextPtr.GetPtr())){
		errorMessage = QString("Unable to change password for user '%1'").arg(qPrintable(login));
		SendErrorMessage(0, errorMessage, "CUserControllerComp");

		return payload;
	}

	payload.Success = true;

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
	if (arguments.input.ProductId){
		productId = *arguments.input.ProductId;
	}

	istd::TDelPtr<imtauth::CIdentifiableUserInfo> userInfoPtr;
	userInfoPtr.SetCastedOrRemove(m_userFactoryCompPtr.CreateInstance());
	if (!userInfoPtr.IsValid()){
		Q_ASSERT_X(false, "User instance is invalid", "CUserControllerComp");
		return response;
	}

	sdl::imtauth::Users::CUserData::V1_0 userData = *arguments.input.UserData;

	QByteArray userId;
	if (arguments.input.UserData->Id){
		userId = *arguments.input.UserData->Id;
	}

	userInfoPtr->SetObjectUuid(userId);

	if (!m_userRepresentationController.FillUserInfoFromRepresentation(userData, *userInfoPtr, *m_userCollectionCompPtr, userId, errorMessage)){
		errorMessage = QString("Unable to register user. Error: '%1'").arg(errorMessage);
		return response;
	}

	imtauth::IUserInfo::SystemInfo systemInfo;
	userInfoPtr->AddToSystem(systemInfo);

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

	if (m_roleCollectionCompPtr.IsValid()){
		imtbase::IObjectCollection::Ids roleIds = m_roleCollectionCompPtr->GetElementIds(0, -1, &filterParam);
		if (!roleIds.isEmpty()){
			userInfoPtr->AddRole(productId, roleIds[0]);
		}
	}

	QByteArray objectId = m_userCollectionCompPtr->InsertNewObject("User", "", "", userInfoPtr.GetPtr(), userId);
	if (objectId.isEmpty()){
		errorMessage = QString("Unable to register user. Error: Insert object to collection failed");
		SendWarningMessage(0, errorMessage, "CUserControllerComp");
		return response;
	}

	return response;
}


sdl::imtauth::Users::CCheckEmailPayload::V1_0 CUserControllerComp::OnCheckEmail(
	const sdl::imtauth::Users::V1_0::CCheckEmailGqlRequest& checkEmailRequest,
	const ::imtgql::CGqlRequest& gqlRequest,
	QString& /*errorMessage*/) const
{
	sdl::imtauth::Users::CCheckEmailPayload::V1_0 response;
	if (!m_userCollectionCompPtr.IsValid()){
		Q_ASSERT_X(false, "Attribute 'UserCollection' was not set", "CUserControllerComp");
		return response;
	}

	if (!m_userVerificationControllerCompPtr.IsValid()){
		Q_ASSERT_X(false, "Attribute 'UserVerificationController' was not set", "CUserControllerComp");
		return response;
	}

	response.Success = false;

	sdl::imtauth::Users::V1_0::CheckEmailRequestArguments arguments = checkEmailRequest.GetRequestedArguments();

	QString email;
	if (arguments.input.Email){
		email = *arguments.input.Email;
	}

	if (email.isEmpty()){
		response.Message = QString("Unable to check email. Error: Email is empty");
		return response;
	}

	QByteArray userId = GetUserIdByEmail(email);

	const imtauth::CUserInfo* userInfoPtr = nullptr;
	imtbase::IObjectCollection::DataPtr dataPtr;
	if (m_userCollectionCompPtr->GetObjectData(userId, dataPtr)){
		userInfoPtr = dynamic_cast<const imtauth::CUserInfo*>(dataPtr.GetPtr());
	}

	if (userInfoPtr == nullptr){
		response.Message = QString("There are no users with email '%1' in the system").arg(email);
		return response;
	}

	response.Success = true;
	response.UserName = userInfoPtr->GetName();
	response.Login = userInfoPtr->GetId();

	return response;
}


sdl::imtauth::Users::CSendEmailCodePayload::V1_0 CUserControllerComp::OnSendEmailCode(
	const sdl::imtauth::Users::V1_0::CSendEmailCodeGqlRequest& sendEmailCodeRequest,
	const ::imtgql::CGqlRequest& /*gqlRequest*/,
	QString& /*errorMessage*/) const
{
	sdl::imtauth::Users::CSendEmailCodePayload::V1_0 response;

	if (!m_userVerificationControllerCompPtr.IsValid()){
		Q_ASSERT_X(false, "Attribute 'UserVerificationController' was not set", "CUserControllerComp");
		return response;
	}

	sdl::imtauth::Users::V1_0::SendEmailCodeRequestArguments arguments = sendEmailCodeRequest.GetRequestedArguments();

	QByteArray login;
	if (arguments.input.Login){
		login = *arguments.input.Login;
	}

	response.Login = login;
	response.Success = false;

	QByteArray objectId = GetUserIdByLogin(login);
	istd::TDelPtr<const imtauth::IUserInfo> userInfoPtr = GetUserInfoByLogin(login);
	if (!userInfoPtr.IsValid()){
		response.Message = QString("Unable to send email code. Error: User is invalid");
	}
	else{
		bool ok = SendUserCode(objectId, *userInfoPtr.GetPtr());
		if (ok){
			response.Success = ok;
		}
		else{
			response.Message = QString("Unable to send email code. Error: Unknown error");
		}
	}

	return response;
}


sdl::imtauth::Users::CCheckEmailCodePayload::V1_0 CUserControllerComp::OnCheckEmailCode(
	const sdl::imtauth::Users::V1_0::CCheckEmailCodeGqlRequest& checkEmailCodeRequest,
	const ::imtgql::CGqlRequest& /*gqlRequest*/,
	QString& /*errorMessage*/) const
{
	sdl::imtauth::Users::CCheckEmailCodePayload::V1_0 response;

	if (!m_userVerificationControllerCompPtr.IsValid()){
		Q_ASSERT_X(false, "Attribute 'UserVerificationController' was not set", "CUserControllerComp");
		return response;
	}

	sdl::imtauth::Users::V1_0::CheckEmailCodeRequestArguments arguments = checkEmailCodeRequest.GetRequestedArguments();

	QByteArray login;
	if (arguments.input.Login){
		login = *arguments.input.Login;
	}

	QByteArray userId = GetUserIdByLogin(login);

	QString code;
	if (arguments.input.Code){
		code = *arguments.input.Code;
	}

	response.CorrectCode = m_userVerificationControllerCompPtr->VerifyUser(userId, code.toUtf8());

	return response;
}



sdl::imtauth::Users::CCheckSuperuserPayload::V1_0 CUserControllerComp::OnCheckSuperuserExists(
	const sdl::imtauth::Users::V1_0::CCheckSuperuserExistsGqlRequest& checkSuperuserExistsRequest,
	const ::imtgql::CGqlRequest& /*gqlRequest*/,
	QString& errorMessage) const
{
	sdl::imtauth::Users::CCheckSuperuserPayload::V1_0 response;

	if (!m_userCollectionCompPtr.IsValid()){
		Q_ASSERT_X(false, "Attribute 'UserCollection' was not set", "CUserControllerComp");
		return response;
	}

	response.Exists = false;
	response.ErrorType = "";
	response.Message = "";

	if (m_databaseConnectionCheckerCompPtr.IsValid()){
		QString errorMessage;
		bool ok = m_databaseConnectionCheckerCompPtr->CheckDatabaseConnection(errorMessage);
		if (!ok){
			response.ErrorType = sdl::imtauth::Users::CCheckSuperuserErrorType::V1_0::CheckSuperuserErrorTypeFields::DbNotConnection;
			response.Message = errorMessage;

			return response;
		}
	}

	istd::TDelPtr<const imtauth::IUserInfo> userInfoPtr = GetUserInfoByLogin("su");
	if (!userInfoPtr.IsValid()){
		response.ErrorType = sdl::imtauth::Users::CCheckSuperuserErrorType::V1_0::CheckSuperuserErrorTypeFields::NotExists;
		return response;
	}

	response.Exists = true;

	return response;
}


sdl::imtauth::Users::CCreateSuperuserPayload::V1_0 CUserControllerComp::OnCreateSuperuser(
	const sdl::imtauth::Users::V1_0::CCreateSuperuserGqlRequest& createSuperuserRequest,
	const ::imtgql::CGqlRequest& /*gqlRequest*/,
	QString& /*errorMessage*/) const
{
	sdl::imtauth::Users::CCreateSuperuserPayload::V1_0 response;
	if (!m_userCollectionCompPtr.IsValid()){
		Q_ASSERT_X(false, "Attribute 'UserCollection' was not set", "CUserControllerComp");
		return response;
	}

	if (!m_hashCalculatorCompPtr.IsValid()){
		Q_ASSERT_X(false, "Attribute 'HashCalculator' was not set", "CUserControllerComp");
		return response;
	}

	response.Success = false;
	response.Message = "";

	istd::TDelPtr<const imtauth::IUserInfo> userInfoPtr = GetUserInfoByLogin("su");
	if (userInfoPtr.IsValid()){
		response.Message = QString("Superuser already exists");
		return response;
	}

	sdl::imtauth::Users::V1_0::CreateSuperuserRequestArguments arguments = createSuperuserRequest.GetRequestedArguments();

	QString name;
	if (arguments.input.Name){
		name = *arguments.input.Name;
	}

	if (name.isEmpty()){
		name = "superuser";
	}

	QString mail;
	if (arguments.input.Mail){
		mail = *arguments.input.Mail;
	}

	if (mail.isEmpty()){
		response.Message = QString("Unable to create superuser with empty email");
		return response;
	}

	QString password;
	if (arguments.input.Password){
		password = *arguments.input.Password;
	}

	if (password.isEmpty()){
		response.Message = QString("Unable to create superuser with empty password");
		return response;
	}

	istd::TDelPtr<imtauth::CIdentifiableUserInfo> superuserInfoPtr;
	superuserInfoPtr.SetCastedOrRemove(m_userFactoryCompPtr.CreateInstance());
	if (!superuserInfoPtr.IsValid()){
		Q_ASSERT_X(false, "User instance is invalid", "CUserControllerComp");
		return response;
	}

	QByteArray login = "su";

	QString passwordHash = m_hashCalculatorCompPtr->GenerateHash(login + password.toUtf8());
	superuserInfoPtr->SetId(login);
	superuserInfoPtr->SetName(name);
	superuserInfoPtr->SetMail(mail);
	superuserInfoPtr->SetPasswordHash(passwordHash.toUtf8());
	imtauth::IUserInfo::SystemInfo systemInfo;
	superuserInfoPtr->AddToSystem(systemInfo);

	QByteArray objectId = m_userCollectionCompPtr->InsertNewObject("User", "", "", superuserInfoPtr.GetPtr(), login);
	if (objectId.isEmpty()){
		response.Message = QString("Unable to insert superuser to user collection");
		return response;
	}

	response.Success = true;

	return response;
}


sdl::imtauth::Users::CRemoveUserPayload::V1_0 CUserControllerComp::OnUsersRemove(
	const sdl::imtauth::Users::V1_0::CUsersRemoveGqlRequest& /*removeUserRequest*/,
	const ::imtgql::CGqlRequest& /*gqlRequest*/,
	QString& /*errorMessage*/) const
{
	return sdl::imtauth::Users::CRemoveUserPayload::V1_0();
}


// private methods

bool CUserControllerComp::SendUserCode(const QByteArray& userId, const imtauth::IUserInfo& userInfo) const
{
	if (!m_userVerificationControllerCompPtr.IsValid()){
		return false;
	}

	if (!m_userVerificationCodeSenderCompPtr.IsValid()){
		return false;
	}

	imtauth::CUserVerification userVerification;
	bool ok = m_userVerificationControllerCompPtr->GenerateVerificationData(userId, &userVerification);
	if (ok){
		if (!m_userVerificationCodeSenderCompPtr->SendCode(userInfo, userVerification.GetVerificationCode())){
			SendErrorMessage(0,
							 QString("Unable to send verification code to '%1' for user '%2'. Error: Unknown error")
								 .arg(userInfo.GetMail())
								 .arg(userInfo.GetName()),
							 "CUserControllerComp");

			return false;
		}
	}

	return ok;
}


const imtauth::IUserInfo* CUserControllerComp::GetUserInfoByLogin(const QByteArray& login) const
{
	QByteArray userObjectId = GetUserIdByLogin(login);
	if (userObjectId.isEmpty()){
		return nullptr;
	}

	istd::TDelPtr<const imtauth::IUserInfo> userInfoPtr;
	imtbase::IObjectCollection::DataPtr dataPtr;
	if (m_userCollectionCompPtr->GetObjectData(userObjectId, dataPtr)){
		if (dataPtr.IsValid()){
			userInfoPtr.SetCastedOrRemove(dataPtr->CloneMe());
		}
	}

	return userInfoPtr.PopPtr();
}


const QByteArray CUserControllerComp::GetUserIdByLogin(const QByteArray& login) const
{
	iprm::CParamsSet filterParam;
	iprm::CParamsSet paramsSet;

	iprm::CTextParam userIdParam;
	userIdParam.SetText(login);

	paramsSet.SetEditableParameter("Id", &userIdParam);
	filterParam.SetEditableParameter("ObjectFilter", &paramsSet);

	QByteArray userObjectId;
	imtbase::IObjectCollection::Ids userIds = m_userCollectionCompPtr->GetElementIds(0, -1, &filterParam);
	if (!userIds.isEmpty()){
		userObjectId = userIds[0];
	}

	return userObjectId;
}


const QByteArray CUserControllerComp::GetUserIdByEmail(const QString& email) const
{
	iprm::CParamsSet filterParam;
	iprm::CParamsSet paramsSet;

	iprm::CTextParam emailParam;
	emailParam.SetText(email);

	paramsSet.SetEditableParameter("Mail", &emailParam);
	filterParam.SetEditableParameter("ObjectFilter", &paramsSet);

	QByteArray userObjectId;
	imtbase::IObjectCollection::Ids userIds = m_userCollectionCompPtr->GetElementIds(0, -1, &filterParam);
	if (!userIds.isEmpty()){
		userObjectId = userIds[0];
	}

	return userObjectId;
}


} // namespace imtauthgql
