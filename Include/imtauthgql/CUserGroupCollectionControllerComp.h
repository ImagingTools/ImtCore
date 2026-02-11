// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtauth/IUserGroupInfo.h>
#include <imtauth/IUserInfoProvider.h>
#include <imtauth/IUserGroupInfoProvider.h>
#include <GeneratedFiles/imtauthsdl/SDL/1.0/CPP/Groups.h>


namespace imtauthgql
{


class CUserGroupCollectionControllerComp: public sdl::imtauth::Groups::CGroupCollectionControllerCompBase
{
public:
	typedef sdl::imtauth::Groups::CGroupCollectionControllerCompBase BaseClass;

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

	// reimplemented (sdl::imtbase::ImtCollection::CGraphQlHandlerCompBase)
	virtual sdl::imtbase::ImtCollection::CVisualStatus OnGetObjectVisualStatus(
				const sdl::imtbase::ImtCollection::CGetObjectVisualStatusGqlRequest& getObjectVisualStatusRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::imtbase::ImtCollection::CGetElementMetaInfoPayload OnGetElementMetaInfo(
				const sdl::imtbase::ImtCollection::CGetElementMetaInfoGqlRequest& getElementMetaInfoRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;

	// reimplemented (sdl::imtauth::Groups::CGroupCollectionControllerCompBase)
	virtual bool CreateRepresentationFromObject(
				const imtbase::IObjectCollectionIterator& objectCollectionIterator,
				const sdl::imtauth::Groups::CGroupsListGqlRequest& groupsListRequest,
				sdl::imtauth::Groups::CGroupItemData::V1_0& representationObject,
				QString& errorMessage) const override;
	virtual istd::IChangeableUniquePtr CreateObjectFromRepresentation(
				const sdl::imtauth::Groups::CGroupData::V1_0& groupDataRepresentation,
				QByteArray& newObjectId,
				QString& errorMessage) const override;
	virtual bool CreateRepresentationFromObject(
				const istd::IChangeable& data,
				const sdl::imtauth::Groups::CGroupItemGqlRequest& groupItemRequest,
				sdl::imtauth::Groups::CGroupData::V1_0& representationPayload,
				QString& errorMessage) const override;
	virtual bool UpdateObjectFromRepresentationRequest(
				const ::imtgql::CGqlRequest& rawGqlRequest,
				const sdl::imtauth::Groups::CGroupUpdateGqlRequest& groupUpdateRequest,
				istd::IChangeable& object,
				QString& errorMessage) const override;

	// reimplemented (imtservergql::CPermissibleGqlRequestHandlerComp)
	virtual bool CheckPermissions(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const override;

protected:
	I_REF(imtauth::IRoleInfoProvider, m_roleInfoProviderCompPtr);
	I_REF(imtauth::IUserInfoProvider, m_userInfoProviderCompPtr);
	I_REF(imtauth::IUserGroupInfoProvider, m_userGroupInfoProviderCompPtr);
	I_FACT(imtauth::IUserGroupInfo, m_userGroupInfoFactCompPtr);
};


} // namespace imtauthgql


