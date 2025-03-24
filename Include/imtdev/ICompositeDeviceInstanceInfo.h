#pragma once


// ImtCore includes
#include <imtdev/IDeviceInstanceInfo.h>
#include <imtdev/ICompositeDeviceStaticInfo.h>


namespace imtbase
{
	class ICollectionInfo;
}


namespace imtdev
{


/**
	Interface for describing information about a composite device instance
*/
class ICompositeDeviceInstanceInfo: virtual public IDeviceInstanceInfo
{
public:
	/**
		Get device static info associated with composite device instance
	*/
	virtual const ICompositeDeviceStaticInfo* GetCompositeStaticInfo() const = 0;

	/**
		Get the list of supported sub-devices type IDs
	*/
	virtual QSet<QByteArray> GetSupportedSubDeviceTypeIds() const = 0;

	/**
		Get the list of available sub-devices
	*/
	virtual const imtbase::ICollectionInfo& GetSubDeviceList() const = 0;

	/**
		Get device instance info of sub-device with given id
	*/
	virtual const IDeviceInstanceInfo* GetSubDeviceInstanceInfo(const QByteArray& subDeviceId) const = 0;
};


typedef std::unique_ptr<ICompositeDeviceInstanceInfo> CompositeDeviceInstanceInfoPtr;


} // namespace imtdev


