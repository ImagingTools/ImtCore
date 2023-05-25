#pragma once


// Qt includes
#include <QtCore/QSharedPointer>

// ACF includes
#include <iprm/IParamsSet.h>
#include <istd/IChangeable.h>


namespace imtdev
{


typedef QSharedPointer<iprm::IParamsSet> DeviceConfigurationPtr;


class IDeviceConfigurationManager: virtual public istd::IChangeable
{
public:
	virtual DeviceConfigurationPtr GetDeviceConfiguration(
				const QByteArray& deviceId,
				const QByteArray& deviceTypeId) const = 0;
	virtual bool SetDeviceConfiguration(
				const QByteArray& deviceId,
				const QByteArray& deviceTypeId,
				const iprm::IParamsSet& configuration) = 0;
};


} // namespace imtdev


