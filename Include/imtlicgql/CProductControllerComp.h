#pragma once


// ImtCore includes
#include <imtservergql/CObjectCollectionControllerCompBase.h>
#include <imtlic/IProductLicensingInfo.h>
#include <GeneratedFiles/imtlicsdl/SDL/1.0/CPP/Products.h>
// #include <GeneratedFiles/imtbasesdl//SDL/1.0/CPP/ImtCollection.h>


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
	virtual sdl::imtbase::ImtCollection::CVisualStatus::V1_0 OnGetObjectVisualStatus(
				const sdl::imtlic::Products::V1_0::CGetObjectVisualStatusGqlRequest& getObjectVisualStatusRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::imtlic::Products::CProductExportResponse::V1_0 OnProductsExport(
				const sdl::imtlic::Products::V1_0::CProductsExportGqlRequest& productsExportRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::imtlic::Products::CProductImportResponse::V1_0 OnProductsImport(
				const sdl::imtlic::Products::V1_0::CProductsImportGqlRequest& productsImportRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
};


} // namespace imtlicgql


