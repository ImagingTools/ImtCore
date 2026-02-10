// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QMap>

// ACF includes
#include <ilog/TLoggerCompWrap.h>
#include <istd/TIFactory.h>

// ImtCore includes
#include <imtdev/IDeviceConfigurationManager.h>
#include <imtdev/IDeviceController.h>


namespace imtdev
{


/**
	@brief Device-ID-based configuration manager component
	
	CDeviceIdBasedConfigurationManagerComp implements IDeviceConfigurationManager to
	provide centralized storage and retrieval of device-specific configurations indexed
	by device ID. It supports factory-based deserialization and serialization for
	configuration persistence across sessions.
	
	@par Purpose:
	Manages the lifecycle of device configurations including:
	- Storage of configuration parameter sets per device ID
	- Factory-based deserialization for loading saved configurations
	- Serialization support for persisting configurations
	- Validation against device specifications
	- Change notifications when configurations are modified
	
	@par Key Features:
	- **Device-ID Indexed Storage**: QMap-based storage keyed by device ID
	- **Factory Pattern**: Multiple configuration factories for different device types
	- **Serialization**: Implements iser::ISerializable for save/load support
	- **Validation**: Uses device controller to validate configurations
	- **Change Tracking**: Notifies observers when configurations change
	- **Type Association**: Stores device type ID with each configuration
	
	@par Component Configuration:
	- **DeviceController**: Reference to IDeviceController for validation and device info
	- **ConfigurationFactory**: Multi-reference to configuration factories for deserialization
	
	@par Storage Structure:
	@code
	Internal Storage:
	{
	  "DEVICE_001": {
	    deviceTypeId: "StandardCamera",
	    configuration: {BaudRate: 115200, Exposure: 100, ...}
	  },
	  "DEVICE_002": {
	    deviceTypeId: "TemperatureSensor",
	    configuration: {SampleRate: 10, Units: "Celsius", ...}
	  }
	}
	@endcode
	
	@par Usage Pattern:
	@code{.cpp}
	// Obtain configuration manager instance (via component system)
	CDeviceIdBasedConfigurationManagerComp* pConfigMgr = // get from component system
	
	// Component is configured with:
	// - DeviceController reference for validation
	// - ConfigurationFactory references for deserialization
	
	// Store device configuration
	iprm::CParamsSet* pConfig = // your params set
	pConfig->SetParamValue("BaudRate", 115200);
	pConfigMgr->SetDeviceConfiguration("DEVICE_001", *pConfig);
	
	// Retrieve device configuration
	DeviceConfigurationPtr pStoredConfig = 
	    pConfigMgr->GetDeviceConfiguration("DEVICE_001");
	
	// Serialize to archive
	iser::IArchive& archive = // your archive
	pConfigMgr->Serialize(archive);
	@endcode
	
	@par Serialization:
	Configurations are serialized with their associated device type IDs, enabling
	proper factory-based deserialization when loading from persistent storage.
	
	@see IDeviceConfigurationManager
	@see CDeviceIdBasedConfigurationComp
	@see iser::ISerializable
	@ingroup imtdev
*/
class CDeviceIdBasedConfigurationManagerComp:
			public ilog::CLoggerComponentBase,
			virtual public IDeviceConfigurationManager,
			virtual public iser::ISerializable
{
public:
	typedef ilog::CLoggerComponentBase BaseClass;

	I_BEGIN_COMPONENT(CDeviceIdBasedConfigurationManagerComp)
		I_REGISTER_INTERFACE(IDeviceConfigurationManager);
		I_REGISTER_INTERFACE(iser::ISerializable);
		I_REGISTER_INTERFACE(istd::IChangeable);
		I_ASSIGN(m_deviceControllerCompPtr, "DeviceController", "Device controller for creating and validating configurations", false, "DeviceController");
		I_ASSIGN_MULTI_0(m_deviceConfigurationFactoryCompPtr, "ConfigurationFactory", "Configuration factories for deserialization", false);
	I_END_COMPONENT;

	// reimplemented (IDeviceConfigurationManager)
	virtual DeviceConfigurationPtr GetDeviceConfiguration(const QByteArray& deviceId) const override;
	virtual bool SetDeviceConfiguration(const QByteArray& deviceId, const iprm::IParamsSet& configuration) override;

	// reimplemented (iser::ISerializable)
	virtual bool Serialize(iser::IArchive& archive) override;

private:
	typedef istd::TIFactory<iprm::IParamsSet> ConfigurationFactory;

	struct ConfigurationInfo
	{
		QByteArray deviceTypeId;
		DeviceConfigurationPtr configurationPtr;
	};

private:
	DeviceInstancePtr GetDeviceInstance(const QByteArray& deviceId) const;
	ConfigurationFactory* FindConfigurationFactory(const QByteArray& deviceTypeId) const;

private:
	I_REF(IDeviceController, m_deviceControllerCompPtr);
	I_MULTIREF(ConfigurationFactory, m_deviceConfigurationFactoryCompPtr);

	QMap<QByteArray, ConfigurationInfo> m_configurations;
};


} // namespace imtdev


