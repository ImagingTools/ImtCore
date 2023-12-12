#include <imtbase/CCachedObjectCollectionComp.h>


// ACF includes
#include <istd/TDelPtr.h>
#include <istd/CChangeNotifier.h>
#include <iser/CMemoryWriteArchive.h>


namespace imtbase
{


// public methods

CCachedObjectCollectionComp::CCachedObjectCollectionComp()
	:m_operationFlags(IObjectCollection::OF_ALL & ~OF_SUPPORT_PAGINATION)
{
}


// reimplemented (icomp::CComponentBase)

void CCachedObjectCollectionComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	if (m_objectCollectionModelCompPtr.IsValid()){
		m_objectCollectionModelCompPtr->AttachObserver(this);
	}
}


void CCachedObjectCollectionComp::OnComponentDestroyed()
{
	if (m_objectCollectionModelCompPtr.IsValid()){
		m_objectCollectionModelCompPtr->DetachObserver(this);
	}

	BaseClass::OnComponentDestroyed();
}


// reimplemented (imod::CSingleModelObserverBase)

void CCachedObjectCollectionComp::OnUpdate(const istd::IChangeable::ChangeSet& changeSet)
{
	istd::CChangeNotifier changeNotifier(this, &changeSet);

	ClearCache();
}


// reimplemented (imtbase::IObjectCollection)

const IRevisionController* CCachedObjectCollectionComp::GetRevisionController() const
{
	if (m_objectCollectionCompPtr.IsValid()){
		return m_objectCollectionCompPtr->GetRevisionController();
	}

	return nullptr;
}


const ICollectionDataController* CCachedObjectCollectionComp::GetDataController() const
{
	if (m_objectCollectionCompPtr.IsValid()){
		return m_objectCollectionCompPtr->GetDataController();
	}

	return nullptr;
}


int CCachedObjectCollectionComp::GetOperationFlags(const QByteArray& /*objectId*/) const
{
	return m_operationFlags;
}


ICollectionInfo::Id CCachedObjectCollectionComp::InsertNewObject(
			const QByteArray& typeId,
			const QString& name,
			const QString& description,
			DataPtr defaultValuePtr,
			const Id& proposedElementId,
			const idoc::IDocumentMetaInfo* dataMetaInfoPtr,
			const idoc::IDocumentMetaInfo* elementMetaInfoPtr,
			const IOperationContext* operationContextPtr)
{
	if (!m_objectCollectionCompPtr.IsValid()){
		return ICollectionInfo::Id();
	}

	QByteArray id = proposedElementId.isEmpty() ? QUuid::createUuid().toByteArray(QUuid::WithoutBraces) : proposedElementId;

	istd::IChangeable::ChangeSet changeSet(CF_ADDED);
	changeSet.SetChangeInfo(CN_ELEMENT_INSERTED, id);
	istd::CChangeNotifier changeNotifier(this, &changeSet);

	m_collectionCacheItems.Reset();

	QByteArray retVal =  m_objectCollectionCompPtr->InsertNewObject(
				typeId,
				name,
				description,
				defaultValuePtr,
				id,
				dataMetaInfoPtr,
				elementMetaInfoPtr,
				operationContextPtr);
	if (retVal.isEmpty()){
		changeNotifier.Abort();
	}

	return retVal;
}


bool CCachedObjectCollectionComp::RemoveElement(const Id& elementId, const IOperationContext* operationContextPtr)
{
	if (!m_objectCollectionCompPtr.IsValid()){
		return false;
	}

	istd::IChangeable::ChangeSet changeSet(CF_REMOVED);
	changeSet.SetChangeInfo(CN_ELEMENT_REMOVED, elementId);
	istd::CChangeNotifier changeNotifier(this, &changeSet);

	bool retVal = m_objectCollectionCompPtr->RemoveElement(elementId, operationContextPtr);
	if (retVal){
		m_collectionCacheItems.Reset();
	}

	return  retVal;
}


const istd::IChangeable* CCachedObjectCollectionComp::GetObjectPtr(const Id& objectId) const
{
	if (m_objectCollectionCompPtr.IsValid()){
		return m_objectCollectionCompPtr->GetObjectPtr(objectId);
	}

	return nullptr;
}


bool CCachedObjectCollectionComp::GetObjectData(const Id& objectId, DataPtr& dataPtr) const
{
	if (!m_objectCollectionCompPtr.IsValid()){
		return false;
	}

	if (m_cacheItems.contains(objectId)){
		dataPtr = m_cacheItems[objectId].dataPtr;

		return true;
	}

	int cacheSize = m_cacheItems.size();
	if (cacheSize >= *m_objectCacheLimitAttrPtr){
		RemoveOldestObjectFromCache();
	}

	bool retVal = m_objectCollectionCompPtr->GetObjectData(objectId, dataPtr);
	if (retVal){
		m_cacheItems.insert(objectId, {dataPtr, QDateTime::currentMSecsSinceEpoch()});
	}

	return  retVal;
}


