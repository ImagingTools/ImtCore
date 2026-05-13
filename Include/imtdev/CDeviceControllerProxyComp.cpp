// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtdev/CDeviceControllerProxyComp.h>


// Qt includes
#include <QtCore/QThread>

// ACF includes
#include <istd/CChangeGroup.h>


namespace imtdev
{


// public methods

CDeviceControllerProxyComp::CDeviceControllerProxyComp()
	:m_deviceStateProviderUpdateBridge(&m_deviceStateProvider, imod::CModelUpdateBridge::UF_SOURCE),
	m_enumeratorIndex(-1),
	m_resultHandlerPtr(nullptr),
	m_overriddenDeviceInfoObserver(*this)
{
	m_deviceStateProvider.SetParent(*this);
}


// reimplemented (IDeviceController)

const QByteArrayList& CDeviceControllerProxyComp::GetSupportedDeviceTypeIds() const
{
	return m_supportedDeviceTypeIds;
}


const IDeviceSpecification* CDeviceControllerProxyComp::GetDeviceStaticInfo(const QByteArray& deviceTypeId) const
{
	IDeviceController* controllerPtr = FindDeviceController(deviceTypeId);
	if (controllerPtr != nullptr){
		return controllerPtr->GetDeviceStaticInfo(deviceTypeId);
	}

	return nullptr;
}


const imtbase::ICollectionInfo& CDeviceControllerProxyComp::GetDeviceInstanceList() const
{
	return m_deviceList;
}


bool CDeviceControllerProxyComp::SetDeviceInstanceName(const QByteArray& deviceId, const QString& name)
{
	if (!m_overriddenDeviceInfo.GetElementIds().contains(deviceId)){
		return !m_overriddenDeviceInfo.InsertItem(deviceId, name, "").isEmpty();
	}

	return m_overriddenDeviceInfo.SetElementName(deviceId, name);
}


bool CDeviceControllerProxyComp::SetDeviceInstanceDescription(const QByteArray& deviceId, const QString& description)
{
	if (!m_overriddenDeviceInfo.GetElementIds().contains(deviceId)){
		return !m_overriddenDeviceInfo.InsertItem(deviceId, "", description).isEmpty();
	}

	return m_overriddenDeviceInfo.SetElementDescription(deviceId, description);
}


DeviceInstancePtr CDeviceControllerProxyComp::GetDeviceInstance(const QByteArray& deviceId) const
{
	IDeviceController* controllerPtr = FindDeviceController(deviceId);
	if (controllerPtr != nullptr){
		return controllerPtr->GetDeviceInstance(deviceId);
	}

	return nullptr;
}


const IDeviceStateProvider& CDeviceControllerProxyComp::GetDeviceStateProvider() const
{
	return m_deviceStateProvider;
}


DeviceAccessorPtr CDeviceControllerProxyComp::OpenDevice(
			const QByteArray& deviceId,
			const iprm::IParamsSet* paramsPtr)
{
	IDeviceController* controllerPtr = FindDeviceController(deviceId);
	if (controllerPtr != nullptr){
		return controllerPtr->OpenDevice(deviceId, paramsPtr);
	}

	return DeviceAccessorPtr();
}


bool CDeviceControllerProxyComp::CloseDevice(const QByteArray& deviceId)
{
	IDeviceController* controllerPtr = FindDeviceController(deviceId);
	if (controllerPtr != nullptr){
		return controllerPtr->CloseDevice(deviceId);
	}

	return false;
}


// reimplemented (IDeviceEnumerator)

IDeviceEnumerator::StartResult CDeviceControllerProxyComp::StartEnumeration(IDeviceEnumerator::IResultHandler* /*resultHandlerPtr*/)
{
	return SR_FAILED;
}


void CDeviceControllerProxyComp::CancelEnumeration()
{
	IDeviceEnumerator* deviceEnumeratorPtr = GetCurrentDeviceEnumerator();
	if (deviceEnumeratorPtr != nullptr){
		deviceEnumeratorPtr->CancelEnumeration();
	}

	m_resultHandlerPtr = nullptr;
}


// reimplemented (imtdev::IDeviceEnumerationResultHandler)

void CDeviceControllerProxyComp::OnEnumerationResult(
			EnumerationResult /*result*/,
			const IDeviceEnumerator* /*deviceEnumeratorPtr*/)
{
	IDeviceEnumerator* deviceEnumeratorPtr = GetNextDeviceEnumerator();
	if (deviceEnumeratorPtr != nullptr){
		deviceEnumeratorPtr->StartEnumeration(this);
	}
	else{
		if (*m_enableAttrPtr && !m_intervalTimer.isActive()){
			StartEnumeration();
		}
		else{
			if (m_resultHandlerPtr != nullptr){
				m_resultHandlerPtr->OnEnumerationResult(ER_OK, this);
			}
		}

		m_resultHandlerPtr = nullptr;
	}
}


// protected methods

// reimplemented (imod::CMultiModelDispatcherBase)

void CDeviceControllerProxyComp::OnModelChanged(int /*modelId*/, const istd::IChangeable::ChangeSet& /*changeSet*/)
{
	Q_ASSERT(qApp->thread() == QThread::currentThread());

	UpdateDeviceList();

	istd::IChangeable::ChangeSet localChangeSet(imtdev::IDeviceStateProvider::CF_STATE_CHANGED);
	istd::CChangeNotifier notifier(&m_deviceStateProvider, &localChangeSet);
}


// reimplemented (icomp::CComponentBase)

void CDeviceControllerProxyComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	if (*m_intervalAttrPtr > 0){
		m_intervalTimer.setInterval(std::chrono::seconds(*m_intervalAttrPtr));
	}

