#pragma once


// Qt includes
#include <QtCore/QDateTime>

// ACF includes
#include <iser/IObject.h>

// ImtCore includes
#include <imtlic/ILicenseInstanceProvider.h>


namespace imtbase
{
	class IObjectCollection;
}


namespace imtlic
{


class IProductInfo;


/**
	Common interface for a product instance. A product instance is the result of the product installation by the customer.
	A product instance can have a subset of the defined product licenses and is therefore a provider of actually available licenses for the current instance.
	There is no license validation logic yet at this level. It is only an overview of the registered licenses in the specified product installation.
	\ingroup LicenseManagement
*/
class IProductInstanceInfo: virtual public imtlic::ILicenseInstanceProvider, virtual public iser::IObject
{
public:
	/**
		Get access to the product database.
	*/
	virtual const imtbase::IObjectCollection* GetProductDatabase() const = 0;

	/**
		Get access to the customer database.
	*/
	virtual const imtbase::IObjectCollection* GetCustomerDatabase() const = 0;

	/**
		Setup the instance of a given product.
	*/
	virtual void SetupProductInstance(
				const QByteArray& productId,
				const QByteArray& instanceId,
				const QByteArray& customerId) = 0;

	/**
		Add a license to this product instance.
		\param licenseId	ID of the license to be added to the product. The license should be available in the license collection related to the product.
		\note Full information about the license is managed by the product licensing info, the license-ID is used only as a link to the license information.
		\sa IProductLicensingInfo
	*/
	virtual void AddLicense(const QByteArray& licenseId, const QDateTime& expirationDate = QDateTime()) = 0;

	/**
		Remove an existing license from this product instance.
	*/
	virtual void RemoveLicense(const QByteArray& licenseId) = 0;

	/**
		Get the unique-ID of this product instance.
	*/
	virtual QByteArray GetProductInstanceId() const = 0;

	/**
		Get related product-ID.
		Additional informations about the product can be accessed using this ID in the product database.
		\sa GetProductDatabase
	*/
	virtual QByteArray GetProductId() const = 0;

	/**
		Get customer-ID of this product instance.
		Additional informations about the customer can be accessed using this ID in the customer database.
		\sa GetCustomerDatabase
	*/
	virtual QByteArray GetCustomerId() const = 0;
};


} // namespace imtlic


