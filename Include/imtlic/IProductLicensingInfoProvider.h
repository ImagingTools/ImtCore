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
	/**
		Get list of all available products.
	*/
	virtual const imtbase::ICollectionInfo& GetProductList() const = 0;

	/**
		Get license information for a given product. All available licenses are returned.
	*/
	virtual const imtlic::IProductLicensingInfo* GetProductLicenses(const QByteArray& productId) const = 0;
};


} // namespace imtlic