bool CCachedObjectCollectionComp::SetObjectData(
			const Id& objectId,
			const istd::IChangeable& object,
			CompatibilityMode mode,
			const IOperationContext* operationContextPtr)
{
	if (!m_objectCollectionCompPtr.IsValid()){
		return false;
	}

	istd::IChangeable::ChangeSet changeSet(CF_UPDATED);
	changeSet.SetChangeInfo(CN_ELEMENT_UPDATED, objectId);
	istd::CChangeNotifier changeNotifier(this, &changeSet);

	m_collectionCacheItems.Reset();

	bool retVal = m_objectCollectionCompPtr->SetObjectData(objectId, object, mode, operationContextPtr);
	if (!retVal){
		changeNotifier.Abort();
	}

	return retVal;
}


IObjectCollection* CCachedObjectCollectionComp::CreateSubCollection(int offset, int count, const iprm::IParamsSet *selectionParamsPtr) const
{
	FilteredCollection* collectionChacheItemPtr = CheckCache(offset, count, selectionParamsPtr);
	if (collectionChacheItemPtr != nullptr){
		return collectionChacheItemPtr->cachePtr->CreateSubCollection(offset, count, selectionParamsPtr);
	}

	return nullptr;
}


imtbase::IObjectCollectionIterator* CCachedObjectCollectionComp::CreateObjectCollectionIterator(
			int /*offset*/,
			int /*count*/,
			const iprm::IParamsSet* /*selectionParamsPtr*/) const
{
	return nullptr;
}


// reimplemented (IObjectCollectionInfo)

const iprm::IOptionsList* CCachedObjectCollectionComp::GetObjectTypesInfo() const
{
	if (!m_objectCollectionCompPtr.IsValid()){
		return nullptr;
	}

	return m_objectCollectionCompPtr->GetObjectTypesInfo();
}


QByteArray CCachedObjectCollectionComp::GetObjectTypeId(const Id& objectId) const
{
	if (!m_objectCollectionCompPtr.IsValid()){
		return QByteArray();
	}

	return m_objectCollectionCompPtr->GetObjectTypeId(objectId);
}


idoc::MetaInfoPtr CCachedObjectCollectionComp::GetDataMetaInfo(const Id& objectId) const
{
	for (int index = 0; index < m_collectionCacheItems.GetCount(); index++){
		FilteredCollection* collectionChacheItemPtr = m_collectionCacheItems.GetAt(index);
		if (collectionChacheItemPtr->cachePtr->GetElementIds().contains(objectId)){
			return collectionChacheItemPtr->cachePtr->GetDataMetaInfo(objectId);
		}
	}

	return idoc::MetaInfoPtr();
}


int CCachedObjectCollectionComp::GetElementsCount(const iprm::IParamsSet* selectionParamsPtr, ilog::IMessageConsumer* logPtr) const
{
	if (m_objectCollectionCompPtr.IsValid()){
		return m_objectCollectionCompPtr->GetElementsCount(selectionParamsPtr, logPtr);
	}

	return 0;
}


ICollectionInfo::Ids CCachedObjectCollectionComp::GetElementIds(
			int offset,
			int count,
			const iprm::IParamsSet* selectionParamsPtr,
			ilog::IMessageConsumer* logPtr) const
{
	FilteredCollection* collectionChacheItemPtr = CheckCache(offset, count, selectionParamsPtr);
	if (collectionChacheItemPtr != nullptr){
		return collectionChacheItemPtr->cachePtr->GetElementIds(0, -1, nullptr, logPtr);
	}

	return ICollectionInfo::Ids();
}


bool CCachedObjectCollectionComp::GetSubsetInfo(
			ICollectionInfo& /*subsetInfo*/,
			int /*offset*/,
			int /*count*/,
			const iprm::IParamsSet* /*selectionParamsPtr*/,
			ilog::IMessageConsumer* /*logPtr*/) const
{
	return false;
}


QVariant CCachedObjectCollectionComp::GetElementInfo(const Id& elementId, int infoType, ilog::IMessageConsumer* logPtr) const
{
	for (int index = 0; index < m_collectionCacheItems.GetCount(); index++){
		FilteredCollection* collectionChacheItemPtr = m_collectionCacheItems.GetAt(index);
		if (collectionChacheItemPtr->cachePtr->GetElementIds().contains(elementId)){
			return collectionChacheItemPtr->cachePtr->GetElementInfo(elementId, infoType, logPtr);
		}
	}

	return QVariant();
}


idoc::MetaInfoPtr CCachedObjectCollectionComp::GetElementMetaInfo(const Id& elementId, ilog::IMessageConsumer* logPtr) const
{
	for (int index = 0; index < m_collectionCacheItems.GetCount(); index++){
		FilteredCollection* collectionChacheItemPtr = m_collectionCacheItems.GetAt(index);
		if (collectionChacheItemPtr->cachePtr->GetElementIds().contains(elementId)){
			return collectionChacheItemPtr->cachePtr->GetElementMetaInfo(elementId, logPtr);
		}
	}

	return idoc::MetaInfoPtr();
}


