// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <idoc/IDocumentMetaInfo.h>

// ImtCore includes
#include <imtlic/ILicenseInfoProvider.h>


namespace imtbase
{
	class IObjectCollection;
}


namespace imtlic
{


/**
	Interface for accessing all available licenses for a product.
	
	Product Licensing Info serves as the complete catalog of licensing options for a product.
	It provides access to all License Definitions that can be purchased and activated for
	the product.
	
	From a conceptual perspective:
	- A Product is a container of Features
	- Each License Definition is a container of Features
	- Product Licensing Info aggregates all License Definitions for a product
	- Therefore, it represents the complete licensing landscape for the product
	
	This interface is used to:
	- Browse available license types for a product
	- Retrieve license definitions for activation
	- Understand the complete set of licensing options
	
	\sa ILicenseDefinition, IProductInfo, ILicenseInfoProvider
	\ingroup LicenseManagement
*/
class IProductLicensingInfo: virtual public ILicenseInfoProvider
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
};


} // namespace imtlic


