#include <imtdev/CDeviceStateProviderAdapterComp.h>


// ACF includes
#include <istd/CChangeNotifier.h>


namespace imtdev
{


// public methods

CDeviceStateProviderAdapterComp::CDeviceStateProviderAdapterComp()
	:m_updateBridge(this)
{
}


// reimplemented (imtdev::IDeviceStateProvider)

IDeviceStateProvider::DeviceState CDeviceStateProviderAdapterComp::GetDeviceState(const QByteArray& deviceId) const
{
	if (m_deviceControllerCompPtr.IsValid()){
		m_deviceControllerCompPtr->GetDeviceStateProvider().GetDeviceState(deviceId);
	}

	return DeviceState::DS_NONE;
}


// protected methods

// reimplemented (icomp::CComponentBase)

void CDeviceStateProviderAdapterComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	if (m_deviceControllerCompPtr.IsValid()){
		imod::IModel* modelPtr = const_cast<imod::IModel*>(
			dynamic_cast<const imod::IModel*>(&m_deviceControllerCompPtr->GetDeviceStateProvider()));
		if (modelPtr != nullptr){
			modelPtr->AttachObserver(&m_updateBridge);
		}
	}
}


void CDeviceStateProviderAdapterComp::OnComponentDestroyed()
{
	m_updateBridge.EnsureModelsDetached();

	BaseClass::OnComponentDestroyed();
}


} //namespace imtdev