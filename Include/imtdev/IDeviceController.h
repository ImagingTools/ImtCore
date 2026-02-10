// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
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
	@brief Common interface for controlling devices
	
	IDeviceController is the central control hub for device management in the imtdev library.
	It provides comprehensive device lifecycle management including enumeration, opening/closing,
	state tracking, and access to device information.
	
	The controller acts as a facade that coordinates multiple subsystems:
	- Device enumeration and discovery (via IDeviceEnumerator)
	- Device state tracking (via IDeviceStateProvider)
	- Device access and command execution (via IDeviceAccessor)
	- Static device type information
	- Instance-specific device information
	
	@par Device Lifecycle:
	Devices managed by the controller go through several states:
	- **DS_NONE**: Device is not available
	- **DS_CLOSED**: Device is available but not actively processing
	- **DS_OPENED**: Device is active and can execute commands
	
	@par Usage Pattern:
	@code{.cpp}
	// Obtain device controller reference (via component system)
	IDeviceController* pController = // get from component system
	
	// Enumerate available devices
	pController->EnumerateDevices();
	
	// Get list of discovered devices
	const ICollectionInfo& devices = pController->GetDeviceInstanceList();
	
	// Get device instance information
	QByteArray deviceId = // get device ID from list
	DeviceInstancePtr pInstance = pController->GetDeviceInstance(deviceId);
	
	// Open device for command execution
	DeviceAccessorPtr pAccessor = pController->OpenDevice(deviceId, nullptr);
	if (pAccessor)
	{
		// Execute commands on the device
		pAccessor->ExecuteCommand(commandId, params);
		
		// Close device when done
		pController->CloseDevice(deviceId);
	}
	@endcode
	
	@par Thread Safety:
	Implementations should provide thread-safe access to device lists and opened devices.
	The base implementation (CDeviceControllerCompBase) uses mutex protection for this purpose.
	
	@see IDeviceEnumerator
	@see IDeviceAccessor
	@see IDeviceStateProvider
	@see IDeviceInstance
	@see CDeviceControllerCompBase
	@ingroup imtdev
*/
class IDeviceController: virtual public IDeviceEnumerator
{
public:
	/**
		@brief Get IDs of device types supported by the controller
		
		Returns a list of device type identifiers that this controller can manage.
		Each device type has associated static information (IDeviceSpecification)
		that describes its capabilities.
		
		@return Reference to list of supported device type IDs
		@see GetDeviceStaticInfo()
	*/
	virtual const QByteArrayList& GetSupportedDeviceTypeIds() const = 0;

	/**
		@brief Get device static info for the given device type ID
		
		Retrieves the device specification that describes static properties,
		capabilities, and supported commands for a specific device type.
		
		@param deviceTypeId Unique identifier of the device type
		@return Pointer to device specification, or nullptr if device type is not supported
		@see IDeviceSpecification
		@see GetSupportedDeviceTypeIds()
	*/
	virtual const IDeviceSpecification* GetDeviceStaticInfo(const QByteArray& deviceTypeId) const = 0;

	/**
		@brief Get the list of available device instances
		
		Returns a collection containing information about all devices that have been
		discovered through enumeration. The collection supports iteration and provides
		change notifications when devices are added or removed.
		
		@return Reference to collection info containing device instances
		@see EnumerateDevices()
		@see imtbase::ICollectionInfo
	*/
	virtual const imtbase::ICollectionInfo& GetDeviceInstanceList() const = 0;

	/**
		@brief Set custom name for a device instance
		
		Allows overriding the default device name with a user-friendly name.
		This name is typically displayed in user interfaces.
		
		@param deviceId Unique identifier of the device
		@param name Custom name to assign to the device
		@return true if name was set successfully, false otherwise
		@see SetDeviceInstanceDescription()
	*/
	virtual bool SetDeviceInstanceName(const QByteArray& deviceId, const QString& name) = 0;

	/**
		@brief Set custom description for a device instance
		
		Allows overriding the default device description with custom text.
		This description provides additional context about the device.
		
		@param deviceId Unique identifier of the device
		@param description Custom description to assign to the device
		@return true if description was set successfully, false otherwise
		@see SetDeviceInstanceName()
	*/
	virtual bool SetDeviceInstanceDescription(const QByteArray& deviceId, const QString& description) = 0;

	/**
		@brief Get device instance info for the given device ID
		
		Retrieves runtime information about a specific device instance, including
		identifiers, version information, and attributes.
		
		@note For some devices, instance information can only be obtained when the
		      device is open (in DS_OPENED state).
		
		@param deviceId Unique identifier of the device
		@return Shared pointer to device instance, or nullptr if not found
		@see IDeviceInstance
		@see OpenDevice()
	*/
	virtual DeviceInstancePtr GetDeviceInstance(const QByteArray& deviceId) const = 0;

	/**
		@brief Get provider of current device states
		
		Returns the state provider that tracks the current state (DS_NONE, DS_CLOSED, DS_OPENED)
		of all devices managed by this controller.
		
		@return Reference to device state provider
		@see IDeviceStateProvider
	*/
	virtual const IDeviceStateProvider& GetDeviceStateProvider() const = 0;

	/**
		@brief Open a device and start its processing loop
		
		Transitions a device from DS_CLOSED to DS_OPENED state, making it ready for
		command execution. Returns a device accessor that can be used to interact
		with the device.
		
		@param deviceId Unique identifier of the device to open
		@param paramsPtr Optional parameters for device initialization (can be nullptr)
		@return Device accessor for command execution, or nullptr if open failed
		@see CloseDevice()
		@see IDeviceAccessor
		@note The device must be in DS_CLOSED state for this operation to succeed
	*/
	virtual DeviceAccessorPtr OpenDevice(
				const QByteArray& deviceId,
				const iprm::IParamsSet* paramsPtr) = 0;

	/**
		@brief Close a device and stop its processing loop
		
		Transitions a device from DS_OPENED to DS_CLOSED state, releasing resources
		and stopping any active processing.
		
		@param deviceId Unique identifier of the device to close
		@return true if device was closed successfully, false otherwise
		@see OpenDevice()
		@note The device must be in DS_OPENED state for this operation to succeed
	*/
	virtual bool CloseDevice(const QByteArray& deviceId) = 0;
};


} // namespace imtdev


