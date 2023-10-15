#pragma once


// ACF includes
#include <iprm/INameParam.h>

// ImtCore includes
#include <imtlic/ILicenseInfoProvider.h>
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
		Get features includes in this product.
	*/
	virtual QByteArrayList GetFeatureIds() const = 0;

	/**
		Set features.
	*/
	virtual void SetFeatureIds(QByteArrayList featureIds) = 0;

	/**
		Get feature info provider.
	*/
	virtual IFeatureInfoProvider* GetFeatureInfoProvider() const = 0;
};


} // namespace imtlic


