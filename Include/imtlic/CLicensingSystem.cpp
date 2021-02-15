#include <imtlic/CLicensingSystem.h>


namespace imtlic
{


// public methods

CLicensingSystem::CLicensingSystem()
	:BaseClass("ProductLicensingInfo", "Product Licensing", "ProductLicenses")
{
}


// reimplemented (imtbase::IProductLicensingInfoProvider)

const imtbase::ICollectionInfo& CLicensingSystem::GetProductList() const
{
	return m_collection;
}


const imtlic::IProductLicensingInfo* CLicensingSystem::GetProductLicenses(const QByteArray & productId) const
{
	return dynamic_cast<const imtlic::IProductLicensingInfo*>(m_collection.GetObjectPtr(productId));
}


} // namespace imtlic


