#pragma once


// ACF includes
#include <istd/IChangeable.h>


namespace imtdev
{


/**
	Information about current device connection state
*/
class IDeviceStateProvider : virtual public istd::IChangeable
{
public:

	/**
		Runtime state of a device.
	*/
	enum DeviceState
	{
		/**
			Device is not available
		*/
		DS_NONE,

		/**
			Device is available but not running
		*/
		DS_CLOSED,

		/**
			Processing loop of the device is running
			In this state the device can execute commands
		*/
		DS_OPENED
	};

	enum ChangeFlags
	{
		CF_STATE_CHANGED = istd::IChangeable::ChangeFlags::CF_NO_UNDO + 7171
	};

	/**
		Get the current state of the device
	*/
	virtual DeviceState GetDeviceState(const QByteArray& deviceId) const = 0;
};


} // namespace imtdev


