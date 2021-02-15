#pragma once


// ACF includes
#include <istd/IChangeable.h>


namespace imtbase
{
	class ICollectionInfo;
}


namespace imtlic
{


class IProductLicensingInfo;


/**
	Interface for a collection of licensed products.
	\ingroup LicenseManagement
*/
class IProductLicensingInfoProvider: virtual public istd::IChangeable
{
public:
	virtual const imtbase::ICollectionInfo& GetProductList() const = 0;

	/**
		Get license information for a given product.
	*/
	virtual const imtlic::IProductLicensingInfo* GetProductLicenses(const QByteArray& productId) const = 0;
};


} // namespace imtlic


