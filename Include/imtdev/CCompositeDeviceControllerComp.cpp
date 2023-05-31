#include <imtdev/CCompositeDeviceControllerComp.h>


// Qt includes
#include <QtCore/QThread>

// ACF includes
#include <istd/CChangeGroup.h>


namespace imtdev
{


// public methods

CCompositeDeviceControllerComp::CCompositeDeviceControllerComp()
	:m_deviceListObserver(*this),
	m_enumeratorIndex(-1),
	m_resultHandlerPtr(nullptr)
{
	m_deviceTypeList.SetParent(*this);
}


// reimplemented (IDeviceController)

const QByteArrayList& CCompositeDeviceControllerComp::GetSupportedDeviceTypeIds() const
{
	return m_supportedDeviceTypeIds;
}


const IDeviceStaticInfo* CCompositeDeviceControllerComp::GetDeviceStaticInfo(const QByteArray& deviceTypeId) const
{
	int count = m_deviceControllerCompPtr.GetCount();
	for (int i = 0; i < count; i++){
		IDeviceController* controllerPtr = m_deviceControllerCompPtr[i];
		if (controllerPtr != nullptr){
			return controllerPtr->GetDeviceStaticInfo(deviceTypeId);
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


DeviceInstanceInfoPtr CCompositeDeviceControllerComp::GetDeviceInstanceInfo(const QByteArray& deviceId) const
{
	if (m_deviceControllerMap.contains(deviceId)){
		istd::TDelPtr<imtdev::IDeviceInstanceInfo> instanceInfoPtr;
		instanceInfoPtr.SetCastedOrRemove(m_deviceControllerMap[deviceId]->GetDeviceInstanceInfo(deviceId)->CloneMe());

		DeviceInstanceInfoPtr retVal(instanceInfoPtr.PopPtr());

		return retVal;
	}

	return DeviceInstanceInfoPtr();
}


DeviceAccessorPtr CCompositeDeviceControllerComp::OpenDevice(const QByteArray& deviceTypeId, const QByteArray& deviceId, const iprm::IParamsSet* paramsPtr)
{
	if (!deviceId.isEmpty()){
		if (m_deviceControllerMap.contains(deviceId)){
			return m_deviceControllerMap[deviceId]->OpenDevice(deviceTypeId, deviceId, paramsPtr);
		}
	}
	else{
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

// reimplemented (icomp::CComponentBase)

void CCompositeDeviceControllerComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	SendInfoMessage(0, "Created");

	if (*m_intervalAttrPtr > 0){
		m_intervalTimer.setInterval(*m_intervalAttrPtr * 1000);
	}
	connect(&m_intervalTimer, &QTimer::timeout, this, &CCompositeDeviceControllerComp::OnIntervalTimer);

	SendInfoMessage(0, QString("Enumeration interval set to %1").arg(m_intervalTimer.interval()));

	int count = m_deviceControllerCompPtr.GetCount();
	for (int i = 0; i < count; i++){
		const IDeviceController* deviceControllerPtr = m_deviceControllerCompPtr[i];
		Q_ASSERT(deviceControllerPtr != nullptr);

		if (deviceControllerPtr != nullptr){
			QByteArrayList ids = deviceControllerPtr->GetSupportedDeviceTypeIds();
			for (const QByteArray& id : ids){
				m_deviceTypeList.InsertItem(id, deviceControllerPtr->GetDeviceStaticInfo(id)->GetDeviceTypeName(), "");
			}

			m_supportedDeviceTypeIds += ids;

			imod::IModel* modelPtr = const_cast<imod::IModel*>(dynamic_cast<const imod::IModel*>(&deviceControllerPtr->GetAvailableDeviceList()));
			Q_ASSERT(modelPtr != nullptr);

			if (modelPtr != nullptr){
				m_deviceListObserver.RegisterModel(modelPtr);
			}
		}
	}

	if (*m_enableAttrPtr){
		StartEnumeration();
	}
}


void CCompositeDeviceControllerComp::OnComponentDestroyed()
{
	CancelEnumeration();

	m_intervalTimer.stop();

	m_deviceListObserver.UnregisterAllModels();

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


void CCompositeDeviceControllerComp::OnDeviceListChanged(int modelId, const istd::IChangeable::ChangeSet& changeSet)
{
	Q_ASSERT(qApp->thread() == QThread::currentThread());

	istd::CChangeGroup group(&m_deviceList);

	m_deviceList.CopyFrom(imtbase::CCollectionInfo());
	m_deviceControllerMap.clear();

	QString devices;

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

				devices += name + ", ";
			}
		}
	}

	devices.chop(2);

	SendInfoMessage(0, "Updating device list: [" + devices + "]");

	istd::IChangeable::ChangeSet localChangeSet(imtdev::IDeviceState::CF_CONNECTION_STATE_CHANGED);
	istd::CChangeNotifier notifier(&m_deviceTypeList, &localChangeSet);
}


// public methods of the embedded class DeviceControllerObserver

CCompositeDeviceControllerComp::DeviceListObserver::DeviceListObserver(CCompositeDeviceControllerComp& parent)
	:m_parent(parent)
{
}

// reimplemented (imod::CMultiModelDispatcherBase)

void CCompositeDeviceControllerComp::DeviceListObserver::OnModelChanged(int modelId, const istd::IChangeable::ChangeSet& changeSet)
{
	m_parent.OnDeviceListChanged(modelId, changeSet);
}


// public methods of the embedded class DeviceTypeList

CCompositeDeviceControllerComp::DeviceTypeList::DeviceTypeList()
	:m_parentPtr(nullptr)
{
}


void CCompositeDeviceControllerComp::DeviceTypeList::SetParent(CCompositeDeviceControllerComp& parent)
{
	m_parentPtr = &parent;
}


// reimplemented (IDeviceState)

bool CCompositeDeviceControllerComp::DeviceTypeList::IsDeviceConnected(const QByteArray& deviceTypeId)
{
	if (m_parentPtr != nullptr && m_parentPtr->m_deviceControllerCompPtr.IsValid()){
		int count = m_parentPtr->m_deviceControllerCompPtr.GetCount();
		for (int i = 0; i < count; i++){
			if (m_parentPtr->m_deviceControllerCompPtr[i] != nullptr){
				if (m_parentPtr->m_deviceControllerCompPtr[i]->GetSupportedDeviceTypeIds().contains(deviceTypeId)){
					Ids ids = m_parentPtr->m_deviceControllerCompPtr[i]->GetAvailableDeviceList().GetElementIds();
					for (const QByteArray& id : ids){
						DeviceInstanceInfoPtr infoPtr = m_parentPtr->m_deviceControllerCompPtr[i]->GetDeviceInstanceInfo(id);
						if (!infoPtr.isNull()){
							if (infoPtr->GetDeviceStaticInfo().GetDeviceTypeId() == deviceTypeId){
								return true;
							}
						}
					}
				}
			}
		}
	}

	return false;
}


} // namespace imtdev


