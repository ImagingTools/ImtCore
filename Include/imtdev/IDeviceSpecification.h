// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <istd/IChangeable.h>
#include <iattr/IAttributesManager.h>
#include <iprm/IParamsSet.h>


namespace imtdev
{


/**
	@brief Interface for describing static information about a device type/class
	
	IDeviceSpecification defines the static properties and capabilities of a device type
	or model. Unlike IDeviceInstance which represents a specific device instance at runtime,
	IDeviceSpecification describes what all devices of a particular type can do.
	
	This interface provides:
	- Device classification and identification (class, type ID, type name)
	- Capability flags (read-only, read-write)
	- Supported command list
	- Default configuration and configuration validation
	- Static attributes applicable to all devices of this type
	
	@par Key Concepts:
	- **Device Class**: High-level category (e.g., "Camera", "Sensor", "Motor")
	- **Device Type ID**: Unique identifier for specific model (e.g., "Logitech_MX_Master_3")
	- **Device Type Name**: Human-readable name (e.g., "Logitech MX Master 3 Wireless Mouse")
	- **Capabilities**: What operations the device supports (read, write, etc.)
	- **Commands**: Specific operations that can be executed on the device
	
	@par Usage Example:
	@code{.cpp}
	const IDeviceSpecification* pSpec = pController->GetDeviceStaticInfo(deviceTypeId);
	
	// Check device class and identification
	QByteArray deviceClass = pSpec->GetClass();
	QString typeName = pSpec->GetTypeName();
	
	// Check capabilities
	int caps = pSpec->GetCapabilities();
	bool canWrite = (caps & IDeviceSpecification::DC_WRITE) != 0;
	
	// Get supported commands
	const QByteArrayList& commands = pSpec->GetSupportedCommands();
	
	// Get default configuration
	const iprm::IParamsSet& defaultConfig = pSpec->GetDefaultConfiguration();
	
	// Validate custom configuration
	if (pSpec->AreConfigurationAccepted(customConfig))
	{
		// Configuration is valid
	}
	@endcode
	
	@see IDeviceInstance
	@see ICompositeDeviceSpecification
	@see IDeviceController
	@ingroup imtdev
*/
class IDeviceSpecification: virtual public istd::IChangeable
{
public:
	/**
		@brief Binary flags for describing capabilities of a device
		
		These flags can be combined using bitwise OR to describe
		multiple capabilities.
	*/
	enum DeviceCapabilities
	{
		/// Device supports read operations
		DC_READ = 1,
		/// Device supports write operations
		DC_WRITE = 2
	};

	/**
		@brief Get device class
		
		Returns the high-level category or class of the device (e.g., "Mouse", 
		"Camera", "Sensor"). Devices in the same class typically share similar
		functionality.
		
		@return Device class identifier as a byte array
	*/
	virtual QByteArray GetClass() const = 0;

	/**
		@brief Get unique device type ID
		
		Returns a unique identifier for this specific device type or model.
		This ID is used to distinguish between different models within the same
		device class.
		
		@return Unique device type identifier (e.g., "Logitech_MX_Master_3")
	*/
	virtual QByteArray GetTypeId() const = 0;

	/**
		@brief Get human-readable name of the device type
		
		Returns a user-friendly name suitable for display in user interfaces.
		
		@return Human-readable device type name (e.g., "Logitech MX Master 3 Wireless Mouse")
	*/
	virtual QString GetTypeName() const = 0;

	/**
		@brief Get device static attributes provider
		
		Returns the attributes provider that manages static attributes applicable
		to all devices of this type. May return nullptr if no static attributes
		are defined.
		
		@return Pointer to static attributes provider, or nullptr if not available
		@see iattr::IAttributesProvider
	*/
	virtual const iattr::IAttributesProvider* GetAttributes() const = 0;

	/**
		@brief Get the default device configuration
		
		Returns the default configuration parameters for this device type.
		This configuration can be used as a starting point for device initialization.
		
		@return Reference to default configuration parameter set
		@see iprm::IParamsSet
		@see AreConfigurationAccepted()
	*/
	virtual const iprm::IParamsSet& GetDefaultConfiguration() const = 0;

	/**
		@brief Check if device configuration is valid
		
		Validates whether a given configuration parameter set is acceptable for
		this device type. This can check for required parameters, valid value
		ranges, and parameter compatibility.
		
		@param configuration Configuration to validate
		@return true if configuration is valid and accepted, false otherwise
		@see GetDefaultConfiguration()
	*/
	virtual bool AreConfigurationAccepted(const iprm::IParamsSet& configuration) const = 0;

	/**
		@brief Get capability flags for the device
		
		Returns a bitwise combination of DeviceCapabilities flags indicating
		what operations this device type supports.
		
		@return Capability flags (combination of DC_READ, DC_WRITE, etc.)
		@see DeviceCapabilities
	*/
	virtual int GetCapabilities() const = 0;

	/**
		@brief Get the list of commands supported by the device class
		
		Returns identifiers of all commands that can be executed on devices
		of this type via IDeviceAccessor::ExecuteCommand().
		
		@return Reference to list of supported command identifiers
		@see IDeviceAccessor::ExecuteCommand()
	*/
	virtual const QByteArrayList& GetSupportedCommands() const = 0;
};


} // namespace imtdev


