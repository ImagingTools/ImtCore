// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// STL includes
#include <memory>

// ACF includes
#include <iattr/IAttributesProvider.h>
#include <iser/IVersionInfo.h>

// ImtCore includes
#include <imtdev/IDeviceSpecification.h>


namespace imtdev
{


/**
	@brief Interface for describing runtime information about a device instance
	
	IDeviceInstance represents a specific instance of a device at runtime. It provides
	access to instance-specific information including unique identifiers (such as serial
	numbers), version information (firmware, hardware), and dynamic attributes.
	
	This interface extends istd::IChangeable to support change notifications when device
	instance properties are modified.
	
	@par Key Concepts:
	- **Device Instance**: A runtime object representing a connected or manageable device
	- **Identifiers**: Unique identifiers like serial numbers that distinguish device instances
	- **Version Information**: Firmware and hardware version tracking for compatibility checks
	- **Attributes**: Dynamic properties specific to this device instance
	
	@par Usage Example:
	@code{.cpp}
	DeviceInstancePtr pInstance = pController->GetDeviceInstance(deviceId);
	
	// Get device type information
	const IDeviceSpecification& spec = pInstance->GetDeviceSpecification();
	
	// Get serial number
	QByteArray serialNumber = pInstance->GetIdentifier(IDeviceInstance::IT_SERIAL);
	
	// Check firmware version
	const iser::IVersionInfo& version = pInstance->GetVersion();
	int fwMajor = version.GetVersion(IDeviceInstance::VI_FIRMWARE_VERSION_MAJOR);
	
	// Access device-specific attributes
	const iattr::IAttributesProvider* attrs = pInstance->GetAttributes();
	@endcode
	
	@see IDeviceSpecification
	@see IEditableDeviceInstance
	@see ICompositeDeviceInstance
	@ingroup imtdev
*/
class IDeviceInstance: virtual public istd::IChangeable
{
public:
	/**
		@brief Types of device identifiers
		
		Enumeration of different identifier types that can be used to uniquely
		identify device instances.
	*/
	enum IdentifierTypes
	{
		/// Serial number identifier
		IT_SERIAL
	};

	/**
		@brief Version information identifiers
		
		Enumeration of version components that can be queried from device instances.
		Extends iser::IVersionInfo::UserVersionId to add device-specific version types.
	*/
	enum VersionIds
	{
		/// Major firmware version number
		VI_FIRMWARE_VERSION_MAJOR = iser::IVersionInfo::UserVersionId,
		/// Minor firmware version number
		VI_FIRMWARE_VERSION_MINOR,
		/// Patch firmware version number
		VI_FIRMWARE_VERSION_PATCH,
		/// Major hardware version number
		VI_HARDWARE_VERSION_MAJOR,
		/// Minor hardware version number
		VI_HARDWARE_VERSION_MINOR,
		/// Patch hardware version number
		VI_HARDWARE_VERSION_PATCH,
	};

	/**
		@brief Get device static info associated with device instance
		
		Returns the device specification that describes the static properties and
		capabilities of this device type.
		
		@return Reference to the device specification object
		@see IDeviceSpecification
	*/
	virtual const IDeviceSpecification& GetDeviceSpecification() const = 0;

	/**
		@brief Get the device identifier of the given type
		
		Retrieves a device identifier such as serial number that uniquely identifies
		this device instance.
		
		@param identifierType Type of identifier to retrieve (e.g., IT_SERIAL)
		@return Device identifier as a byte array
		@see IdentifierTypes
	*/
	virtual QByteArray GetIdentifier(int identifierType) const = 0;

	/**
		@brief Get device instance version information
		
		Returns version information containing firmware and hardware version numbers
		for this specific device instance.
		
		@return Reference to version information object
		@see iser::IVersionInfo
		@see VersionIds
	*/
	virtual const iser::IVersionInfo& GetVersion() const = 0;

	/**
		@brief Get device instance attributes provider
		
		Returns the attributes provider that manages dynamic, instance-specific
		properties for this device. May return nullptr if no attributes are available.
		
		@return Pointer to attributes provider, or nullptr if not available
		@see iattr::IAttributesProvider
	*/
	virtual const iattr::IAttributesProvider* GetAttributes() const = 0;
};


/**
	@brief Shared pointer to IDeviceInstance
	
	Smart pointer type for managing device instance lifetimes. Use this type
	when storing or passing device instances to ensure proper memory management.
*/
typedef std::shared_ptr<IDeviceInstance> DeviceInstancePtr;


} // namespace imtdev


