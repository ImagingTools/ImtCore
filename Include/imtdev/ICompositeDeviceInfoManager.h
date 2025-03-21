#pragma once


// ImtCore includes
#include <imtdev/ICompositeDeviceInfo.h>
#include <imtdev/IDeviceInstanceInfoManager.h>
#include <imtdev/ICompositeDeviceInfoManager.h>


namespace imtbase
{
	class ICollectionInfo;
}


namespace imtdev
{


class IDeviceStaticInfo;


class ICompositeDeviceInfoManager: virtual public ICompositeDeviceInfo
{
public:
	virtual imtdev::IDeviceInstanceInfoManager* GetDeviceInstanceInfo() = 0;
	virtual ICompositeDeviceInfoManager* GetSubDeviceInfo(const QByteArray& subDeviceId) = 0;
	virtual QSet<QByteArray> GetSupportedSubDeviceTypeIds() const = 0;
	virtual QByteArray AddSubDevice(
		const QByteArray& deviceTypeId,
		const QString& name,
		const QString& description = QString(),
		const istd::IChangeable* defaultValuePtr = nullptr,
		const QByteArray& proposedId = QByteArray()) = 0;
	virtual bool RemoveSubDevice(const QByteArray& id) = 0;
};


typedef std::unique_ptr<ICompositeDeviceInfoManager> CompositeDeviceInfoManagerPtr;


} // namespace imtdev


