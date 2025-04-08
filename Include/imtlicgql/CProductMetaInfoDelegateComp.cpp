#include <imtlicgql/CProductMetaInfoDelegateComp.h>


// ImtCore includes
#include <imtlic/IProductInfo.h>


namespace imtlicgql
{


// protected methods

bool CProductMetaInfoDelegateComp::FillRepresentation(sdl::imtlic::Products::CProductData::V1_0& metaInfoRepresentation, const idoc::IDocumentMetaInfo& metaInfo) const
{
	QByteArray productId = metaInfo.GetMetaInfo(imtlic::IProductInfo::MIT_PRODUCT_ID).toByteArray();
	metaInfoRepresentation.productId = productId;
	
	QString name = metaInfo.GetMetaInfo(imtlic::IProductInfo::MIT_PRODUCT_NAME).toString();
	metaInfoRepresentation.productName = name;
	
	QString description = metaInfo.GetMetaInfo(imtlic::IProductInfo::MIT_PRODUCT_DESCRIPTION).toString();
	metaInfoRepresentation.description = description;
	
	QByteArray category = metaInfo.GetMetaInfo(imtlic::IProductInfo::MIT_PRODUCT_CATEGORY).toByteArray();
	metaInfoRepresentation.categoryId = category;
	
	return true;
}


bool CProductMetaInfoDelegateComp::FillMetaInfo(idoc::IDocumentMetaInfo& metaInfo, const sdl::imtlic::Products::CProductData::V1_0& metaInfoRepresentation) const
{
	if (metaInfoRepresentation.productId){
		metaInfo.SetMetaInfo(imtlic::IProductInfo::MIT_PRODUCT_ID, *metaInfoRepresentation.productId);
	}
	
	if (metaInfoRepresentation.productName){
		metaInfo.SetMetaInfo(imtlic::IProductInfo::MIT_PRODUCT_NAME, *metaInfoRepresentation.productName);
	}
	
	if (metaInfoRepresentation.description){
		metaInfo.SetMetaInfo(imtlic::IProductInfo::MIT_PRODUCT_DESCRIPTION, *metaInfoRepresentation.description);
	}
	
	if (metaInfoRepresentation.categoryId){
		metaInfo.SetMetaInfo(imtlic::IProductInfo::MIT_PRODUCT_CATEGORY, *metaInfoRepresentation.categoryId);
	}
	
	return true;
}


} // namespace imtlicgql


