#pragma once


// ACF includes
#include <iprm/INameParam.h>

// ImtCore includes
#include <imtlic/ILicenseInfoProvider.h>


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
};


} // namespace imtlic


