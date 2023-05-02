#include <imtdev/CCompositeDeviceControllerComp.h>


// Qt includes
#include <QtCore/QThread>

// ACF includes
#include <istd/CChangeGroup.h>

// ImtCore includes
#include <imtdev/IDeviceController.h>


namespace imtdev
{


// public methods

CCompositeDeviceControllerComp::CCompositeDeviceControllerComp()
	:m_deviceListObserver(*this),
	m_enumeratorIndex(-1)
{
}


// reimplemented (IDeviceController)

QByteArrayList CCompositeDeviceControllerComp::GetSupportedDeviceTypeIds() const
{
	QByteArrayList retVal;

	int count = m_deviceControllerCompPtr.GetCount();
	for (int i = 0; i < count; i++){
		IDeviceController* controllerPtr = m_deviceControllerCompPtr[i];
		if (controllerPtr != nullptr){
			retVal += controllerPtr->GetSupportedDeviceTypeIds();
		}
	}

	return retVal;
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


const iprm::IOptionsList& CCompositeDeviceControllerComp::GetAvailableDeviceList() const
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


const IDeviceInstanceInfo* CCompositeDeviceControllerComp::GetDeviceInstanceInfo(const QByteArray& deviceId) const
{
	if (m_deviceControllerMap.contains(deviceId)){
		return m_deviceControllerMap[deviceId]->GetDeviceInstanceInfo(deviceId);
	}

	return nullptr;
}


DeviceAccessorPtr CCompositeDeviceControllerComp::OpenDevice(const QByteArray& deviceId, const iprm::IParamsSet* paramsPtr)
{
	if (m_deviceControllerMap.contains(deviceId)){
		return m_deviceControllerMap[deviceId]->OpenDevice(deviceId, paramsPtr);
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

IDeviceEnumerator::StartResult CCompositeDeviceControllerComp::StartEnumeration(IResultHandler* resultHandlerPtr)
{
	return SR_FAILED;
}


void CCompositeDeviceControllerComp::CancelEnumeration()
{
}


// reimplemented (imtdev::IDeviceEnumerationResultHandler)

void CCompositeDeviceControllerComp::OnEnumerationResult(
			EnumerationResult result,
			const IDeviceEnumerator* /*deviceEnumeratorPtr*/)
{
	switch (result){
	case ER_OK:
		break;
	case ER_CANCELED:
		SendInfoMessage(0, "Enumeration canceled");
		break;
	case ER_FAILED:
		SendInfoMessage(0, "Enumeration failed");
		break;
	}

	IDeviceEnumerator* deviceEnumeratorPtr = GetNextDeviceEnumerator();
	if (deviceEnumeratorPtr != nullptr){
		deviceEnumeratorPtr->StartEnumeration(this);
	}
	else{
		SendInfoMessage(0, QString("Enumeration finished"));

		m_enumeratorIndex = -1;

		if (!m_enumerationTimer.isActive()){
			StartEnumeration();
		}
	}
}


// protected methods

// reimplemented (icomp::CComponentBase)

void CCompositeDeviceControllerComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	SendInfoMessage(0, "Created");

	m_enumerationTimer.setInterval(*m_enumerationIntervalAttrPtr * 1000);
	connect(&m_enumerationTimer, &QTimer::timeout, this, &CCompositeDeviceControllerComp::OnTimer);

	SendInfoMessage(0, QString("Enumeration interval set to %1").arg(m_enumerationTimer.interval()));

	bool modelFound = false;
	
	int count = m_deviceControllerCompPtr.GetCount();
	for (int i = 0; i < count; i++){
		const IDeviceController* deviceControllerPtr = m_deviceControllerCompPtr[i];
		if (deviceControllerPtr != nullptr){
			imod::IModel* modelPtr = const_cast<imod::IModel*>(dynamic_cast<const imod::IModel*>(&deviceControllerPtr->GetAvailableDeviceList()));
			if (modelPtr != nullptr){
				m_deviceListObserver.RegisterModel(modelPtr);

				modelFound = true;
			}
		}
	}

	if (modelFound){
		StartEnumeration();
	}
}


void CCompositeDeviceControllerComp::OnComponentDestroyed()
{
	CacnelEnumeration();

	m_enumerationTimer.stop();

	m_deviceListObserver.UnregisterAllModels();

	BaseClass::OnComponentDestroyed();
}


// private slots

void CCompositeDeviceControllerComp::OnTimer()
{
	if (m_enumeratorIndex < 0){
		StartEnumeration();
	}
}


// private methods

IDeviceEnumerator* CCompositeDeviceControllerComp::GetCurrentDeviceEnumerator()
{
	int count = m_deviceEnumeratorCompPtr.GetCount();

	Q_ASSERT(m_enumeratorIndex >= 0 && m_enumeratorIndex < count);

	if (m_enumeratorIndex >= 0 && m_enumeratorIndex < count){
		return m_deviceEnumeratorCompPtr[m_enumeratorIndex];
	}

	return nullptr;
}


IDeviceEnumerator* CCompositeDeviceControllerComp::GetNextDeviceEnumerator()
{
	if (m_enumeratorIndex < m_deviceEnumeratorCompPtr.GetCount()){
		m_enumeratorIndex++;
		if (m_enumeratorIndex < m_deviceEnumeratorCompPtr.GetCount()){
			return m_deviceEnumeratorCompPtr[m_enumeratorIndex];
		}
	}

	return nullptr;
}


bool CCompositeDeviceControllerComp::StartEnumeration()
{
	Q_ASSERT(m_enumeratorIndex < 0 );

	IDeviceEnumerator* deviceEnumeratorPtr = GetNextDeviceEnumerator();
	if (deviceEnumeratorPtr){
		SendInfoMessage(0, "Enumeration started");

		if (m_enumerationTimer.interval() > 0){
			m_enumerationTimer.start();

			SendInfoMessage(0, "Enumeration interval timer started");
		}

		deviceEnumeratorPtr->StartEnumeration(this);

		return true;
	}

	return false;
}

void CCompositeDeviceControllerComp::CacnelEnumeration()
{
	SendInfoMessage(0, "Cancellation of enumeration");

	IDeviceEnumerator* deviceEnumeratorPtr = GetCurrentDeviceEnumerator();
	if (deviceEnumeratorPtr){
		deviceEnumeratorPtr->CancelEnumeration();
	}
}


void CCompositeDeviceControllerComp::OnDeviceListChanged(int modelId, const istd::IChangeable::ChangeSet& changeSet)
{
	Q_ASSERT(qApp->thread() == QThread::currentThread());

	istd::CChangeGroup group(&m_deviceList);

	m_deviceList.ResetData();
	m_deviceControllerMap.clear();

	QString devices;

	int count = m_deviceControllerCompPtr.GetCount();
	for (int i = 0; i < count; i++){
		IDeviceController* deviceControllerPtr = m_deviceControllerCompPtr[i];
		if (deviceControllerPtr != nullptr){
			const iprm::IOptionsList& deviceList = deviceControllerPtr->GetAvailableDeviceList();

			int count = deviceList.GetOptionsCount();
			for (int i = 0; i < count; i++){
				QByteArray id = deviceList.GetOptionId(i);
				QString name = deviceList.GetOptionName(i);
				QString description = deviceList.GetOptionDescription(i);

				m_deviceList.InsertOption(name, id, description);
				m_deviceControllerMap[id] = deviceControllerPtr;

				devices += name + ", ";
			}
		}
	}

	devices.chop(2);

	SendInfoMessage(0, "Updating device list: [" + devices + "]");
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


} // namespace imtdev


