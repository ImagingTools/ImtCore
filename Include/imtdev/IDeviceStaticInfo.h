#pragma once


// ACF includes
#include <istd/IChangeable.h>


namespace iprm
{
	class IOptionsList;
	class IParamsSet;
}


namespace imtdev
{


class IDeviceInstanceInfo;


class IDeviceStaticInfo: virtual public istd::IChangeable
{
public:
	/**
		Get device class. e.g. "Mouse"
	*/
	virtual QByteArray GetDeviceClass() const = 0;

	/**
		Get device type e.g. "Logitech_MX_Master_3"
		DeviceTypeId unique id to identify a specific device type/model
	*/
	virtual QByteArray GetDeviceTypeId() const = 0;

	/**
		Get device type name e.g. "Logitech MX Master 3 Wireless Mouse"
	*/
	virtual QString GetDeviceTypeName() const = 0;

	/**
		Get the default device configuration
	*/
	virtual const iprm::IParamsSet& GetDefaultConfiguration() const = 0;

	/**
		Check if device configuration is valid
	*/
	virtual bool AreConfigurationAccepted(const iprm::IParamsSet* configurationPtr) const = 0;

	/**
		Get a list of subdevices for a given device type
	*/
	virtual const iprm::IOptionsList& GetSubDeviceList() const = 0;

	/**
		Get static device info of subdevice with given id
	*/
	virtual const IDeviceStaticInfo* GetSubDeviceStaticInfo(const QByteArray& id) const = 0;

	/**
		Binary flags for describing capabilities of a device.
	*/
	enum DeviceCapabilities
	{
		DC_READ = 1,
		DC_WRITE = 2
	};

	/**
		Get capability flags for the given device.
	*/
	virtual int GetDeviceCapabilities() const = 0;

	/**
		Get the list of commands supported by the device class.
	*/
	virtual const QByteArrayList& GetSupportedCommands() const = 0;
};


} // namespace imtdev


