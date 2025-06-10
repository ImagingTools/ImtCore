#pragma once


// ACF includes
#include <istd/IChangeable.h>


namespace imtdev
{


/**
	Information about current device connection state
*/
class IDeviceConnectionState: virtual public istd::IChangeable
{
public:

	enum ChangeFlags
	{
		CF_CONNECTION_STATE_CHANGED = istd::IChangeable::ChangeFlags::CF_NO_UNDO + 7171
	};

	/**
		Check connection state for device
		\return \c true if the device is connected, otherwise \c false.
	*/
	virtual bool IsDeviceConnected(const QByteArray& deviceId) = 0;
};


} // namespace imtdev


