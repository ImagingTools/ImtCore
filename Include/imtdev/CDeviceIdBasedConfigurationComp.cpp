// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtdev/CDeviceIdBasedConfigurationComp.h>


// ACF includes
#include <iprm/CParamsSet.h>
#include <istd/CChangeNotifier.h>

// ImtCore includes
#include <imtdev/IDeviceController.h>
#include <imtdev/IDeviceSpecification.h>


namespace imtdev
{


// public methods

CDeviceIdBasedConfigurationComp::CDeviceIdBasedConfigurationComp()
	:m_selectionObserver(*this),
	m_configurationObserver(*this),
	m_stateProviderObserver(*this)
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

	if (m_stateProviderCompPtr.IsValid()){
		m_stateProviderObserver.RegisterObject(m_stateProviderCompPtr.GetPtr(), &CDeviceIdBasedConfigurationComp::OnDeviceStateChanged);
	}
}


void CDeviceIdBasedConfigurationComp::OnComponentDestroyed()
{
	m_stateProviderObserver.UnregisterAllObjects();
	m_configurationObserver.UnregisterAllObjects();
	m_selectionObserver.UnregisterAllObjects();

	BaseClass::OnComponentDestroyed();
}


// private methods

DeviceInstancePtr CDeviceIdBasedConfigurationComp::GetDeviceInstance(const QByteArray& deviceId) const
{
	if (!deviceId.isEmpty() && m_controllerCompPtr.IsValid()){
		DeviceInstancePtr deviceInstanceInfoPtr = m_controllerCompPtr->GetDeviceInstance(deviceId);

		return deviceInstanceInfoPtr;
	}

	return nullptr;
}


void CDeviceIdBasedConfigurationComp::UpdateModel()
{
	FlagLocker locker(m_isConfigurationStoreBlocked);

	{
		istd::CChangeNotifier notifier(this);

		ResetData();

		if (m_deviceSelectionCompPtr.IsValid() && m_configurationManagerCompPtr.IsValid()){

			imtbase::ISelection::Ids ids = m_deviceSelectionCompPtr->GetSelectedIds();
			if (ids.count() != 1){
				return;
			}

			QByteArray deviceId = *ids.begin();
			QByteArray deviceTypeId;

			DeviceInstancePtr instancePtr = GetDeviceInstance(deviceId);
			if (instancePtr != nullptr){
				deviceTypeId = instancePtr->GetDeviceSpecification().GetTypeId();
			}

			if (deviceTypeId.isEmpty() && m_controllerCompPtr.IsValid()){
				if (m_controllerCompPtr->GetSupportedDeviceTypeIds().contains(deviceId)){
					deviceTypeId = deviceId;
				}
			}

			if (!deviceTypeId.isEmpty()){
				DeviceConfigurationPtr configurationPtr = m_configurationManagerCompPtr->GetDeviceConfiguration(deviceId);
				if (configurationPtr.IsValid()){
					CopyFrom(*configurationPtr);
				}
			}
		}
	}
}


void CDeviceIdBasedConfigurationComp::OnSelectionChanged(
			const istd::IChangeable::ChangeSet& /*changeSet*/,
			const imtbase::ISelection* /*objectPtr*/)
{
	UpdateModel();
}


void CDeviceIdBasedConfigurationComp::OnConfigurationChanged(
			const istd::IChangeable::ChangeSet& /*changeSet*/,
			const iprm::IParamsSet* /*objectPtr*/)
{
	if (m_isConfigurationStoreBlocked){
		return;
	}

	if (!m_deviceSelectionCompPtr.IsValid() || !m_configurationManagerCompPtr.IsValid() || !m_controllerCompPtr.IsValid()){
		return;
	}

	imtbase::ISelection::Ids ids = m_deviceSelectionCompPtr->GetSelectedIds();
	if (ids.count() == 1 && !ids.begin()->isEmpty()){
		QByteArray deviceId = *ids.begin();

		if (m_configurationManagerCompPtr.IsValid()){
			DeviceInstancePtr deviceInstanceInfoPtr = GetDeviceInstance(deviceId);

			QByteArray deviceTypeId;
			if (deviceInstanceInfoPtr != nullptr){
				deviceTypeId = deviceInstanceInfoPtr->GetDeviceSpecification().GetTypeId();
			}

			if (deviceTypeId.isEmpty()){
				if (m_controllerCompPtr->GetSupportedDeviceTypeIds().contains(deviceId)){
					deviceTypeId = deviceId;
				}
			}

			if (!deviceTypeId.isEmpty()){
				m_configurationManagerCompPtr->SetDeviceConfiguration(deviceId, *this);
			}
		}
	}
}


void CDeviceIdBasedConfigurationComp::OnDeviceStateChanged(
			const istd::IChangeable::ChangeSet& /*changeSet*/,
			const IDeviceStateProvider* /*objectPtr*/)
{
	UpdateModel();
}


} // namespace imtdev


