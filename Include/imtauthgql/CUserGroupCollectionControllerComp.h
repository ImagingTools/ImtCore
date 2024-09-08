#pragma once


// ImtCore includes
#include <imtgql/CObjectCollectionControllerCompBase.h>
#include <imtauth/IUserGroupInfo.h>
#include <GeneratedFiles/imtauthgql/SDL/CPP/Groups/GroupsAPIv1_0.h>


namespace imtauthgql
{


class CUserGroupCollectionControllerComp: public imtcore::sdl::Groups::CGroupCollectionControllerCompBase
{
public:
	typedef imtcore::sdl::Groups::CGroupCollectionControllerCompBase BaseClass;

	I_BEGIN_COMPONENT(CUserGroupCollectionControllerComp);
		I_ASSIGN(m_roleInfoProviderCompPtr, "RoleInfoProvider", "Role info provider", false, "RoleInfoProvider");
		I_ASSIGN(m_userInfoProviderCompPtr, "UserInfoProvider", "User info provider", false, "UserInfoProvider");
		I_ASSIGN(m_userGroupInfoProviderCompPtr, "UserGroupInfoProvider", "User group info provider", false, "UserGroupInfoProvider");
		I_ASSIGN(m_userGroupInfoFactCompPtr, "UserGroupFactory", "Factory used for creation of the new group instance", true, "UserGroupFactory");
	I_END_COMPONENT;

protected:
	// reimplemented (imtcore::sdl::Groups::CGroupCollectionControllerCompBase)
	virtual bool CreateRepresentationFromObject(
				const imtbase::IObjectCollectionIterator& objectCollectionIterator,
				const imtcore::sdl::Groups::CGroupsListGqlRequest& groupsListRequest,
				imtcore::sdl::Groups::CGroupItem& representationObject,
				QString& errorMessage) const override;
	virtual istd::IChangeable* CreateObjectFromRepresentation(
				const imtcore::sdl::Groups::CGroupData& groupDataRepresentation,
				QByteArray& newObjectId,
				QString& name,
				QString& description,
				QString& errorMessage) const override;
	virtual bool CreateRepresentationFromObject(
				const istd::IChangeable& data,
				const imtcore::sdl::Groups::CGroupItemGqlRequest& groupItemRequest,
				imtcore::sdl::Groups::CGroupDataPayload& representationPayload,
				QString& errorMessage) const override;
	virtual imtbase::CTreeItemModel* GetMetaInfo(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const override;

protected:
	I_REF(imtauth::IRoleInfoProvider, m_roleInfoProviderCompPtr);
	I_REF(imtauth::IUserInfoProvider, m_userInfoProviderCompPtr);
	I_REF(imtauth::IUserGroupInfoProvider, m_userGroupInfoProviderCompPtr);
	I_FACT(imtauth::IUserGroupInfo, m_userGroupInfoFactCompPtr);
};


} // namespace imtauthgql


