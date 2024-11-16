#include <imtlicgql/CProductControllerComp.h>


// ACF includes
#include <iprm/CIdParam.h>
#include <iprm/CParamsSet.h>

// ImtCore includes
#include <imtlic/CProductInfo.h>


namespace imtlicgql
{


// protected methods

sdl::imtlic::Products::V1_0::CProductsListPayload CProductControllerComp::OnProductsList(
	const sdl::imtlic::Products::V1_0::CProductsListGqlRequest& /*productsListRequest*/,
	const ::imtgql::CGqlRequest& /*gqlRequest*/,
	QString& /*errorMessage*/) const
{
	return sdl::imtlic::Products::V1_0::CProductsListPayload();
}


sdl::imtlic::Products::V1_0::CProductDataPayload CProductControllerComp::OnProductItem(
	const sdl::imtlic::Products::V1_0::CProductItemGqlRequest& /*productItemRequest*/,
	const ::imtgql::CGqlRequest& /*gqlRequest*/,
	QString& /*errorMessage*/) const
{
	return sdl::imtlic::Products::V1_0::CProductDataPayload();
}


sdl::imtlic::Products::V1_0::CProductExportResponse CProductControllerComp::OnProductsExport(
			const sdl::imtlic::Products::V1_0::CProductsExportGqlRequest& /*productsExportRequest*/,
			const ::imtgql::CGqlRequest& /*gqlRequest*/,
			QString& /*errorMessage*/) const
{
	return sdl::imtlic::Products::V1_0::CProductExportResponse();
}


::sdl::imtbase::ImtCollection::V1_0::CUpdatedNotificationPayload CProductControllerComp::OnProductUpdate(
	const sdl::imtlic::Products::V1_0::CProductUpdateGqlRequest& /*productUpdateRequest*/,
	const ::imtgql::CGqlRequest& /*gqlRequest*/,
	QString& /*errorMessage*/) const
{
	return ::sdl::imtbase::ImtCollection::V1_0::CUpdatedNotificationPayload();
}


::sdl::imtbase::ImtCollection::V1_0::CAddedNotificationPayload CProductControllerComp::OnProductAdd(
			const sdl::imtlic::Products::V1_0::CProductAddGqlRequest& /*productAddRequest*/,
			const ::imtgql::CGqlRequest& /*gqlRequest*/,
			QString& /*errorMessage*/) const
{
	return ::sdl::imtbase::ImtCollection::V1_0::CAddedNotificationPayload();
}


sdl::imtlic::Products::V1_0:: CProductImportResponse CProductControllerComp::OnProductsImport(
			const sdl::imtlic::Products::V1_0::CProductsImportGqlRequest& /*productsImportRequest*/,
			const ::imtgql::CGqlRequest& /*gqlRequest*/,
			QString& /*errorMessage*/) const
{
	return sdl::imtlic::Products::V1_0:: CProductImportResponse();
}


} // namespace imtlicgql


