#pragma once


// ImtCore includes
#include <imtdev/ICompositeDeviceInstance.h>


namespace imtdev
{


class IDeviceSpecification;


/**
	Interface for changing information about a composite device instance
*/
class IEditableCompositeDeviceInstance : virtual public ICompositeDeviceInstance
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


typedef std::shared_ptr<IEditableCompositeDeviceInstance> EditableCompositeDeviceInstancePtr;


} // namespace imtdev


