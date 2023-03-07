#pragma once


// ACF includes
#include <iprm/IOptionsList.h>
#include <iprm/IParamsSet.h>

// Acula includes
#include <imtdev/IDeviceInfo.h>


namespace imtdev
{


/**
	Interface for device data provider.
	The configuration data of the device is given as a group of parameter sets
*/
class IDeviceDataProvider: virtual public imtdev::IDeviceInfo
{
public:
	/**
		Get the group info list for the data stored on the device.
	*/
	virtual const iprm::IOptionsList& GetDeviceConfigurationGroupList() const = 0;

	/**
		Get a configuration stored on the device for a given group-ID.
	*/
	virtual const iprm::IParamsSet* GetDeviceConfiguration(const QByteArray& configurationGroupId) const = 0;
};


} // namespace imtdev


