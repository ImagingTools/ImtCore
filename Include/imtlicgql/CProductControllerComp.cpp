#include <imtlicgql/CProductControllerComp.h>


// ACF includes
#include <iprm/CIdParam.h>
#include <iprm/CParamsSet.h>

// ImtCore includes
#include <imtlic/CProductInfo.h>


namespace imtlicgql
{


// protected methods


sdl::imtbase::ImtCollection::CVisualStatus CProductControllerComp::OnGetObjectVisualStatus(
	const sdl::imtlic::Products::CGetObjectVisualStatusGqlRequest& /*getObjectVisualStatusRequest*/,
	const ::imtgql::CGqlRequest& /*gqlRequest*/,
	QString& /*errorMessage*/) const
{
	return sdl::imtbase::ImtCollection::CVisualStatus();
}


sdl::imtlic::Products::CProductExportResponse CProductControllerComp::OnProductsExport(
			const sdl::imtlic::Products::CProductsExportGqlRequest& /*productsExportRequest*/,
			const ::imtgql::CGqlRequest& /*gqlRequest*/,
			QString& /*errorMessage*/) const
{
	return sdl::imtlic::Products::CProductExportResponse();
}


sdl::imtlic::Products::CProductImportResponse CProductControllerComp::OnProductsImport(
			const sdl::imtlic::Products::CProductsImportGqlRequest& /*productsImportRequest*/,
			const ::imtgql::CGqlRequest& /*gqlRequest*/,
			QString& /*errorMessage*/) const
{
	return sdl::imtlic::Products::CProductImportResponse();
}


} // namespace imtlicgql


