// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtlic/CProductCollection.h>


namespace imtlic
{


// public methods

CProductCollection::CProductCollection()
	:BaseClass("ProductLicensingInfo", "Product Licensing", "ProductLicensingInfo")
{
}


// reimplemented (imtbase::IProductLicensingInfoProvider)

const imtbase::ICollectionInfo& CProductCollection::GetProductList() const
{
	return m_collection;
}


const imtlic::IProductLicensingInfo* CProductCollection::GetProductLicenses(const QByteArray& productId) const
{
	return dynamic_cast<const imtlic::IProductLicensingInfo*>(m_collection.GetObjectPtr(productId));
}


} // namespace imtlic


