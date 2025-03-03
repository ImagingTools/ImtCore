#include <imtlicgql/CProductMetaInfoDelegateComp.h>


// ImtCore includes
#include <imtlic/IProductInfo.h>


namespace imtlicgql
{


// protected methods

bool CProductMetaInfoDelegateComp::FillRepresentation(sdl::imtlic::Products::CProductData::V1_0& metaInfoRepresentation, const idoc::IDocumentMetaInfo& metaInfo) const
{
	QByteArray productId = metaInfo.GetMetaInfo(imtlic::IProductInfo::MIT_PRODUCT_ID).toByteArray();
	metaInfoRepresentation.ProductId = productId;
	
	QString name = metaInfo.GetMetaInfo(imtlic::IProductInfo::MIT_PRODUCT_NAME).toString();
	metaInfoRepresentation.ProductName = name;
	
	QString description = metaInfo.GetMetaInfo(imtlic::IProductInfo::MIT_PRODUCT_DESCRIPTION).toString();
	metaInfoRepresentation.Description = description;
	
	QByteArray category = metaInfo.GetMetaInfo(imtlic::IProductInfo::MIT_PRODUCT_CATEGORY).toByteArray();
	metaInfoRepresentation.CategoryId = category;
	
	return true;
}


bool CProductMetaInfoDelegateComp::FillMetaInfo(idoc::IDocumentMetaInfo& metaInfo, const sdl::imtlic::Products::CProductData::V1_0& metaInfoRepresentation) const
{
	if (metaInfoRepresentation.ProductId){
		metaInfo.SetMetaInfo(imtlic::IProductInfo::MIT_PRODUCT_ID, *metaInfoRepresentation.ProductId);
	}
	
	if (metaInfoRepresentation.ProductName){
		metaInfo.SetMetaInfo(imtlic::IProductInfo::MIT_PRODUCT_NAME, *metaInfoRepresentation.ProductName);
	}
	
	if (metaInfoRepresentation.Description){
		metaInfo.SetMetaInfo(imtlic::IProductInfo::MIT_PRODUCT_DESCRIPTION, *metaInfoRepresentation.Description);
	}
	
	if (metaInfoRepresentation.CategoryId){
		metaInfo.SetMetaInfo(imtlic::IProductInfo::MIT_PRODUCT_CATEGORY, *metaInfoRepresentation.CategoryId);
	}
	
	return true;
}


} // namespace imtlicgql


