#pragma once


// ACF includes
#include <iser/IObject.h>

// ImtCore includes
#include <imtlic/ILicenseInfoProvider.h>


namespace imtlic
{


class IProductInfo;


/**
	Common interface for a product instance. A product instance is the result of the product installation by the customer.
	\ingroup LicenseManagement
*/
class IProductInstanceInfo: virtual public imtlic::ILicenseInfoProvider, virtual public iser::IObject
{
public:
	/**
		Setup the instance of a given product.
	*/
	virtual void SetupProductInstance(
				const imtlic::IProductInfo& product,
				const QByteArray& instanceId,
				const QByteArray& customerId) = 0;

	/**
		Get related product-ID.
	*/
	virtual QByteArray GetProductId() const = 0;

	/**
		Get the unique-ID of this product instance.
	*/
	virtual QByteArray GetProductInstanceId() const = 0;

	/**
		Get the customer-ID if available.
	*/
	virtual QByteArray GetCustomerId() const = 0;
};


} // namespace imtlic


