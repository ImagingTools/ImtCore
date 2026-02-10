// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <istd/IChangeable.h>


namespace imtdev
{


/**
	@brief Interface for tracking device connection states
	
	IDeviceStateProvider monitors and reports the current runtime state of devices
	managed by a device controller. It provides change notifications when device
	states transition between unavailable, closed, and opened states.
	
	This interface extends istd::IChangeable to support observer pattern, allowing
	clients to register for notifications when device states change.
	
	@par Device States:
	Devices managed by the system can be in one of three states:
	- **DS_NONE**: Device is not available or not connected
	- **DS_CLOSED**: Device is available but not actively processing
	- **DS_OPENED**: Device is active and can execute commands
	
	@par State Transitions:
	@code
	[DS_NONE] <--> [DS_CLOSED] <--> [DS_OPENED]
	     ^                              ^
	     |                              |
	     +------------------------------+
	     (Communication loss or device removal)
	@endcode
	
	@par Usage Example:
	@code{.cpp}
	const IDeviceStateProvider& stateProvider = pController->GetDeviceStateProvider();
	
	// Check current device state
	IDeviceStateProvider::DeviceState state = stateProvider.GetDeviceState(deviceId);
	
	switch (state)
	{
		case IDeviceStateProvider::DS_NONE:
			// Device not available
			break;
		case IDeviceStateProvider::DS_CLOSED:
			// Device available, can be opened
			pController->OpenDevice(deviceId, nullptr);
			break;
		case IDeviceStateProvider::DS_OPENED:
			// Device is active, can execute commands
			break;
	}
	
	// Register for state change notifications
	class StateObserver : public imod::CSingleModelObserverBase
	{
	protected:
		void OnUpdate(const istd::IChangeable::ChangeSet& changeSet) override
		{
			int changeFlags = changeSet.GetFlags();
			if (changeFlags & IDeviceStateProvider::CF_STATE_CHANGED)
			{
				// Device state changed - update UI
			}
		}
	};
	@endcode
	
	@see IDeviceController
	@see IDeviceEnumerator
	@see CDeviceStateProviderAdapterComp
	@ingroup imtdev
*/
class IDeviceStateProvider : virtual public istd::IChangeable
{
public:

	/**
		@brief Runtime state of a device
		
		Enumeration of possible device states in the system lifecycle.
	*/
	enum DeviceState
	{
		/**
			@brief Device is not available
			
			The device is not connected, not discovered, or has been removed
			from the system.
		*/
		DS_NONE,

		/**
			@brief Device is available but not running
			
			The device has been discovered and is available for use, but its
			processing loop is not active. The device can be opened to transition
			to DS_OPENED state.
		*/
		DS_CLOSED,

		/**
			@brief Processing loop of the device is running
			
			The device is active and its processing loop is running. In this state
			the device can execute commands via IDeviceAccessor.
		*/
		DS_OPENED
	};

	/**
		@brief Change notification flags
		
		Flags used with istd::IChangeable to indicate what type of change occurred.
	*/
	enum ChangeFlags
	{
		/// Device state has changed
		/// Offset value (7171) ensures no collision with base class change flags
		CF_STATE_CHANGED = istd::IChangeable::ChangeFlags::CF_NO_UNDO + 7171
	};

	/**
		@brief Get the current state of a device
		
		Queries the current runtime state of the specified device.
		
		@param deviceId Unique identifier of the device
		@return Current state of the device (DS_NONE, DS_CLOSED, or DS_OPENED)
		@see DeviceState
	*/
	virtual DeviceState GetDeviceState(const QByteArray& deviceId) const = 0;
};


} // namespace imtdev


