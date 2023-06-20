#include <imtdev/CCompositeDeviceControllerComp.h>


// Qt includes
#include <QtCore/QThread>

// ACF includes
#include <istd/CChangeGroup.h>


namespace imtdev
{


// public methods

CCompositeDeviceControllerComp::CCompositeDeviceControllerComp()
	:m_enumeratorIndex(-1),
	m_resultHandlerPtr(nullptr)
{
	m_deviceConnectionState.SetParent(*this);
}


// reimplemented (IDeviceController)

const QByteArrayList& CCompositeDeviceControllerComp::GetSupportedDeviceTypeIds() const
{
	return m_supportedDeviceTypeIds;
}


const IDeviceStaticInfo* CCompositeDeviceControllerComp::GetDeviceStaticInfo(const QByteArray& deviceTypeId) const
{
	if (m_deviceControllerCompPtr.IsValid()){
		int count = m_deviceControllerCompPtr.GetCount();
		for (int i = 0; i < count; i++){
			IDeviceController* controllerPtr = m_deviceControllerCompPtr[i];
			if (controllerPtr != nullptr){
				if (controllerPtr->GetSupportedDeviceTypeIds().contains(deviceTypeId)){
					return controllerPtr->GetDeviceStaticInfo(deviceTypeId);
				}
			}
		}
	}

	return nullptr;
}


const imtbase::ICollectionInfo& CCompositeDeviceControllerComp::GetAvailableDeviceList() const
{
	return m_deviceList;
}


IDeviceController::DeviceState CCompositeDeviceControllerComp::GetDeviceState(const QByteArray& deviceId) const
{
	if (m_deviceControllerMap.contains(deviceId)){
		return m_deviceControllerMap[deviceId]->GetDeviceState(deviceId);
	}

	return DS_NONE;
}


DeviceInstanceInfoPtr CCompositeDeviceControllerComp::GetDeviceInstanceInfo(
			const QByteArray& deviceTypeId,
			const QByteArray& deviceId) const
{
	if (!deviceId.isEmpty() && m_deviceControllerMap.contains(deviceId)){
		return m_deviceControllerMap[deviceId]->GetDeviceInstanceInfo(deviceTypeId, deviceId);
	}

	if (!deviceTypeId.isEmpty()){
		for (int i = 0; i < m_deviceControllerCompPtr.GetCount(); i++){
			if (m_deviceControllerCompPtr[i] != nullptr){
				if (m_deviceControllerCompPtr[i]->GetSupportedDeviceTypeIds().contains(deviceTypeId)){
					return m_deviceControllerCompPtr[i]->GetDeviceInstanceInfo(deviceTypeId, deviceId);
				}
			}
		}
	}

	return DeviceInstanceInfoPtr();
}


DeviceAccessorPtr CCompositeDeviceControllerComp::OpenDevice(
			const QByteArray& deviceTypeId,
			const QByteArray& deviceId,
			const iprm::IParamsSet* paramsPtr)
{
	if (GetSupportedDeviceTypeIds().contains(deviceTypeId)){
		int count = m_deviceControllerCompPtr.GetCount();
		for (int i = 0; i < count; i++){
			IDeviceController* controllerPtr = m_deviceControllerCompPtr[i];
			if (controllerPtr != nullptr){
				QByteArrayList deviceTypeIds = controllerPtr->GetSupportedDeviceTypeIds();
				if (deviceTypeIds.contains(deviceTypeId)){
					return controllerPtr->OpenDevice(deviceTypeId, deviceId, paramsPtr);
				}
			}
		}
	}

	return DeviceAccessorPtr();
}


bool CCompositeDeviceControllerComp::CloseDevice(const QByteArray& deviceId)
{
	if (m_deviceControllerMap.contains(deviceId)){
		return m_deviceControllerMap[deviceId]->CloseDevice(deviceId);
	}

	return false;
}


// reimplemented (IDeviceEnumerator)

IDeviceEnumerator::StartResult CCompositeDeviceControllerComp::StartEnumeration(IDeviceEnumerator::IResultHandler* resultHandlerPtr)
{
	return SR_FAILED;
}


void CCompositeDeviceControllerComp::CancelEnumeration()
{
	SendInfoMessage(0, "Cancellation of enumeration");

	IDeviceEnumerator* deviceEnumeratorPtr = GetCurrentDeviceEnumerator();
	if (deviceEnumeratorPtr){
		deviceEnumeratorPtr->CancelEnumeration();
	}

	m_resultHandlerPtr = nullptr;
}


// reimplemented (imtdev::IDeviceEnumerationResultHandler)

void CCompositeDeviceControllerComp::OnEnumerationResult(
			EnumerationResult result,
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

void CCompositeDeviceControllerComp::OnModelChanged(int modelId, const istd::IChangeable::ChangeSet& changeSet)
{
	Q_ASSERT(qApp->thread() == QThread::currentThread());

	UpdateDeviceList();
	UpdateExtendedDeviceList();

	istd::IChangeable::ChangeSet localChangeSet(imtdev::IDeviceConnectionState::CF_CONNECTION_STATE_CHANGED);
	istd::CChangeNotifier notifier(&m_deviceConnectionState, &localChangeSet);
}


// reimplemented (icomp::CComponentBase)

void CCompositeDeviceControllerComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	if (*m_intervalAttrPtr > 0){
		m_intervalTimer.setInterval(*m_intervalAttrPtr * 1000);
	}

	connect(&m_intervalTimer, &QTimer::timeout, this, &CCompositeDeviceControllerComp::OnIntervalTimer);

	UpdateDeviceTypeIdList();

	if (*m_enableAttrPtr){
		StartEnumeration();
	}
}


void CCompositeDeviceControllerComp::OnComponentDestroyed()
{
	CancelEnumeration();

	m_intervalTimer.stop();

	UnregisterAllModels();

	BaseClass::OnComponentDestroyed();
}


// private slots

void CCompositeDeviceControllerComp::OnIntervalTimer()
{
	if (GetCurrentDeviceEnumerator() == nullptr){
		StartEnumeration();
	}
}


// private methods

void CCompositeDeviceControllerComp::StartEnumeration()
{
	Q_ASSERT(m_enumeratorIndex == -1);

	IDeviceEnumerator* deviceEnumeratorPtr = GetNextDeviceEnumerator();
	if (deviceEnumeratorPtr){
		SendInfoMessage(0, "Enumeration started");

		if (m_intervalTimer.interval() > 0){
			m_intervalTimer.start();

			SendInfoMessage(0, "Enumeration interval timer started");
		}

		deviceEnumeratorPtr->StartEnumeration(this);
	}
}


IDeviceEnumerator* CCompositeDeviceControllerComp::GetCurrentDeviceEnumerator()
{
	if (m_enumeratorIndex >= 0 && m_enumeratorIndex < m_deviceEnumeratorCompPtr.GetCount()){
		return m_deviceEnumeratorCompPtr[m_enumeratorIndex];
	}

	return nullptr;
}


IDeviceEnumerator* CCompositeDeviceControllerComp::GetNextDeviceEnumerator()
{
	m_enumeratorIndex++;
	if (m_enumeratorIndex < m_deviceEnumeratorCompPtr.GetCount()){
		if (m_deviceEnumeratorCompPtr[m_enumeratorIndex] != nullptr){
			return m_deviceEnumeratorCompPtr[m_enumeratorIndex];
		}
		else{
			return GetNextDeviceEnumerator();
		}
	}

	m_enumeratorIndex = -1;
	return nullptr;
}


void CCompositeDeviceControllerComp::UpdateDeviceTypeIdList()
{
	if (m_deviceControllerCompPtr.IsValid()){
		int count = m_deviceControllerCompPtr.GetCount();
		for (int i = 0; i < count; i++){
			const IDeviceController* deviceControllerPtr = m_deviceControllerCompPtr[i];
			if (deviceControllerPtr != nullptr){
				QByteArrayList ids = deviceControllerPtr->GetSupportedDeviceTypeIds();
				for (const QByteArray& id : ids){
					const IDeviceStaticInfo* staticInfoPtr = deviceControllerPtr->GetDeviceStaticInfo(id);

					if (staticInfoPtr != nullptr){
						m_deviceTypeList.InsertItem(id, staticInfoPtr->GetDeviceTypeName(), "");
					}
				}

				m_supportedDeviceTypeIds += ids;

				imod::IModel* modelPtr = const_cast<imod::IModel*>(dynamic_cast<const imod::IModel*>(&deviceControllerPtr->GetAvailableDeviceList()));
				Q_ASSERT(modelPtr != nullptr);

				if (modelPtr != nullptr){
					RegisterModel(modelPtr, MI_DEVICE_LIST_BASE + i);
				}
			}
		}
	}
}


void CCompositeDeviceControllerComp::UpdateDeviceList()
{
	istd::CChangeGroup group(&m_deviceList);

	m_deviceList.CopyFrom(imtbase::CCollectionInfo());
	m_deviceControllerMap.clear();

	if (m_deviceControllerCompPtr.IsValid()){
		int count = m_deviceControllerCompPtr.GetCount();
		for (int i = 0; i < count; i++){
			IDeviceController* deviceControllerPtr = m_deviceControllerCompPtr[i];
			if (deviceControllerPtr != nullptr){
				const imtbase::ICollectionInfo& deviceList = deviceControllerPtr->GetAvailableDeviceList();

				imtbase::ICollectionInfo::Ids ids = deviceList.GetElementIds();
				for (const imtbase::ICollectionInfo::Id id : ids){
					QString name = deviceList.GetElementInfo(id, imtbase::ICollectionInfo::EIT_NAME).toString();
					QString description = deviceList.GetElementInfo(id, imtbase::ICollectionInfo::EIT_DESCRIPTION).toString();

					m_deviceList.InsertItem(id, name, description);
					m_deviceControllerMap[id] = deviceControllerPtr;
				}
			}
		}
	}
}


void CCompositeDeviceControllerComp::UpdateExtendedDeviceList()
{
	istd::CChangeGroup group(&m_extendedDeviceList);

	m_extendedDeviceList.CopyFrom(imtbase::CCollectionInfo());

	QMultiMap<QByteArray, QByteArray> sortedDeviceList;
	imtbase::ICollectionInfo::Ids deviceIds = m_deviceList.GetElementIds();
	for (const QByteArray& deviceId : deviceIds){
		QByteArray deviceTypeId = GetDeviceTypeId(deviceId);
		if (!deviceTypeId.isEmpty()){
			sortedDeviceList.insert(deviceTypeId, deviceId);
		}
	}

	imtbase::ICollectionInfo::Ids deviceTypeIds = m_deviceTypeList.GetElementIds();
	for (const QByteArray& deviceTypeId : deviceTypeIds){
		if (sortedDeviceList.contains(deviceTypeId)){
			QByteArrayList deviceIds = sortedDeviceList.values(deviceTypeId);
			for (const QByteArray& deviceId : deviceIds){
				m_extendedDeviceList.InsertItem(
							deviceId,
							m_deviceList.GetElementInfo(deviceId, imtbase::ICollectionInfo::EIT_NAME).toString(),
							m_deviceList.GetElementInfo(deviceId, imtbase::ICollectionInfo::EIT_DESCRIPTION).toString());
			}
		}
		else{
			m_extendedDeviceList.InsertItem(
					deviceTypeId,
					m_deviceTypeList.GetElementInfo(deviceTypeId, imtbase::ICollectionInfo::EIT_NAME).toString(),
					m_deviceTypeList.GetElementInfo(deviceTypeId, imtbase::ICollectionInfo::EIT_DESCRIPTION).toString());
		}
	}
}


QByteArray CCompositeDeviceControllerComp::GetDeviceTypeId(const QByteArray& deviceId) const
{
	DeviceInstanceInfoPtr instanceInfoPtr = GetDeviceInstanceInfo("", deviceId);
	if (!instanceInfoPtr.isNull()){
		QByteArray deviceTypeId = instanceInfoPtr->GetDeviceStaticInfo().GetDeviceTypeId();

		return deviceTypeId;
	}

	return QByteArray();
}


// public methods of the embedded class DeviceConnectionState

CCompositeDeviceControllerComp::DeviceConnectionState::DeviceConnectionState()
	:m_parentPtr(nullptr)
{
}


void CCompositeDeviceControllerComp::DeviceConnectionState::SetParent(CCompositeDeviceControllerComp& parent)
{
	m_parentPtr = &parent;
}


// reimplemented (IDeviceState)

bool CCompositeDeviceControllerComp::DeviceConnectionState::IsDeviceConnected(const QByteArray& deviceId)
{
	if (m_parentPtr != nullptr){
		return m_parentPtr->m_deviceControllerMap.contains(deviceId);
	}

	return false;
}


} // namespace imtdev


