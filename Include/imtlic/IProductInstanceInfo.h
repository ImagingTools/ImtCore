// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
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
	Interface for a product instance.
	
	A Product Instance is created when a product is sold, installed, or deployed to a customer.
	It represents a specific installation of a product and contains:
	- The relationship to the product definition (via product ID)
	- Customer information (via customer ID)
	- Instance-specific identification (instance ID, serial number)
	- A collection of activated License Instances
	
	The Product Instance acts as a provider of actually available licenses for this specific
	installation. It maintains which licenses have been purchased and activated, but does not
	perform license validation logic at this level - it provides an overview of registered
	licenses in the product installation.
	
	Key responsibilities:
	- Links to product and customer databases
	- Manages activated License Instances with their expiration dates
	- Tracks instance-specific metadata (serial number, project, usage status)
	- Provides access to all licenses available in this installation
	
	\sa IProductInfo, ILicenseInstance, ILicenseInstanceProvider
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
		MIT_PRODUCT_COUNT,
		MIT_PARENT_INSTANCE_ID
	};

	/**
		Get access to the product database.
		The product database contains definitions of all available products.
		\return Pointer to the product database collection
	*/
	virtual const imtbase::IObjectCollection* GetProductDatabase() const = 0;

	/**
		Get access to the customer database.
		The customer database contains information about all customers.
		\return Pointer to the customer database collection
	*/
	virtual const imtbase::IObjectCollection* GetCustomerDatabase() const = 0;

	/**
		Set up this instance for a given product.
		This establishes the relationship between the instance and its product/customer.
		\param productId Logical ID of the product (not a repository/database ID)
		\param instanceId Unique identifier for this product instance (e.g., MAC address, hardware ID)
		\param customerId Logical ID of the customer (not a repository/database ID)
	*/
	virtual void SetupProductInstance(
				const QByteArray& productId,
				const QByteArray& instanceId,
				const QByteArray& customerId) = 0;

	/**
		Add an activated license to this product instance.
		\param licenseId ID of the license to activate (must exist in the product's license definitions)
		\param expirationDate Optional expiration date for the license (invalid QDateTime means no expiration)
		\note Full license information is managed by IProductLicensingInfo; the license ID serves as a reference
		\sa IProductLicensingInfo
	*/
	virtual void AddLicense(const QByteArray& licenseId, const QDateTime& expirationDate = QDateTime()) = 0;

	/**
		Remove an activated license from this product instance.
		\param licenseId ID of the license to deactivate
	*/
	virtual void RemoveLicense(const QByteArray& licenseId) = 0;

	/**
		Remove all activated licenses from this product instance.
	*/
	virtual void ClearLicenses() = 0;

	/**
		Get the unique ID of this product instance.
		This ID uniquely identifies this specific installation (e.g., MAC address, hardware ID).
		\return Unique instance identifier
	*/
	virtual QByteArray GetProductInstanceId() const = 0;

	/**
		Set the unique ID of this product instance.
		\param instanceId Unique identifier for this installation
	*/
	virtual void SetProductInstanceId(const QByteArray& instanceId) = 0;

	/**
		Get the ID of the product that this is an instance of.
		Use this ID to look up additional product information in the product database.
		\return Product ID
		\sa GetProductDatabase
	*/
	virtual QByteArray GetProductId() const = 0;

	/**
		Get the ID of the customer who owns this product instance.
		Use this ID to look up additional customer information in the customer database.
		\return Customer ID
		\sa GetCustomerDatabase
	*/
	virtual QByteArray GetCustomerId() const = 0;

	/**
		Get the serial number of this product instance.
		\return Serial number
	*/
	virtual QByteArray GetSerialNumber() const = 0;

	/**
		Set the serial number of this product instance.
		\param serialNumber Serial number to assign
	*/
	virtual void SetSerialNumber(const QByteArray& serialNumber) = 0;

	/**
		Get the project this product instance is assigned to.
		\return Project identifier
	*/
	virtual QByteArray GetProject() const = 0;

	/**
		Set the project assignment for this product instance.
		\param project Project identifier
	*/
	virtual void SetProject(const QByteArray& project) = 0;

	/**
		Check if this product instance is currently in use.
		\return true if the instance is active/in-use, false otherwise
	*/
	virtual bool IsInUse() const = 0;

	/**
		Set the usage status for this product instance.
		\param inUse true to mark as in-use, false otherwise
	*/
	virtual void SetInUse(bool inUse) = 0;

	/**
		Check if this product instance is marked for internal use only.
		Internal-use instances are typically for testing or company-internal purposes.
		\return true if marked for internal use only
	*/
	virtual bool IsInternalUse() const = 0;

	/**
		Set whether this product instance is for internal use only.
		\param internalUse true to mark for internal use only
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

	/**
		Get the ID of the parent product instance.
		For multi-product instances that distribute licenses to other accounts,
		this references the parent instance that owns the original licenses.
		\return Parent instance ID, or empty if this is a root instance
	*/
	virtual QByteArray GetParentInstanceId() const = 0;

	/**
		Set the ID of the parent product instance.
		This establishes a parent-child relationship for license distribution.
		\param parentInstanceId ID of the parent instance, or empty to mark as root instance
	*/
	virtual void SetParentInstanceId(const QByteArray& parentInstanceId) = 0;
};


} // namespace imtlic
