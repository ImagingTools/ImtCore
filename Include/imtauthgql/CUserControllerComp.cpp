// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauthgql/CUserControllerComp.h>


// ACF includes
#include <iprm/CParamsSet.h>

// ImtCore includes
#include <imtauth/CUserVerification.h>


namespace imtauthgql
{


// reimplemented (sdl::imtauth::Users::CGraphQlHandlerCompBase)

sdl::imtauth::Users::CChangePasswordPayload CUserControllerComp::OnChangePassword(
			const sdl::imtauth::Users::CChangePasswordGqlRequest& changePasswordRequest,
			const imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	sdl::imtauth::Users::CChangePasswordPayload::V1_0 payload;
	payload.success = false;

	if (!m_userCollectionCompPtr.IsValid()){
		Q_ASSERT_X(false, "Attribute 'UserCollection' was not set", "CUserControllerComp");
		return sdl::imtauth::Users::CChangePasswordPayload();
	}

	if (!m_hashCalculatorCompPtr.IsValid()){
		Q_ASSERT_X(false, "Attribute 'HashCalculator' was not set", "CUserControllerComp");
		return sdl::imtauth::Users::CChangePasswordPayload();
	}

	sdl::imtauth::Users::ChangePasswordRequestArguments arguments = changePasswordRequest.GetRequestedArguments();
	if (!arguments.input.Version_1_0.has_value()){
		Q_ASSERT(false);
		return sdl::imtauth::Users::CChangePasswordPayload();
	}

	sdl::imtauth::Users::CChangePasswordInput::V1_0 inputArgument = *arguments.input.Version_1_0;
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
		return sdl::imtauth::Users::CChangePasswordPayload();
	}

	if (userInfoPtr == nullptr){
		errorMessage = QString("Unable to change password for user '%1'. Error: The user does not exist").arg(qPrintable(login));
		SendErrorMessage(0, errorMessage, "CUserControllerComp");

		return sdl::imtauth::Users::CChangePasswordPayload();
	}

