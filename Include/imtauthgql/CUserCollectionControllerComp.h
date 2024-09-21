#pragma once


// ImtCore includes
#include <imtgql/CObjectCollectionControllerCompBase.h>
#include <imtcrypt/IHashGenerator.h>
#include <GeneratedFiles/imtauthsdl/SDL/1.0/CPP/Users.h>


namespace imtauthgql
{


class CUserCollectionControllerComp: public sdl::imtauth::Users::V1_0::CUserCollectionControllerCompBase
{
public:
	typedef sdl::imtauth::Users::V1_0::CUserCollectionControllerCompBase BaseClass;

	I_BEGIN_COMPONENT(CUserCollectionControllerComp);
		I_ASSIGN(m_roleInfoProviderCompPtr, "RoleInfoProvider", "Role info provider", true, "RoleInfoProvider");
		I_ASSIGN(m_userGroupInfoProviderCompPtr, "UserGroupInfoProvider", "User group info provider", true, "UserGroupInfoProvider");
		I_ASSIGN(m_userInfoFactCompPtr, "UserFactory", "Factory used for creation of the new user instance", true, "UserFactory");
		I_ASSIGN(m_hashCalculatorCompPtr, "HashCalculator", "Hash calculator", true, "HashCalculator");
	I_END_COMPONENT;

protected:
	// reimplemented (sdl::imtauth::Users::V1_0::CUserCollectionControllerCompBase)
	virtual bool CreateRepresentationFromObject(
				const imtbase::IObjectCollectionIterator& objectCollectionIterator,
				const sdl::imtauth::Users::V1_0::CUsersListGqlRequest& usersListRequest,
				sdl::imtauth::Users::V1_0::CUserItem& representationObject,
				QString& errorMessage) const override;
	virtual istd::IChangeable* CreateObjectFromRepresentation(
				const sdl::imtauth::Users::V1_0::CUserData& userDataRepresentation,
				QByteArray& newObjectId,
				QString& name,
				QString& description,
				QString& errorMessage) const override;
	virtual bool CreateRepresentationFromObject(
				const istd::IChangeable& data,
				const sdl::imtauth::Users::V1_0::CUserItemGqlRequest& userItemRequest,
				sdl::imtauth::Users::V1_0::CUserDataPayload& representationPayload,
				QString& errorMessage) const override;
	virtual imtbase::CTreeItemModel* GetMetaInfo(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const override;

protected:
	I_REF(imtauth::IRoleInfoProvider, m_roleInfoProviderCompPtr);
	I_REF(imtauth::IUserGroupInfoProvider, m_userGroupInfoProviderCompPtr);
	I_FACT(imtauth::IUserInfo, m_userInfoFactCompPtr);
	I_REF(imtcrypt::IHashGenerator, m_hashCalculatorCompPtr);
};


} // namespace imtauthgql


