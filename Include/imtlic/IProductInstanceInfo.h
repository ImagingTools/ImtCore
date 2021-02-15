#pragma once


// ImtCore includes
#include <imtlic/IProductInfo.h>


namespace imtlic
{


/**
	Common interface for a product instance. A product instance is the result of the product installation by the customer.
	\ingroup LicenseManagement
*/
class IProductInstanceInfo: virtual public IProductInfo
{
public:
	/**
		Get the unique-ID of this product instance.
	*/
	virtual QByteArray GetProductInstanceId() const = 0;

	/**
		Set ID of the product instance.
	*/
	virtual void SetProductInstanceId(const QByteArray& instanceId) = 0;

	/**
		Get the customer-ID if available.
	*/
	virtual QByteArray GetCustomerId() const = 0;

	/**
		Set ID of the customer.
	*/
	virtual void SetCustomerId(const QByteArray& customerId) = 0;
};


} // namespace imtlic


