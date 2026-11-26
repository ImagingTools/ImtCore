// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtbase/CCachedObjectCollectionComp.h>


// Qt includes
#include <QtCore/QUuid>

// ACF includes
#include <istd/TDelPtr.h>
#include <istd/CChangeNotifier.h>
#include <iser/CMemoryWriteArchive.h>
#include <ilog/CMessage.h>


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
		BaseClass2::RegisterModel(m_objectCollectionModelCompPtr.GetPtr(), 0);
	}

	for (int i = 0; i < m_invalidationModels.GetCount(); ++i){
		imod::IModel* modelPtr = m_invalidationModels[i];
		if (modelPtr != nullptr){
			BaseClass2::RegisterModel(modelPtr, 100 + i);
		}
	}
}


void CCachedObjectCollectionComp::OnComponentDestroyed()
{
	BaseClass2::UnregisterAllModels();

	BaseClass::OnComponentDestroyed();
}


// reimplemented (imod::CMultiModelDispatcherBase)

void CCachedObjectCollectionComp::OnModelChanged(int modelId, const istd::IChangeable::ChangeSet& changeSet)
{
	if (modelId == 0 && changeSet.Contains(CF_SOURCE_UNWATCHED)){
		m_isSourceUnwatched = true;
	}
	else if (modelId == 0 && changeSet.Contains(CF_SOURCE_WATCHED)){
		m_isSourceUnwatched = false;
	}

	InvalidateCache();
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
	else{
		QWriteLocker locker(&m_lock);
		ClearFilteredCacheLocked();
		m_cacheItems.remove(retVal);
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
		ClearFilteredCacheLocked();
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


bool CCachedObjectCollectionComp::GetObjectData(const Id& objectId, DataPtr& dataPtr, const iprm::IParamsSet* dataConfigurationPtr) const
{
	if (!m_objectCollectionCompPtr.IsValid()){
		return false;
	}

	quint64 generation;
	bool cacheable;
	{
		QReadLocker locker(&m_lock);
		generation = m_cacheGeneration;
		cacheable = !m_isSourceUnwatched && dataConfigurationPtr == nullptr;

		CacheItemMap::const_iterator it = m_cacheItems.constFind(objectId);
		if (cacheable && it != m_cacheItems.constEnd() && it.value().dataPtr.IsValid()){
			// Callers modify the returned object in place, so the cached instance is never handed out.
			istd::IChangeableUniquePtr clonePtr = it.value().dataPtr->CloneMe();
			if (clonePtr.IsValid()){
				dataPtr.FromUnique(std::move(clonePtr));

				return true;
			}
		}
	}

	DataPtr loaded;
	if (!m_objectCollectionCompPtr->GetObjectData(objectId, loaded, dataConfigurationPtr) || !loaded.IsValid()){
		return false;
	}
	DataPtr cachedDataPtr;
	if (cacheable && *m_objectCacheLimitAttrPtr > 0){
		cachedDataPtr.FromUnique(loaded->CloneMe());
	}

	QWriteLocker locker(&m_lock);
	if (generation == m_cacheGeneration && cachedDataPtr.IsValid() && !m_isSourceUnwatched){
		if (m_cacheItems.size() >= *m_objectCacheLimitAttrPtr){
			RemoveOldestObjectFromCache();
		}
		m_cacheItems.insert(objectId, {cachedDataPtr, QDateTime::currentMSecsSinceEpoch()});
	}
	dataPtr = loaded;

	return true;
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

	bool retVal = m_objectCollectionCompPtr->SetObjectData(objectId, object, mode, operationContextPtr);
	{
		QWriteLocker locker(&m_lock);
		ClearFilteredCacheLocked();
		m_cacheItems.remove(objectId);
	}
	if (!retVal){
		changeNotifier.Abort();
	}

	return retVal;
}


IObjectCollectionUniquePtr CCachedObjectCollectionComp::CreateSubCollection(int offset, int count, const iprm::IParamsSet *selectionParamsPtr) const
{
	if (m_isSourceUnwatched && m_objectCollectionCompPtr.IsValid()){
		return m_objectCollectionCompPtr->CreateSubCollection(offset, count, selectionParamsPtr);
	}

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
	// The iterator can refer to its collection; do not create it from a temporary snapshot.
	FilteredCollectionPtr collectionCacheItemPtr = GetFilteredCollection(offset, count, selectionParamsPtr, false);
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
	if (m_isSourceUnwatched && m_objectCollectionCompPtr.IsValid()){
		return m_objectCollectionCompPtr->GetDataMetaInfo(objectId);
	}

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
	if (m_isSourceUnwatched && m_objectCollectionCompPtr.IsValid()){
		return m_objectCollectionCompPtr->GetElementIds(offset, count, selectionParamsPtr, logPtr);
	}

	FilteredCollectionPtr collectionCacheItemPtr = GetFilteredCollection(offset, count, selectionParamsPtr);
	if (collectionCacheItemPtr != nullptr){
		return collectionCacheItemPtr->cachePtr->GetElementIds(0, -1, nullptr, logPtr);
	}

	// The source was not read: the caller must not take the empty list for an empty selection
	if (logPtr != nullptr){
		logPtr->AddMessage(ilog::IMessageConsumer::MessagePtr(new ilog::CMessage(istd::IInformationProvider::IC_ERROR, 0, QStringLiteral("The source collection could not be read."), QStringLiteral("CCachedObjectCollectionComp"))));
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
	if (m_isSourceUnwatched && m_objectCollectionCompPtr.IsValid()){
		return m_objectCollectionCompPtr->GetElementInfo(elementId, infoType, logPtr);
	}

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
	if (m_isSourceUnwatched && m_objectCollectionCompPtr.IsValid()){
		return m_objectCollectionCompPtr->GetElementMetaInfo(elementId, logPtr);
	}

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

	istd::IChangeable::ChangeSet changeSet(CF_ELEMENT_RENAMED);
	changeSet.SetChangeInfo(CN_ELEMENT_RENAMED, elementId);
	istd::CChangeNotifier changeNotifier(this, &changeSet);

	bool retVal = m_objectCollectionCompPtr->SetElementName(elementId, name);
	{
		QWriteLocker locker(&m_lock);
		ClearFilteredCacheLocked();
	}
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

	istd::IChangeable::ChangeSet changeSet(CF_ELEMENT_DESCRIPTION_CHANGED);
	changeSet.SetChangeInfo(CN_ELEMENT_DESCRIPTION_CHANGED, elementId);
	istd::CChangeNotifier changeNotifier(this, &changeSet);

	bool retVal = m_objectCollectionCompPtr->SetElementDescription(elementId, description, logPtr);
	{
		QWriteLocker locker(&m_lock);
		ClearFilteredCacheLocked();
	}
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

	istd::IChangeable::ChangeSet changeSet(CF_ELEMENT_STATE);
	changeSet.SetChangeInfo(CN_ELEMENT_STATE, elementId);
	istd::CChangeNotifier changeNotifier(this, &changeSet);

	bool retVal = m_objectCollectionCompPtr->SetElementEnabled(elementId, isEnabled);
	{
		QWriteLocker locker(&m_lock);
		ClearFilteredCacheLocked();
	}
	if (!retVal){
		changeNotifier.Abort();
	}

	return retVal;
}


CCachedObjectCollectionComp::FilteredCollectionPtr CCachedObjectCollectionComp::GetFilteredCollection(
			int offset,
			int count,
			const iprm::IParamsSet* selectionParamsPtr,
			bool allowUncachedSnapshot) const
{
	if (!m_objectCollectionCompPtr.IsValid() || m_isSourceUnwatched){
		return nullptr;
	}

	iser::CMemoryWriteArchive archive;
	iprm::IParamsSet* paramsSet = const_cast<iprm::IParamsSet*>(selectionParamsPtr);
	if (paramsSet != nullptr && !paramsSet->Serialize(archive)){
		Q_ASSERT_X(false, __FILE__, "Serialization of the filter parameters was failed");

		return nullptr;
	}

	QByteArray data((char*)archive.GetBuffer(), archive.GetBufferSize());

	quint64 generation;
	{
		QReadLocker locker(&m_lock);
		generation = m_cacheGeneration;
		if (m_isSourceUnwatched){
			return nullptr;
		}

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

	auto item = std::make_shared<FilteredCollection>(offset, count, data, std::move(subCollectionPtr));
	QWriteLocker locker(&m_lock);
	if (generation != m_cacheGeneration || m_isSourceUnwatched || *m_metaInfoCacheLimitAttrPtr <= 0){
		return allowUncachedSnapshot ? item : nullptr;
	}

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

	m_cachedCollections.append(item);

	return m_cachedCollections.last();
}


void CCachedObjectCollectionComp::ClearCache()
{
	QWriteLocker locker(&m_lock);

	ClearFilteredCacheLocked();
	m_cacheItems.clear();
}


void CCachedObjectCollectionComp::ClearFilteredCacheLocked()
{
	++m_cacheGeneration;
	m_cachedCollections.clear();
}


// reimplemented (IObjectCollectionCacheController)

void CCachedObjectCollectionComp::InvalidateCache()
{
	istd::CChangeNotifier changeNotifier(this);

	ClearCache();
}


const IObjectCollection* CCachedObjectCollectionComp::GetSourceCollection() const
{
	return m_objectCollectionCompPtr.IsValid() ? m_objectCollectionCompPtr.GetPtr() : nullptr;
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
