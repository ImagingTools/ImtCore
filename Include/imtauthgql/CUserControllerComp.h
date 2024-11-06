#pragma once


// ImtCore includes
#include <imtbase/IObjectCollection.h>
#include <imtauth/ICredentialController.h>
#include <imtcrypt/IHashGenerator.h>
#include <GeneratedFiles/imtauthsdl/SDL/1.0/CPP/Users.h>


namespace imtauthgql
{


class CUserControllerComp: public sdl::imtauth::Users::V1_0::CGraphQlHandlerCompBase
{
public:
	typedef sdl::imtauth::Users::V1_0::CGraphQlHandlerCompBase BaseClass;

	I_BEGIN_COMPONENT(CUserControllerComp);
		I_ASSIGN(m_userCollectionCompPtr, "UserCollection", "User collection", true, "UserCollection");
		I_ASSIGN(m_hashCalculatorCompPtr, "HashCalculator", "Hash calculator", true, "HashCalculator");
		I_ASSIGN(m_credentialControllerCompPtr, "CredentialController", "Credential controller", true, "CredentialController");
	I_END_COMPONENT;

protected:
	virtual sdl::imtauth::Users::V1_0::CUsersListPayload OnUsersList(
				const sdl::imtauth::Users::V1_0::CUsersListGqlRequest& usersListRequest,
				const imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::imtauth::Users::V1_0::CUserDataPayload OnUserItem(
				const sdl::imtauth::Users::V1_0::CUserItemGqlRequest& userItemRequest,
				const imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::imtbase::ImtCollection::V1_0::CUpdatedNotificationPayload OnUserUpdate(
				const sdl::imtauth::Users::V1_0::CUserUpdateGqlRequest& userUpdateRequest,
				const imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::imtbase::ImtCollection::V1_0::CAddedNotificationPayload OnUserAdd(
				const sdl::imtauth::Users::V1_0::CUserAddGqlRequest& userAddRequest,
				const imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::imtauth::Users::V1_0::CChangePasswordPayload OnChangePassword(
				const sdl::imtauth::Users::V1_0::CChangePasswordGqlRequest& changePasswordRequest,
				const imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;

private:
	I_REF(imtbase::IObjectCollection, m_userCollectionCompPtr);
	I_REF(imtcrypt::IHashGenerator, m_hashCalculatorCompPtr);
	I_REF(imtauth::ICredentialController, m_credentialControllerCompPtr);
};


} // namespace imtauthgql


