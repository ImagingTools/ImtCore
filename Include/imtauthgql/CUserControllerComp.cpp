// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauthgql/CUserControllerComp.h>
#include <GeneratedFiles/imtauthsdl/SDL/1.0/CPP/Users.h>


// ACF includes
#include <iprm/CParamsSet.h>

// ImtCore includes
#include <imtauth/CUserVerification.h>


namespace imtauthgql
{


// reimplemented (sdl::V1_0::imtauth::CUsersGqlHandlerCompBase)

sdl::V1_0::imtauth::CChangePasswordPayload CUserControllerComp::OnChangePassword(
			const sdl::V1_0::imtauth::CChangePasswordGqlRequest& changePasswordRequest,
			const imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	sdl::V1_0::imtauth::CChangePasswordPayload payload;
	payload.success = false;

	if (!m_userCollectionCompPtr.IsValid()){
		Q_ASSERT_X(false, "Attribute 'UserCollection' was not set", "CUserControllerComp");
		return sdl::V1_0::imtauth::CChangePasswordPayload();
	}

	if (!m_hashCalculatorCompPtr.IsValid()){
		Q_ASSERT_X(false, "Attribute 'HashCalculator' was not set", "CUserControllerComp");
		return sdl::V1_0::imtauth::CChangePasswordPayload();
	}

	sdl::V1_0::imtauth::ChangePasswordRequestArguments arguments = changePasswordRequest.GetRequestedArguments();
	sdl::V1_0::imtauth::CChangePasswordInput inputArgument = *arguments.input;
	QByteArray login;
	if (inputArgument.login){
		login = *inputArgument.login;
	}

	QString oldPassword;
	if (inputArgument.oldPassword){
		oldPassword = *inputArgument.oldPassword;
	}

	QString newPassword;
	if (inputArgument.newPassword){
		newPassword = *inputArgument.newPassword;
	}

	QByteArray userId = GetUserIdByLogin(login);

	imtauth::IUserInfo* userInfoPtr = nullptr;
	imtbase::IObjectCollection::DataPtr dataPtr;
	if (m_userCollectionCompPtr->GetObjectData(userId, dataPtr)){
		userInfoPtr = dynamic_cast<imtauth::IUserInfo*>(dataPtr.GetPtr());
	}

	if (userInfoPtr == nullptr){
		errorMessage = QString("Unable to change password for user '%1'. Error: The user does not exist").arg(qPrintable(login));
		return sdl::V1_0::imtauth::CChangePasswordPayload();
	}

	if (userInfoPtr == nullptr){
		errorMessage = QString("Unable to change password for user '%1'. Error: The user does not exist").arg(qPrintable(login));
		SendErrorMessage(0, errorMessage, "CUserControllerComp");

		return sdl::V1_0::imtauth::CChangePasswordPayload();
	}

	imtauth::IUserInfo::SystemInfoList systemInfoList = userInfoPtr->GetSystemInfos();
	for (const imtauth::IUserInfo::SystemInfo& systemInfo : systemInfoList){
		if (systemInfo.enabled && !systemInfo.systemId.isEmpty()){
			errorMessage = QString("Unable to change password for user '%1'. Error: A user from an external system").arg(qPrintable(login));
			SendErrorMessage(0, errorMessage, "CUserControllerComp");

			return sdl::V1_0::imtauth::CChangePasswordPayload();
		}
	}

	bool ok = false;

	const imtgql::IGqlContext* gqlContextPtr = gqlRequest.GetRequestContext();
	if (gqlContextPtr != nullptr){
		const imtauth::IUserInfo* contextUserInfoPtr = gqlContextPtr->GetUserInfo();
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

		return sdl::V1_0::imtauth::CChangePasswordPayload();
	}

	QByteArray passwordHash = m_hashCalculatorCompPtr->GenerateHash(login + newPassword.toUtf8());
	userInfoPtr->SetPasswordHash(passwordHash);

	istd::TDelPtr<imtbase::IOperationContext> operationContextPtr =  nullptr;
	if (m_userOperationContextControllerCompPtr.IsValid()){
		operationContextPtr.SetPtr(m_userOperationContextControllerCompPtr->CreateOperationContext("ChangePassword", userId, userInfoPtr));
	}

	if (!m_userCollectionCompPtr->SetObjectData(userId, *userInfoPtr, istd::IChangeable::CM_WITHOUT_REFS, operationContextPtr.GetPtr())){
		errorMessage = QString("Unable to change password for user '%1'").arg(qPrintable(login));
		SendErrorMessage(0, errorMessage, "CUserControllerComp");

		return sdl::V1_0::imtauth::CChangePasswordPayload();
	}

	payload.success = true;

	return payload;
}


sdl::V1_0::imtauth::CRegisterUserPayload CUserControllerComp::OnRegisterUser(
			const sdl::V1_0::imtauth::CRegisterUserGqlRequest& registerUserRequest,
			const ::imtgql::CGqlRequest& /*gqlRequest*/,
			QString& errorMessage) const
{
	sdl::V1_0::imtauth::CRegisterUserPayload response;
	if (!m_userCollectionCompPtr.IsValid()){
		Q_ASSERT_X(false, "Attribute 'UserCollection' was not set", "CUserControllerComp");
		return sdl::V1_0::imtauth::CRegisterUserPayload();
	}

	if (!m_userFactoryCompPtr.IsValid()){
		Q_ASSERT_X(false, "Attribute 'UserFactory' was not set", "CUserControllerComp");
		return sdl::V1_0::imtauth::CRegisterUserPayload();
	}

	if (!m_hashCalculatorCompPtr.IsValid()){
		Q_ASSERT_X(false, "Attribute 'HashCalculator' was not set", "CUserControllerComp");
		return sdl::V1_0::imtauth::CRegisterUserPayload();
	}

	sdl::V1_0::imtauth::RegisterUserRequestArguments arguments = registerUserRequest.GetRequestedArguments();
	if (!arguments.input.userData){
		errorMessage = QString("Unable to register user. Error: User data is invalid");
		return sdl::V1_0::imtauth::CRegisterUserPayload();
	}

	QByteArray productId;
	if (arguments.input.productId){
		productId = *arguments.input.productId;
	}

	imtauth::IUserInfoUniquePtr userInfoPtr = m_userFactoryCompPtr.CreateInstance();
	if (!userInfoPtr.IsValid()){
		Q_ASSERT_X(false, "User instance is invalid", "CUserControllerComp");

		return sdl::V1_0::imtauth::CRegisterUserPayload();
	}

	auto userIdentifierPtr = dynamic_cast<imtbase::IIdentifiable*>(userInfoPtr.GetPtr());
	if (userIdentifierPtr == nullptr){
		Q_ASSERT_X(false, "User instance is invalid", "CUserControllerComp");

		return sdl::V1_0::imtauth::CRegisterUserPayload();
	}

	sdl::V1_0::imtauth::CUserData userData = *arguments.input.userData;

	QByteArray userId;
	if (arguments.input.userData->id){
		userId = *arguments.input.userData->id;
	}

	userIdentifierPtr->SetObjectUuid(userId);

	imtbase::ICollectionInfo::Ids elementIds = m_userCollectionCompPtr->GetElementIds();
	if (elementIds.contains(userId)){
		errorMessage = QString("Unable to register user. Error: User with ID: '%1' already exists")
					.arg(QString::fromUtf8(userId)).arg(errorMessage);

		return sdl::V1_0::imtauth::CRegisterUserPayload();
	}

	if (!m_userRepresentationController.FillUserInfoFromRepresentation(userData, *userInfoPtr, m_userCollectionCompPtr.GetPtr(), userId, errorMessage)){
		errorMessage = QString("Unable to register user. Error: %1").arg(errorMessage);

		return sdl::V1_0::imtauth::CRegisterUserPayload();
	}

	imtauth::IUserInfo::SystemInfo systemInfo;
	userInfoPtr->AddToSystem(systemInfo);

	QString password;
	if (userData.password){
		password = *userData.password;
	}

	if (password.isEmpty()){
		errorMessage = QString("Unable to register user. Error: password cannot be empty");

		return sdl::V1_0::imtauth::CRegisterUserPayload();
	}

	if (!userData.username){
		errorMessage = QString("Unable to register user. Error: Invalid username");

		return sdl::V1_0::imtauth::CRegisterUserPayload();
	}

	password = m_hashCalculatorCompPtr->GenerateHash(*userData.username + password.toUtf8());

	userInfoPtr->SetPasswordHash(password.toUtf8());

	response.id = userId;

	if (m_roleCollectionCompPtr.IsValid()){
		imtbase::IObjectCollection::Ids roleIds = m_roleCollectionCompPtr->GetElementIds();
		for (const QByteArray& roleId : roleIds){
			imtbase::IObjectCollection::DataPtr roleDataPtr;
			if (m_roleCollectionCompPtr->GetObjectData(roleId, roleDataPtr)){
				auto roleInfoPtr = dynamic_cast<const imtauth::IRole*>(roleDataPtr.GetPtr());
				if (roleInfoPtr != nullptr){
					if (roleInfoPtr->GetProductId() == productId && roleInfoPtr->IsDefault()){
						userInfoPtr->AddRole(productId, roleId);
						break;
					}
				}
			}
		}
	}

	QByteArray objectId = m_userCollectionCompPtr->InsertNewObject("User", userInfoPtr->GetName(), "", userInfoPtr.GetPtr(), userId);
	if (objectId.isEmpty()){
		errorMessage = QString("Unable to register user. Error: Insert object to collection failed");
		SendWarningMessage(0, errorMessage, "CUserControllerComp");

		return sdl::V1_0::imtauth::CRegisterUserPayload();
	}

	return response;
}


sdl::V1_0::imtauth::CCheckEmailPayload CUserControllerComp::OnCheckEmail(
			const sdl::V1_0::imtauth::CCheckEmailGqlRequest& checkEmailRequest,
			const ::imtgql::CGqlRequest& /*gqlRequest*/,
			QString& /*errorMessage*/) const
{
	sdl::V1_0::imtauth::CCheckEmailPayload retVal;

	sdl::V1_0::imtauth::CCheckEmailPayload& response = retVal;
	if (!m_userCollectionCompPtr.IsValid()){
		Q_ASSERT_X(false, "Attribute 'UserCollection' was not set", "CUserControllerComp");

		return sdl::V1_0::imtauth::CCheckEmailPayload();
	}

	if (!m_userVerificationControllerCompPtr.IsValid()){
		Q_ASSERT_X(false, "Attribute 'UserVerificationController' was not set", "CUserControllerComp");

		return sdl::V1_0::imtauth::CCheckEmailPayload();
	}

	response.success = false;

	sdl::V1_0::imtauth::CheckEmailRequestArguments arguments = checkEmailRequest.GetRequestedArguments();
	
	QString email;
	if (arguments.input.email){
		email = *arguments.input.email;
	}

	if (email.isEmpty()){
		response.message = QString("Unable to check email. Error: email is empty");

		return retVal;
	}

	QByteArray userId = GetUserIdByEmail(email);

	const imtauth::IUserInfo* userInfoPtr = nullptr;
	imtbase::IObjectCollection::DataPtr dataPtr;
	if (m_userCollectionCompPtr->GetObjectData(userId, dataPtr)){
		userInfoPtr = dynamic_cast<const imtauth::IUserInfo*>(dataPtr.GetPtr());
	}

	if (userInfoPtr == nullptr){
		response.message = QString("There are no users with email '%1' in the system").arg(email);
		return retVal;
	}

	response.success = true;
	response.userName = userInfoPtr->GetName();
	response.login = userInfoPtr->GetId();

	return retVal;
}


sdl::V1_0::imtauth::CSendEmailCodePayload CUserControllerComp::OnSendEmailCode(
			const sdl::V1_0::imtauth::CSendEmailCodeGqlRequest& sendEmailCodeRequest,
			const ::imtgql::CGqlRequest& /*gqlRequest*/,
			QString& /*errorMessage*/) const
{
	sdl::V1_0::imtauth::CSendEmailCodePayload retVal;
	sdl::V1_0::imtauth::CSendEmailCodePayload& response = retVal;

	if (!m_userVerificationControllerCompPtr.IsValid()){
		Q_ASSERT_X(false, "Attribute 'UserVerificationController' was not set", "CUserControllerComp");
		return retVal;
	}

	sdl::V1_0::imtauth::SendEmailCodeRequestArguments arguments = sendEmailCodeRequest.GetRequestedArguments();
	QByteArray login;
	if (arguments.input.login){
		login = *arguments.input.login;
	}

	response.login = login;
	response.success = false;

	QByteArray objectId = GetUserIdByLogin(login);
	imtauth::IUserInfoSharedPtr userInfoPtr = GetUserInfoByLogin(login);
	if (!userInfoPtr.IsValid()){
		response.message = QString("Unable to send email code. Error: User is invalid");
	}
	else{
		bool ok = SendUserCode(objectId, *userInfoPtr.GetPtr());
		if (ok){
			response.success = ok;
		}
		else{
			response.message = QString("Unable to send email code. Error: Unknown error");
		}
	}

	return retVal;
}


sdl::V1_0::imtauth::CCheckEmailCodePayload CUserControllerComp::OnCheckEmailCode(
			const sdl::V1_0::imtauth::CCheckEmailCodeGqlRequest& checkEmailCodeRequest,
			const ::imtgql::CGqlRequest& /*gqlRequest*/,
			QString& /*errorMessage*/) const
{
	sdl::V1_0::imtauth::CCheckEmailCodePayload response;

	if (!m_userVerificationControllerCompPtr.IsValid()){
		Q_ASSERT_X(false, "Attribute 'UserVerificationController' was not set", "CUserControllerComp");

		return sdl::V1_0::imtauth::CCheckEmailCodePayload();
	}

	sdl::V1_0::imtauth::CheckEmailCodeRequestArguments arguments = checkEmailCodeRequest.GetRequestedArguments();
	
	QByteArray login;
	if (arguments.input.login){
		login = *arguments.input.login;
	}

	QByteArray userId = GetUserIdByLogin(login);

	QString code;
	if (arguments.input.code){
		code = *arguments.input.code;
	}

	response.correctCode = m_userVerificationControllerCompPtr->VerifyUser(userId, code.toUtf8());

	return response;
}


sdl::V1_0::imtauth::CCheckSuperuserPayload CUserControllerComp::OnCheckSuperuserExists(
			const sdl::V1_0::imtauth::CCheckSuperuserExistsGqlRequest& /*checkSuperuserExistsRequest*/,
			const ::imtgql::CGqlRequest& /*gqlRequest*/,
			QString& /*errorMessage*/) const
{
	sdl::V1_0::imtauth::CCheckSuperuserPayload retVal;
	sdl::V1_0::imtauth::CCheckSuperuserPayload& response = retVal;

	if (!m_userCollectionCompPtr.IsValid()){
		Q_ASSERT_X(false, "Attribute 'UserCollection' was not set", "CUserControllerComp");

		return retVal;
	}

	response.message = "";

	if (m_databaseConnectionCheckerCompPtr.IsValid()){
		QString connectionMessage;
		bool ok = m_databaseConnectionCheckerCompPtr->CheckDatabaseConnection(connectionMessage);
		if (!ok){
			response.status = sdl::V1_0::imtauth::ExistsStatus::UNKNOWN;
			response.message = connectionMessage;

			return retVal;
		}
	}

	imtauth::IUserInfoSharedPtr userInfoPtr = GetUserInfoByLogin("su");
	if (!userInfoPtr.IsValid()){
		response.status = sdl::V1_0::imtauth::ExistsStatus::NOT_EXISTS;
		return retVal;
	}

	response.status = sdl::V1_0::imtauth::ExistsStatus::EXISTS;

	return retVal;
}


sdl::V1_0::imtauth::CCreateSuperuserPayload CUserControllerComp::OnCreateSuperuser(
			const sdl::V1_0::imtauth::CCreateSuperuserGqlRequest& createSuperuserRequest,
			const ::imtgql::CGqlRequest& /*gqlRequest*/,
			QString& /*errorMessage*/) const
{
	sdl::V1_0::imtauth::CCreateSuperuserPayload retVal;
	sdl::V1_0::imtauth::CCreateSuperuserPayload& response = retVal;

	if (!m_userCollectionCompPtr.IsValid()){
		Q_ASSERT_X(false, "Attribute 'UserCollection' was not set", "CUserControllerComp");

		return retVal;
	}

	if (!m_hashCalculatorCompPtr.IsValid()){
		Q_ASSERT_X(false, "Attribute 'HashCalculator' was not set", "CUserControllerComp");

		return retVal;
	}

	response.success = false;
	response.message = "";

	imtauth::IUserInfoSharedPtr userInfoPtr = GetUserInfoByLogin("su");
	if (userInfoPtr.IsValid()){
		response.message = QString("Superuser already exists");

		return retVal;
	}

	sdl::V1_0::imtauth::CreateSuperuserRequestArguments arguments = createSuperuserRequest.GetRequestedArguments();

	QString name;
	if (arguments.input.name){
		name = *arguments.input.name;
	}

	if (name.isEmpty()){
		name = "superuser";
	}

	QString mail;
	if (arguments.input.mail){
		mail = *arguments.input.mail;
	}

	if (mail.isEmpty()){
		response.message = QString("Unable to create superuser with empty email");
		return retVal;
	}

	QString password;
	if (arguments.input.password){
		password = *arguments.input.password;
	}

	if (password.isEmpty()){
		response.message = QString("Unable to create superuser with empty password");
		return retVal;
	}

	imtauth::IUserInfoUniquePtr superuserInfoPtr = m_userFactoryCompPtr.CreateInstance();
	if (!superuserInfoPtr.IsValid()){
		Q_ASSERT_X(false, "User instance is invalid", "CUserControllerComp");
		return retVal;
	}
	
	if (!superuserInfoPtr.IsValid()){
		Q_ASSERT_X(false, "User instance is invalid", "CUserControllerComp");
		return retVal;
	}

	auto userIdentifierPtr = dynamic_cast<imtbase::IIdentifiable*>(superuserInfoPtr.GetPtr());
	if (userIdentifierPtr == nullptr){
		Q_ASSERT_X(false, "User instance is invalid", "CUserControllerComp");

		return retVal;
	}

	QByteArray objectId = QUuid::createUuid().toByteArray(QUuid::WithoutBraces);
	QByteArray login = "su";

	QString passwordHash = m_hashCalculatorCompPtr->GenerateHash(login + password.toUtf8());

	userIdentifierPtr->SetObjectUuid(objectId);
	superuserInfoPtr->SetId(login);
	superuserInfoPtr->SetName(name);
	superuserInfoPtr->SetMail(mail);
	superuserInfoPtr->SetPasswordHash(passwordHash.toUtf8());
	imtauth::IUserInfo::SystemInfo systemInfo;
	superuserInfoPtr->AddToSystem(systemInfo);

	QByteArray result = m_userCollectionCompPtr->InsertNewObject("User", "superuser", "System administrator", superuserInfoPtr.GetPtr(), objectId);
	if (result.isEmpty()){
		response.message = QString("Unable to insert superuser to user collection");
		return retVal;
	}

	response.success = true;

	return retVal;
}


sdl::V1_0::imtauth::CUserObjectId CUserControllerComp::OnGetUserObjectId(
			const sdl::V1_0::imtauth::CGetUserObjectIdGqlRequest& getUserObjectIdRequest,
			const ::imtgql::CGqlRequest& /*gqlRequest*/,
			QString& errorMessage) const
{
	sdl::V1_0::imtauth::CUserObjectId response;

	sdl::V1_0::imtauth::GetUserObjectIdRequestArguments arguments = getUserObjectIdRequest.GetRequestedArguments();
	if (!arguments.input.HasValue()){
		errorMessage = QString("Unable to get user object-ID. Error: GraphQL version is invalid");
		return response;
	}

	if (!arguments.input.login.HasValue()){
		errorMessage = QString("Unable to get user object-ID. Error: Login field is invalid");
		return response;
	}

	QByteArray login = *arguments.input.login;
	QByteArray objectId = GetUserIdByLogin(login);

	response.objectId = objectId;

	return response;
}


// reimplemented (imtservergql::CPermissibleGqlRequestHandlerComp)

bool CUserControllerComp::CheckPermissions(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const
{
	QByteArray commandId = gqlRequest.GetCommandId();
	if (commandId == sdl::V1_0::imtauth::CCheckSuperuserExistsGqlRequest::GetCommandId() ||
		commandId == sdl::V1_0::imtauth::CCreateSuperuserGqlRequest::GetCommandId() ||
		commandId == sdl::V1_0::imtauth::CRegisterUserGqlRequest::GetCommandId()){
		return true;
	}

	return BaseClass::CheckPermissions(gqlRequest, errorMessage);
}


// private methods

bool CUserControllerComp::SendUserCode(const QByteArray& userId, const imtauth::IUserInfo& userInfo) const
{
	if (!m_userVerificationControllerCompPtr.IsValid()){
		Q_ASSERT_X(false, "Attribute 'UserVerificationController' was not set", "CUserControllerComp");
		return false;
	}

	if (!m_userVerificationCodeSenderCompPtr.IsValid()){
		Q_ASSERT_X(false, "Attribute 'UserVerificationCodeSender' was not set", "CUserControllerComp");
		return false;
	}

	imtauth::CUserVerification userVerification;
	bool ok = m_userVerificationControllerCompPtr->GenerateVerificationData(userId, &userVerification);
	if (ok){
		if (!m_userVerificationCodeSenderCompPtr->SendCode(userInfo, userVerification.GetVerificationCode())){
			SendErrorMessage(0,
							QString("Unable to send verification code to '%1' for user '%2'. Error: Unknown error")
								.arg(userInfo.GetMail(), userInfo.GetName()),
							"CUserControllerComp");

			return false;
		}
	}

	return ok;
}


imtauth::IUserInfoSharedPtr CUserControllerComp::GetUserInfoByLogin(const QByteArray& login) const
{
	QByteArray userObjectId = GetUserIdByLogin(login);
	if (userObjectId.isEmpty()){
		return nullptr;
	}

	imtauth::IUserInfoSharedPtr userInfoPtr;
	imtbase::IObjectCollection::DataPtr dataPtr;
	if (m_userCollectionCompPtr->GetObjectData(userObjectId, dataPtr)){
		if (dataPtr.IsValid()){
			userInfoPtr.SetCastedPtr<istd::IChangeable>(dataPtr);
		}
	}

	return userInfoPtr;
}


QByteArray CUserControllerComp::GetUserIdByLogin(const QByteArray& login) const
{
	imtbase::IComplexCollectionFilter::FieldFilter fieldFilter;
	fieldFilter.fieldId = "Id";
	fieldFilter.filterValue = login;
	
	imtbase::CComplexCollectionFilter complexFilter;
	complexFilter.AddFieldFilter(fieldFilter);
	
	iprm::CParamsSet filterParam;
	filterParam.SetEditableParameter("ComplexFilter", &complexFilter);

	imtbase::IObjectCollection::Ids userIds = m_userCollectionCompPtr->GetElementIds(0, -1, &filterParam);
	if (!userIds.isEmpty()){
		return userIds[0];
	}

	return QByteArray();
}


QByteArray CUserControllerComp::GetUserIdByEmail(const QString& email) const
{
	if (!m_userCollectionCompPtr.IsValid()){
		Q_ASSERT_X(false, "Attribute 'UserCollection' was not set", "CUserControllerComp");
		return QByteArray();
	}

	imtbase::IComplexCollectionFilter::FieldFilter fieldFilter;
	fieldFilter.fieldId = "Mail";
	fieldFilter.filterValue = email;

	imtbase::CComplexCollectionFilter complexFilter;
	complexFilter.AddFieldFilter(fieldFilter);
	
	iprm::CParamsSet filterParam;
	filterParam.SetEditableParameter("ComplexFilter", &complexFilter);

	imtbase::IObjectCollection::Ids userIds = m_userCollectionCompPtr->GetElementIds(0, -1, &filterParam);
	if (!userIds.isEmpty()){
		return userIds[0];
	}

	return QByteArray();
}


} // namespace imtauthgql
