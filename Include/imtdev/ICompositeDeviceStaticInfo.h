#pragma once


// ImtCore includes
#include <imtdev/IDeviceStaticInfo.h>
#include <imtbase/ICollectionInfo.h>


namespace imtdev
{


/**
	Interface for describing "static" information about a composite device
*/
class ICompositeDeviceStaticInfo: virtual public IDeviceStaticInfo
{
public:
	/**
		Get the list of available sub-devices
	*/
	virtual const imtbase::ICollectionInfo& GetSubDeviceList() const = 0;

	/**
		Get static device info of subdevice with given id
	*/
	virtual const IDeviceStaticInfo* GetSubDeviceStaticInfo(const QByteArray& id) const = 0;
};


} // namespace imtdev


