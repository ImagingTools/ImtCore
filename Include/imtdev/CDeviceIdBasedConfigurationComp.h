// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <ilog/TLoggerCompWrap.h>
#include <iprm/CParamsSet.h>

// ImtCore includes
#include <imtbase/ISelection.h>
#include <imtbase/TModelUpdateBinder.h>
#include <imtdev/IDeviceConfigurationManager.h>
#include <imtdev/IDeviceController.h>


namespace imtdev
{


/**
	@brief Selection-based device configuration component
	
	CDeviceIdBasedConfigurationComp implements the **Adapter Pattern** to provide
	device-specific configuration as an iprm::IParamsSet that automatically synchronizes
	with device selection. It bridges device configuration management with parameter-based
	UI frameworks.
	
	This component observes device selection, configuration changes, and device state,
	maintaining bidirectional synchronization between the UI parameter set and the
	device's persistent configuration.
	
	@par Purpose:
	Enables seamless UI binding to device configurations by:
	- Automatically loading configuration when device is selected
	- Synchronizing parameter changes back to configuration manager
	- Handling device state transitions (updating when device opens/closes)
	- Preventing recursive updates with FlagLocker pattern
	
	@par Key Features:
	- **Automatic Synchronization**: Bidirectional sync between UI params and device config
	- **Selection Awareness**: Updates when selected device changes
	- **State Tracking**: Refreshes configuration on device state changes
	- **Anti-Recursion**: FlagLocker prevents infinite update loops
	- **Change Notifications**: Triggers updates through model update binders
	
	@par Component Configuration:
	- **DeviceSelection**: Reference to ISelection providing selected device ID
	- **DeviceController**: Reference to IDeviceController for device instance access
	- **DeviceConfigurationManager**: Reference to IDeviceConfigurationManager for persistence
	- **DeviceStateProvider**: Reference to IDeviceStateProvider for state monitoring
	
	@par Synchronization Flow:
	@code
	User selects device
	  ↓
	Configuration loaded from manager
	  ↓
	Parameters updated in UI
	  ↓
	User modifies parameter
	  ↓
	Configuration saved to manager
	  ↓
	Change notification fired
	@endcode
	
	@par Usage Pattern:
	@code{.cpp}
	// Obtain configuration component instance (via component system)
	CDeviceIdBasedConfigurationComp* pConfig = // get from component system
	
	// Component is configured with required dependencies:
	// - DeviceSelection reference
	// - DeviceController reference
	// - DeviceConfigurationManager reference
	// - DeviceStateProvider reference
	
	// Use as parameter set for UI binding
	iprm::IParamsSet* pParams = pConfig;
	QVariant value = pParams->GetParamValue("BaudRate");
	pParams->SetParamValue("BaudRate", 115200);  // Auto-saves to manager
	@endcode
	
	@see IDeviceConfigurationManager
	@see imtbase::ISelection
	@see CDeviceIdBasedConfigurationManagerComp
	@ingroup imtdev
*/
class CDeviceIdBasedConfigurationComp:
			public ilog::CLoggerComponentBase,
			public iprm::CParamsSet
{
public:
	typedef ilog::CLoggerComponentBase BaseClass;

	I_BEGIN_COMPONENT(CDeviceIdBasedConfigurationComp)
		I_REGISTER_INTERFACE(iprm::IParamsSet);
		I_ASSIGN(m_deviceSelectionCompPtr, "DeviceSelection", "ID of the selected device", false, "DeviceSelection");
		I_ASSIGN(m_controllerCompPtr, "DeviceController", "Device controller", false, "DeviceController");
		I_ASSIGN(m_configurationManagerCompPtr, "DeviceConfigurationManager", "Configuration manager of the devices", false, "DeviceConfigurationManager");
		I_ASSIGN(m_stateProviderCompPtr, "DeviceStateProvider", "Device state provider", false, "DeviceStateProvider");
	I_END_COMPONENT;

	CDeviceIdBasedConfigurationComp();

protected:
	// reimplemented (icomp::CComponentBase)
	void OnComponentCreated() override;
	void OnComponentDestroyed() override;

private:
	DeviceInstancePtr GetDeviceInstance(const QByteArray& deviceId) const;
	void UpdateModel();

	void OnSelectionChanged(const istd::IChangeable::ChangeSet& changeSet, const imtbase::ISelection* objectPtr);
	void OnConfigurationChanged(const istd::IChangeable::ChangeSet& changeSet, const iprm::IParamsSet* objectPtr);
	void OnDeviceStateChanged(const istd::IChangeable::ChangeSet& changeSet, const IDeviceStateProvider* objectPtr);

private:
	class FlagLocker
	{
	public:
		FlagLocker()
			:m_flagPtr(nullptr),
			m_controlAllowed(false)
		{
		};

		FlagLocker(bool &flag)
			:m_flagPtr(&flag)
		{
			m_controlAllowed = !flag;

			flag = true;
		}

		~FlagLocker()
		{
			if (m_controlAllowed){
				*m_flagPtr = false;
			}
		}

	private:
		bool* m_flagPtr;
		bool m_controlAllowed;
	};

private:
	I_REF(imtbase::ISelection, m_deviceSelectionCompPtr);
	I_REF(imtdev::IDeviceController, m_controllerCompPtr);
	I_REF(imtdev::IDeviceConfigurationManager, m_configurationManagerCompPtr);
	I_REF(IDeviceStateProvider, m_stateProviderCompPtr);

	imtbase::TModelUpdateBinder<imtbase::ISelection, CDeviceIdBasedConfigurationComp> m_selectionObserver;
	imtbase::TModelUpdateBinder<iprm::IParamsSet, CDeviceIdBasedConfigurationComp> m_configurationObserver;
	imtbase::TModelUpdateBinder<IDeviceStateProvider, CDeviceIdBasedConfigurationComp> m_stateProviderObserver;

	bool m_isConfigurationStoreBlocked = false;
};


} // namespace imtdev


