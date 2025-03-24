#pragma once


// ImtCore includes
#include <imtdev/ICompositeDeviceInstanceInfo.h>


namespace imtdev
{


class IDeviceStaticInfo;


class ICompositeDeviceInstanceInfoEditor : virtual public ICompositeDeviceInstanceInfo
{
public:
	virtual QByteArray AddSubDevice(
				const QByteArray& deviceTypeId,
				const QString& name,
				const QString& description = QString(),
				const istd::IChangeable* defaultValuePtr = nullptr,
				const QByteArray& proposedId = QByteArray()) = 0;
	virtual bool RemoveSubDevice(const QByteArray& subDeviceId) = 0;
};


typedef std::unique_ptr<ICompositeDeviceInstanceInfoEditor> CompositeDeviceInstanceInfoEditorPtr;


} // namespace imtdev


