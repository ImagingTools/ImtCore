#pragma once


// ImtCore includes
#include <imtservergql/CObjectCollectionControllerCompBase.h>
#include <imtlic/IProductLicensingInfo.h>
#include <GeneratedFiles/imtlicsdl/SDL/1.0/CPP/Products.h>


namespace imtlicgql
{


class CProductControllerComp: public sdl::imtlic::Products::V1_0::CGraphQlHandlerCompBase
{
public:
	typedef sdl::imtlic::Products::V1_0::CGraphQlHandlerCompBase BaseClass;

	I_BEGIN_COMPONENT(CProductControllerComp);
	I_END_COMPONENT;

protected:
	// reimplemented (sdl::imtlic::Products::V1_0::CGraphQlHandlerCompBase)
	virtual sdl::imtlic::Products::V1_0::CProductsListPayload OnProductsList(
				const sdl::imtlic::Products::V1_0::CProductsListGqlRequest& productsListRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;

	virtual sdl::imtlic::Products::V1_0::CProductDataPayload OnProductItem(
				const sdl::imtlic::Products::V1_0::CProductItemGqlRequest& productItemRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;

	virtual sdl::imtlic::Products::V1_0::CProductExportResponse OnProductsExport(
				const sdl::imtlic::Products::V1_0::CProductsExportGqlRequest& productsExportRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;

	virtual ::sdl::imtbase::ImtCollection::V1_0::CUpdatedNotificationPayload OnProductUpdate(
				const sdl::imtlic::Products::V1_0::CProductUpdateGqlRequest& productUpdateRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;

	virtual ::sdl::imtbase::ImtCollection::V1_0::CAddedNotificationPayload OnProductAdd(
				const sdl::imtlic::Products::V1_0::CProductAddGqlRequest& productAddRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;

	virtual sdl::imtlic::Products::V1_0:: CProductImportResponse OnProductsImport(
				const sdl::imtlic::Products::V1_0::CProductsImportGqlRequest& productsImportRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
};


} // namespace imtlicgql


