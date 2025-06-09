#include <imtdev/CDeviceFrameworkComp.h>


namespace imtdev
{


// public methods

// reimplemented (IDeviceFramework)

imtbase::ICollectionInfo* CDeviceFrameworkComp::GetDeviceTypeList() const
{
	return m_typeListCompPtr.GetPtr();
}


imtbase::ICollectionInfo* CDeviceFrameworkComp::GetDeviceList() const
{
	return m_deviceListCompPtr.GetPtr();
}


imtbase::ICollectionInfo* CDeviceFrameworkComp::GetExtendedDeviceList() const
{
	return m_extendedDeviceListCompPtr.GetPtr();
}


IDeviceStateProvider* CDeviceFrameworkComp::GetDeviceStateProvider() const
{
	return m_stateProviderCompPtr.GetPtr();
}


IDeviceController* CDeviceFrameworkComp::GetDeviceController() const
{
	return m_controllerCompPtr.GetPtr();
}


} // namespace imtdev


