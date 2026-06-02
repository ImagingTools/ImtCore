// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtlic/IProductInfo.h>
#include <imtsdl/TElementList.h>
#include <imtclientgql/TClientRequestManagerCompWrap.h>
#include <GeneratedFiles/imtauthsdl/SDL/1.0/CPP/TenantCollectionDocumentService.h>


namespace imtauthgql
{


class CRemoteTenantCollectionDocumentServiceControllerComp:
			public imtclientgql::TClientRequestManagerCompWrap<
							sdl::V1_0::imtauth::CTenantCollectionDocumentServiceGqlHandlerCompBase>
{
public:
	typedef imtclientgql::TClientRequestManagerCompWrap<
					sdl::V1_0::imtauth::CTenantCollectionDocumentServiceGqlHandlerCompBase> BaseClass;

	I_BEGIN_COMPONENT(CRemoteTenantCollectionDocumentServiceControllerComp);
		I_ASSIGN(m_productInfoCompPtr, "ProductInfo", "Product info for loading available permissions", true, "ProductInfo");
	I_END_COMPONENT;

protected:
	// reimplemented (sdl::V1_0::imtauth::CTenantCollectionDocumentServiceGqlHandlerCompBase)
	virtual sdl::V1_0::imtauth::CTenantData OnGetTenantRepresentation(
				const sdl::V1_0::imtauth::CGetTenantRepresentationGqlRequest& getTenantRepresentationRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::V1_0::imtbase::CDocumentOperationStatus OnUpdateTenantFromRepresentation(
				const sdl::V1_0::imtauth::CUpdateTenantFromRepresentationGqlRequest& updateTenantFromRepresentationRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;

private:
	void CollectPermissionsTree(
				const imtlic::IFeatureInfo* featureInfoPtr,
				imtsdl::TElementList<sdl::V1_0::imtauth::CTenantPermissionOption>& permissions) const;

	I_REF(imtlic::IProductInfo, m_productInfoCompPtr);
};


} // namespace imtauthgql
