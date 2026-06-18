// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtlic/IProductInfo.h>
#include <imtlic/CFeatureInfo.h>
#include <imtauth/ITenantManager.h>
#include <imtsdl/TElementList.h>
#include <iqt/ITranslationManager.h>
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
		I_ASSIGN(m_translationManagerCompPtr, "TranslationManager", "Translation manager", false, "TranslationManager");
	I_END_COMPONENT

protected:
	/**
	 * Collect flat permission entries from a feature subtree into entries array.
	 * Each entry has permissionId, displayName (path-style "Parent / Child"), description.
	 * Returns true if at least one entry was added (used for tenant filtering).
	 */
	bool CollectPermissionEntries(
				const imtlic::CFeatureInfo& featureInfo,
				imtsdl::TElementList<sdl::V1_0::imtauth::CPermissionEntry>& entries,
				const QByteArray& languageId,
				const QSet<QByteArray>* allowedPermissionsPtr,
				const QString& parentPath) const;

	// reimplemented (sdl::V1_0::imtauth::CPermissionsGqlHandlerCompBase)
	virtual sdl::V1_0::imtauth::CGetProductPermissionsPayload OnGetProductPermissions(
				const sdl::V1_0::imtauth::CGetProductPermissionsGqlRequest& getProductPermissionsRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;

protected:
	I_REF(imtlic::IProductInfo, m_productInfoCompPtr);
	I_REF(imtauth::ITenantManager, m_tenantManagerCompPtr);
	I_REF(iqt::ITranslationManager, m_translationManagerCompPtr);
};


} // namespace imtauthgql
