#pragma once


// Qt includes
#include <QtCore/QDateTime>

// ACF includes
#include <iser/IObject.h>
#include <idoc/IDocumentMetaInfo.h>

// ImtCore includes
#include <imtbase/IObjectCollection.h>
#include <imtlic/ILicenseInstanceProvider.h>


namespace imtbase
{
//	class IObjectCollection;
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
class IProductInstanceInfo: virtual public ILicenseInstanceProvider, virtual public iser::IObject
{
public:
	enum MetaInfoTypes
	{
		MIT_PRODUCT_INSTANCE_ID = idoc::IDocumentMetaInfo::MIT_USER + 1000,
		MIT_CUSTOMER_NAME,
		MIT_PRODUCT_ID,
		MIT_PRODUCT_CATEGORY_ID,
		MIT_PROJECT,
		MIT_SERIAL_NUMBER,
		MIT_CUSTOMER_ID,
		MIT_IN_USE,
		MIT_PRODUCT_UUID,
		MIT_PRODUCT_NAME,
		MIT_LICENSE_UUID,
		MIT_LICENSE_ID,
		MIT_LICENSE_NAME,
		MIT_ORDER_ID,
		MIT_DELIVERY_ID,
		MIT_PURCHASE_ID,
		MIT_HARDWARE_ID,
		MIT_HARDWARE_MAC_ADDRESS,
		MIT_IS_PAIRED,
		MIT_INTERNAL_USE,
		MIT_IS_MULTI_PRODUCT,
		MIT_PRODUCT_COUNT
	};

	/**
		Get access to the product database.
	*/
	virtual const imtbase::IObjectCollection* GetProductDatabase() const = 0;

	/**
		Get access to the customer database.
	*/
	virtual const imtbase::IObjectCollection* GetCustomerDatabase() const = 0;

	/**
		Setups the instance of a given product.
		\param productId	Logical ID of the product (not some repository ID!)
		\param instanceId	A unique identifier for the product instance. For example - it can be a MAC-address of some hardware device.
		\param customerId	Logical ID of the product (not some repository ID!)
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
		Remove all existing licenses from this product instance.
	*/
	virtual void ClearLicenses() = 0;

	/**
		Get the unique-ID of this product instance.
	*/
	virtual QByteArray GetProductInstanceId() const = 0;

	/**
		Set the unique-ID of this product instance.
	*/
	virtual void SetProductInstanceId(const QByteArray& instanceId) = 0;

	/**
		Get related product-ID.
		Additional information about the product can be accessed using this ID in the product database.
		\sa GetProductDatabase
	*/
	virtual QByteArray GetProductId() const = 0;

	/**
		Get customer-ID of this product instance.
		Additional information about the customer can be accessed using this ID in the customer database.
		\sa GetCustomerDatabase
	*/
	virtual QByteArray GetCustomerId() const = 0;

	/**
		Get the serial number of this product instance.
	*/
	virtual QByteArray GetSerialNumber() const = 0;

	/**
		Set the serial number of this product instance.
	*/
	virtual void SetSerialNumber(const QByteArray& serialNumber) = 0;

	/**
		Get the project of this product instance.
	*/
	virtual QByteArray GetProject() const = 0;

	/**
		Set the project of this product instance.
	*/
	virtual void SetProject(const QByteArray& project) = 0;

	/**
		Check this product instance is in use.
	*/
	virtual bool IsInUse() const = 0;

	/**
		Set in use for this product instance.
	*/
	virtual void SetInUse(bool inUse) = 0;

	/**
		Checks whether this product instance is marked for internal use only.
	*/
	virtual bool IsInternalUse() const = 0;

	/**
		Sets whether this product instance is for internal use only.
	*/
	virtual void SetInternalUse(bool internalUse) = 0;

	/**
		Checks whether this product instance represents multiple items (multi-product).
	*/
	virtual bool IsMultiProduct() const = 0;

	/**
		Sets whether this product instance represents multiple items.
	*/
	virtual void SetMultiProduct(bool isMulti) = 0;

	/**
		Get the count of items in this product instance.
		Valid only if IsMultiProduct() returns true.
	*/
	virtual int GetProductCount() const = 0;

	/**
		Set the count of items in this product instance.
		This usually makes sense only if the product is marked as multi-product.
	*/
	virtual void SetProductCount(int count) = 0;
};


} // namespace imtlic
