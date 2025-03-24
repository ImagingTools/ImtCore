#pragma once


// ACF includes
#include <iprm/IOptionsList.h>
#include <iprm/IParamsSet.h>
#include <idoc/IDocumentMetaInfo.h>

// Acula includes
#include <imtdev/IDeviceInstanceInfo.h>


namespace imtdev
{


/**
	Interface for device data provider.
	The configuration data of the device is given as a group of parameter sets
*/
class IDeviceDataProvider: virtual public imtdev::IDeviceInstanceInfo
{
public:
	/**
		Get device metainfo
	*/
	virtual idoc::IDocumentMetaInfo* GetDeviceMetaInfo() const = 0;

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


