// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QSharedPointer>

// ACF includes
#include <istd/IChangeable.h>


namespace iprm
{
	class IOptionsList;
	class IParamsSet;
}


namespace imtdev
{


class IDeviceAccessor;

/**
	@brief Shared pointer to IDeviceAccessor
	
	Smart pointer type for managing device accessor lifetimes. Device accessors
	are typically returned by IDeviceController::OpenDevice() and should be stored
	using this pointer type.
*/
typedef QSharedPointer<IDeviceAccessor> DeviceAccessorPtr;


/**
	@brief Common interface for accessing an open device
	
	IDeviceAccessor provides the mechanism to interact with an opened device,
	enabling command execution and navigation of hierarchical device structures.
	
	An accessor is obtained by opening a device via IDeviceController::OpenDevice()
	and remains valid as long as the device stays open and communication is maintained.
	The accessor becomes invalid if communication with the device is lost.
	
	@par Key Features:
	- **Command Execution**: Execute device-specific commands with parameters
	- **Sub-Device Access**: Navigate and access sub-devices in composite configurations
	- **Validity Checking**: Monitor connection status and accessor validity
	- **Device Identification**: Track which device this accessor controls
	
	@par Usage Example:
	@code{.cpp}
	// Open device and get accessor
	DeviceAccessorPtr pAccessor = pController->OpenDevice(deviceId, nullptr);
	
	if (pAccessor && pAccessor->IsValid())
	{
		// Execute a command
		IDeviceAccessor::CommandResultPtr result = 
			pAccessor->ExecuteCommand("READ_DATA", paramsPtr);
		
		// Access sub-devices if available
		const iprm::IOptionsList& subDevices = pAccessor->GetSubDeviceList();
		if (!subDevices.IsEmpty())
		{
			QByteArray subDeviceId = subDevices.GetOption(0).GetId();
			DeviceAccessorPtr pSubAccessor = pAccessor->GetSubDevice(subDeviceId);
			
			// Execute command on sub-device
			pSubAccessor->ExecuteCommand("SUB_COMMAND", nullptr);
		}
	}
	@endcode
	
	@par Lifetime:
	The accessor is valid from successful OpenDevice() until CloseDevice() is called
	or communication is lost. Always check IsValid() before executing commands.
	
	@see IDeviceController
	@see IDeviceInstance
	@see ICompositeDeviceInstance
	@ingroup imtdev
*/
class IDeviceAccessor: virtual public istd::IPolymorphic
{
public:
	/**
		@brief Shared pointer type for command results
		
		Command execution results are returned as changeable objects wrapped
		in shared pointers for proper lifetime management.
	*/
	typedef QSharedPointer<istd::IChangeable> CommandResultPtr;

	/**
		@brief Get device accessor validity status
		
		Checks if the accessor is valid and can be used to perform operations.
		An accessor becomes invalid when communication with the device is lost
		or the device is closed.
		
		@return true if accessor is valid and can execute commands, false otherwise
		@note Always check validity before executing commands to avoid errors
	*/
	virtual bool IsValid() const = 0;

	/**
		@brief Get the device ID associated with this accessor
		
		Returns the unique identifier of the device that this accessor controls.
		
		@return Device identifier as a byte array
		@see IDeviceController::OpenDevice()
	*/
	virtual QByteArray GetDeviceId() const = 0;

	/**
		@brief Get the list of sub-devices
		
		For composite devices, returns a list of available sub-devices that can
		be accessed through this accessor. For simple devices, returns an empty list.
		
		@return Reference to options list containing sub-device information
		@see GetSubDevice()
		@see ICompositeDeviceInstance
	*/
	virtual const iprm::IOptionsList& GetSubDeviceList() const = 0;

	/**
		@brief Access a sub-device
		
		Creates an accessor for a specific sub-device within a composite device.
		The sub-device must be in the list returned by GetSubDeviceList().
		
		@param deviceId Unique identifier of the sub-device to access
		@return Accessor for the sub-device, or nullptr if sub-device not found
		@see GetSubDeviceList()
	*/
	virtual DeviceAccessorPtr GetSubDevice(const QByteArray& deviceId) const = 0;

	/**
		@brief Execute a command on the device
		
		Executes a device-specific command with optional parameters. The command
		must be in the list of supported commands defined by IDeviceSpecification.
		
		@param commandId Unique identifier of the command to execute
		@param commandParamsPtr Optional parameters for the command (can be nullptr)
		@return Shared pointer to command result object, or empty pointer if accessor is invalid
		@see IDeviceSpecification::GetSupportedCommands()
		@note Returns empty pointer if accessor is not valid - always check IsValid() first
	*/
	virtual CommandResultPtr ExecuteCommand(
				const QByteArray& commandId,
				const iprm::IParamsSet* commandParamsPtr = nullptr) = 0;
};


} // namespace imtdev


