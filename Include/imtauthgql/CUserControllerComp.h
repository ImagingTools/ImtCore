// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtbase/IObjectCollection.h>
#include <imtauth/ICredentialController.h>
#include <imtauth/IPasswordPolicy.h>
#include <imtauth/IUserVerificationController.h>
#include <imtauthgql/CUserRepresentationController.h>
#include <imtcrypt/IHashGenerator.h>
#include <imtauth/IUserVerificationCodeSender.h>
#include <imtdb/IDatabaseServerConnectionChecker.h>
#include <GeneratedFiles/imtauthsdl/SDL/1.0/CPP/Users_fwd.h>


namespace imtauthgql
{


class CUserControllerComp: public sdl::V1_0::imtauth::CUsersGqlHandlerCompBase
{
public:
	typedef sdl::V1_0::imtauth::CUsersGqlHandlerCompBase BaseClass;

	I_BEGIN_COMPONENT(CUserControllerComp);
		I_ASSIGN(m_userVerificationControllerCompPtr, "UserVerificationController", "User verification controller", true, "UserVerificationController");
		I_ASSIGN(m_userCollectionCompPtr, "UserCollection", "User collection", true, "UserCollection");
		I_ASSIGN(m_roleCollectionCompPtr, "RoleCollection", "Role collection", false, "RoleCollection");
		I_ASSIGN(m_hashCalculatorCompPtr, "HashCalculator", "Hash calculator", true, "HashCalculator");
		I_ASSIGN(m_credentialControllerCompPtr, "CredentialController", "Credential controller", true, "CredentialController");
		I_ASSIGN(m_userFactoryCompPtr, "UserFactory", "User factory", true, "UserFactory");
		I_ASSIGN(m_userOperationContextControllerCompPtr, "UserOperationContextController", "User operation context controller", false, "UserOperationContextController");
		I_ASSIGN(m_userVerificationCodeSenderCompPtr, "UserVerificationCodeSender", "User verification code sender", false, "UserVerificationCodeSender");
		I_ASSIGN(m_databaseConnectionCheckerCompPtr, "DatabaseConnectionChecker", "Database connection checker", false, "DatabaseConnectionChecker");
		I_ASSIGN(m_passwordPolicyCompPtr, "PasswordPolicy", "Password policy for strength, history and lifetime checks", false, "PasswordPolicy");
	I_END_COMPONENT;

protected:
	// reimplemented (sdl::V1_0::imtauth::CUsersGqlHandlerCompBase)
	virtual sdl::V1_0::imtauth::CChangePasswordPayload OnChangePassword(
				const sdl::V1_0::imtauth::CChangePasswordGqlRequest& changePasswordRequest,
				const imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::V1_0::imtauth::CRegisterUserPayload OnRegisterUser(
				const sdl::V1_0::imtauth::CRegisterUserGqlRequest& registerUserRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::V1_0::imtauth::CCheckEmailPayload OnCheckEmail(
				const sdl::V1_0::imtauth::CCheckEmailGqlRequest& checkEmailRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::V1_0::imtauth::CSendEmailCodePayload OnSendEmailCode(
				const sdl::V1_0::imtauth::CSendEmailCodeGqlRequest& sendEmailCodeRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::V1_0::imtauth::CCheckEmailCodePayload OnCheckEmailCode(
				const sdl::V1_0::imtauth::CCheckEmailCodeGqlRequest& checkEmailCodeRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::V1_0::imtauth::CCheckSuperuserPayload OnCheckSuperuserExists(
				const sdl::V1_0::imtauth::CCheckSuperuserExistsGqlRequest& checkSuperuserExistsRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::V1_0::imtauth::CCreateSuperuserPayload OnCreateSuperuser(
				const sdl::V1_0::imtauth::CCreateSuperuserGqlRequest& createSuperuserRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::V1_0::imtauth::CUserObjectId OnGetUserObjectId(
				const sdl::V1_0::imtauth::CGetUserObjectIdGqlRequest& getUserObjectIdRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;

	// reimplemented (imtservergql::CPermissibleGqlRequestHandlerComp)
	virtual bool CheckPermissions(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const override;

private:
	bool SendUserCode(const QByteArray& userId, const imtauth::IUserInfo& userInfo) const;
	imtauth::IUserInfoSharedPtr GetUserInfoByLogin(const QByteArray& login) const;
	QByteArray GetUserIdByLogin(const QByteArray& login) const;
	QByteArray GetUserIdByEmail(const QString& email) const;

private:
	I_REF(imtauth::IUserVerificationController, m_userVerificationControllerCompPtr);
	I_REF(imtbase::IObjectCollection, m_userCollectionCompPtr);
	I_REF(imtbase::IObjectCollection, m_roleCollectionCompPtr);
	I_REF(imtcrypt::IHashGenerator, m_hashCalculatorCompPtr);
	I_REF(imtauth::ICredentialController, m_credentialControllerCompPtr);
	I_FACT(imtauth::IUserInfo, m_userFactoryCompPtr);
	I_REF(imtbase::IOperationContextController, m_userOperationContextControllerCompPtr);
	I_REF(imtauth::IUserVerificationCodeSender, m_userVerificationCodeSenderCompPtr);
	I_REF(imtdb::IDatabaseServerConnectionChecker, m_databaseConnectionCheckerCompPtr);
	I_REF(imtauth::IPasswordPolicy, m_passwordPolicyCompPtr);

private:
	imtauthgql::CUserRepresentationController m_userRepresentationController;
};


} // namespace imtauthgql


