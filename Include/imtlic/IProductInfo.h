#pragma once


// ACF includes
#include <iprm/INameParam.h>

// ImtCore includes
#include <imtbase/IObjectCollection.h>
#include <imtlic/IFeatureInfoProvider.h>


namespace imtlic
{


/**
	Common interface for access information about a product.
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
		Get feature collection.
	*/
	virtual imtbase::IObjectCollection* GetFeatures() = 0;

	/**
		Insert sub feature.
	*/
	virtual bool AddFeature(const QByteArray& featureId, const IFeatureInfo& featureInfo) = 0;

	/**
		Remove sub feature.
	*/
	virtual bool RemoveFeature(const QByteArray& featureId) = 0;
};


} // namespace imtlic


