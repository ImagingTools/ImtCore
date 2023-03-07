#pragma once


namespace iprm
{
	class IParamsSet;
	class IOptionsList;
}


namespace imtdev
{


class IDeviceInfo;


/**
	Common interface for controlling devices in a general way.
	Devices are abstract operating units having internal processing loop.
	Devices can be started, stopped and also used for executing external commands.
*/
class IDeviceController: virtual public istd::IChangeable
{
public:
	/**
		Runtime state of a device.
	*/
	enum DeviceState
	{
		/**
			No information about the device state is available.
		*/
		DS_NONE,

		/**
			Device is available but not running.
		*/
		DS_INACTIVE,

		/**
			Processing loop of the device is running.
			In this state the device can execute commands.
		*/
		DS_ACTIVE
	};

	/**
		Binary flags for describing capabilities of a device.
	*/
	enum DeviceCapabilities
	{
		DC_READ = 1,
		DC_WRITE = 2
	};

	/**
		Get the list of devices.
	*/
	virtual const iprm::IOptionsList& GetDeviceInfoList() const = 0;

	/**
		Get capability flags for the given device.
	*/
	virtual int GetDeviceCapabilities(const QByteArray& deviceId) const = 0;

	/**
		Get information about the device
	*/
	virtual const IDeviceInfo* GetDeviceInfo(const QByteArray& deviceId) const = 0;

	/**
		Get the current state of the device.
	*/
	virtual DeviceState GetDeviceState(const QByteArray& deviceId) const = 0;

	/**
		Do device initialization. After correct initialization the device is still inactive, but prepared for working.
		\note Only correctly initialized devices can be used.
	*/
	virtual bool InitializeDevice(const QByteArray& deviceId, const iprm::IParamsSet* paramsPtr = nullptr) = 0;

	/**
		Start processing loop of the given device. The state of the device will be switched into DS_ACTIVE, if the operation was successful.
		The method should be called after \c InitializeDevice
		\sa InitializeDevice
	*/
	virtual bool StartDevice(const QByteArray& deviceId) = 0;

	/**
		Stop processing loop of the given device. The device will be in the \c DS_INACTIVE state, if the operation was successful.
	*/
	virtual bool StopDevice(const QByteArray& deviceId) = 0;

	/**
		Get the list of commands supported by the device with the given ID.
	*/
	virtual QByteArrayList GetSupportedCommands(const QByteArray& deviceId) const = 0;

	/**
		Execute a command on the device.
		\return Processing state of the command. \sa iproc::IProcessor::TaskState
	*/
	virtual int ExecuteCommand(
				const QByteArray& deviceId,
				const QByteArray& commandId,
				const iprm::IParamsSet* commandParamsPtr = nullptr) = 0;

	virtual int GetConfiguration(
				const QByteArray& deviceId,
				iprm::IParamsSet& configuration) const = 0;

	virtual int SetConfiguration(
				const QByteArray& deviceId,
				const iprm::IParamsSet& configuration) = 0;
};


} // namespace imtdev


