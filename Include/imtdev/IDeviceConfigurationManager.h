// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QSharedPointer>

// ACF includes
#include <iprm/IParamsSet.h>
#include <istd/IChangeable.h>


namespace imtdev
{


/**
	@brief Shared pointer to device configuration parameter set
	
	Smart pointer type for managing device configuration lifetimes. Configurations
	are parameter sets (iprm::IParamsSet) wrapped in shared pointers.
*/
typedef QSharedPointer<iprm::IParamsSet> DeviceConfigurationPtr;


/**
	@brief Interface for managing device-specific configurations
	
	IDeviceConfigurationManager provides centralized management of device configurations,
	enabling storage and retrieval of device-specific parameter sets indexed by device ID.
	
	This interface supports:
	- Getting configuration for a specific device
	- Setting/updating configuration for a device
	- Change notifications when configurations are modified
	
	Configuration data is persisted and can be serialized for storage across sessions.
	Each device instance can have its own unique configuration, identified by device ID.
	
	@par Key Concepts:
	- **Device ID**: Unique identifier linking configuration to specific device instance
	- **Configuration**: Parameter set (iprm::IParamsSet) containing device settings
	- **Persistence**: Configurations can be serialized and stored across sessions
	- **Change Tracking**: Extends istd::IChangeable for configuration change notifications
	
	@par Usage Pattern:
	@code{.cpp}
	// Obtain configuration manager reference (via component system)
	IDeviceConfigurationManager* pConfigMgr = // get from component system
	
	// Get existing configuration for a device
	QByteArray deviceId = "DEVICE_001";
	DeviceConfigurationPtr pConfig = pConfigMgr->GetDeviceConfiguration(deviceId);
	
	if (pConfig)
	{
		// Read configuration parameters
		int baudRate = pConfig->GetParamValue("BaudRate").toInt();
		
		// Modify configuration
		pConfig->SetParamValue("BaudRate", 115200);
		
		// Save modified configuration
		pConfigMgr->SetDeviceConfiguration(deviceId, *pConfig);
	}
	else
	{
		// No configuration exists for this device - create new one
		iprm::IParamsSet* pNewConfig = // create params set
		pNewConfig->SetParamValue("BaudRate", 9600);
		pConfigMgr->SetDeviceConfiguration(deviceId, *pNewConfig);
	}
	
	// Register for configuration change notifications
	class ConfigObserver : public imod::CSingleModelObserverBase
	{
	protected:
		void OnUpdate(const istd::IChangeable::ChangeSet& changeSet) override
		{
			// Configuration changed - update UI or sync with device
		}
	};
	@endcode
	
	@par Implementation Notes:
	Implementations typically use factory-based deserialization for loading stored
	configurations and maintain an internal map of device ID to configuration.
	
	@see CDeviceIdBasedConfigurationManagerComp
	@see CDeviceIdBasedConfigurationComp
	@see IDeviceInstance
	@ingroup imtdev
*/
class IDeviceConfigurationManager: virtual public istd::IChangeable
{
public:
	/**
		@brief Get configuration for a specific device
		
		Retrieves the configuration parameter set associated with the given device ID.
		Returns nullptr if no configuration exists for the device.
		
		@param deviceId Unique identifier of the device
		@return Shared pointer to device configuration, or nullptr if not found
		@see SetDeviceConfiguration()
	*/
	virtual DeviceConfigurationPtr GetDeviceConfiguration(const QByteArray& deviceId) const = 0;
	
	/**
		@brief Set configuration for a specific device
		
		Stores or updates the configuration for the given device ID. This triggers
		change notifications to any registered observers.
		
		@param deviceId Unique identifier of the device
		@param configuration Configuration parameter set to store
		@return true if configuration was set successfully, false otherwise
		@see GetDeviceConfiguration()
	*/
	virtual bool SetDeviceConfiguration(const QByteArray& deviceId, const iprm::IParamsSet& configuration) = 0;
};


} // namespace imtdev


