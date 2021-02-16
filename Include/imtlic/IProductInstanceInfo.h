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
	A product instance can have a subset of the defined product licenses and is therefore a provider of actually available licenses for the current instance.
	There is no license validation logic yet at this level. It is only an overview of the registered licenses in the specified product installation.
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
		Add a license to this product instance.
		\param licenseId	ID of the license to be added to the product. The license should be available in the license collection related to the product.
		\note Full information about the license is managed by the product licensing info, the license-ID is used only as a link to the license information.
		\sa IProductLicensingInfo
	*/
	virtual void AddLicense(const QByteArray& licenseId) = 0;

	/**
		Remove an existing license from this product instance.
	*/
	virtual void RemoveLicense(const QByteArray& licenseId) = 0;

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


