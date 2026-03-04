// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
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


