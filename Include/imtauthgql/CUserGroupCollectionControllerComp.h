#pragma once


// ImtCore includes
#include <imtservergql/CObjectCollectionControllerCompBase.h>
#include <imtauth/IUserGroupInfo.h>
#include <GeneratedFiles/imtauthsdl/SDL/1.0/CPP/Groups.h>


namespace imtauthgql
{


class CUserGroupCollectionControllerComp: public sdl::imtauth::Groups::V1_0::CGroupCollectionControllerCompBase
{
public:
	typedef sdl::imtauth::Groups::V1_0::CGroupCollectionControllerCompBase BaseClass;

	I_BEGIN_COMPONENT(CUserGroupCollectionControllerComp);
		I_ASSIGN(m_roleInfoProviderCompPtr, "RoleInfoProvider", "Role info provider", false, "RoleInfoProvider");
		I_ASSIGN(m_userInfoProviderCompPtr, "UserInfoProvider", "User info provider", false, "UserInfoProvider");
		I_ASSIGN(m_userGroupInfoProviderCompPtr, "UserGroupInfoProvider", "User group info provider", false, "UserGroupInfoProvider");
		I_ASSIGN(m_userGroupInfoFactCompPtr, "UserGroupFactory", "Factory used for creation of the new group instance", true, "UserGroupFactory");
	I_END_COMPONENT;

protected:
	virtual bool FillObjectFromRepresentation(
				const sdl::imtauth::Groups::CGroupData::V1_0& representation,
				istd::IChangeable& object,
				QByteArray& newObjectId,
				QString& errorMessage) const;

	// reimplemented (sdl::imtbase::ImtCollection::V1_0::CGraphQlHandlerCompBase)
	virtual sdl::imtbase::ImtCollection::CVisualStatus::V1_0 OnGetObjectVisualStatus(
				const sdl::imtbase::ImtCollection::V1_0::CGetObjectVisualStatusGqlRequest& getObjectVisualStatusRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;

	// reimplemented (sdl::imtauth::Groups::V1_0::CGroupCollectionControllerCompBase)
	virtual bool CreateRepresentationFromObject(
				const imtbase::IObjectCollectionIterator& objectCollectionIterator,
				const sdl::imtauth::Groups::V1_0::CGroupsListGqlRequest& groupsListRequest,
				sdl::imtauth::Groups::CGroupItem::V1_0& representationObject,
				QString& errorMessage) const override;
	virtual istd::IChangeable* CreateObjectFromRepresentation(
				const sdl::imtauth::Groups::CGroupData::V1_0& groupDataRepresentation,
				QByteArray& newObjectId,
				QString& errorMessage) const override;
	virtual bool CreateRepresentationFromObject(
				const istd::IChangeable& data,
				const sdl::imtauth::Groups::V1_0::CGroupItemGqlRequest& groupItemRequest,
				sdl::imtauth::Groups::CGroupDataPayload::V1_0& representationPayload,
				QString& errorMessage) const override;
	virtual bool UpdateObjectFromRepresentationRequest(
				const ::imtgql::CGqlRequest& rawGqlRequest,
				const sdl::imtauth::Groups::V1_0::CGroupUpdateGqlRequest& groupUpdateRequest,
				istd::IChangeable& object,
				QString& errorMessage) const override;
	virtual imtbase::CTreeItemModel* GetMetaInfo(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const override;

protected:
	I_REF(imtauth::IRoleInfoProvider, m_roleInfoProviderCompPtr);
	I_REF(imtauth::IUserInfoProvider, m_userInfoProviderCompPtr);
	I_REF(imtauth::IUserGroupInfoProvider, m_userGroupInfoProviderCompPtr);
	I_FACT(imtauth::IUserGroupInfo, m_userGroupInfoFactCompPtr);
};


} // namespace imtauthgql


