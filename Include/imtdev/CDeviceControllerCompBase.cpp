// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtdev/CDeviceControllerCompBase.h>


// Qt includes
#include <QtCore/QMutexLocker>
#include <QtCore/QThread>

// ACF includes
#include <imod/TModelWrap.h>
#include <iprm/IOptionsList.h>
#include <ilog/TLoggerCompWrap.h>
#include <istd/CChangeNotifier.h>
#include <istd/CChangeGroup.h>

// Acula includes
#include <imtdev/IDeviceController.h>


namespace imtdev
{


// public methods

CDeviceControllerCompBase::CDeviceControllerCompBase()
	:m_overriddenDeviceInfoObserver(*this)
{
	m_deviceStateProvider.SetParent(*this);
}


// reimplemented (IDeviceController)

const imtbase::ICollectionInfo& CDeviceControllerCompBase::GetDeviceInstanceList() const
{
	return m_deviceList;
}


bool CDeviceControllerCompBase::SetDeviceInstanceName(const QByteArray& deviceId, const QString& name)
{
	if (!m_overriddenDeviceInfo.GetElementIds().contains(deviceId)){
		return !m_overriddenDeviceInfo.InsertItem(deviceId, name, "").isEmpty();
	}

	return m_overriddenDeviceInfo.SetElementName(deviceId, name);
}


bool CDeviceControllerCompBase::SetDeviceInstanceDescription(const QByteArray& deviceId, const QString& description)
{
	if (!m_overriddenDeviceInfo.GetElementIds().contains(deviceId)){
		return !m_overriddenDeviceInfo.InsertItem(deviceId, "", description).isEmpty();
	}

	return m_overriddenDeviceInfo.SetElementDescription(deviceId, description);
}


const IDeviceStateProvider& CDeviceControllerCompBase::GetDeviceStateProvider() const
{
	return m_deviceStateProvider;
}


// protected methods

void CDeviceControllerCompBase::UpdateDeviceList(EnumeratedDeviceList& enumeratedDeviceList)
{
	Q_ASSERT(qApp->thread() == QThread::currentThread());

	imtbase::CCollectionInfo tempDeviceList;

	for (int i = 0; i < enumeratedDeviceList.count(); i++){
		QString name = enumeratedDeviceList[i].name;
		QString description = enumeratedDeviceList[i].description;

		if (m_overriddenDeviceInfo.GetElementIds().contains(enumeratedDeviceList[i].id)){
			QString overriddenName = m_overriddenDeviceInfo.GetElementInfo(
				enumeratedDeviceList[i].id,
				imtbase::ICollectionInfo::EIT_NAME).toString();
			QString overriddenDescription = m_overriddenDeviceInfo.GetElementInfo(
				enumeratedDeviceList[i].id,
				imtbase::ICollectionInfo::EIT_DESCRIPTION).toString();

			if (!overriddenName.isEmpty()){
				name = overriddenName;
			}

			if (!overriddenDescription.isEmpty()){
				description = overriddenDescription;
			}
		}

		tempDeviceList.InsertItem(
					enumeratedDeviceList[i].id,
					name,
					description);
	}

	{
		QMutexLocker listLocker(&m_deviceListMutex);

		if (!m_deviceList.IsEqual(tempDeviceList)){
			m_deviceList.CopyFrom(tempDeviceList);

			istd::CChangeNotifier notifier(&m_deviceStateProvider);
		}
	}

	if (*m_isAutoCloseEnabledAttrPtr){
		AutoCloseDisconnectedDevices();
	}
}


void CDeviceControllerCompBase::UpdateDeviceList()
{
	istd::CChangeGroup group(&m_deviceList);

	for (const QByteArray& id : m_deviceList.GetElementIds()){
		if (m_overriddenDeviceInfo.GetElementIds().contains(id)){
			QString name = m_overriddenDeviceInfo.GetElementInfo(id, imtbase::ICollectionInfo::EIT_NAME).toString();
			QString description = m_overriddenDeviceInfo.GetElementInfo(id, imtbase::ICollectionInfo::EIT_DESCRIPTION).toString();

			if (!name.isEmpty()){
				m_deviceList.SetElementName(id, name);
			}

			if (!description.isEmpty()){
				m_deviceList.SetElementDescription(id, description);
			}
		}
	}
}


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


void CDeviceControllerCompBase::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	m_overriddenDeviceInfoObserver.RegisterObject(&m_overriddenDeviceInfo, &CDeviceControllerCompBase::OnOverriddenDeviceInfoUpdated);
}


void CDeviceControllerCompBase::OnComponentDestroyed()
{
	m_overriddenDeviceInfoObserver.UnregisterAllObjects();

	BaseClass::OnComponentDestroyed();
}


// private methods

void CDeviceControllerCompBase::OnOverriddenDeviceInfoUpdated(const istd::IChangeable::ChangeSet& /*changeset*/, const imtbase::ICollectionInfo* /*objectPtr*/)
{
	UpdateDeviceList();
}


// public methods of the embedded class DeviceStateProvider

CDeviceControllerCompBase::DeviceStateProvider::DeviceStateProvider()
	:m_parentPtr(nullptr)
{
}


void CDeviceControllerCompBase::DeviceStateProvider::SetParent(CDeviceControllerCompBase& parent)
{
	m_parentPtr = &parent;
}


// reimplemented (IDeviceStateProvider)

IDeviceStateProvider::DeviceState CDeviceControllerCompBase::DeviceStateProvider::GetDeviceState(const QByteArray& deviceId) const
{
	Q_ASSERT(m_parentPtr != nullptr);
	if (m_parentPtr != nullptr){
		QMutexLocker listLocker(&m_parentPtr->m_deviceListMutex);
		QMutexLocker mapLocker(&m_parentPtr->m_openedDevicesMutex);

		imtbase::ICollectionInfo::Ids ids = m_parentPtr->m_deviceList.GetElementIds();
		if (ids.contains(deviceId)){
			if (m_parentPtr->m_openedDevices.contains(deviceId)){
				return DS_OPENED;
			}

			return DS_CLOSED;
		}
	}

	return DS_NONE;
}


} // namespace imtdev


