// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtbase/IObjectCollection.h>
#include <imtlic/IProductInfo.h>
#include <imtauth/ITenantMembership.h>
#include <imtauth/ITenantMembershipManager.h>
#include <imtauth/ITenantManager.h>
#include <GeneratedFiles/imtauthsdl/SDL/1.0/CPP/Profile>


namespace imtauthgql
{


class CProfileControllerComp: public sdl::V1_0::imtauth::CProfileGqlHandlerCompBase
{
public:
	typedef sdl::V1_0::imtauth::CProfileGqlHandlerCompBase BaseClass;

	I_BEGIN_COMPONENT(CProfileControllerComp);
		I_ASSIGN(m_userCollectionCompPtr, "UserCollection", "User collection", false, "UserCollection");
		I_ASSIGN(m_roleCollectionCompPtr, "RoleCollection", "Role collection", false, "RoleCollection");
		I_ASSIGN(m_groupCollectionCompPtr, "GroupCollection", "Group collection", false, "GroupCollection");
		I_ASSIGN(m_tenantManagerCompPtr, "TenantManager", "Tenant manager", false, "TenantManager");
		I_ASSIGN(m_membershipManagerCompPtr, "MembershipManager", "Tenant membership manager", false, "TenantMembershipManager");
	I_END_COMPONENT;

protected:
	// reimplemented (sdl::V1_0::imtauth::CProfileGqlHandlerCompBase)
	virtual sdl::V1_0::imtauth::CProfileData OnGetProfile(
				const sdl::V1_0::imtauth::CGetProfileGqlRequest& getProfileRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::V1_0::imtauth::CSetProfileResponse OnSetProfile(
				const sdl::V1_0::imtauth::CSetProfileGqlRequest& setProfileRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::V1_0::imtauth::CGetUserOrganizationsPayload OnGetUserOrganizations(
				const sdl::V1_0::imtauth::CGetUserOrganizationsGqlRequest& getUserOrganizationsRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
private:
	imtsdl::TElementList<sdl::V1_0::imtauth::CProfileTenantInfo> CreateOrganizationList(
				const QByteArray& userId) const;
	QString TenantMembershipRoleToString(const QByteArray& roleId) const;
protected:
	I_REF(imtbase::IObjectCollection, m_userCollectionCompPtr);
	I_REF(imtbase::IObjectCollection, m_roleCollectionCompPtr);
	I_REF(imtbase::IObjectCollection, m_groupCollectionCompPtr);
	I_REF(imtauth::ITenantManager, m_tenantManagerCompPtr);
	I_REF(imtauth::ITenantMembershipManager, m_membershipManagerCompPtr);
};


} // namespace imtauthgql

