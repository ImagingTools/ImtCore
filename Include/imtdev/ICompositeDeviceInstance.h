#pragma once


// ImtCore includes
#include <imtdev/IDeviceInstance.h>
#include <imtdev/ICompositeDeviceSpecification.h>


namespace imtbase
{
	class ICollectionInfo;
}


namespace imtdev
{


/**
	Interface for describing information about a composite device instance
*/
class ICompositeDeviceInstance: virtual public IDeviceInstance
{
public:
	/**
		Get device static info associated with composite device instance
	*/
	virtual const ICompositeDeviceSpecification* GetCompositeDeviceSpecification() const = 0;

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
	virtual const IDeviceInstance* GetSubDeviceInstance(const QByteArray& subDeviceId) const = 0;
};


typedef std::shared_ptr<ICompositeDeviceInstance> CompositeDeviceInstancePtr;


} // namespace imtdev