bool CCachedObjectCollectionComp::SetElementName(const Id& elementId, const QString& name, ilog::IMessageConsumer* logPtr)
{
	if (!m_objectCollectionCompPtr.IsValid()){
		return false;
	}

	m_collectionCacheItems.Reset();

	istd::IChangeable::ChangeSet changeSet(CF_UPDATED);
	changeSet.SetChangeInfo(CN_ELEMENT_UPDATED, elementId);
	istd::CChangeNotifier changeNotifier(this, &changeSet);

	bool retVal =  m_objectCollectionCompPtr->SetElementName(elementId, name);
	if (!retVal){
		changeNotifier.Abort();
	}

	return retVal;
}


bool CCachedObjectCollectionComp::SetElementDescription(const Id& elementId, const QString& description, ilog::IMessageConsumer* logPtr)
{
	if (!m_objectCollectionCompPtr.IsValid()){
		return false;
	}

	m_collectionCacheItems.Reset();

	istd::IChangeable::ChangeSet changeSet(CF_UPDATED);
	changeSet.SetChangeInfo(CN_ELEMENT_UPDATED, elementId);
	istd::CChangeNotifier changeNotifier(this, &changeSet);

	bool retVal = m_objectCollectionCompPtr->SetElementDescription(elementId, description, logPtr);
	if (!retVal){
		changeNotifier.Abort();
	}

	return retVal;
}


bool CCachedObjectCollectionComp::SetElementEnabled(const Id& elementId, bool isEnabled, ilog::IMessageConsumer* logPtr)
{
	if (!m_objectCollectionCompPtr.IsValid()){
		return false;
	}

	m_collectionCacheItems.Reset();

	istd::IChangeable::ChangeSet changeSet(CF_UPDATED);
	changeSet.SetChangeInfo(CN_ELEMENT_UPDATED, elementId);
	istd::CChangeNotifier changeNotifier(this, &changeSet);

	bool retVal = m_objectCollectionCompPtr->SetElementEnabled(elementId, isEnabled);
	if (!retVal){
		changeNotifier.Abort();
	}

	return retVal;
}


CCachedObjectCollectionComp::FilteredCollection* CCachedObjectCollectionComp::CheckCache(
			int offset,
			int count,
			const iprm::IParamsSet* selectionParamsPtr) const
{
	if (!m_objectCollectionCompPtr.IsValid()){
		return nullptr;
	}

	iser::CMemoryWriteArchive archive;
	iprm::IParamsSet* paramsSet = const_cast<iprm::IParamsSet*>(selectionParamsPtr);
	if (paramsSet != nullptr && !paramsSet->Serialize(archive)){
		Q_ASSERT_X(false, __FILE__, "Serialization of the filter parameters was failed");

		return nullptr;
	}

	QByteArray data((char*)archive.GetBuffer(), archive.GetBufferSize());

	for (int index = 0; index < m_collectionCacheItems.GetCount(); index++){
		FilteredCollection* collectionChacheItemPtr = m_collectionCacheItems.GetAt(index);
		if (		collectionChacheItemPtr != nullptr &&
					collectionChacheItemPtr->offset == offset &&
					collectionChacheItemPtr->count == count &&
					collectionChacheItemPtr->selectionParamsData == data){
			return collectionChacheItemPtr;
		}
	}

	IObjectCollection* subcollection = m_objectCollectionCompPtr->CreateSubCollection(offset, count, selectionParamsPtr);
	if (subcollection == nullptr){
		Q_ASSERT_X(false, __FILE__, "Sub-collection coult not be created");

		return nullptr;
	}

	if (!m_collectionCacheItems.IsEmpty()){
		if (m_collectionCacheItems.GetCount() >= *m_metaInfoCacheLimitAttrPtr){
			m_collectionCacheItems.RemoveAt(0);
		}
	}

	m_collectionCacheItems.PushBack(new FilteredCollection(offset, count, data, subcollection));

	return m_collectionCacheItems.GetAt(m_collectionCacheItems.GetCount() - 1);
}


void CCachedObjectCollectionComp::ClearCache()
{
	m_collectionCacheItems.Reset();

	m_cacheItems.clear();
}


void CCachedObjectCollectionComp::RemoveOldestObjectFromCache() const
{
	QByteArray oldestObjectId;

	if (!m_cacheItems.isEmpty()){
		qint64 minTimeStamp = m_cacheItems.first().timestamp;

		for (CacheItemMap::ConstIterator iter = m_cacheItems.constBegin(); iter != m_cacheItems.constEnd(); ++iter){
			const CacheItem& item = iter.value();

			if (item.timestamp < minTimeStamp){
				minTimeStamp = item.timestamp;
				oldestObjectId = iter.key();
			}
		}
	}

	if (!oldestObjectId.isEmpty()){
		m_cacheItems.remove(oldestObjectId);
	}
}


} // namespace imtbase


