// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtlic/IProductInfo.h>
#include <imtsdl/TElementList.h>
#include <imtclientgql/TClientRequestManagerCompWrap.h>
#include <GeneratedFiles/imtauthsdl/SDL/1.0/CPP/TenantCollectionDocumentManager.h>


namespace imtauthgql
{


class CRemoteTenantCollectionDocumentManagerControllerComp:
			public imtclientgql::TClientRequestManagerCompWrap<
							sdl::imtauth::TenantCollectionDocumentManager::CGraphQlHandlerCompBase>
{
public:
	typedef imtclientgql::TClientRequestManagerCompWrap<
					sdl::imtauth::TenantCollectionDocumentManager::CGraphQlHandlerCompBase> BaseClass;

	I_BEGIN_COMPONENT(CRemoteTenantCollectionDocumentManagerControllerComp);
		I_ASSIGN(m_productInfoCompPtr, "ProductInfo", "Product info for loading available permissions", true, "ProductInfo");
	I_END_COMPONENT;

protected:
	// reimplemented (sdl::imtauth::TenantCollectionDocumentManager::CGraphQlHandlerCompBase)
	virtual sdl::imtauth::Tenants::CTenantData OnGetTenantRepresentation(
				const sdl::imtauth::TenantCollectionDocumentManager::CGetTenantRepresentationGqlRequest& getTenantRepresentationRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::imtbase::CollectionDocumentManager::CDocumentOperationStatus OnUpdateTenantFromRepresentation(
				const sdl::imtauth::TenantCollectionDocumentManager::CUpdateTenantFromRepresentationGqlRequest& updateTenantFromRepresentationRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;

private:
	void CollectLeafPermissions(
				const imtlic::IFeatureInfo* featureInfoPtr,
				imtsdl::TElementList<sdl::imtauth::Tenants::CTenantPermissionOption::V1_0>& leafPermissions) const;

	I_REF(imtlic::IProductInfo, m_productInfoCompPtr);
};


} // namespace imtauthgql
