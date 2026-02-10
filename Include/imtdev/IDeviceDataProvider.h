// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <iprm/IOptionsList.h>
#include <iprm/IParamsSet.h>
#include <idoc/IDocumentMetaInfo.h>

// Acula includes
#include <imtdev/IDeviceInstance.h>


namespace imtdev
{


/**
	@brief Interface for device data provider
	
	IDeviceDataProvider extends IDeviceInstance to provide access to device configuration
	data stored on the device itself. It combines device instance information with
	configuration retrieval capabilities, enabling access to device-stored parameter sets
	organized into logical groups.
	
	This interface is particularly useful for:
	- Accessing factory configurations stored on device
	- Reading device-specific calibration data
	- Retrieving user configurations saved to device storage
	- Managing multiple configuration groups on a single device
	
	The configuration data is organized as groups of parameter sets, where each group
	represents a logical collection of related settings.
	
	@par Key Concepts:
	- **Device Instance**: Base device information (inherited from IDeviceInstance)
	- **Configuration Groups**: Logical groupings of related configuration parameters
	- **Device Metadata**: Additional metadata stored on device (via IDocumentMetaInfo)
	- **Parameter Sets**: Configuration data structured as key-value parameters
	
	@par Usage Pattern:
	@code{.cpp}
	// Obtain data provider reference (via component system)
	IDeviceDataProvider* pDataProvider = // get from component system
	
	// Access device metadata
	idoc::IDocumentMetaInfo* pMetaInfo = pDataProvider->GetDeviceMetaInfo();
	if (pMetaInfo)
	{
		QString serialNum = pMetaInfo->GetMetaInfo(
			IDeviceDataPersistence::MIT_SERIAL_NUMBER);
	}
	
	// Get list of configuration groups
	const iprm::IOptionsList& groups = 
		pDataProvider->GetDeviceConfigurationGroupList();
	
	// Iterate through configuration groups
	for (int i = 0; i < groups.GetOptionsCount(); ++i)
	{
		const iprm::IOption& group = groups.GetOption(i);
		QByteArray groupId = group.GetId();
		QString groupName = group.GetName();
		
		// Get configuration for this group
		const iprm::IParamsSet* pConfig = 
			pDataProvider->GetDeviceConfiguration(groupId);
		
		if (pConfig)
		{
			// Access parameters in this configuration group
			// e.g., calibration data, user presets, etc.
		}
	}
	
	// Access device instance information (inherited from IDeviceInstance)
	const IDeviceSpecification& spec = pDataProvider->GetDeviceSpecification();
	QByteArray serialNumber = pDataProvider->GetIdentifier(IDeviceInstance::IT_SERIAL);
	@endcode
	
	@see IDeviceInstance
	@see IDeviceDataPersistence
	@see IDeviceConfigurationManager
	@ingroup imtdev
*/
class IDeviceDataProvider: virtual public imtdev::IDeviceInstance
{
public:
	/**
		@brief Get device metadata information
		
		Returns document metadata that may include serial numbers, version information,
		firmware versions, and other device-specific metadata stored on the device.
		
		@return Pointer to document metadata object, or nullptr if not available
		@see idoc::IDocumentMetaInfo
		@see IDeviceDataPersistence::ReadDeviceMetaInfo()
	*/
	virtual idoc::IDocumentMetaInfo* GetDeviceMetaInfo() const = 0;

	/**
		@brief Get the list of configuration groups stored on device
		
		Returns a list of logical configuration groups available on the device.
		Each group represents a related set of configuration parameters (e.g.,
		"Calibration", "UserPreset1", "FactoryDefaults").
		
		@return Reference to options list containing configuration group information
		@see GetDeviceConfiguration()
	*/
	virtual const iprm::IOptionsList& GetDeviceConfigurationGroupList() const = 0;

	/**
		@brief Get configuration for a specific group
		
		Retrieves the parameter set containing configuration data for the specified
		configuration group.
		
		@param configurationGroupId Unique identifier of the configuration group
		@return Pointer to parameter set containing the configuration, or nullptr if not found
		@see GetDeviceConfigurationGroupList()
		@see iprm::IParamsSet
	*/
	virtual const iprm::IParamsSet* GetDeviceConfiguration(const QByteArray& configurationGroupId) const = 0;
};


} // namespace imtdev


