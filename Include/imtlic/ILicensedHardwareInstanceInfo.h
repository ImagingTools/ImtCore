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
class ILicensedHardwareInstanceInfo : virtual public IPartProductionInfo //, IProductInstanceInfo
{
public:
	enum MetaInfoTypes
	{
		/**
			Serial number of the product..
		*/
		MIT_SERIAL_NUMBER = idoc::IDocumentMetaInfo::MIT_USER + 2000,

	};

	/**
		Get the serial number of this product instance.
	*/
	virtual QByteArray GetSerialNumber() const = 0;

	/**
		Set the serial number of this product instance.
	*/
	virtual void SetSerialNumber(const QByteArray& serialNumber) = 0;

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


