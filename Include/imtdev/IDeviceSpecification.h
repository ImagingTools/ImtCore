#pragma once


// ACF includes
#include <istd/IChangeable.h>
#include <iattr/IAttributesManager.h>
#include <iprm/IParamsSet.h>


namespace imtdev
{


/**
	Interface for describing "static" information about a device type/class
*/
class IDeviceSpecification: virtual public istd::IChangeable
{
public:
	/**
		Get device class. e.g. "Mouse"
	*/
	virtual QByteArray GetClass() const = 0;

	/**
		Get an unique device type-ID e.g. "Logitech_MX_Master_3"
		to identify a specific device type/model
	*/
	virtual QByteArray GetTypeId() const = 0;

	/**
		Get human readable name of the device type/device model e.g. "Logitech MX Master 3 Wireless Mouse"
	*/
	virtual QString GetTypeName() const = 0;

	/**
		Get device static attributes provider
	*/
	virtual const iattr::IAttributesProvider* GetAttributes() const = 0;

	/**
		Get the default device configuration
	*/
	virtual const iprm::IParamsSet& GetDefaultConfiguration() const = 0;

	/**
		Check if device configuration is valid
	*/
	virtual bool AreConfigurationAccepted(const iprm::IParamsSet& configuration) const = 0;

	/**
		Binary flags for describing capabilities of a device
	*/
	enum DeviceCapabilities
	{
		DC_READ = 1,
		DC_WRITE = 2
	};

	/**
		Get capability flags for the given device
	*/
	virtual int GetCapabilities() const = 0;

	/**
		Get the list of commands supported by the device class
	*/
	virtual const QByteArrayList& GetSupportedCommands() const = 0;
};


} // namespace imtdev


