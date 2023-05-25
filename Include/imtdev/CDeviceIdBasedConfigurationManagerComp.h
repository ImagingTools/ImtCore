#pragma once


// Qt includes
#include <QtCore/QMap>

// ACF includes
#include <ilog/TLoggerCompWrap.h>
#include <ilog/TLoggerCompWrap.h>
#include <istd/TIFactory.h>

// ImtCore includes
#include <imtdev/IDeviceConfigurationManager.h>


namespace imtdev
{


class IDeviceController;
class IDeviceStaticInfo;


class CDeviceIdBasedConfigurationManagerComp:
			public ilog::CLoggerComponentBase,
			virtual public imtdev::IDeviceConfigurationManager,
			virtual public iser::ISerializable
{
public:
	typedef ilog::CLoggerComponentBase BaseClass;

	I_BEGIN_BASE_COMPONENT(CDeviceIdBasedConfigurationManagerComp)
		I_REGISTER_INTERFACE(IDeviceConfigurationManager);
		I_REGISTER_INTERFACE(iser::ISerializable);
		I_REGISTER_INTERFACE(istd::IChangeable);
		I_ASSIGN(m_deviceControllerCompPtr, "DeviceController", "Device controller for creating and validating configurations", false, "");
		I_ASSIGN_MULTI_0(m_deviceConfigurationFactoryCompPtr, "ConfigurationFactory", "Configuration factories for deserialization", false);
	I_END_COMPONENT;

	// reimplemented (IDeviceConfigurationManager)
	virtual DeviceConfigurationPtr GetDeviceConfiguration(
				const QByteArray& deviceId,
				const QByteArray& deviceTypeId) const override;
	virtual bool SetDeviceConfiguration(
				const QByteArray& deviceId,
				const QByteArray& deviceTypeId,
				const iprm::IParamsSet& configuration) override;

	// reimplemented (iser::ISerializable)
	virtual bool Serialize(iser::IArchive& archive) override;

private:
	typedef istd::TIFactory<iprm::IParamsSet> ConfigurationFactory;

	struct ConfigurationInfo
	{
		QByteArray deviceTypeId;
		DeviceConfigurationPtr configurationPtr;
	};

private:
	const IDeviceStaticInfo* FindDeviceStaticInfo(const QByteArray& deviceId) const;
	ConfigurationFactory* FindConfigurationFactory(const QByteArray& deviceTypeId) const;

private:
	I_REF(IDeviceController, m_deviceControllerCompPtr);
	I_MULTIREF(ConfigurationFactory, m_deviceConfigurationFactoryCompPtr);

	QMap<QByteArray, ConfigurationInfo> m_configurations;
};


} // namespace imtdev


