// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <istd/IChangeable.h>
#include <idoc/IDocumentMetaInfo.h>


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
	enum MetaInfoTypes
	{
		/**
			List of license infos given as QStringList.
		*/
		MIT_LICENSES_INFO_LIST = idoc::IDocumentMetaInfo::MIT_USER + 1,
	};

	/**
		Get list of all available products.
	*/
	virtual const imtbase::ICollectionInfo& GetProductList() const = 0;

	/**
		Get license information for a given product. All available licenses are returned.
	*/
	virtual const IProductLicensingInfo* GetProductLicenses(const QByteArray& productId) const = 0;
};


} // namespace imtlic


