#include <imtdev/CDeviceControllerCompBase.h>


// Qt includes
#include <QtCore/QMutexLocker>
#include <QtCore/QThread>

// ACF includes
#include <imod/TModelWrap.h>
#include <iprm/IOptionsList.h>
#include <ilog/TLoggerCompWrap.h>
#include <istd/CChangeNotifier.h>

// Acula includes
#include <imtdev/IDeviceController.h>


namespace imtdev
{


// public methods

// reimplemented (IDeviceController)

const imtbase::ICollectionInfo& CDeviceControllerCompBase::GetAvailableDeviceList() const
{
	return m_deviceList;
}


IDeviceController::DeviceState CDeviceControllerCompBase::GetDeviceState(const QByteArray& deviceId) const
{
	QMutexLocker listLocker(&m_deviceListMutex);
	QMutexLocker mapLocker(&m_openedDevicesMutex);

	imtbase::ICollectionInfo::Ids ids = m_deviceList.GetElementIds();

	if (ids.contains(deviceId)){
		if (m_openedDevices.contains(deviceId)){
			return DS_ACTIVE;
		}

		return DS_INACTIVE;
	}

	return DS_NONE;
}


// protected methods

void CDeviceControllerCompBase::UpdateDeviceList(EnumeratedDeviceList& enumeratedDeviceList)
{
	Q_ASSERT(qApp->thread() == QThread::currentThread());

	imtbase::CCollectionInfo tempDeviceList;

	for (int i = 0; i < enumeratedDeviceList.count(); i++){
		tempDeviceList.InsertItem(
					enumeratedDeviceList[i].id,
					enumeratedDeviceList[i].name,
					enumeratedDeviceList[i].description);
	}

	{
		QMutexLocker listLocker(&m_deviceListMutex);

		if (!m_deviceList.IsEqual(tempDeviceList)){
			m_deviceList.CopyFrom(tempDeviceList);
		}
	}

	if (*m_isAutoCloseEnabledAttrPtr){
		AutoCloseDisconnectedDevices();
	}
}


// reimplemented (icomp::CComponentBase)

void CDeviceControllerCompBase::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	SetTracingEnabled(IsVerboseEnabled());
}


// private methods

void CDeviceControllerCompBase::AutoCloseDisconnectedDevices()
{
	QMutexLocker listLocker(&m_deviceListMutex);
	QMutexLocker mapLocker(&m_openedDevicesMutex);

	imtbase::ICollectionInfo::Ids deviceListIds = m_deviceList.GetElementIds();

	QSet<QByteArray> idsForClose;
	for (const QByteArray& id : m_openedDevices.keys()){
		if (!deviceListIds.contains(id)){
			idsForClose += id;
		}
	}

	for (const QByteArray& id : idsForClose){
		CloseDevice(id);

		m_openedDevices.remove(id);
	}
}


} // namespace imtdev


