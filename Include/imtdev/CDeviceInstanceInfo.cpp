#include "CDeviceInstanceInfo.h"


namespace imtdev
{


CDeviceInstanceInfo::CDeviceInstanceInfo()
	:m_deviceStaticInfoPtr(nullptr)
{
}


void CDeviceInstanceInfo::SetDeviceStaticInfo(const IDeviceStaticInfo& deviceStaticInfo)
{
	m_deviceStaticInfoPtr = &deviceStaticInfo;
}


const IDeviceStaticInfo& CDeviceInstanceInfo::GetDeviceStaticInfo() const
{
	Q_ASSERT(m_deviceStaticInfoPtr != nullptr);

	return *m_deviceStaticInfoPtr;
}


} // namespace imtdev


