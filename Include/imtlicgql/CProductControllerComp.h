#pragma once


// ImtCore includes
#include <imtservergql/CObjectCollectionControllerCompBase.h>
#include <imtlic/IProductLicensingInfo.h>
#include <GeneratedFiles/imtlicsdl/SDL/1.0/CPP/Products.h>
// #include <GeneratedFiles/imtbasesdl//SDL/1.0/CPP/ImtCollection.h>


namespace imtlicgql
{


class CProductControllerComp: public sdl::imtlic::Products::CGraphQlHandlerCompBase
{
public:
	typedef sdl::imtlic::Products::CGraphQlHandlerCompBase BaseClass;

	I_BEGIN_COMPONENT(CProductControllerComp);
	I_END_COMPONENT;

protected:
	// reimplemented (sdl::imtlic::Products::CGraphQlHandlerCompBase)
	virtual sdl::imtlic::Products::CProductExportResponse OnProductsExport(
				const sdl::imtlic::Products::CProductsExportGqlRequest& productsExportRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::imtlic::Products::CProductImportResponse OnProductsImport(
				const sdl::imtlic::Products::CProductsImportGqlRequest& productsImportRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
};


} // namespace imtlicgql


