#pragma once


// ImtCore includes
#include <imtdev/ICompositeDeviceInstanceInfo.h>


namespace imtdev
{


class IDeviceStaticInfo;


/**
	Interface for changing information about a composite device instance
*/
class ICompositeDeviceInstanceInfoEditor : virtual public ICompositeDeviceInstanceInfo
{
public:
	/**
		Add new sub-device with given type id
	*/
	virtual QByteArray AddSubDevice(
				const QByteArray& deviceTypeId,
				const QString& name,
				const QString& description = QString(),
				const istd::IChangeable* defaultValuePtr = nullptr,
				const QByteArray& proposedId = QByteArray()) = 0;
	/**
		Remove sub-device with given id
	*/
	virtual bool RemoveSubDevice(const QByteArray& subDeviceId) = 0;
};


typedef std::shared_ptr<ICompositeDeviceInstanceInfoEditor> CompositeDeviceInstanceInfoEditorPtr;


} // namespace imtdev


