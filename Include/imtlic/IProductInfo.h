// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <iprm/INameParam.h>

// ImtCore includes
#include <imtbase/IObjectCollection.h>
#include <imtlic/IFeatureInfoProvider.h>


namespace imtlic
{


/**
	Interface for accessing information about a product (software or hardware).
	
	A Product represents a software or hardware offering and is fundamentally defined as
	a collection of Features. From a licensing perspective, a Product also contains all
	available License Definitions, where each license "unlocks" a specific set of features.
	
	Key responsibilities:
	- Maintains product identification (ID, name, description, category)
	- Manages the collection of Features that define the product's capabilities
	- Can be part of a product family hierarchy
	
	\sa IFeatureInfo, ILicenseDefinition, IProductInstanceInfo
	\ingroup LicenseManagement
*/
class IProductInfo: virtual public iprm::INameParam
{
public:
	enum MetaInfoTypes
	{
		/**
			Product Name given as QString.
		*/
		MIT_PRODUCT_NAME = idoc::IDocumentMetaInfo::MIT_USER,

		/**
			Product-ID given as QByteArray.
		*/
		MIT_PRODUCT_ID,

		/**
			Product Description given as QString.
		*/
		MIT_PRODUCT_DESCRIPTION,

		/**
			Product category given as QByteArray.
		*/
		MIT_PRODUCT_CATEGORY
	};

	/**
		Get information about the related product family of this product, if available.
	*/
	virtual const IProductInfo* GetProductFamilyInfo() const = 0;

	/**
		Get a unique ID of this product.
	*/
	virtual QByteArray GetProductId() const = 0;

	/**
		Set product-ID.
	*/
	virtual void SetProductId(const QByteArray& productId) = 0;

	/**
		Get a description of this product.
	*/
	virtual QString GetProductDescription() const = 0;

	/**
		Set product description.
	*/
	virtual void SetProductDescription(const QString& description) = 0;

	/**
		Get a category ID of this product.
	*/
	virtual QByteArray GetCategoryId() const = 0;

	/**
		Set category-ID.
	*/
	virtual void SetCategoryId(const QByteArray& categoryId) = 0;

	/**
		Get the collection of features that define this product's capabilities.
		A Product is essentially a container of Features.
	*/
	virtual imtbase::IObjectCollection* GetFeatures() = 0;

	/**
		Add a feature to this product's feature collection.
		\param featureId Unique identifier for the feature
		\param featureInfo Feature information to be added
		\return true if the feature was successfully added, false otherwise
	*/
	virtual bool AddFeature(const QByteArray& featureId, const IFeatureInfo& featureInfo) = 0;

	/**
		Remove a feature from this product's feature collection.
		\param featureId Unique identifier of the feature to remove
		\return true if the feature was successfully removed, false otherwise
	*/
	virtual bool RemoveFeature(const QByteArray& featureId) = 0;
};


typedef istd::TUniqueInterfacePtr<IProductInfo> IProductInfoUniquePtr;
typedef istd::TSharedInterfacePtr<IProductInfo> IProductInfoSharedPtr;


} // namespace imtlic


