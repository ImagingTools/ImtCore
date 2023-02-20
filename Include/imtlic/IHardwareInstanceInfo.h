#pragma once


// ImtCore includes
#include <imtlic/IProductInstanceInfo.h>
#include <imtlic/IPartProductionInfo.h>

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
};


} // namespace imtlic


