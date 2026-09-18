// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <iqt/ITranslationManager.h>

// ImtCore includes
#include <imtauth/ITenantManager.h>
#include <imtauth/ITenantEntityBindingManager.h>
#include <imtauth/IDelegatedAccess.h>
#include <imtauth/ITenantMembershipManager.h>
#include <imtauth/IRoleInfoProvider.h>
#include <imtlic/IProductInfo.h>
#include <GeneratedFiles/imtauthsdl/SDL/1.0/CPP/Permissions_fwd.h>


namespace imtauthgql
{


class CPermissionsProviderComp: public sdl::V1_0::imtauth::CPermissionsGqlHandlerCompBase
{
public:
	typedef sdl::V1_0::imtauth::CPermissionsGqlHandlerCompBase BaseClass;

	I_BEGIN_COMPONENT(CPermissionsProviderComp)
		I_ASSIGN(m_productInfoCompPtr, "FeatureContainer", "Feature container", true, "FeatureContainer");
		I_ASSIGN(m_tenantManagerCompPtr, "TenantManager", "Tenant manager for filtering permissions by tenant scope", false, "TenantManager");
		I_ASSIGN(m_bindingManagerCompPtr, "BindingManager", "Tenant entity binding manager for tenant-scoped adaptation of user roles/groups/permissions", false, "TenantEntityBindingManager");
		I_ASSIGN(m_delegatedAccessCompPtr, "DelegatedAccess", "Delegated access resolver for cross-org grants", false, "DelegatedAccessResolver");
		I_ASSIGN(m_membershipManagerCompPtr, "MembershipManager", "Tenant membership manager", false, "TenantMembershipManager");
		I_ASSIGN(m_roleInfoProviderCompPtr, "RoleInfoProvider", "Role info provider (used for delegated role product validation)", false, "RoleInfoProvider");
		I_ASSIGN(m_translationManagerCompPtr, "TranslationManager", "Translation manager", false, "TranslationManager");
	I_END_COMPONENT

protected:
	// reimplemented (sdl::V1_0::imtauth::CPermissionsGqlHandlerCompBase)
	virtual sdl::V1_0::imtauth::CGetProductPermissionsPayload OnGetProductPermissions(
				const sdl::V1_0::imtauth::CGetProductPermissionsGqlRequest& getProductPermissionsRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::V1_0::imtauth::CGetProductPermissionsPayload OnGetUserPermissions(
				const sdl::V1_0::imtauth::CGetUserPermissionsGqlRequest& getUserPermissionsRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;

protected:
	I_REF(imtlic::IProductInfo, m_productInfoCompPtr);
	I_REF(imtauth::ITenantManager, m_tenantManagerCompPtr);
	I_REF(imtauth::ITenantEntityBindingManager, m_bindingManagerCompPtr);
	I_REF(imtauth::IDelegatedAccess, m_delegatedAccessCompPtr);
	I_REF(imtauth::ITenantMembershipManager, m_membershipManagerCompPtr);
	I_REF(imtauth::IRoleInfoProvider, m_roleInfoProviderCompPtr);
	I_REF(iqt::ITranslationManager, m_translationManagerCompPtr);
};


} // namespace imtauthgql
