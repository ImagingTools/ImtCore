#pragma once


// ImtCore includes
#include <imtdev/IDeviceAccessor.h>
#include <imtdev/IDeviceEnumerator.h>
#include <imtdev/IDeviceInstance.h>
#include <imtdev/IDeviceStateProvider.h>


namespace imtbase
{
	class ICollectionInfo;
}


namespace imtdev
{


class IDeviceSpecification;


/**
	Common interface for controlling a device
	Devices are abstract operating units having internal processing loop
	Devices can be opened, closed and also used for executing external commands
	Controller enumerates, opens, closes devices, accesses static information about devices,
	as well as information about the device instances
*/
class IDeviceController: virtual public IDeviceEnumerator
{
public:
	/**
		Get IDs of device types supported by the controller
	*/
	virtual const QByteArrayList& GetSupportedDeviceTypeIds() const = 0;

	/**
		Get device static info for the given DeviceTypeId
	*/
	virtual const IDeviceSpecification* GetDeviceStaticInfo(const QByteArray& deviceTypeId) const = 0;

	/**
		Get the list of available devices
	*/
	virtual const imtbase::ICollectionInfo& GetDeviceInstanceList() const = 0;

	/**
		Set device name for the given device ID
	*/
	virtual bool SetDeviceInstanceName(const QByteArray& deviceId, const QString& name) = 0;

	/**
		Set device description for the given device ID
	*/
	virtual bool SetDeviceInstanceDescription(const QByteArray& deviceId, const QString& description) = 0;

	/**
		Get device instance info for the given DeviceId
		For some devices DeviceInstanceInfo can only be obtained for open devices
	*/
	virtual DeviceInstancePtr GetDeviceInstance(const QByteArray& deviceId) const = 0;

	/**
		Get provider of current device states
	*/
	virtual const IDeviceStateProvider& GetDeviceStateProvider() const = 0;

	/**
		Start processing loop of the given device. The state of the device will be switched into DS_ACTIVE, if the operation was successful
	*/
	virtual DeviceAccessorPtr OpenDevice(
				const QByteArray& deviceId,
				const iprm::IParamsSet* paramsPtr) = 0;

	/**
		Stop processing loop of the given device. The device will be in the \c DS_INACTIVE state, if the operation was successful
	*/
	virtual bool CloseDevice(const QByteArray& deviceId) = 0;
};


} // namespace imtdev


