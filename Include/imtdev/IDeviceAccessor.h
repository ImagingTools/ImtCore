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
typedef QSharedPointer<IDeviceAccessor> DeviceAccessorPtr;


/**
	Common interface for accessing the device.
	DeviceAccessor is a device access mechanism that allows you to execute commands on the device,
	as well as access subdevices of an open device
*/
class IDeviceAccessor: virtual public istd::IPolymorphic
{
public:
	typedef QSharedPointer<istd::IChangeable> CommandResultPtr;

	/**
		Get device accessor status.
		If the status is valid, the accessor allows you to perform operations with the device.
		The accessor becomes invalid when communication with the device is lost.
	*/
	virtual bool IsValid() const = 0;

	/**
		Get the DeviceId of the device associated with the accessor
	*/
	virtual QByteArray GetDeviceId() const = 0;

	/**
		Get the list of sub devices.
	*/
	virtual const iprm::IOptionsList& GetSubDeviceList() const = 0;

	/**
		Access to sub device.
	*/
	virtual DeviceAccessorPtr GetSubDevice(const QByteArray& deviceId) const = 0;

	/**
		Execute a command on the device.
		CommandResultPtr will be empty if the accessor is not valid.
	*/
	virtual CommandResultPtr ExecuteCommand(
				const QByteArray& commandId,
				const iprm::IParamsSet* commandParamsPtr = nullptr) = 0;
};


} // namespace imtdev


