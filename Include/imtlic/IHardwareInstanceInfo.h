// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <iser/IObject.h>


namespace imtbase
{
	class IObjectCollection;
}


namespace imtlic
{


class IProductInfo;


/**
	Interface for a hardware product instance.
	
	This interface represents an instance of a hardware product that has been
	installed or deployed. Hardware instances are linked to software products
	through the licensing system.
	
	\sa ILicensedHardwareInstanceInfo, IProductInfo
	\ingroup LicenseManagement
*/
class IHardwareInstanceInfo: virtual public iser::IObject
{
public:
	/**
		Get the unique product ID of this hardware instance.
		\return Product ID identifying the hardware product type
	*/
	virtual QByteArray GetProductId() const = 0;

	/**
		Set the product ID for this hardware instance.
		\param productId Product identifier
	*/
	virtual void SetProductId(const QByteArray& productId) = 0;

	/**
		Get the ID of the linked software product.
		Hardware products are often licensed together with software products.
		\return Software product ID
	*/
	virtual QByteArray GetSoftwareId() const = 0;

	/**
		Set the ID of the linked software product.
		\param softwareId Software product identifier
	*/
	virtual void SetSoftwareId(const QByteArray& softwareId) = 0;

	/**
		Get the model type ID of this hardware instance.
		Different hardware models may have different licensing requirements.
		\return Model type identifier
	*/
	virtual QByteArray GetModelTypeId() const = 0;

	/**
		Set the model type ID for this hardware instance.
		\param typeId Model type identifier
	*/
	virtual void SetModelTypeId(const QByteArray& typeId) = 0;
};


} // namespace imtlic


