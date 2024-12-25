#include <imtlicgql/CProductControllerComp.h>


// ACF includes
#include <iprm/CIdParam.h>
#include <iprm/CParamsSet.h>

// ImtCore includes
#include <imtlic/CProductInfo.h>


namespace imtlicgql
{


// protected methods


sdl::imtbase::ImtCollection::CVisualStatus::V1_0 CProductControllerComp::OnGetObjectVisualStatus(
	const sdl::imtlic::Products::V1_0::CGetObjectVisualStatusGqlRequest& getObjectVisualStatusRequest,
	const ::imtgql::CGqlRequest& gqlRequest,
	QString& errorMessage) const
{
	return sdl::imtbase::ImtCollection::CVisualStatus::V1_0();
}


sdl::imtlic::Products::CProductsListPayload::V1_0 CProductControllerComp::OnProductsList(
	const sdl::imtlic::Products::V1_0::CProductsListGqlRequest& /*productsListRequest*/,
	const ::imtgql::CGqlRequest& /*gqlRequest*/,
	QString& /*errorMessage*/) const
{
	return sdl::imtlic::Products::CProductsListPayload::V1_0();
}


sdl::imtlic::Products::CProductDataPayload::V1_0 CProductControllerComp::OnProductItem(
	const sdl::imtlic::Products::V1_0::CProductItemGqlRequest& /*productItemRequest*/,
	const ::imtgql::CGqlRequest& /*gqlRequest*/,
	QString& /*errorMessage*/) const
{
	return sdl::imtlic::Products::CProductDataPayload::V1_0();
}


sdl::imtlic::Products::CProductExportResponse::V1_0 CProductControllerComp::OnProductsExport(
			const sdl::imtlic::Products::V1_0::CProductsExportGqlRequest& /*productsExportRequest*/,
			const ::imtgql::CGqlRequest& /*gqlRequest*/,
			QString& /*errorMessage*/) const
{
	return sdl::imtlic::Products::CProductExportResponse::V1_0();
}


::sdl::imtbase::ImtCollection::CUpdatedNotificationPayload::V1_0 CProductControllerComp::OnProductUpdate(
	const sdl::imtlic::Products::V1_0::CProductUpdateGqlRequest& /*productUpdateRequest*/,
	const ::imtgql::CGqlRequest& /*gqlRequest*/,
	QString& /*errorMessage*/) const
{
	return ::sdl::imtbase::ImtCollection::CUpdatedNotificationPayload::V1_0();
}


::sdl::imtbase::ImtCollection::CAddedNotificationPayload::V1_0 CProductControllerComp::OnProductAdd(
			const sdl::imtlic::Products::V1_0::CProductAddGqlRequest& /*productAddRequest*/,
			const ::imtgql::CGqlRequest& /*gqlRequest*/,
			QString& /*errorMessage*/) const
{
	return ::sdl::imtbase::ImtCollection::CAddedNotificationPayload::V1_0();
}


sdl::imtlic::Products::CProductImportResponse::V1_0 CProductControllerComp::OnProductsImport(
			const sdl::imtlic::Products::V1_0::CProductsImportGqlRequest& /*productsImportRequest*/,
			const ::imtgql::CGqlRequest& /*gqlRequest*/,
			QString& /*errorMessage*/) const
{
	return sdl::imtlic::Products::CProductImportResponse::V1_0();
}


} // namespace imtlicgql


