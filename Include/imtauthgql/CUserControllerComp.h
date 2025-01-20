#pragma once


// ImtCore includes
#include <imtbase/IObjectCollection.h>
#include <imtauth/ICredentialController.h>
#include <imtauth/IUserVerificationController.h>
#include <imtauthgql/CUserRepresentationController.h>
#include <imtcrypt/IHashGenerator.h>
#include <imtmail/ISmtpClient.h>
#include <imtmail/ISmtpMessageCreator.h>
#include <imtauth/IUserVerificationCodeSender.h>
#include <imtdb/IDatabaseServerConnectionChecker.h>
#include <GeneratedFiles/imtauthsdl/SDL/1.0/CPP/Users.h>


namespace imtauthgql
{


class CUserControllerComp: public sdl::imtauth::Users::V1_0::CGraphQlHandlerCompBase
{
public:
	typedef sdl::imtauth::Users::V1_0::CGraphQlHandlerCompBase BaseClass;

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
	I_END_COMPONENT;

protected:
	// reimplemented (sdl::imtauth::Users::V1_0::CGraphQlHandlerCompBase)
	virtual sdl::imtbase::ImtCollection::CVisualStatus::V1_0 OnGetObjectVisualStatus(
		const sdl::imtauth::Users::V1_0::CGetObjectVisualStatusGqlRequest& getObjectVisualStatusRequest,
		const ::imtgql::CGqlRequest& gqlRequest,
		QString& errorMessage) const override;
	virtual sdl::imtauth::Users::CUsersListPayload::V1_0 OnUsersList(
		const sdl::imtauth::Users::V1_0::CUsersListGqlRequest& usersListRequest,
		const imtgql::CGqlRequest& gqlRequest,
		QString& errorMessage) const override;
	virtual sdl::imtauth::Users::CUserDataPayload::V1_0 OnUserItem(
		const sdl::imtauth::Users::V1_0::CUserItemGqlRequest& userItemRequest,
		const imtgql::CGqlRequest& gqlRequest,
		QString& errorMessage) const override;
	virtual sdl::imtbase::ImtCollection::CUpdatedNotificationPayload::V1_0 OnUserUpdate(
		const sdl::imtauth::Users::V1_0::CUserUpdateGqlRequest& userUpdateRequest,
		const imtgql::CGqlRequest& gqlRequest,
		QString& errorMessage) const override;
	virtual sdl::imtbase::ImtCollection::CAddedNotificationPayload::V1_0 OnUserAdd(
		const sdl::imtauth::Users::V1_0::CUserAddGqlRequest& userAddRequest,
		const imtgql::CGqlRequest& gqlRequest,
		QString& errorMessage) const override;
	virtual sdl::imtauth::Users::CChangePasswordPayload::V1_0 OnChangePassword(
		const sdl::imtauth::Users::V1_0::CChangePasswordGqlRequest& changePasswordRequest,
		const imtgql::CGqlRequest& gqlRequest,
		QString& errorMessage) const override;
	virtual sdl::imtauth::Users::CRegisterUserPayload::V1_0 OnRegisterUser(
		const sdl::imtauth::Users::V1_0::CRegisterUserGqlRequest& registerUserRequest,
		const ::imtgql::CGqlRequest& gqlRequest,
		QString& errorMessage) const override;
	virtual sdl::imtauth::Users::CCheckEmailPayload::V1_0 OnCheckEmail(
		const sdl::imtauth::Users::V1_0::CCheckEmailGqlRequest& checkEmailRequest,
		const ::imtgql::CGqlRequest& gqlRequest,
		QString& errorMessage) const override;
	virtual sdl::imtauth::Users::CSendEmailCodePayload::V1_0 OnSendEmailCode(
		const sdl::imtauth::Users::V1_0::CSendEmailCodeGqlRequest& sendEmailCodeRequest,
		const ::imtgql::CGqlRequest& gqlRequest,
		QString& errorMessage) const override;
	virtual sdl::imtauth::Users::CCheckEmailCodePayload::V1_0 OnCheckEmailCode(
		const sdl::imtauth::Users::V1_0::CCheckEmailCodeGqlRequest& checkEmailCodeRequest,
		const ::imtgql::CGqlRequest& gqlRequest,
		QString& errorMessage) const override;
	virtual sdl::imtauth::Users::CCheckSuperuserPayload::V1_0 OnCheckSuperuserExists(
		const sdl::imtauth::Users::V1_0::CCheckSuperuserExistsGqlRequest& checkSuperuserExistsRequest,
		const ::imtgql::CGqlRequest& gqlRequest,
		QString& errorMessage) const override;
	virtual sdl::imtauth::Users::CCreateSuperuserPayload::V1_0 OnCreateSuperuser(
		const sdl::imtauth::Users::V1_0::CCreateSuperuserGqlRequest& createSuperuserRequest,
		const ::imtgql::CGqlRequest& gqlRequest,
		QString& errorMessage) const override;

private:
	bool SendUserCode(const QByteArray& userId, const imtauth::IUserInfo& userInfo) const;
	const imtauth::IUserInfo* GetUserInfoByLogin(const QByteArray& login) const;
	const QByteArray GetUserIdByLogin(const QByteArray& login) const;
	const QByteArray GetUserIdByEmail(const QString& email) const;

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

private:
	imtauthgql::CUserRepresentationController m_userRepresentationController;
};


} // namespace imtauthgql


