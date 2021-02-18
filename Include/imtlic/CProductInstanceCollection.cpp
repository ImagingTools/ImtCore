#include <imtlic/CProductInstanceCollection.h>


namespace imtlic
{


// public methods

CProductInstanceCollection::CProductInstanceCollection()
	:BaseClass("ProductInstanceInfo", "Product Instance", "ProductInstances")
{
}


// reimplemented (imtbase::IProductLicensingInfoProvider)

const imtbase::ICollectionInfo& CProductInstanceCollection::GetProductInstanceList() const
{
	return m_collection;
}


const imtlic::IProductInstanceInfo* CProductInstanceCollection::GetProductInstance(const QByteArray& instanceId) const
{
	return dynamic_cast<const imtlic::IProductInstanceInfo*>(m_collection.GetObjectPtr(instanceId));
}


} // namespace imtlic


