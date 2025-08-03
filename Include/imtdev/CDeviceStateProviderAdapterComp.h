#pragma once


// ACF includes
#include <icomp/CComponentBase.h>
#include <imod/CModelUpdateBridge.h>

// ImtCore includes
#include <imtdev/IDeviceController.h>


namespace imtdev
{


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


