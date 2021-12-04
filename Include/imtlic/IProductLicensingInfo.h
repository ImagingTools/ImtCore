#pragma once


// ImtCore includes
#include <imtlic/ILicenseInfoProvider.h>
#include <imtlic/IProductInfo.h>


namespace imtbase
{
	class IObjectCollection;
}


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
		Get all available feature packages.
	*/
	virtual const imtbase::IObjectCollection* GetFeaturePackages() const = 0;
};


} // namespace imtlic


