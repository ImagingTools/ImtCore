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


IDeviceConnectionState* CDeviceFrameworkComp::GetDeviceConnectionState() const
{
	return m_connectionStateCompPtr.GetPtr();
}


IDeviceController* CDeviceFrameworkComp::GetDeviceController() const
{
	return m_deviceControllerCompPtr.GetPtr();
}


} // namespace imtdev


