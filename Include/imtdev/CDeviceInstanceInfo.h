#pragma once


// ImtCore includes
#include <imtdev/IDeviceInstanceInfo.h>



namespace imtdev
{


class CDeviceInstanceInfo: virtual public IDeviceInstanceInfo
{
public:
	CDeviceInstanceInfo();
	void SetDeviceStaticInfo(const IDeviceStaticInfo& deviceStaticInfo);

	// reimplemented (IDeviceInstanceInfo)
	virtual const IDeviceStaticInfo& GetDeviceStaticInfo() const override;

private:
	const IDeviceStaticInfo* m_deviceStaticInfoPtr;
};


} // namespace imtdev


