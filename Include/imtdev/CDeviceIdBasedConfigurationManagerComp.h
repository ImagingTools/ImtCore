#pragma once


// Qt includes
#include <QtCore/QMap>
#include <QtCore/QRecursiveMutex>

// ACF includes
#include <imod/TModelWrap.h>
#include <ilog/TLoggerCompWrap.h>

// ImtCore includes
#include <imtdev/IDeviceController.h>
#include <imtdev/IDeviceConfigurationManager.h>
#include <imtdev/IDeviceStaticInfo.h>


namespace imtdev
{


class CDeviceIdBasedConfigurationManagerComp:
			public ilog::CLoggerComponentBase,
			virtual public imtdev::IDeviceConfigurationManager,
			virtual public iser::ISerializable
{
public:
	typedef ilog::CLoggerComponentBase BaseClass;

	I_BEGIN_BASE_COMPONENT(CDeviceIdBasedConfigurationManagerComp)
		I_REGISTER_INTERFACE(IDeviceConfigurationManager);
		I_ASSIGN(m_deviceControllerCompPtr, "DeviceController", "Device controller for creating and validating configurations", false, "");
	I_END_COMPONENT;

	// reimplemented (IDeviceConfigurationProvider)
	virtual DeviceConfigurationPtr GetDeviceConfiguration(const QByteArray& deviceId) const override;
	virtual bool SetDeviceConfiguration(const QByteArray& deviceId, const iprm::IParamsSet* configurationPtr) override;

	// reimplemented (iser::ISerializable)
	virtual bool Serialize(iser::IArchive& archive) override;

private:
	bool GetStaticDeviceInfo(const QByteArray& deviceId, const IDeviceStaticInfo** staticInfoPtr) const;

private:
	I_REF(IDeviceController, m_deviceControllerCompPtr);

	QMap<QByteArray, DeviceConfigurationPtr> m_configurations;
};


} // namespace imtdev


