#pragma once


// ImtCore includes
#include <imtdev/IDeviceAccessor.h>
#include <imtdev/IDeviceEnumerator.h>
#include <imtdev/IDeviceInstanceInfo.h>


namespace imtbase
{
	class ICollectionInfo;
}


namespace imtdev
{


class IDeviceStaticInfo;


/**
	Common interface for controlling a device.
	Devices are abstract operating units having internal processing loop.
	Devices can be opened, closed and also used for executing external commands.
	Controller enumerates, opens, closes devices, accesses static information about devices,
	as well as information about the device instances.
*/
class IDeviceController: virtual public IDeviceEnumerator
{
public:
	/**
		Runtime state of a device.
	*/
	enum DeviceState
	{
		/**
			No information about the device state is available.
		*/
		DS_NONE,

		/**
			Device is available but not running.
		*/
		DS_INACTIVE,

		/**
			Processing loop of the device is running.
			In this state the device can execute commands.
		*/
		DS_ACTIVE
	};


	/**
		Get IDs of device types supported by the controller.
	*/
	virtual QByteArrayList GetSupportedDeviceTypeIds() const = 0;

	/**
		Get device static info for the given DeviceTypeId.
	*/
	virtual const IDeviceStaticInfo* GetDeviceStaticInfo(const QByteArray& deviceTypeId) const = 0;

	/**
		Get the list of available devices.
	*/
	virtual const imtbase::ICollectionInfo& GetAvailableDeviceList() const = 0;

	/**
		Get the current state of the device.
	*/
	virtual DeviceState GetDeviceState(const QByteArray& deviceId) const = 0;

	/**
		Get device instance info for the given DeviceId.
		For some devices DeviceInstanceInfo can only be obtained for open devices
	*/
	virtual DeviceInstanceInfoPtr GetDeviceInstanceInfo(const QByteArray& deviceId) const = 0;

	/**
		Start processing loop of the given device. The state of the device will be switched into DS_ACTIVE, if the operation was successful.
	*/
	virtual DeviceAccessorPtr OpenDevice(const QByteArray& deviceId, const iprm::IParamsSet* paramsPtr) = 0;

	/**
		Stop processing loop of the given device. The device will be in the \c DS_INACTIVE state, if the operation was successful.
	*/
	virtual bool CloseDevice(const QByteArray& deviceId) = 0;
};


} // namespace imtdev