	connect(&m_intervalTimer, &QTimer::timeout, this, &CDeviceControllerProxyComp::OnIntervalTimer);

	UpdateDeviceTypeIdList();

	if (*m_enableAttrPtr){
		StartEnumeration();
	}

	if (m_deviceControllerCompPtr.IsValid()){
		int count = m_deviceControllerCompPtr.GetCount();
		for (int i = 0; i < count; i++){
			IDeviceController* controllerPtr = m_deviceControllerCompPtr[i];
			imod::IModel* modelPtr = dynamic_cast<imod::IModel*>(
				const_cast<IDeviceStateProvider*>(&controllerPtr->GetDeviceStateProvider()));

			if (modelPtr != nullptr){
				modelPtr->AttachObserver(&m_deviceStateProviderUpdateBridge);
			}
		}
	}

	m_overriddenDeviceInfoObserver.RegisterObject(&m_overriddenDeviceInfo, &CDeviceControllerProxyComp::OnOverriddenDeviceInfoUpdated);
}


void CDeviceControllerProxyComp::OnComponentDestroyed()
{
	m_overriddenDeviceInfoObserver.UnregisterAllObjects();

	CancelEnumeration();

	m_intervalTimer.stop();

	UnregisterAllModels();

	BaseClass::OnComponentDestroyed();
}


// private slots

void CDeviceControllerProxyComp::OnIntervalTimer()
{
	if (GetCurrentDeviceEnumerator() == nullptr){
		StartEnumeration();
	}
}


// private methods

void CDeviceControllerProxyComp::StartEnumeration()
{
	Q_ASSERT(m_enumeratorIndex == -1);

	IDeviceEnumerator* deviceEnumeratorPtr = GetNextDeviceEnumerator();
	if (deviceEnumeratorPtr != nullptr){
		if (m_intervalTimer.interval() > 0){
			m_intervalTimer.start();
		}

		deviceEnumeratorPtr->StartEnumeration(this);
	}
}


IDeviceEnumerator* CDeviceControllerProxyComp::GetCurrentDeviceEnumerator()
{
	if (m_enumeratorIndex >= 0 && m_enumeratorIndex < m_deviceEnumeratorCompPtr.GetCount()){
		return m_deviceEnumeratorCompPtr[m_enumeratorIndex];
	}

	return nullptr;
}


IDeviceEnumerator* CDeviceControllerProxyComp::GetNextDeviceEnumerator()
{
	m_enumeratorIndex++;
	if (m_enumeratorIndex < m_deviceEnumeratorCompPtr.GetCount()){
		if (m_deviceEnumeratorCompPtr[m_enumeratorIndex] != nullptr){
			return m_deviceEnumeratorCompPtr[m_enumeratorIndex];
		}

		return GetNextDeviceEnumerator();
	}

	m_enumeratorIndex = -1;

	return nullptr;
}


void CDeviceControllerProxyComp::UpdateDeviceTypeIdList()
{
	m_supportedDeviceTypeIds.clear();

	if (m_deviceControllerCompPtr.IsValid()){
		int count = m_deviceControllerCompPtr.GetCount();
		for (int i = 0; i < count; i++){
			const IDeviceController* deviceControllerPtr = m_deviceControllerCompPtr[i];
			if (deviceControllerPtr != nullptr){
				QByteArrayList ids = deviceControllerPtr->GetSupportedDeviceTypeIds();
				for (const QByteArray& id : ids){
					const IDeviceSpecification* staticInfoPtr = deviceControllerPtr->GetDeviceStaticInfo(id);

					if (staticInfoPtr != nullptr){
						m_deviceTypeList.InsertItem(id, staticInfoPtr->GetTypeName(), "");
					}
				}

				m_supportedDeviceTypeIds += ids;

				imod::IModel* modelPtr = const_cast<imod::IModel*>(dynamic_cast<const imod::IModel*>(&deviceControllerPtr->GetDeviceInstanceList()));
				Q_ASSERT(modelPtr != nullptr);

				if (modelPtr != nullptr){
					RegisterModel(modelPtr, MI_DEVICE_LIST_BASE + i);
				}
			}
		}
	}
}


