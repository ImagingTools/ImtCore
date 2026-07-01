// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtservergql/CObjectCollectionControllerCompBase.h>
#include <imtcrypt/IHashGenerator.h>
#include <imtauth/IUserGroupInfoProvider.h>
#include <imtauth/IDelegatedAccess.h>
#include <imtauth/ITenantMembershipManager.h>
#include <imtauth/ITenantManager.h>
#include <imtauth/ITenantEntityBindingManager.h>
#include <imtauth/IPersonalAccessTokenManager.h>
#include <imtbase/IObjectCollection.h>
#include <imtauth/ITenantInvitation.h>
#include <imtauth/ISession.h>
#include <GeneratedFiles/imtauthsdl/SDL/1.0/CPP/Users_fwd.h>


namespace imtauthgql
{


class CUserCollectionControllerComp: public sdl::V1_0::imtauth::CUserCollectionControllerCompBase
{
public:
	typedef sdl::V1_0::imtauth::CUserCollectionControllerCompBase BaseClass;

	I_BEGIN_COMPONENT(CUserCollectionControllerComp);
		I_ASSIGN(m_roleInfoProviderCompPtr, "RoleInfoProvider", "Role info provider", true, "RoleInfoProvider");
		I_ASSIGN(m_userGroupInfoProviderCompPtr, "UserGroupInfoProvider", "User group info provider", true, "UserGroupInfoProvider");
		I_ASSIGN(m_userInfoFactCompPtr, "UserFactory", "Factory used for creation of the new user instance", true, "UserFactory");
		I_ASSIGN(m_hashCalculatorCompPtr, "HashCalculator", "Hash calculator", true, "HashCalculator");
		I_ASSIGN(m_delegatedAccessCompPtr, "DelegatedAccess", "Optional delegated access resolver used to enrich the user with roles delegated via cross-org grants", false, "DelegatedAccessResolver");
		I_ASSIGN(m_membershipManagerCompPtr, "MembershipManager", "Tenant membership manager for auto-adding membership on user creation", false, "MembershipManager");
		I_ASSIGN(m_membershipCollectionCompPtr, "MembershipCollection", "Membership collection for direct purge of user memberships on delete", false, "MembershipCollection");
		I_ASSIGN(m_tenantManagerCompPtr, "TenantManager", "Tenant manager to prevent deletion of tenant owners", false, "TenantManager");
		I_ASSIGN(m_bindingManagerCompPtr, "BindingManager", "Tenant entity binding manager for cleanup on user delete", false, "TenantEntityBindingManager");
		I_ASSIGN(m_invitationCollectionCompPtr, "InvitationCollection", "Tenant invitation collection for purging references on user delete", false, "InvitationCollection");
		I_ASSIGN(m_personalAccessTokenManagerCompPtr, "PersonalAccessTokenManager", "PAT manager to remove tokens of deleted user", false, "PersonalAccessTokenManager");
		I_ASSIGN(m_sessionCollectionCompPtr, "SessionCollection", "Session collection for purging user sessions on delete", false, "SessionCollection");
	I_END_COMPONENT;

protected:
	virtual bool FillObjectFromRepresentation(
				const sdl::V1_0::imtauth::CUserData& representation,
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

	// reimplemented (sdl::V1_0::imtauth::CUserCollectionControllerCompBase)
	virtual bool CreateRepresentationFromObject(
				const imtbase::IObjectCollectionIterator& objectCollectionIterator,
				const sdl::V1_0::imtauth::CUsersListGqlRequest& usersListRequest,
				sdl::V1_0::imtauth::CUserItemData& representationObject,
				QString& errorMessage) const override;
	virtual istd::IChangeableUniquePtr CreateObjectFromRepresentation(
				const sdl::V1_0::imtauth::CUserData& userDataRepresentation,
				QByteArray& newObjectId,
				QString& errorMessage) const override;
	virtual bool CreateRepresentationFromObject(
				const istd::IChangeable& data,
				const sdl::V1_0::imtauth::CUserItemGqlRequest& userItemRequest,
				sdl::V1_0::imtauth::CUserData& representationPayload,
				QString& errorMessage) const override;
	virtual bool UpdateObjectFromRepresentationRequest(
				const ::imtgql::CGqlRequest& rawGqlRequest,
				const sdl::V1_0::imtauth::CUserUpdateGqlRequest& userUpdateRequest,
				istd::IChangeable& object,
				QString& errorMessage) const override;

	// reimplemented (imtservergql::CObjectCollectionControllerCompBase)
	virtual istd::IChangeableUniquePtr CreateAdaptedObjectData(
				const QByteArray& objectId,
				const istd::IChangeable& object,
				const imtgql::CGqlRequest& gqlRequest) const override;
	virtual QJsonObject InsertObject(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const override;
	virtual void SetAdditionalFilters(
				const imtgql::CGqlRequest& gqlRequest,
				const imtgql::CGqlParamObject& viewParamsGql,
				iprm::CParamsSet* filterParamsPtr) const override;

	// reimplemented (imtservergql::CPermissibleGqlRequestHandlerComp)
	virtual bool CheckPermissions(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const override;

	// reimplemented (imtservergql::CObjectCollectionControllerCompBase) for tenant-dependent cleanup
	virtual bool OnBeforeRemoveElements(const QByteArrayList& elementIds, const ::imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const override;
	virtual void OnAfterRemoveElements(const QByteArrayList& elementIds, const ::imtgql::CGqlRequest& gqlRequest) const override;

protected:
	I_REF(imtauth::IRoleInfoProvider, m_roleInfoProviderCompPtr);
	I_REF(imtauth::IUserGroupInfoProvider, m_userGroupInfoProviderCompPtr);
	I_FACT(imtauth::IUserInfo, m_userInfoFactCompPtr);
	I_REF(imtcrypt::IHashGenerator, m_hashCalculatorCompPtr);
	I_REF(imtauth::IDelegatedAccess, m_delegatedAccessCompPtr);
	I_REF(imtauth::ITenantMembershipManager, m_membershipManagerCompPtr);
	I_REF(imtbase::IObjectCollection, m_membershipCollectionCompPtr);
	I_REF(imtauth::ITenantManager, m_tenantManagerCompPtr);
	I_REF(imtauth::ITenantEntityBindingManager, m_bindingManagerCompPtr);
	I_REF(imtbase::IObjectCollection, m_invitationCollectionCompPtr);
	I_REF(imtauth::IPersonalAccessTokenManager, m_personalAccessTokenManagerCompPtr);
	I_REF(imtbase::IObjectCollection, m_sessionCollectionCompPtr);
};


} // namespace imtauthgql


