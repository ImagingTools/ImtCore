// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtlic/IProductInfo.h>
#include <imtsdl/TElementList.h>
#include <imtclientgql/TClientRequestManagerCompWrap.h>
#include <GeneratedFiles/imtauthsdl/SDL/1.0/CPP/TenantCollectionDocumentService>


namespace imtauthgql
{


class CRemoteTenantCollectionDocumentServiceControllerComp:
			public imtclientgql::TClientRequestManagerCompWrap<
							sdl::imtauth::TenantCollectionDocumentService::CGraphQlHandlerCompBase>
{
public:
	typedef imtclientgql::TClientRequestManagerCompWrap<
					sdl::imtauth::TenantCollectionDocumentService::CGraphQlHandlerCompBase> BaseClass;

	I_BEGIN_COMPONENT(CRemoteTenantCollectionDocumentServiceControllerComp);
		I_ASSIGN(m_productInfoCompPtr, "ProductInfo", "Product info for loading available permissions", true, "ProductInfo");
	I_END_COMPONENT;

protected:
	// reimplemented (sdl::imtauth::TenantCollectionDocumentService::CGraphQlHandlerCompBase)
	virtual sdl::imtauth::Tenants::CTenantData OnGetTenantRepresentation(
				const sdl::imtauth::TenantCollectionDocumentService::CGetTenantRepresentationGqlRequest& getTenantRepresentationRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::imtbase::CollectionDocumentService::CDocumentOperationStatus OnUpdateTenantFromRepresentation(
				const sdl::imtauth::TenantCollectionDocumentService::CUpdateTenantFromRepresentationGqlRequest& updateTenantFromRepresentationRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;

private:
	void CollectPermissionsTree(
				const imtlic::IFeatureInfo* featureInfoPtr,
				imtsdl::TElementList<sdl::imtauth::Tenants::CTenantPermissionOption::V1_0>& permissions) const;

	I_REF(imtlic::IProductInfo, m_productInfoCompPtr);
};


} // namespace imtauthgql
