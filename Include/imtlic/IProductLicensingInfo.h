#pragma once


// ImtCore includes
#include <imtlic/ILicenseInfoProvider.h>
#include <imtlic/IProductInfo.h>


namespace imtlic
{



/**
	Common interface for access available licenses of a product.
	\ingroup LicenseManagement
*/
class IProductLicensingInfo: virtual public ILicenseInfoProvider, virtual public IProductInfo
{
public:
};


} // namespace imtlic


