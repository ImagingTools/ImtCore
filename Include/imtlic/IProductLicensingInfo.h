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
	Common interface for access all available licenses of a product.
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


