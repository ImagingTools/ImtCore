#pragma once


// ImtCore includes
#include <imtgql/CObjectCollectionControllerCompBase.h>
#include <imtcrypt/IHashGenerator.h>
#include <GeneratedFiles/imtauthgql/SDL/CPP/Users/UsersAPIv1_0.h>


namespace imtauthgql
{


class CUserCollectionControllerComp: public imtcore::sdl::Users::CUserCollectionControllerCompBase
{
public:
	typedef imtcore::sdl::Users::CUserCollectionControllerCompBase BaseClass;

	I_BEGIN_COMPONENT(CUserCollectionControllerComp);
		I_ASSIGN(m_roleInfoProviderCompPtr, "RoleInfoProvider", "Role info provider", true, "RoleInfoProvider");
		I_ASSIGN(m_userGroupInfoProviderCompPtr, "UserGroupInfoProvider", "User group info provider", true, "UserGroupInfoProvider");
		I_ASSIGN(m_userInfoFactCompPtr, "UserFactory", "Factory used for creation of the new user instance", true, "UserFactory");
		I_ASSIGN(m_hashCalculatorCompPtr, "HashCalculator", "Hash calculator", true, "HashCalculator");
	I_END_COMPONENT;

protected:
	// reimplemented (imtcore::sdl::Users::CUserCollectionControllerCompBase)
	virtual bool CreateRepresentationFromObject(
				const imtbase::IObjectCollectionIterator& objectCollectionIterator,
				const imtcore::sdl::Users::CUsersListGqlRequest& usersListRequest,
				imtcore::sdl::Users::CUserItem& representationObject,
				QString& errorMessage) const override;

	// User methods
	virtual istd::IChangeable* CreateObjectFromRepresentation(
				const imtcore::sdl::Users::CUserData& userDataRepresentation,
				QByteArray& newObjectId,
				QString& name,
				QString& description,
				QString& errorMessage) const override;
	virtual bool CreateRepresentationFromObject(
				const istd::IChangeable& data,
				const imtcore::sdl::Users::CUserItemGqlRequest& userItemRequest,
				imtcore::sdl::Users::CUserDataPayload& representationPayload,
				QString& errorMessage) const override;
	virtual imtbase::CTreeItemModel* GetMetaInfo(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const override;

protected:
	I_REF(imtauth::IRoleInfoProvider, m_roleInfoProviderCompPtr);
	I_REF(imtauth::IUserGroupInfoProvider, m_userGroupInfoProviderCompPtr);
	I_FACT(imtauth::IUserInfo, m_userInfoFactCompPtr);
	I_REF(imtcrypt::IHashGenerator, m_hashCalculatorCompPtr);
};


} // namespace imtauthgql


