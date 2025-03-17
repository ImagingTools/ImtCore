#pragma once


// ImtCore includes
#include <imtdev/IDeviceInstanceInfo.h>


namespace imtbase
{
	class ICollectionInfo;
}


namespace imtdev
{


class IDeviceStaticInfo;


class ICompositeDeviceInfo: virtual public istd::IChangeable
{
public:
	/**
		Get device static info.
	*/
	virtual const IDeviceStaticInfo& GetDeviceStaticInfo() const = 0; // Type: 'Agent'

	/**
		Get device instance info.
	*/
	virtual DeviceInstanceInfoPtr GetDeviceInstanceInfo() const = 0; // IP-Address

	/**
		Get the list of available devices.
	*/
	virtual const imtbase::ICollectionInfo& GetSubDeviceList() const = 0; // List of measurement devices

	/**
		Get sub-device information.
	*/
	virtual const ICompositeDeviceInfo* GetSubDeviceInfo(const QByteArray& subDeviceId) const = 0; // Single measurement device
};


} // namespace imtdev


