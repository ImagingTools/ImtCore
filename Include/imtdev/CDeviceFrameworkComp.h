#pragma once


// ACF includes
#include <icomp/CComponentBase.h>

// ImtCore includes
#include <imtdev/IDeviceFramework.h>


namespace imtdev
{


class CDeviceFrameworkComp :
			public icomp::CComponentBase,
			virtual public IDeviceFramework
{
public:
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_COMPONENT(CDeviceFrameworkComp);
		I_REGISTER_INTERFACE(IDeviceFramework);
		I_ASSIGN(m_typeListCompPtr, "DeviceTypeList", "List of supported device types", false, "DeviceTypeList");
		I_ASSIGN(m_deviceListCompPtr, "DeviceList", "List of connected devices", false, "DeviceList");
		I_ASSIGN(m_extendedDeviceListCompPtr, "ExtendedDeviceList", "Extended device list", false, "ExtendedDeviceList");
		I_ASSIGN(m_connectionStateCompPtr, "DeviceConnectionState", "Device connection state provider", false, "DeviceConnectionState");
		I_ASSIGN(m_deviceControllerCompPtr, "DeviceController", "Device controller", false, "DeviceController");
	I_END_COMPONENT;

	// reimplemented (IDeviceFramework)
	virtual imtbase::ICollectionInfo* GetDeviceTypeList() const override;
	virtual imtbase::ICollectionInfo* GetDeviceList() const override;
	virtual imtbase::ICollectionInfo* GetExtendedDeviceList() const override;
	virtual IDeviceConnectionState* GetDeviceConnectionState() const override;
	virtual IDeviceController* GetDeviceController() const override;

private:
	I_REF(imtbase::ICollectionInfo, m_typeListCompPtr);
	I_REF(imtbase::ICollectionInfo, m_deviceListCompPtr);
	I_REF(imtbase::ICollectionInfo, m_extendedDeviceListCompPtr);
	I_REF(IDeviceConnectionState, m_connectionStateCompPtr);
	I_REF(IDeviceController, m_deviceControllerCompPtr);
};


} // namespace imtdev


