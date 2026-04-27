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
	m_cachedCollections.clear();
	locker.unlock();

	QByteArray retVal = m_objectCollectionCompPtr->InsertNewObject(
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
		m_cachedCollections.clear();
		for (int i = 0; i < elementIds.size(); ++i){
			m_cacheItems.remove(elementIds[i]);
		}
	}
	else{
		changeNotifier.Abort();
	}

	return retVal;
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

	{
		QReadLocker locker(&m_lock);

		CacheItemMap::const_iterator it = m_cacheItems.constFind(objectId);
		if (it != m_cacheItems.constEnd()){
			dataPtr = it.value().dataPtr;

			return true;
		}
	}

	bool retVal = m_objectCollectionCompPtr->GetObjectData(objectId, dataPtr);
	if (retVal){
		QWriteLocker locker(&m_lock);
		if (m_cacheItems.size() >= *m_objectCacheLimitAttrPtr){
			RemoveOldestObjectFromCache();
		}
		m_cacheItems.insert(objectId, {dataPtr, QDateTime::currentMSecsSinceEpoch()});
	}

	return retVal;
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
	m_cachedCollections.clear();
	m_cacheItems.remove(objectId);
	locker.unlock();

	bool retVal = m_objectCollectionCompPtr->SetObjectData(objectId, object, mode, operationContextPtr);
	if (!retVal){
		changeNotifier.Abort();
	}

	return retVal;
}


IObjectCollectionUniquePtr CCachedObjectCollectionComp::CreateSubCollection(int offset, int count, const iprm::IParamsSet *selectionParamsPtr) const
{
	FilteredCollectionPtr collectionCacheItemPtr = GetFilteredCollection(offset, count, selectionParamsPtr);
	if (collectionCacheItemPtr != nullptr){
		return collectionCacheItemPtr->cachePtr->CreateSubCollection(0, -1, nullptr);
	}

	return nullptr;
}


imtbase::IObjectCollectionIterator* CCachedObjectCollectionComp::CreateObjectCollectionIterator(
			const QByteArray& objectId,
			int offset,
			int count,
			const iprm::IParamsSet* selectionParamsPtr) const
{
	FilteredCollectionPtr collectionCacheItemPtr = GetFilteredCollection(offset, count, selectionParamsPtr);
	if (collectionCacheItemPtr != nullptr){
		return collectionCacheItemPtr->cachePtr->CreateObjectCollectionIterator(objectId, 0, -1, nullptr);
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

	for (int index = 0; index < m_cachedCollections.size(); index++){
		const FilteredCollectionPtr& collectionCacheItemPtr = m_cachedCollections.at(index);
		if (collectionCacheItemPtr->cachePtr->GetElementIds().contains(objectId)){
			return collectionCacheItemPtr->cachePtr->GetDataMetaInfo(objectId);
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
	FilteredCollectionPtr collectionCacheItemPtr = GetFilteredCollection(offset, count, selectionParamsPtr);
	if (collectionCacheItemPtr != nullptr){
		return collectionCacheItemPtr->cachePtr->GetElementIds(0, -1, nullptr, logPtr);
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

	for (int index = 0; index < m_cachedCollections.size(); index++){
		const FilteredCollectionPtr& collectionCacheItemPtr = m_cachedCollections.at(index);
		if (collectionCacheItemPtr->cachePtr->GetElementIds().contains(elementId)){
			return collectionCacheItemPtr->cachePtr->GetElementInfo(elementId, infoType, logPtr);
		}
	}

	return QVariant();
}


idoc::MetaInfoPtr CCachedObjectCollectionComp::GetElementMetaInfo(const Id& elementId, ilog::IMessageConsumer* logPtr) const
{
	QReadLocker locker(&m_lock);

	for (int index = 0; index < m_cachedCollections.size(); index++){
		const FilteredCollectionPtr& collectionCacheItemPtr = m_cachedCollections.at(index);
		if (collectionCacheItemPtr->cachePtr->GetElementIds().contains(elementId)){
			return collectionCacheItemPtr->cachePtr->GetElementMetaInfo(elementId, logPtr);
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
	m_cachedCollections.clear();
	locker.unlock();

	istd::IChangeable::ChangeSet changeSet(CF_ELEMENT_RENAMED);
	changeSet.SetChangeInfo(CN_ELEMENT_RENAMED, elementId);
	istd::CChangeNotifier changeNotifier(this, &changeSet);

	bool retVal = m_objectCollectionCompPtr->SetElementName(elementId, name);
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
	m_cachedCollections.clear();
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
	m_cachedCollections.clear();
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


CCachedObjectCollectionComp::FilteredCollectionPtr CCachedObjectCollectionComp::GetFilteredCollection(
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

	{
		QReadLocker locker(&m_lock);

		for (int index = 0; index < m_cachedCollections.size(); index++){
			const FilteredCollectionPtr& collectionCacheItemPtr = m_cachedCollections.at(index);
			if (		collectionCacheItemPtr != nullptr &&
						collectionCacheItemPtr->offset == offset &&
						collectionCacheItemPtr->count == count &&
						collectionCacheItemPtr->selectionParamsData == data){
				return collectionCacheItemPtr;
			}
		}
	}

	IObjectCollectionUniquePtr subCollectionPtr = m_objectCollectionCompPtr->CreateSubCollection(offset, count, selectionParamsPtr);
	if (!subCollectionPtr.IsValid()){
		return nullptr;
	}

	QWriteLocker locker(&m_lock);

	// Re-check cache after acquiring write lock to avoid duplicates
	for (int index = 0; index < m_cachedCollections.size(); index++){
		const FilteredCollectionPtr& collectionCacheItemPtr = m_cachedCollections.at(index);
		if (		collectionCacheItemPtr != nullptr &&
					collectionCacheItemPtr->offset == offset &&
					collectionCacheItemPtr->count == count &&
					collectionCacheItemPtr->selectionParamsData == data){
			return collectionCacheItemPtr;
		}
	}

	if (m_cachedCollections.size() >= *m_metaInfoCacheLimitAttrPtr){
		m_cachedCollections.removeFirst();
	}

	m_cachedCollections.append(std::make_shared<FilteredCollection>(offset, count, data, std::move(subCollectionPtr)));

	return m_cachedCollections.last();
}


void CCachedObjectCollectionComp::ClearCache()
{
	QWriteLocker locker(&m_lock);

	m_cachedCollections.clear();

	m_cacheItems.clear();
}


void CCachedObjectCollectionComp::RemoveOldestObjectFromCache() const
{
	if (m_cacheItems.isEmpty()){
		return;
	}

	CacheItemMap::iterator oldestIter = m_cacheItems.begin();

	for (CacheItemMap::iterator iter = m_cacheItems.begin(); iter != m_cacheItems.end(); ++iter){
		if (iter.value().timestamp < oldestIter.value().timestamp){
			oldestIter = iter;
		}
	}

	m_cacheItems.erase(oldestIter);
}


} // namespace imtbase


