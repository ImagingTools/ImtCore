#include <imtdev/CCompositeDeviceInstanceInfoBase.h>


// Qt includes
#include <QtCore/QUuid>

// ACF include
#include <idoc/IDocumentMetaInfo.h>
#include <istd/CChangeNotifier.h>
#include <istd/TDelPtr.h>


namespace imtdev
{


// public methods

CCompositeDeviceInstanceInfoBase::CCompositeDeviceInstanceInfoBase()
{
	m_subDeviceCollection.SetParent(this);
	m_subDeviceCollection.AttachObserver(&m_updateBridge);
}

CCompositeDeviceInstanceInfoBase::~CCompositeDeviceInstanceInfoBase()
{
	if (m_updateBridge.IsModelAttached(&m_subDeviceCollection)){
		m_subDeviceCollection.DetachObserver(&m_updateBridge);
	}
}


// reimplemented (imtdev::IEditableCompositeDeviceInstanceInfo)

QByteArray CCompositeDeviceInstanceInfoBase::AddSubDevice(
	const QByteArray& deviceTypeId,
	const QString& name,
	const QString& description,
	const istd::IChangeable* defaultValuePtr,
	const QByteArray& proposedId)
{
	istd::TDelPtr<imtdev::IDeviceInstanceInfo> deviceInstancePtr;
	deviceInstancePtr.SetCastedOrRemove(CreateDeviceInstanceInfo(deviceTypeId));
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
	m_subDeviceMap[id].instanceInfoPtr = DeviceInstanceInfoPtr(deviceInstancePtr.PopPtr());

	return id;
}


bool CCompositeDeviceInstanceInfoBase::RemoveSubDevice(const QByteArray& id)
{
	if (m_subDeviceMap.contains(id)){
		istd::CChangeNotifier notifier(this);

		imod::IModel* modelPtr = dynamic_cast<imod::IModel*>(m_subDeviceMap[id].instanceInfoPtr.get());
		if (modelPtr != nullptr && modelPtr->IsAttached(&m_updateBridge)){
			modelPtr->DetachObserver(&m_updateBridge);
		}

		m_subDeviceMap.remove(id);

		return true;
	}

	return false;
}


// reimplemented (imtdev::ICompositeDeviceInstanceInfo)

const ICompositeDeviceStaticInfo* CCompositeDeviceInstanceInfoBase::GetCompositeStaticInfo() const
{
	return nullptr;
}


QSet<QByteArray> CCompositeDeviceInstanceInfoBase::GetSupportedSubDeviceTypeIds() const
{
	return {};
}


const imtbase::ICollectionInfo& CCompositeDeviceInstanceInfoBase::GetSubDeviceList() const
{
	return m_subDeviceCollection;
}


const IDeviceInstanceInfo* CCompositeDeviceInstanceInfoBase::GetSubDeviceInstanceInfo(
	const QByteArray& subDeviceId) const
{
	if (m_subDeviceMap.contains(subDeviceId)){
		return m_subDeviceMap[subDeviceId].instanceInfoPtr.get();
	}

	return nullptr;
}


// reimplemented (istd::IChangeable)

int CCompositeDeviceInstanceInfoBase::GetSupportedOperations() const
{
	return SO_COPY | SO_RESET;
}


bool CCompositeDeviceInstanceInfoBase::CopyFrom(const IChangeable& object, CompatibilityMode mode)
{
	const CCompositeDeviceInstanceInfoBase* sourcePtr = dynamic_cast<const CCompositeDeviceInstanceInfoBase*>(&object);
	if (sourcePtr != nullptr){
		if (GetStaticInfo().GetTypeId() != sourcePtr->GetStaticInfo().GetTypeId()){
			return false;
		}

		istd::CChangeNotifier notifier(this);

		RemoveAllSubDevices();

		bool retVal = BaseClass::CopyFrom(object, mode);

		if (retVal){
			for (const QByteArray& subDeviceId : sourcePtr->m_subDeviceMap.keys()){
				QByteArray newId = AddSubDevice(
					sourcePtr->m_subDeviceMap[subDeviceId].instanceInfoPtr->GetStaticInfo().GetTypeId(),
					sourcePtr->m_subDeviceMap[subDeviceId].name,
					sourcePtr->m_subDeviceMap[subDeviceId].description,
					sourcePtr->m_subDeviceMap[subDeviceId].instanceInfoPtr.get(),
					subDeviceId);

				Q_ASSERT(newId == subDeviceId);

				retVal = retVal && !newId.isEmpty();
			}
		}

		return retVal;
	}

	return false;
}


bool CCompositeDeviceInstanceInfoBase::ResetData(CompatibilityMode mode)
{
	istd::CChangeNotifier notifier(this);

	bool retVal = BaseClass::ResetData(mode);

	RemoveAllSubDevices();

	return retVal;
}


// protected methods

IDeviceInstanceInfo* CCompositeDeviceInstanceInfoBase::CreateDeviceInstanceInfo(const QByteArray& /*deviceTypeId*/) const
{
	return nullptr;
}


// private methods

void CCompositeDeviceInstanceInfoBase::RemoveAllSubDevices()
{
	istd::CChangeNotifier notifier(this);

	for (const QByteArray& subDeviceId : m_subDeviceMap.keys()){
		imod::IModel* modelPtr = dynamic_cast<imod::IModel*>(m_subDeviceMap[subDeviceId].instanceInfoPtr.get());
		if (modelPtr != nullptr){
			if (m_updateBridge.IsModelAttached(modelPtr)){
				modelPtr->DetachObserver(&m_updateBridge);
			}
		}
	}

	m_subDeviceMap.clear();
}


// public methods of the embedded class SubDeviceCollectionInfo

void CCompositeDeviceInstanceInfoBase::SubDeviceCollectionInfo::SetParent(CCompositeDeviceInstanceInfoBase* parentPtr)
{
	m_parentPtr = parentPtr;
}


// reimpolemented (imtbase::ICollectionInfo)

int CCompositeDeviceInstanceInfoBase::SubDeviceCollectionInfo::GetElementsCount(
			const iprm::IParamsSet* /*selectionParamsPtr*/,
			ilog::IMessageConsumer* /*logPtr*/) const
{
	return m_parentPtr->m_subDeviceMap.count();
}


imtbase::ICollectionInfo::Ids CCompositeDeviceInstanceInfoBase::SubDeviceCollectionInfo::GetElementIds(
			int /*offset*/,
			int /*count*/,
			const iprm::IParamsSet* /*selectionParamsPtr*/,
			ilog::IMessageConsumer* /*logPtr*/) const
{
	return m_parentPtr->m_subDeviceMap.keys().toVector();
}


bool CCompositeDeviceInstanceInfoBase::SubDeviceCollectionInfo::GetSubsetInfo(
			ICollectionInfo& /*subsetInfo*/,
			int /*offset*/,
			int /*count*/,
			const iprm::IParamsSet* /*selectionParamsPtr*/,
			ilog::IMessageConsumer* /*logPtr*/) const
{
	return false;
}


QVariant CCompositeDeviceInstanceInfoBase::SubDeviceCollectionInfo::GetElementInfo(
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
	}

	return QVariant();
}


idoc::MetaInfoPtr CCompositeDeviceInstanceInfoBase::SubDeviceCollectionInfo::GetElementMetaInfo(
			const Id& /*elementId*/,
			ilog::IMessageConsumer* /*logPtr*/) const
{
	return idoc::MetaInfoPtr();
}


bool CCompositeDeviceInstanceInfoBase::SubDeviceCollectionInfo::SetElementName(
			const Id& /*elementId*/,
			const QString& /*name*/,
			ilog::IMessageConsumer* /*logPtr*/)
{
	return false;
}


bool CCompositeDeviceInstanceInfoBase::SubDeviceCollectionInfo::SetElementDescription(
			const Id& /*elementId*/,
			const QString& /*description*/,
			ilog::IMessageConsumer* /*logPtr*/)
{
	return false;
}


bool CCompositeDeviceInstanceInfoBase::SubDeviceCollectionInfo::SetElementEnabled(
			const Id& /*elementId*/,
			bool /*isEnabled*/,
			ilog::IMessageConsumer* /*logPtr*/)
{
	return false;
}


} // namespace imtdev


