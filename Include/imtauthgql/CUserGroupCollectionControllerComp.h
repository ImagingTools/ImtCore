// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtauth/IUserGroupInfo.h>
#include <imtauth/IUserInfoProvider.h>
#include <imtauth/IUserGroupInfoProvider.h>
#include <GeneratedFiles/imtauthsdl/SDL/1.0/CPP/Groups>


namespace imtauthgql
{


class CUserGroupCollectionControllerComp: public sdl::V1_0::imtauth::CGroupCollectionControllerCompBase
{
public:
	typedef sdl::V1_0::imtauth::CGroupCollectionControllerCompBase BaseClass;

	I_BEGIN_COMPONENT(CUserGroupCollectionControllerComp);
		I_ASSIGN(m_roleInfoProviderCompPtr, "RoleInfoProvider", "Role info provider", false, "RoleInfoProvider");
		I_ASSIGN(m_userInfoProviderCompPtr, "UserInfoProvider", "User info provider", false, "UserInfoProvider");
		I_ASSIGN(m_userGroupInfoProviderCompPtr, "UserGroupInfoProvider", "User group info provider", false, "UserGroupInfoProvider");
		I_ASSIGN(m_userGroupInfoFactCompPtr, "UserGroupFactory", "Factory used for creation of the new group instance", true, "UserGroupFactory");
	I_END_COMPONENT;

protected:
	virtual bool FillObjectFromRepresentation(
				const sdl::V1_0::imtauth::CGroupData& representation,
				istd::IChangeable& object,
				QByteArray& newObjectId,
				QString& errorMessage) const;

	// reimplemented (sdl::V1_0::imtbase::CImtCollectionGqlHandlerCompBase)
	virtual sdl::V1_0::imtbase::CVisualStatus OnGetObjectVisualStatus(
				const sdl::V1_0::imtbase::CGetObjectVisualStatusGqlRequest& getObjectVisualStatusRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::V1_0::imtbase::CGetElementMetaInfoPayload OnGetElementMetaInfo(
				const sdl::V1_0::imtbase::CGetElementMetaInfoGqlRequest& getElementMetaInfoRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;

	// reimplemented (sdl::V1_0::imtauth::CGroupCollectionControllerCompBase)
	virtual bool CreateRepresentationFromObject(
				const imtbase::IObjectCollectionIterator& objectCollectionIterator,
				const sdl::V1_0::imtauth::CGroupsListGqlRequest& groupsListRequest,
				sdl::V1_0::imtauth::CGroupItemData& representationObject,
				QString& errorMessage) const override;
	virtual istd::IChangeableUniquePtr CreateObjectFromRepresentation(
				const sdl::V1_0::imtauth::CGroupData& groupDataRepresentation,
				QByteArray& newObjectId,
				QString& errorMessage) const override;
	virtual bool CreateRepresentationFromObject(
				const istd::IChangeable& data,
				const sdl::V1_0::imtauth::CGroupItemGqlRequest& groupItemRequest,
				sdl::V1_0::imtauth::CGroupData& representationPayload,
				QString& errorMessage) const override;
	virtual bool UpdateObjectFromRepresentationRequest(
				const ::imtgql::CGqlRequest& rawGqlRequest,
				const sdl::V1_0::imtauth::CGroupUpdateGqlRequest& groupUpdateRequest,
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


