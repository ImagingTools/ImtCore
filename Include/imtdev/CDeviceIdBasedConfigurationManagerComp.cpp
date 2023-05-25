#include <imtdev/CDeviceIdBasedConfigurationManagerComp.h>


// ACF includes
#include <iprm/CParamsSet.h>
#include <istd/CChangeNotifier.h>

// ImtCore includes
#include <imtdev/IDeviceController.h>
#include <imtdev/IDeviceStaticInfo.h>


namespace imtdev
{


// public methods

// reimplemented (IDeviceConfigurationManager)

DeviceConfigurationPtr CDeviceIdBasedConfigurationManagerComp::GetDeviceConfiguration(
			const QByteArray& deviceId,
			const QByteArray& deviceTypeId) const
{
	DeviceConfigurationPtr configurationPtr;

	if (!deviceId.isEmpty() && !deviceTypeId.isEmpty()){
		const IDeviceStaticInfo* staticInfoPtr = FindDeviceStaticInfo(deviceId);
		if (staticInfoPtr == nullptr){
			staticInfoPtr = m_deviceControllerCompPtr->GetDeviceStaticInfo(deviceTypeId);
		}

		if (staticInfoPtr != nullptr && staticInfoPtr->GetDeviceTypeId() == deviceTypeId){
			if (m_configurations.contains(deviceId) && m_configurations[deviceId].deviceTypeId == deviceTypeId){
				if (staticInfoPtr->AreConfigurationAccepted(*m_configurations[deviceId].configurationPtr.data())) {
					configurationPtr.reset(new iprm::CParamsSet());
					configurationPtr->CopyFrom(*m_configurations[deviceId].configurationPtr.data());
				}
			}

			if (configurationPtr.isNull()){
				configurationPtr.reset(new iprm::CParamsSet());
				configurationPtr->CopyFrom(staticInfoPtr->GetDefaultConfiguration());
			}
		}
	}

	return configurationPtr;
}


bool CDeviceIdBasedConfigurationManagerComp::SetDeviceConfiguration(
			const QByteArray& deviceId,
			const QByteArray& deviceTypeId,
			const iprm::IParamsSet& configuration)
{
	if (!deviceId.isEmpty() && !deviceTypeId.isEmpty()){
		const IDeviceStaticInfo* staticInfoPtr = FindDeviceStaticInfo(deviceId);
		if (staticInfoPtr == nullptr){
			staticInfoPtr = m_deviceControllerCompPtr->GetDeviceStaticInfo(deviceTypeId);
		}

		if (staticInfoPtr != nullptr && staticInfoPtr->GetDeviceTypeId() == deviceTypeId){
			if (staticInfoPtr->AreConfigurationAccepted(configuration)){
				DeviceConfigurationPtr deviceConfigurationPtr(new iprm::CParamsSet);
				if (deviceConfigurationPtr->CopyFrom(configuration)){
					istd::CChangeNotifier notifier(this);

					m_configurations[deviceId].deviceTypeId = deviceTypeId;
					m_configurations[deviceId].configurationPtr = deviceConfigurationPtr;

					return true;
				}
			}
		}
	}

	return false;
}


// reimplemented (iser::ISerializable)

bool CDeviceIdBasedConfigurationManagerComp::Serialize(iser::IArchive& archive)
{
	istd::CChangeNotifier changeNotifier(archive.IsStoring() ? nullptr : this);

	int itemCount = m_configurations.count();
	if (!archive.IsStoring()){
		m_configurations.clear();
		itemCount = 0;
	}

	static iser::CArchiveTag itemsTag("Items", "Items", iser::CArchiveTag::TT_MULTIPLE);
	static iser::CArchiveTag itemTag("Item", "Item", iser::CArchiveTag::TT_GROUP, &itemsTag);
	bool retVal = archive.BeginMultiTag(itemsTag, itemTag, itemCount);

	QByteArrayList deviceIds = m_configurations.keys();

	for (int i = 0; i < itemCount; i++){
		retVal = retVal && archive.BeginTag(itemTag);

		QByteArray deviceId;
		QByteArray deviceTypeId;
		DeviceConfigurationPtr configurationPtr;

		if (archive.IsStoring()){
			deviceId = deviceIds[i];
			deviceTypeId = m_configurations[deviceId].deviceTypeId;
			configurationPtr = m_configurations[deviceId].configurationPtr;
		}

		static iser::CArchiveTag deviceIdTag("DeviceId", "DeviceId", iser::CArchiveTag::TT_LEAF, &itemTag);
		retVal = retVal && archive.BeginTag(deviceIdTag);
		retVal = retVal && archive.Process(deviceId);
		retVal = retVal && archive.EndTag(deviceIdTag);

		static iser::CArchiveTag deviceTypeIdTag("DeviceTypeId", "DeviceTypeId", iser::CArchiveTag::TT_LEAF, &itemTag);
		retVal = retVal && archive.BeginTag(deviceTypeIdTag);
		retVal = retVal && archive.Process(deviceTypeId);
		retVal = retVal && archive.EndTag(deviceTypeIdTag);

		if (retVal && !archive.IsStoring()){
			ConfigurationFactory* factoryPtr = FindConfigurationFactory(deviceTypeId);
			if (factoryPtr != nullptr){
				configurationPtr.reset(factoryPtr->CreateInstance());
				Q_ASSERT(!configurationPtr.isNull());
				if (configurationPtr.isNull()){
					retVal = false;
				}
			}
			else{
				retVal = false;
			}
		}

		static iser::CArchiveTag configurationTag("Configuration", "Configuration", iser::CArchiveTag::TT_GROUP, &itemTag);
		retVal = retVal && archive.BeginTag(configurationTag);
		retVal = retVal && configurationPtr->Serialize(archive);
		retVal = retVal && archive.EndTag(configurationTag);

		if (retVal && !archive.IsStoring()){
			m_configurations[deviceId].deviceTypeId = deviceTypeId;
			m_configurations[deviceId].configurationPtr = configurationPtr;
		}

		retVal = retVal && archive.EndTag(itemTag);
	}

	retVal = retVal && archive.EndTag(itemsTag);

	if (!retVal){
		m_configurations.clear();
	}

	return retVal;
}


// private methods

const IDeviceStaticInfo* CDeviceIdBasedConfigurationManagerComp::FindDeviceStaticInfo(const QByteArray& deviceId) const
{
	if (!deviceId.isEmpty() && m_deviceControllerCompPtr.IsValid()){
		DeviceInstanceInfoPtr deviceInstanceInfoPtr = m_deviceControllerCompPtr->GetDeviceInstanceInfo(deviceId);
		if (!deviceInstanceInfoPtr.isNull()){
			return &deviceInstanceInfoPtr->GetDeviceStaticInfo();
		}
	}

	return nullptr;
}


CDeviceIdBasedConfigurationManagerComp::ConfigurationFactory* CDeviceIdBasedConfigurationManagerComp::FindConfigurationFactory(const QByteArray& deviceTypeId) const
{
	if (m_deviceConfigurationFactoryCompPtr.IsValid()){
		int count = m_deviceConfigurationFactoryCompPtr.GetCount();
		for (int i = 0; i < count; i++){
			if (m_deviceConfigurationFactoryCompPtr[i] != nullptr){
				if (m_deviceConfigurationFactoryCompPtr[i]->GetFactoryKeys().contains(deviceTypeId)){
					return m_deviceConfigurationFactoryCompPtr[i];
				}
			}
		}
	}

	return nullptr;
}


} // namespace imtdev


