// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtauth/ITenantManager.h>
#include <imtlic/IProductInfo.h>
#include <imtlic/CFeatureInfo.h>
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
