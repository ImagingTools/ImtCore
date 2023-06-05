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
	Common interface for a hardware product instance. A product instance is the result of the hardware installation by the customer.
	\ingroup LicenseManagement
*/
class IHardwareInstanceInfo : virtual public iser::IObject
{
public:
	/**
		Get a unique ID of this product.
	*/
	virtual QByteArray GetProductId() const = 0;

	/**
		Set product-ID.
	*/
	virtual void SetProductId(const QByteArray& productId) = 0;

	/**
		Get the device Id (identifacator of sensor in the table of sensors) of this product instance.
	*/
	virtual QByteArray GetDeviceId() const = 0;

	/**
		Set the device Id (identifacator of sensor in the table of sensors) of this product instance.
	*/
	virtual void SetDeviceId(const QByteArray& deviceId) = 0;

	/**
		Get the link to licensing software of this product instance.
	*/
	virtual QByteArray GetSoftwareId() const = 0;

	/**
		Set the link to licensing software of this product instance.
	*/
	virtual void SetSoftwareId(const QByteArray& softwareId) = 0;

	/**
		Get the model type ID of this product instance.
	*/
	virtual QByteArray GetModelTypeId() const = 0;

	/**
		Set the model type ID of this product instance.
	*/
	virtual void SetModelTypeId(const QByteArray& typeId) = 0;
};


} // namespace imtlic


