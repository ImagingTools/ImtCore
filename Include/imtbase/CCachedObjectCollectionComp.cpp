// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtbase/CCachedObjectCollectionComp.h>


// Qt includes
#include <QtCore/QUuid>

// ACF includes
#include <istd/TDelPtr.h>
#include <istd/CChangeNotifier.h>
#include <iser/CMemoryWriteArchive.h>


namespace imtbase
{


// public methods

CCachedObjectCollectionComp::CCachedObjectCollectionComp()
	:m_operationFlags(IObjectCollection::OF_ALL & ~OF_SUPPORT_PAGINATION),
	m_lock(QReadWriteLock::Recursive)
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
			const istd::IChangeable* defaultValuePtr,
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

	QWriteLocker locker(&m_lock);
	m_cachedCollections.Reset();
	locker.unlock();

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


bool CCachedObjectCollectionComp::RemoveElements(const Ids& elementIds, const IOperationContext* operationContextPtr)
{
	if (!m_objectCollectionCompPtr.IsValid()){
		return false;
	}

	imtbase::ICollectionInfo::MultiElementNotifierInfo notifierInfo;
	notifierInfo.elementIds = elementIds;

	istd::IChangeable::ChangeSet changeSet(CF_REMOVED);
	changeSet.SetChangeInfo(CN_ELEMENTS_REMOVED, QVariant::fromValue(notifierInfo));
	istd::CChangeNotifier changeNotifier(this, &changeSet);

	bool retVal = m_objectCollectionCompPtr->RemoveElements(elementIds, operationContextPtr);
	if (retVal){
		QWriteLocker locker(&m_lock);
		m_cachedCollections.Reset();
	}

	return  retVal;
}


bool CCachedObjectCollectionComp::RemoveElementSet(
			const iprm::IParamsSet* /*selectionParamsPtr*/,
			const IOperationContext* /*operationContextPtr*/)
{
	return false;
}


bool CCachedObjectCollectionComp::RestoreObjects(
			const Ids& /*objectIds*/,
			const IOperationContext* /*operationContextPtr*/)
{
	return false;
}


bool CCachedObjectCollectionComp::RestoreObjectSet(
			const iprm::IParamsSet* /*selectionParamsPtr*/,
			const IOperationContext* /*operationContextPtr*/)
{
	return false;
}


const istd::IChangeable* CCachedObjectCollectionComp::GetObjectPtr(const Id& objectId) const
{
	if (m_objectCollectionCompPtr.IsValid()){
		return m_objectCollectionCompPtr->GetObjectPtr(objectId);
	}

	return nullptr;
}


bool CCachedObjectCollectionComp::GetObjectData(const Id& objectId, DataPtr& dataPtr, const iprm::IParamsSet* /*dataConfigurationPtr*/) const
{
	if (!m_objectCollectionCompPtr.IsValid()){
		return false;
	}

	QReadLocker locker(&m_lock);

	if (m_cacheItems.contains(objectId)){
		dataPtr = m_cacheItems[objectId].dataPtr;

		return true;
	}

	locker.unlock();

	int cacheSize = m_cacheItems.size();
	if (cacheSize >= *m_objectCacheLimitAttrPtr){
		RemoveOldestObjectFromCache();
	}

	bool retVal = m_objectCollectionCompPtr->GetObjectData(objectId, dataPtr);
	if (retVal){
		m_lock.lockForWrite();
		m_cacheItems.insert(objectId, {dataPtr, QDateTime::currentMSecsSinceEpoch()});
		m_lock.unlock();
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

	istd::IChangeable::ChangeSet changeSet(CF_OBJECT_DATA_CHANGED);
	changeSet.SetChangeInfo(CN_OBJECT_DATA_CHANGED, objectId);
	istd::CChangeNotifier changeNotifier(this, &changeSet);

	QWriteLocker locker(&m_lock);
	m_cachedCollections.Reset();
	locker.unlock();

	bool retVal = m_objectCollectionCompPtr->SetObjectData(objectId, object, mode, operationContextPtr);
	if (!retVal){
		changeNotifier.Abort();
	}

	return retVal;
}


IObjectCollectionUniquePtr CCachedObjectCollectionComp::CreateSubCollection(int offset, int count, const iprm::IParamsSet *selectionParamsPtr) const
{
	FilteredCollection* collectionChacheItemPtr = GetFilteredCollection(offset, count, selectionParamsPtr);
	if (collectionChacheItemPtr != nullptr){
		return collectionChacheItemPtr->cachePtr->CreateSubCollection(offset, count, selectionParamsPtr);
	}

	return nullptr;
}


imtbase::IObjectCollectionIterator* CCachedObjectCollectionComp::CreateObjectCollectionIterator(
			const QByteArray& objectId,
			int offset,
			int count,
			const iprm::IParamsSet* selectionParamsPtr) const
{
	FilteredCollection* collectionChacheItemPtr = GetFilteredCollection(offset, count, selectionParamsPtr);
	if (collectionChacheItemPtr != nullptr){
		return collectionChacheItemPtr->cachePtr->CreateObjectCollectionIterator(objectId, offset, count, selectionParamsPtr);
	}
	
	if (!m_objectCollectionCompPtr.IsValid()){
		return nullptr;
	}
	
	return m_objectCollectionCompPtr->CreateObjectCollectionIterator(objectId, offset, count, selectionParamsPtr);
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
	QReadLocker locker(&m_lock);

	for (int index = 0; index < m_cachedCollections.GetCount(); index++){
		FilteredCollection* collectionChacheItemPtr = m_cachedCollections.GetAt(index);
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
	FilteredCollection* collectionChacheItemPtr = GetFilteredCollection(offset, count, selectionParamsPtr);
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
	QReadLocker locker(&m_lock);

	for (int index = 0; index < m_cachedCollections.GetCount(); index++){
		FilteredCollection* collectionChacheItemPtr = m_cachedCollections.GetAt(index);
		if (collectionChacheItemPtr->cachePtr->GetElementIds().contains(elementId)){
			return collectionChacheItemPtr->cachePtr->GetElementInfo(elementId, infoType, logPtr);
		}
	}

	return QVariant();
}


idoc::MetaInfoPtr CCachedObjectCollectionComp::GetElementMetaInfo(const Id& elementId, ilog::IMessageConsumer* logPtr) const
{
	QReadLocker locker(&m_lock);

	for (int index = 0; index < m_cachedCollections.GetCount(); index++){
		FilteredCollection* collectionChacheItemPtr = m_cachedCollections.GetAt(index);
		if (collectionChacheItemPtr->cachePtr->GetElementIds().contains(elementId)){
			return collectionChacheItemPtr->cachePtr->GetElementMetaInfo(elementId, logPtr);
		}
	}

	return idoc::MetaInfoPtr();
}


bool CCachedObjectCollectionComp::SetElementName(const Id& elementId, const QString& name, ilog::IMessageConsumer* /*logPtr*/)
{
	if (!m_objectCollectionCompPtr.IsValid()){
		return false;
	}

	QWriteLocker locker(&m_lock);
	m_cachedCollections.Reset();
	locker.unlock();

	istd::IChangeable::ChangeSet changeSet(CF_ELEMENT_RENAMED);
	changeSet.SetChangeInfo(CN_ELEMENT_RENAMED, elementId);
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

	QWriteLocker locker(&m_lock);
	m_cachedCollections.Reset();
	locker.unlock();

	istd::IChangeable::ChangeSet changeSet(CF_ELEMENT_DESCRIPTION_CHANGED);
	changeSet.SetChangeInfo(CN_ELEMENT_DESCRIPTION_CHANGED, elementId);
	istd::CChangeNotifier changeNotifier(this, &changeSet);

	bool retVal = m_objectCollectionCompPtr->SetElementDescription(elementId, description, logPtr);
	if (!retVal){
		changeNotifier.Abort();
	}

	return retVal;
}


bool CCachedObjectCollectionComp::SetElementEnabled(const Id& elementId, bool isEnabled, ilog::IMessageConsumer* /*logPtr*/)
{
	if (!m_objectCollectionCompPtr.IsValid()){
		return false;
	}

	QWriteLocker locker(&m_lock);
	m_cachedCollections.Reset();
	locker.unlock();

	istd::IChangeable::ChangeSet changeSet(CF_ELEMENT_STATE);
	changeSet.SetChangeInfo(CN_ELEMENT_STATE, elementId);
	istd::CChangeNotifier changeNotifier(this, &changeSet);

	bool retVal = m_objectCollectionCompPtr->SetElementEnabled(elementId, isEnabled);
	if (!retVal){
		changeNotifier.Abort();
	}

	return retVal;
}


// TODO: cannot return a pointer in this function it's not thread-safe.

CCachedObjectCollectionComp::FilteredCollection* CCachedObjectCollectionComp::GetFilteredCollection(
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

	m_lock.lockForRead();

	QByteArray data((char*)archive.GetBuffer(), archive.GetBufferSize());

	for (int index = 0; index < m_cachedCollections.GetCount(); index++){
		FilteredCollection* collectionChacheItemPtr = m_cachedCollections.GetAt(index);
		if (		collectionChacheItemPtr != nullptr &&
					collectionChacheItemPtr->offset == offset &&
					collectionChacheItemPtr->count == count &&
					collectionChacheItemPtr->selectionParamsData == data){
			m_lock.unlock();
			return collectionChacheItemPtr;
		}
	}

	m_lock.unlock();

	IObjectCollectionUniquePtr subCollectionPtr = m_objectCollectionCompPtr->CreateSubCollection(offset, count, selectionParamsPtr);
	if (!subCollectionPtr.IsValid()){
		return nullptr;
	}

	QWriteLocker locker(&m_lock);

	if (!m_cachedCollections.IsEmpty()){
		if (m_cachedCollections.GetCount() >= *m_metaInfoCacheLimitAttrPtr){
			m_cachedCollections.RemoveAt(0);
		}
	}

	m_cachedCollections.PushBack(new FilteredCollection(offset, count, data, std::move(subCollectionPtr)));

	return m_cachedCollections.GetAt(m_cachedCollections.GetCount() - 1);
}


void CCachedObjectCollectionComp::ClearCache()
{
	QWriteLocker locker(&m_lock);

	m_cachedCollections.Reset();

	m_cacheItems.clear();
}


void CCachedObjectCollectionComp::RemoveOldestObjectFromCache() const
{
	QByteArray oldestObjectId;

	QReadLocker locker(&m_lock);

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

	locker.unlock();

	if (!oldestObjectId.isEmpty()){
		m_lock.lockForWrite();
		m_cacheItems.remove(oldestObjectId);
		m_lock.unlock();
	}
}


} // namespace imtbase