void CDeviceControllerProxyComp::UpdateDeviceList()
{
	istd::CChangeGroup group(&m_deviceList);

	m_deviceList.CopyFrom(imtbase::CCollectionInfo());
	m_deviceControllerMap.clear();

	if (m_deviceControllerCompPtr.IsValid()){
		int count = m_deviceControllerCompPtr.GetCount();
		for (int i = 0; i < count; i++){
			IDeviceController* deviceControllerPtr = m_deviceControllerCompPtr[i];
			if (deviceControllerPtr != nullptr){
				const imtbase::ICollectionInfo& deviceList = deviceControllerPtr->GetDeviceInstanceList();

				imtbase::ICollectionInfo::Ids ids = deviceList.GetElementIds();
				for (const imtbase::ICollectionInfo::Id& id : ids){
					QString name = deviceList.GetElementInfo(id, imtbase::ICollectionInfo::EIT_NAME).toString();
					QString description = deviceList.GetElementInfo(id, imtbase::ICollectionInfo::EIT_DESCRIPTION).toString();

					if (m_overriddenDeviceInfo.GetElementIds().contains(id)){
						QString overriddenName = m_overriddenDeviceInfo.GetElementInfo(id, imtbase::ICollectionInfo::EIT_NAME).toString();
						QString overriddenDescription = m_overriddenDeviceInfo.GetElementInfo(id, imtbase::ICollectionInfo::EIT_DESCRIPTION).toString();

						if (!overriddenName.isEmpty()){
							name = overriddenName;
						}

						if (!overriddenDescription.isEmpty()){
							description = overriddenDescription;
						}
					}

					m_deviceList.InsertItem(id, name, description);
					m_deviceControllerMap[id] = deviceControllerPtr;
				}
			}
		}
	}
}


IDeviceController* CDeviceControllerProxyComp::FindDeviceController(const QByteArray& deviceId) const
{
	if (deviceId.isEmpty()){
		return nullptr;
	}

	if (m_deviceControllerMap.contains(deviceId)){
		return m_deviceControllerMap[deviceId];
	}

	int count = m_deviceControllerCompPtr.GetCount();
	for (int i = 0; i < count; i++){
		IDeviceController* controllerPtr = m_deviceControllerCompPtr[i];
		if (controllerPtr != nullptr){
			QByteArrayList typeIds = controllerPtr->GetSupportedDeviceTypeIds();
			if (typeIds.contains(deviceId)){
				return controllerPtr;
			}
		}
	}

	return nullptr;
}


QByteArray CDeviceControllerProxyComp::GetDeviceTypeId(const QByteArray& deviceId) const
{
	DeviceInstancePtr instancePtr = GetDeviceInstance(deviceId);
	if (instancePtr != nullptr){
		QByteArray deviceTypeId = instancePtr->GetDeviceSpecification().GetTypeId();

		return deviceTypeId;
	}

	return QByteArray();
}


// private methods

void CDeviceControllerProxyComp::OnOverriddenDeviceInfoUpdated(const istd::IChangeable::ChangeSet& /*changeset*/, const imtbase::ICollectionInfo* /*objectPtr*/)
{
	UpdateDeviceList();
}


// public methods of the embedded class DeviceStateProvider

CDeviceControllerProxyComp::DeviceStateProvider::DeviceStateProvider()
	:m_parentPtr(nullptr)
{
}


void CDeviceControllerProxyComp::DeviceStateProvider::SetParent(CDeviceControllerProxyComp& parent)
{
	m_parentPtr = &parent;
}


// reimplemented (IDeviceStateProvider)

IDeviceStateProvider::DeviceState CDeviceControllerProxyComp::DeviceStateProvider::GetDeviceState(const QByteArray& deviceId) const
{
	if (m_parentPtr != nullptr){
		IDeviceController* controllerPtr = m_parentPtr->FindDeviceController(deviceId);
		if (controllerPtr != nullptr){
			return controllerPtr->GetDeviceStateProvider().GetDeviceState(deviceId);
		}
	}

	return DS_NONE;
}


} // namespace imtdev