	imtauth::IUserInfo::SystemInfoList systemInfoList = userInfoPtr->GetSystemInfos();
	for (const imtauth::IUserInfo::SystemInfo& systemInfo : systemInfoList){
		if (systemInfo.enabled && !systemInfo.systemId.isEmpty()){
			errorMessage = QString("Unable to change password for user '%1'. Error: A user from an external system").arg(qPrintable(login));
			SendErrorMessage(0, errorMessage, "CUserControllerComp");

			return sdl::imtauth::Users::CChangePasswordPayload();
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

		return sdl::imtauth::Users::CChangePasswordPayload();
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

		return sdl::imtauth::Users::CChangePasswordPayload();
	}

	payload.success = true;

	sdl::imtauth::Users::CChangePasswordPayload retVal;
	retVal.Version_1_0 = std::move(payload);

	return retVal;
}


sdl::imtauth::Users::CRegisterUserPayload CUserControllerComp::OnRegisterUser(
			const sdl::imtauth::Users::CRegisterUserGqlRequest& registerUserRequest,
			const ::imtgql::CGqlRequest& /*gqlRequest*/,
			QString& errorMessage) const
{
	sdl::imtauth::Users::CRegisterUserPayload::V1_0 response;
	if (!m_userCollectionCompPtr.IsValid()){
		Q_ASSERT_X(false, "Attribute 'UserCollection' was not set", "CUserControllerComp");
		return sdl::imtauth::Users::CRegisterUserPayload();
	}

	if (!m_userFactoryCompPtr.IsValid()){
		Q_ASSERT_X(false, "Attribute 'UserFactory' was not set", "CUserControllerComp");
		return sdl::imtauth::Users::CRegisterUserPayload();
	}

	if (!m_hashCalculatorCompPtr.IsValid()){
		Q_ASSERT_X(false, "Attribute 'HashCalculator' was not set", "CUserControllerComp");
		return sdl::imtauth::Users::CRegisterUserPayload();
	}

	sdl::imtauth::Users::RegisterUserRequestArguments arguments = registerUserRequest.GetRequestedArguments();
	if (!arguments.input.Version_1_0.has_value()){
		Q_ASSERT(false);
		return sdl::imtauth::Users::CRegisterUserPayload();
	}

	if (!arguments.input.Version_1_0->userData){
		errorMessage = QString("Unable to register user. Error: User data is invalid");
		return sdl::imtauth::Users::CRegisterUserPayload();
	}

	QByteArray productId;
	if (arguments.input.Version_1_0->productId){
		productId = *arguments.input.Version_1_0->productId;
	}

	imtauth::IUserInfoUniquePtr userInfoPtr = m_userFactoryCompPtr.CreateInstance();
	if (!userInfoPtr.IsValid()){
		Q_ASSERT_X(false, "User instance is invalid", "CUserControllerComp");

		return sdl::imtauth::Users::CRegisterUserPayload();
	}

	auto userIdentifierPtr = dynamic_cast<imtbase::IIdentifiable*>(userInfoPtr.GetPtr());
	if (userIdentifierPtr == nullptr){
		Q_ASSERT_X(false, "User instance is invalid", "CUserControllerComp");

		return sdl::imtauth::Users::CRegisterUserPayload();
	}

	sdl::imtauth::Users::CUserData::V1_0 userData = *arguments.input.Version_1_0->userData;

	QByteArray userId;
	if (arguments.input.Version_1_0->userData->id){
		userId = *arguments.input.Version_1_0->userData->id;
	}

	userIdentifierPtr->SetObjectUuid(userId);

	imtbase::ICollectionInfo::Ids elementIds = m_userCollectionCompPtr->GetElementIds();
	if (elementIds.contains(userId)){
		errorMessage = QString("Unable to register user. Error: User with ID: '%1' already exists")
					.arg(QString::fromUtf8(userId)).arg(errorMessage);

		return sdl::imtauth::Users::CRegisterUserPayload();
	}

	if (!m_userRepresentationController.FillUserInfoFromRepresentation(userData, *userInfoPtr, m_userCollectionCompPtr.GetPtr(), userId, errorMessage)){
		errorMessage = QString("Unable to register user. Error: %1").arg(errorMessage);

		return sdl::imtauth::Users::CRegisterUserPayload();
	}

	imtauth::IUserInfo::SystemInfo systemInfo;
	userInfoPtr->AddToSystem(systemInfo);

	QString password;
	if (userData.password){
		password = *userData.password;
	}

	if (password.isEmpty()){
		errorMessage = QString("Unable to register user. Error: password cannot be empty");

		return sdl::imtauth::Users::CRegisterUserPayload();
	}

	if (!userData.username){
		errorMessage = QString("Unable to register user. Error: Invalid username");

		return sdl::imtauth::Users::CRegisterUserPayload();
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

	QByteArray objectId = m_userCollectionCompPtr->InsertNewObject("User", "", "", userInfoPtr.GetPtr(), userId);
	if (objectId.isEmpty()){
		errorMessage = QString("Unable to register user. Error: Insert object to collection failed");
		SendWarningMessage(0, errorMessage, "CUserControllerComp");

		return sdl::imtauth::Users::CRegisterUserPayload();
	}

	sdl::imtauth::Users::CRegisterUserPayload retVal;
	retVal.Version_1_0 = response;

	return retVal;
}


sdl::imtauth::Users::CCheckEmailPayload CUserControllerComp::OnCheckEmail(
			const sdl::imtauth::Users::CCheckEmailGqlRequest& checkEmailRequest,
			const ::imtgql::CGqlRequest& /*gqlRequest*/,
			QString& /*errorMessage*/) const
{
	sdl::imtauth::Users::CCheckEmailPayload retVal;
	retVal.Version_1_0 = sdl::imtauth::Users::CCheckEmailPayload::V1_0();

	sdl::imtauth::Users::CCheckEmailPayload::V1_0& response = *retVal.Version_1_0;
	if (!m_userCollectionCompPtr.IsValid()){
		Q_ASSERT_X(false, "Attribute 'UserCollection' was not set", "CUserControllerComp");

		return sdl::imtauth::Users::CCheckEmailPayload();
	}

	if (!m_userVerificationControllerCompPtr.IsValid()){
		Q_ASSERT_X(false, "Attribute 'UserVerificationController' was not set", "CUserControllerComp");

		return sdl::imtauth::Users::CCheckEmailPayload();
	}

	response.success = false;

	sdl::imtauth::Users::CheckEmailRequestArguments arguments = checkEmailRequest.GetRequestedArguments();
	
	if (!arguments.input.Version_1_0.has_value()){
		Q_ASSERT(false);
		return sdl::imtauth::Users::CCheckEmailPayload();
	}

	QString email;
	if (arguments.input.Version_1_0->email){
		email = *arguments.input.Version_1_0->email;
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


sdl::imtauth::Users::CSendEmailCodePayload CUserControllerComp::OnSendEmailCode(
			const sdl::imtauth::Users::CSendEmailCodeGqlRequest& sendEmailCodeRequest,
			const ::imtgql::CGqlRequest& /*gqlRequest*/,
			QString& /*errorMessage*/) const
{
	sdl::imtauth::Users::CSendEmailCodePayload retVal;
	retVal.Version_1_0 = sdl::imtauth::Users::CSendEmailCodePayload::V1_0();
	sdl::imtauth::Users::CSendEmailCodePayload::V1_0& response = *retVal.Version_1_0;

	if (!m_userVerificationControllerCompPtr.IsValid()){
		Q_ASSERT_X(false, "Attribute 'UserVerificationController' was not set", "CUserControllerComp");
		return retVal;
	}

	sdl::imtauth::Users::SendEmailCodeRequestArguments arguments = sendEmailCodeRequest.GetRequestedArguments();
	if (!arguments.input.Version_1_0.has_value()){
		Q_ASSERT(false);
		return retVal;
	}

	QByteArray login;
	if (arguments.input.Version_1_0->login){
		login = *arguments.input.Version_1_0->login;
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


sdl::imtauth::Users::CCheckEmailCodePayload CUserControllerComp::OnCheckEmailCode(
			const sdl::imtauth::Users::CCheckEmailCodeGqlRequest& checkEmailCodeRequest,
			const ::imtgql::CGqlRequest& /*gqlRequest*/,
			QString& /*errorMessage*/) const
{
	sdl::imtauth::Users::CCheckEmailCodePayload::V1_0 response;

	if (!m_userVerificationControllerCompPtr.IsValid()){
		Q_ASSERT_X(false, "Attribute 'UserVerificationController' was not set", "CUserControllerComp");

		return sdl::imtauth::Users::CCheckEmailCodePayload();
	}

	sdl::imtauth::Users::CheckEmailCodeRequestArguments arguments = checkEmailCodeRequest.GetRequestedArguments();
	
	if (!arguments.input.Version_1_0.has_value()){
		Q_ASSERT(false);
		return sdl::imtauth::Users::CCheckEmailCodePayload();
	}
	
	QByteArray login;
	if (arguments.input.Version_1_0->login){
		login = *arguments.input.Version_1_0->login;
	}

	QByteArray userId = GetUserIdByLogin(login);

	QString code;
	if (arguments.input.Version_1_0->code){
		code = *arguments.input.Version_1_0->code;
	}

	response.correctCode = m_userVerificationControllerCompPtr->VerifyUser(userId, code.toUtf8());

	sdl::imtauth::Users::CCheckEmailCodePayload retVal;
	retVal.Version_1_0 = std::move(response);

	return retVal;
}


sdl::imtauth::Users::CCheckSuperuserPayload CUserControllerComp::OnCheckSuperuserExists(
			const sdl::imtauth::Users::CCheckSuperuserExistsGqlRequest& /*checkSuperuserExistsRequest*/,
			const ::imtgql::CGqlRequest& /*gqlRequest*/,
			QString& /*errorMessage*/) const
{
	sdl::imtauth::Users::CCheckSuperuserPayload retVal;
	retVal.Version_1_0 = sdl::imtauth::Users::CCheckSuperuserPayload::V1_0();
	sdl::imtauth::Users::CCheckSuperuserPayload::V1_0& response = *retVal.Version_1_0;

	if (!m_userCollectionCompPtr.IsValid()){
		Q_ASSERT_X(false, "Attribute 'UserCollection' was not set", "CUserControllerComp");

		return retVal;
	}

	response.message = "";

	if (m_databaseConnectionCheckerCompPtr.IsValid()){
		QString connectionMessage;
		bool ok = m_databaseConnectionCheckerCompPtr->CheckDatabaseConnection(connectionMessage);
		if (!ok){
			response.status = sdl::imtauth::Users::ExistsStatus::UNKNOWN;
			response.message = connectionMessage;

			return retVal;
		}
	}

	imtauth::IUserInfoSharedPtr userInfoPtr = GetUserInfoByLogin("su");
	if (!userInfoPtr.IsValid()){
		response.status = sdl::imtauth::Users::ExistsStatus::NOT_EXISTS;
		return retVal;
	}

	response.status = sdl::imtauth::Users::ExistsStatus::EXISTS;

	return retVal;
}


sdl::imtauth::Users::CCreateSuperuserPayload CUserControllerComp::OnCreateSuperuser(
			const sdl::imtauth::Users::CCreateSuperuserGqlRequest& createSuperuserRequest,
			const ::imtgql::CGqlRequest& /*gqlRequest*/,
			QString& /*errorMessage*/) const
{
	sdl::imtauth::Users::CCreateSuperuserPayload retVal;
	retVal.Version_1_0 = sdl::imtauth::Users::CCreateSuperuserPayload::V1_0();
	sdl::imtauth::Users::CCreateSuperuserPayload::V1_0& response = *retVal.Version_1_0;

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

	sdl::imtauth::Users::CreateSuperuserRequestArguments arguments = createSuperuserRequest.GetRequestedArguments();

	if (!arguments.input.Version_1_0.has_value()){
		Q_ASSERT(false);
		return retVal;
	}

	QString name;
	if (arguments.input.Version_1_0->name){
		name = *arguments.input.Version_1_0->name;
	}

	if (name.isEmpty()){
		name = "superuser";
	}

	QString mail;
	if (arguments.input.Version_1_0->mail){
		mail = *arguments.input.Version_1_0->mail;
	}

	if (mail.isEmpty()){
		response.message = QString("Unable to create superuser with empty email");
		return retVal;
	}

	QString password;
	if (arguments.input.Version_1_0->password){
		password = *arguments.input.Version_1_0->password;
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

	QByteArray result = m_userCollectionCompPtr->InsertNewObject("User", "", "", superuserInfoPtr.GetPtr(), objectId);
	if (result.isEmpty()){
		response.message = QString("Unable to insert superuser to user collection");
		return retVal;
	}

	response.success = true;

	return retVal;
}


sdl::imtauth::Users::CUserObjectId CUserControllerComp::OnGetUserObjectId(
			const sdl::imtauth::Users::CGetUserObjectIdGqlRequest& getUserObjectIdRequest,
			const ::imtgql::CGqlRequest& /*gqlRequest*/,
			QString& errorMessage) const
{
	sdl::imtauth::Users::CUserObjectId response;

	sdl::imtauth::Users::GetUserObjectIdRequestArguments arguments = getUserObjectIdRequest.GetRequestedArguments();
	if (!arguments.input.Version_1_0.HasValue()){
		errorMessage = QString("Unable to get user object-ID. Error: GraphQL version is invalid");
		return response;
	}

	if (!arguments.input.Version_1_0->login.HasValue()){
		errorMessage = QString("Unable to get user object-ID. Error: Login field is invalid");
		return response;
	}

	QByteArray login = *arguments.input.Version_1_0->login;
	QByteArray objectId = GetUserIdByLogin(login);

	response.Version_1_0.Emplace();
	response.Version_1_0->objectId = objectId;

	return response;
}


// reimplemented (imtservergql::CPermissibleGqlRequestHandlerComp)

bool CUserControllerComp::CheckPermissions(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const
{
	QByteArray commandId = gqlRequest.GetCommandId();
	if (commandId == sdl::imtauth::Users::CCheckSuperuserExistsGqlRequest::GetCommandId() ||
		commandId == sdl::imtauth::Users::CCreateSuperuserGqlRequest::GetCommandId() ||
		commandId == sdl::imtauth::Users::CRegisterUserGqlRequest::GetCommandId()){
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
