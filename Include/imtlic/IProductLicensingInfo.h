#pragma once


// ACF includes
#include <idoc/IDocumentMetaInfo.h>

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
	enum MetaInfoTypes
	{
		/**
		Product name.
		*/
		MIT_PRODUCT_NAME = idoc::IDocumentMetaInfo::MIT_USER + 1000,

		/**
			Product category.
		*/
		MIT_PRODUCT_CATEGORY_ID
	};

	/**
		Get all available feature packages.
	*/
	virtual const imtbase::IObjectCollection* GetFeaturePackages() const = 0;
};


} // namespace imtlic


