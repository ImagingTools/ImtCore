#pragma once


// ImtCore includes
#include <imtlic/ILicenseInfoProvider.h>
#include <imtlic/IProductInfo.h>


namespace imtlic
{


/**
	Common interface for access all available licenses of a product.
	\ingroup LicenseManagement
*/
class IProductLicensingInfo: virtual public ILicenseInfoProvider, virtual public IProductInfo
{
public:
	/**
		Get the list of all available products.
	*/
	virtual const imtbase::ICollectionInfo* GetProductList() const = 0;
};


} // namespace imtlic


