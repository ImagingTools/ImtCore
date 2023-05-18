#include <imtdev/CDeviceIdBasedConfigurationManagerComp.h>


// Qt includes
#include <QtCore/QMutexLocker>
#include <QtCore/QThread>

// ACF includes
#include <iprm/CParamsSet.h>
#include <imod/TModelWrap.h>
#include <iprm/IOptionsList.h>
#include <ilog/TLoggerCompWrap.h>
#include <istd/CChangeNotifier.h>

// ImtCore includes
#include <imtbase/ICollectionInfo.h>
#include <imtdev/IDeviceController.h>


namespace imtdev
{


// public methods

// reimplemented (IDeviceConfigurationManager)

IDeviceConfigurationManager::DeviceConfigurationPtr CDeviceIdBasedConfigurationManagerComp::GetDeviceConfiguration(const QByteArray& deviceId) const
{
	DeviceConfigurationPtr configurationPtr;

	if (!deviceId.isEmpty()){
		const IDeviceStaticInfo* staticInfoPtr = nullptr;
		GetStaticDeviceInfo(deviceId, &staticInfoPtr);

		if (m_configurations.contains(deviceId)){
			if (staticInfoPtr != nullptr){
				if (staticInfoPtr->AreConfigurationAccepted(m_configurations[deviceId].data())) {
					configurationPtr.reset(new iprm::CParamsSet());
					configurationPtr->CopyFrom(*m_configurations[deviceId].data());
				}
				else{
					configurationPtr.reset(new iprm::CParamsSet());
					configurationPtr->CopyFrom(staticInfoPtr->GetDefaultConfiguration());
				}
			}
			else{
				configurationPtr->CopyFrom(*m_configurations[deviceId].data());
			}
		}
		else if (staticInfoPtr != nullptr){
			configurationPtr.reset(new iprm::CParamsSet());
			configurationPtr->CopyFrom(staticInfoPtr->GetDefaultConfiguration());
		}
	}

	return configurationPtr;
}


bool CDeviceIdBasedConfigurationManagerComp::SetDeviceConfiguration(const QByteArray& deviceId, const iprm::IParamsSet* configurationPtr)
{
	if (!deviceId.isEmpty() && configurationPtr != nullptr){
		const IDeviceStaticInfo* staticInfoPtr = nullptr;
		GetStaticDeviceInfo(deviceId, &staticInfoPtr);

		// If the (device is connected) && (the configuration is not compatible)
		if (staticInfoPtr != nullptr){
			if (!staticInfoPtr->AreConfigurationAccepted(configurationPtr)){
				return false;
			}
		}

		// If (device not connected) || (device connected && configuration compatible)
		DeviceConfigurationPtr deviceConfigurationPtr(new iprm::CParamsSet);
		if (deviceConfigurationPtr->CopyFrom(*configurationPtr)){
			istd::CChangeNotifier notifier(this);

			m_configurations[deviceId] = deviceConfigurationPtr;

			return true;
		}
	}

	return false;
}


// reimplemented (iser::ISerializable)

bool CDeviceIdBasedConfigurationManagerComp::Serialize(iser::IArchive& archive)
{
	istd::CChangeNotifier changeNotifier(archive.IsStoring() ? nullptr : this);

	int configurationCount = m_configurations.count();

	if (!archive.IsStoring()){
		m_configurations.clear();
		configurationCount = 0;
	}

	bool retVal = true;

	static iser::CArchiveTag configurationsTag("Configurations", "List of configurations", iser::CArchiveTag::TT_MULTIPLE);
	static iser::CArchiveTag configurationTag("Configuration", "Configuration", iser::CArchiveTag::TT_GROUP, &configurationsTag);
	retVal = retVal && archive.BeginMultiTag(configurationsTag, configurationTag, configurationCount);

	QByteArrayList deviceIds = m_configurations.keys();

	for (int i = 0; i < configurationCount; i++){
		retVal = retVal && archive.BeginTag(configurationTag);

		QByteArray deviceId;
		DeviceConfigurationPtr configurationPtr;

		if (archive.IsStoring()){
			deviceId = deviceIds[i];
			configurationPtr = m_configurations[deviceId];
		}
		else{
			configurationPtr.reset(new iprm::CParamsSet());
		}

		static iser::CArchiveTag idTag("Id", "Id", iser::CArchiveTag::TT_LEAF, &configurationTag);
		retVal = retVal && archive.BeginTag(idTag);
		retVal = retVal && archive.Process(deviceId);
		retVal = retVal && archive.EndTag(idTag);

		static iser::CArchiveTag dataTag("Data", "Data", iser::CArchiveTag::TT_LEAF, &configurationTag);
		retVal = retVal && archive.BeginTag(dataTag);
		retVal = retVal && configurationPtr->Serialize(archive);
		retVal = retVal && archive.EndTag(dataTag);

		if (!archive.IsStoring()){
			m_configurations[deviceId] = configurationPtr;
		}

		retVal = retVal && archive.EndTag(configurationTag);
	}

	retVal = retVal && archive.EndTag(configurationsTag);

	return false;
}


// private methods

bool CDeviceIdBasedConfigurationManagerComp::GetStaticDeviceInfo(const QByteArray& deviceId, const IDeviceStaticInfo** staticInfoPtr) const
{
	if (!deviceId.isEmpty() && staticInfoPtr != nullptr && m_deviceControllerCompPtr.IsValid()){
		DeviceInstanceInfoPtr deviceInstanceInfoPtr = m_deviceControllerCompPtr->GetDeviceInstanceInfo(deviceId);
		if (!deviceInstanceInfoPtr.isNull()){
			*staticInfoPtr = &deviceInstanceInfoPtr->GetDeviceStaticInfo();

			return true;
		}
	}

	return false;
}


} // namespace imtdev


