#include <imtdev/CDeviceIdBasedConfigurationComp.h>


// ACF includes
#include <iprm/CParamsSet.h>
#include <istd/CChangeNotifier.h>

// ImtCore includes
#include <imtdev/IDeviceController.h>
#include <imtdev/IDeviceStaticInfo.h>


namespace imtdev
{


// public methods

CDeviceIdBasedConfigurationComp::CDeviceIdBasedConfigurationComp()
	:m_selectionObserver(*this),
	m_configurationObserver(*this),
	m_connectionStateObserver(*this)
{
}


// protected methods

// reimplemented (icomp::CComponentBase)

void CDeviceIdBasedConfigurationComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	if (m_deviceSelectionCompPtr.IsValid()){
		m_selectionObserver.RegisterObject(m_deviceSelectionCompPtr.GetPtr(), &CDeviceIdBasedConfigurationComp::OnSelectionChanged);
	}

	m_configurationObserver.RegisterObject(this, &CDeviceIdBasedConfigurationComp::OnConfigurationChanged);

	if (m_connectionStateCompPtr.IsValid()){
		m_connectionStateObserver.RegisterObject(m_connectionStateCompPtr.GetPtr(), &CDeviceIdBasedConfigurationComp::OnConnectionStateChanged);
	}
}


void CDeviceIdBasedConfigurationComp::OnComponentDestroyed()
{
	m_connectionStateObserver.UnregisterAllObjects();
	m_configurationObserver.UnregisterAllObjects();
	m_selectionObserver.UnregisterAllObjects();

	BaseClass::OnComponentDestroyed();
}


// private methods

DeviceInstanceInfoPtr CDeviceIdBasedConfigurationComp::GetDeviceInstanceInfo(const QByteArray& deviceId) const
{
	if (!deviceId.isEmpty() && m_deviceControllerCompPtr.IsValid()){
		DeviceInstanceInfoPtr deviceInstanceInfoPtr = m_deviceControllerCompPtr->GetDeviceInstanceInfo("", deviceId);
		if (deviceInstanceInfoPtr != nullptr){
			return deviceInstanceInfoPtr;
		}

		return m_deviceControllerCompPtr->GetDeviceInstanceInfo(deviceId, "");
	}

	return DeviceInstanceInfoPtr();
}


void CDeviceIdBasedConfigurationComp::UpdateModel()
{
	FlagLocker(m_isConfigurationStoreBlocked);


	istd::CChangeNotifier notifier(this);

	ResetData();

	if (m_deviceSelectionCompPtr.IsValid() && m_configurationManagerCompPtr.IsValid()){

		imtbase::ISelection::Ids ids = m_deviceSelectionCompPtr->GetSelectedIds();
		if (ids.count() != 1){
			return;
		}

		QByteArray id = *ids.begin();

		DeviceInstanceInfoPtr instanceInfoPtr = GetDeviceInstanceInfo(id);
		if (instanceInfoPtr != nullptr && m_configurationManagerCompPtr.IsValid()){
			QByteArray deviceTypeId = instanceInfoPtr->GetStaticInfo().GetTypeId();
			if (!deviceTypeId.isEmpty()){
				DeviceConfigurationPtr configurationPtr =
					m_configurationManagerCompPtr->GetDeviceConfiguration(*ids.begin(), deviceTypeId);
				if (configurationPtr != nullptr){
					CopyFrom(*configurationPtr);
				}
			}
		}
	}
}


void CDeviceIdBasedConfigurationComp::OnSelectionChanged(const istd::IChangeable::ChangeSet& changeSet, const imtbase::ISelection* objectPtr)
{
	UpdateModel();
}


void CDeviceIdBasedConfigurationComp::OnConfigurationChanged(const istd::IChangeable::ChangeSet& changeSet, const iprm::IParamsSet* objectPtr)
{
	if (m_isConfigurationStoreBlocked){
		return;
	}

	if (!m_deviceSelectionCompPtr.IsValid()){
		return;
	}

	imtbase::ISelection::Ids ids = m_deviceSelectionCompPtr->GetSelectedIds();
	if (ids.count() == 1 && !ids.begin()->isEmpty()){
		QByteArray deviceId = *ids.begin();

		if (m_configurationManagerCompPtr.IsValid()){
			DeviceInstanceInfoPtr deviceInstanceInfoPtr = GetDeviceInstanceInfo(deviceId);
			if (deviceInstanceInfoPtr != nullptr){
				QByteArray deviceTypeId = deviceInstanceInfoPtr->GetStaticInfo().GetTypeId();

				m_configurationManagerCompPtr->SetDeviceConfiguration(deviceId, deviceTypeId, *this);
			}
		}
	}
}


void CDeviceIdBasedConfigurationComp::OnConnectionStateChanged(const istd::IChangeable::ChangeSet& changeSet, const IDeviceConnectionState* objectPtr)
{
	UpdateModel();
}


} // namespace imtdev


