#pragma once


// ACF includes
#include <istd/IChangeable.h>
#include <idoc/IDocumentMetaInfo.h>


namespace iprm
{
	class IOptionsList;
	class IParamsSet;
}


namespace imtdev
{


class IDeviceInstanceInfo;


/**
	Interface for describing "static" information about a device type/class.
*/
class IDeviceStaticInfo: virtual public istd::IChangeable
{
public:
	enum MetaInfoTypes
	{
		MIT_MANUFACTURER = idoc::IDocumentMetaInfo::MIT_USER
	};

	/**
		Get device class. e.g. "Mouse"
	*/
	virtual QByteArray GetDeviceClass() const = 0;

	/**
		Get an unique device type-ID e.g. "Logitech_MX_Master_3"
		to identify a specific device type/model
	*/
	virtual QByteArray GetDeviceTypeId() const = 0;

	/**
		Get human readable name of the device type/device model e.g. "Logitech MX Master 3 Wireless Mouse"
	*/
	virtual QString GetDeviceTypeName() const = 0;

	/**
		Get the default device configuration
	*/
	virtual const iprm::IParamsSet& GetDefaultConfiguration() const = 0;

	/**
		Check if device configuration is valid
	*/
	virtual bool AreConfigurationAccepted(const iprm::IParamsSet& configuration) const = 0;

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


