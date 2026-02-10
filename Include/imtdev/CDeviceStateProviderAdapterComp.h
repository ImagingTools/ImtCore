// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <icomp/CComponentBase.h>
#include <imod/CModelUpdateBridge.h>

// ImtCore includes
#include <imtdev/IDeviceController.h>


namespace imtdev
{


/**
	@brief Adapter component providing standalone device state provider
	
	CDeviceStateProviderAdapterComp implements the **Adapter Pattern** to extract
	and expose the IDeviceStateProvider interface from an IDeviceController as a
	standalone component. This enables state tracking to be used independently of
	the full controller interface.
	
	@par Purpose:
	Decouples device state tracking from device control operations, enabling:
	- State monitoring without controller access
	- Independent state provider references
	- Simplified dependency injection
	- UI binding to state changes only
	
	@par Key Features:
	- **Standalone Interface**: Provides IDeviceStateProvider without full controller
	- **Change Forwarding**: Propagates state change notifications from controller
	- **Automatic Binding**: Connects to controller's state provider on creation
	- **Cleanup Handling**: Properly unbinds on destruction
	
	@par Component Configuration:
	- **DeviceController**: Reference to IDeviceController whose state provider to adapt
	
	@par Lifecycle:
	- OnComponentCreated(): Binds to controller's state provider for change notifications
	- OnComponentDestroyed(): Unbinds from controller's state provider
	
	@par Usage Pattern:
	@code{.cpp}
	// Obtain state provider adapter instance (via component system)
	CDeviceStateProviderAdapterComp* pStateAdapter = // get from component system
	
	// Component is configured with DeviceController reference
	
	// Use as standalone state provider
	IDeviceStateProvider* pStateProvider = pStateAdapter;
	IDeviceStateProvider::DeviceState state = pStateProvider->GetDeviceState(deviceId);
	
	// Register for state change notifications
	class StateObserver : public imod::CSingleModelObserverBase
	{
	protected:
		void OnUpdate(const istd::IChangeable::ChangeSet& changeSet) override
		{
			int changeFlags = changeSet.GetFlags();
			if (changeFlags & IDeviceStateProvider::CF_STATE_CHANGED)
			{
				// Update UI based on state change
			}
		}
	};
	@endcode
	
	@see IDeviceStateProvider
	@see IDeviceController
	@see CDeviceInstanceListAdapterComp
	@ingroup imtdev
*/
class CDeviceStateProviderAdapterComp:
			public icomp::CComponentBase,
			public IDeviceStateProvider
{
public:
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_COMPONENT(CDeviceStateProviderAdapterComp);
		I_REGISTER_INTERFACE(IDeviceStateProvider);
		I_ASSIGN(m_deviceControllerCompPtr, "DeviceController", "Device controller", false, "DeviceController");
	I_END_COMPONENT;

	CDeviceStateProviderAdapterComp();

	// reimplemented (imtdev::IDeviceStateProvider)
	virtual DeviceState GetDeviceState(const QByteArray& deviceId) const override;

protected:
	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;
	virtual void OnComponentDestroyed() override;

private:
	I_REF(IDeviceController, m_deviceControllerCompPtr);

	imod::CModelUpdateBridge m_updateBridge;
};


} // namespace imtdev


