#pragma once


// ACF includes
#include <istd/IChangeable.h>


namespace imtdev
{


/**
	Information about current measurement device state
*/
class IDeviceState: virtual public istd::IChangeable
{
public:

	enum ChangeFlags
	{
		CF_CONNECTION_STATE_CHANGED = istd::IChangeable::ChangeFlags::CF_NO_UNDO + 7171
	};

	/**
		Check connection state for measurement device
		\return \c true if the device is connected, otherwise \c false.
	*/
	virtual bool IsDeviceConnected(const QByteArray& deviceTypeId) = 0;
};


} // namespace imtdev


