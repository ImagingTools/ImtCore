// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtdev/CCompositeDeviceInstanceBase.h>


// Qt includes
#include <QtCore/QUuid>

// ACF include
#include <idoc/IDocumentMetaInfo.h>
#include <istd/CChangeNotifier.h>
#include <istd/TDelPtr.h>

// ImtCore includes
#include <imtbase/imtbase.h>


namespace imtdev
{


// public methods

CCompositeDeviceInstanceBase::CCompositeDeviceInstanceBase()
{
	m_subDeviceCollection.SetParent(this);
	m_subDeviceCollection.AttachObserver(&m_updateBridge);
}

CCompositeDeviceInstanceBase::~CCompositeDeviceInstanceBase()
{
	if (m_updateBridge.IsModelAttached(&m_subDeviceCollection)){
		m_subDeviceCollection.DetachObserver(&m_updateBridge);
	}
}


// reimplemented (imtdev::IEditableCompositeDeviceInstance)

QByteArray CCompositeDeviceInstanceBase::AddSubDevice(
	const QByteArray& deviceTypeId,
	const QString& name,
	const QString& description,
	const istd::IChangeable* defaultValuePtr,
	const QByteArray& proposedId)
{
	istd::TDelPtr<imtdev::IDeviceInstance> deviceInstancePtr;
	deviceInstancePtr.SetCastedOrRemove(CreateDeviceInstance(deviceTypeId));
	if (deviceInstancePtr == nullptr){
		return QByteArray();
	}

	if (defaultValuePtr != nullptr && !deviceInstancePtr->CopyFrom(*defaultValuePtr)){
		return QByteArray();
	}

	imod::IModel* modelPtr = dynamic_cast<imod::IModel*>(deviceInstancePtr.GetPtr());
	if (modelPtr != nullptr){
		modelPtr->AttachObserver(&m_updateBridge);
	}

	QByteArray id = proposedId;
	if (id.isEmpty() || m_subDeviceMap.contains(id)){
		id = QUuid::createUuid().toByteArray(QUuid::WithoutBraces);
	}

	istd::CChangeNotifier notifier(this);

	m_subDeviceMap[id].name = name;
	m_subDeviceMap[id].description = description;
	m_subDeviceMap[id].instancePtr = DeviceInstancePtr(deviceInstancePtr.PopPtr());

	return id;
}


bool CCompositeDeviceInstanceBase::RemoveSubDevice(const QByteArray& id)
{
	if (m_subDeviceMap.contains(id)){
		istd::CChangeNotifier notifier(this);

		imod::IModel* modelPtr = dynamic_cast<imod::IModel*>(m_subDeviceMap[id].instancePtr.get());
		if (modelPtr != nullptr && modelPtr->IsAttached(&m_updateBridge)){
			modelPtr->DetachObserver(&m_updateBridge);
		}

		m_subDeviceMap.remove(id);

		return true;
	}

	return false;
}


// reimplemented (imtdev::ICompositeDeviceInstance)

const ICompositeDeviceSpecification* CCompositeDeviceInstanceBase::GetCompositeDeviceSpecification() const
{
	return nullptr;
}


QSet<QByteArray> CCompositeDeviceInstanceBase::GetSupportedSubDeviceTypeIds() const
{
	return {};
}


const imtbase::ICollectionInfo& CCompositeDeviceInstanceBase::GetSubDeviceList() const
{
	return m_subDeviceCollection;
}


const IDeviceInstance* CCompositeDeviceInstanceBase::GetSubDeviceInstance(
	const QByteArray& subDeviceId) const
{
	if (m_subDeviceMap.contains(subDeviceId)){
		return m_subDeviceMap[subDeviceId].instancePtr.get();
	}

	return nullptr;
}


// reimplemented (istd::IChangeable)

int CCompositeDeviceInstanceBase::GetSupportedOperations() const
{
	return SO_COPY | SO_RESET;
}


bool CCompositeDeviceInstanceBase::CopyFrom(const IChangeable& object, CompatibilityMode mode)
{
	const CCompositeDeviceInstanceBase* sourcePtr = dynamic_cast<const CCompositeDeviceInstanceBase*>(&object);
	if (sourcePtr != nullptr){
		if (GetDeviceSpecification().GetTypeId() != sourcePtr->GetDeviceSpecification().GetTypeId()){
			return false;
		}

		istd::CChangeNotifier notifier(this);

		RemoveAllSubDevices();

		bool retVal = BaseClass::CopyFrom(object, mode);

		if (retVal){
			for (const QByteArray& subDeviceId : sourcePtr->m_subDeviceMap.keys()){
				QByteArray newId = AddSubDevice(
					sourcePtr->m_subDeviceMap[subDeviceId].instancePtr->GetDeviceSpecification().GetTypeId(),
					sourcePtr->m_subDeviceMap[subDeviceId].name,
					sourcePtr->m_subDeviceMap[subDeviceId].description,
					sourcePtr->m_subDeviceMap[subDeviceId].instancePtr.get(),
					subDeviceId);

				Q_ASSERT(newId == subDeviceId);

				retVal = retVal && !newId.isEmpty();
			}
		}

		return retVal;
	}

	return false;
}


bool CCompositeDeviceInstanceBase::ResetData(CompatibilityMode mode)
{
	istd::CChangeNotifier notifier(this);

	bool retVal = BaseClass::ResetData(mode);

	RemoveAllSubDevices();

	return retVal;
}


// protected methods

IDeviceInstance* CCompositeDeviceInstanceBase::CreateDeviceInstance(const QByteArray& /*deviceTypeId*/) const
{
	return nullptr;
}


// private methods

void CCompositeDeviceInstanceBase::RemoveAllSubDevices()
{
	istd::CChangeNotifier notifier(this);

	for (const QByteArray& subDeviceId : m_subDeviceMap.keys()){
		imod::IModel* modelPtr = dynamic_cast<imod::IModel*>(m_subDeviceMap[subDeviceId].instancePtr.get());
		if (modelPtr != nullptr){
			if (m_updateBridge.IsModelAttached(modelPtr)){
				modelPtr->DetachObserver(&m_updateBridge);
			}
		}
	}

	m_subDeviceMap.clear();
}


// public methods of the embedded class SubDeviceCollectionInfo

void CCompositeDeviceInstanceBase::SubDeviceCollectionInfo::SetParent(CCompositeDeviceInstanceBase* parentPtr)
{
	m_parentPtr = parentPtr;
}


// reimpolemented (imtbase::ICollectionInfo)

int CCompositeDeviceInstanceBase::SubDeviceCollectionInfo::GetElementsCount(
			const iprm::IParamsSet* /*selectionParamsPtr*/,
			ilog::IMessageConsumer* /*logPtr*/) const
{
	return imtbase::narrow_cast<int>(m_parentPtr->m_subDeviceMap.count());
}


imtbase::ICollectionInfo::Ids CCompositeDeviceInstanceBase::SubDeviceCollectionInfo::GetElementIds(
			int /*offset*/,
			int /*count*/,
			const iprm::IParamsSet* /*selectionParamsPtr*/,
			ilog::IMessageConsumer* /*logPtr*/) const
{
	return m_parentPtr->m_subDeviceMap.keys().toVector();
}


bool CCompositeDeviceInstanceBase::SubDeviceCollectionInfo::GetSubsetInfo(
			ICollectionInfo& /*subsetInfo*/,
			int /*offset*/,
			int /*count*/,
			const iprm::IParamsSet* /*selectionParamsPtr*/,
			ilog::IMessageConsumer* /*logPtr*/) const
{
	return false;
}


QVariant CCompositeDeviceInstanceBase::SubDeviceCollectionInfo::GetElementInfo(
			const Id& elementId,
			int infoType,
			ilog::IMessageConsumer* /*logPtr*/) const
{
	if (!m_parentPtr->m_subDeviceMap.contains(elementId)){
		return QVariant();
	}

	switch (infoType){
	case EIT_NAME:
		return m_parentPtr->m_subDeviceMap[elementId].name;
	case EIT_DESCRIPTION:
		return m_parentPtr->m_subDeviceMap[elementId].description;
	case EIT_ENABLED:
		return true;
	default:
		I_IF_DEBUG(qDebug() << __FILE__ << __LINE__ << "Unknown info type: " << infoType;)
		break;
	}

	return QVariant();
}


idoc::MetaInfoPtr CCompositeDeviceInstanceBase::SubDeviceCollectionInfo::GetElementMetaInfo(
			const Id& /*elementId*/,
			ilog::IMessageConsumer* /*logPtr*/) const
{
	return idoc::MetaInfoPtr();
}


bool CCompositeDeviceInstanceBase::SubDeviceCollectionInfo::SetElementName(
			const Id& /*elementId*/,
			const QString& /*name*/,
			ilog::IMessageConsumer* /*logPtr*/)
{
	return false;
}


bool CCompositeDeviceInstanceBase::SubDeviceCollectionInfo::SetElementDescription(
			const Id& /*elementId*/,
			const QString& /*description*/,
			ilog::IMessageConsumer* /*logPtr*/)
{
	return false;
}


bool CCompositeDeviceInstanceBase::SubDeviceCollectionInfo::SetElementEnabled(
			const Id& /*elementId*/,
			bool /*isEnabled*/,
			ilog::IMessageConsumer* /*logPtr*/)
{
	return false;
}


} // namespace imtdev


