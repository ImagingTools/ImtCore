#include <imtbase/CObjectCollectionBase.h>


// Qt includes
#include <QtCore/QUuid>
#include <QtCore/QFileInfo>

// ACF includes
#include <istd/TDelPtr.h>
#include <istd/CChangeNotifier.h>
#include <iser/IArchive.h>
#include <iser/CArchiveTag.h>

// ImtCore includes
#include <imtcore/Version.h>
#include <imtbase/MetaTypes.h>
#include <imtbase/CObjectCollection.h>


namespace imtbase
{


// public methods

CObjectCollectionBase::CObjectCollectionBase()
	:m_modelUpdateBridge(this),
	m_lock(QReadWriteLock::Recursive)
{

}


CObjectCollectionBase::~CObjectCollectionBase()
{	
	RemoveAllObjects();
}


// reimplemented (ICollectionDataController)

const ifile::IFilePersistence* CObjectCollectionBase::GetPersistenceForObjectType(const QByteArray& /*typeId*/) const
{
	return nullptr;
}


bool CObjectCollectionBase::ExportFile(const imtbase::IObjectCollection& /*collection*/, const Id& objectId, const QString& targetFilePath) const
{
	QByteArray objectTypeId = GetObjectTypeId(objectId);
	if (!objectTypeId.isEmpty()){
		const ifile::IFilePersistence* persistencePtr = GetPersistenceForObjectType(objectTypeId);
		if (persistencePtr != nullptr){
			DataPtr objectDataPtr;
			if (GetObjectData(objectId, objectDataPtr)){
				return persistencePtr->SaveToFile(*objectDataPtr, targetFilePath) == ifile::IFilePersistence::OS_OK;
			}
		}
	}

	return false;
}


ICollectionInfo::Id CObjectCollectionBase::ImportFile(imtbase::IObjectCollection& /*collection*/, const QByteArray& typeId, const QString& sourceFilePath) const
{
	if (!typeId.isEmpty()){
		const ifile::IFilePersistence* persistencePtr = GetPersistenceForObjectType(typeId);
		if (persistencePtr != nullptr){
			DataPtr objectDataPtr(CreateObjectInstance(typeId));
			if (objectDataPtr.IsValid()){
				if (persistencePtr->LoadFromFile(*objectDataPtr, sourceFilePath) == ifile::IFilePersistence::OS_OK){
					QFileInfo fileInfo(sourceFilePath);

					QString newObjectName = fileInfo.baseName();

					return (const_cast<CObjectCollectionBase*>(this))->InsertNewObject(typeId, newObjectName, QString("Imported from %1").arg(sourceFilePath), objectDataPtr.GetPtr());
				}
			}
		}
	}

	return QByteArray();
}


// reimplemented (IObjectCollection)

const imtbase::IRevisionController* CObjectCollectionBase::GetRevisionController() const
{
	return nullptr;
}


const imtbase::ICollectionDataController* CObjectCollectionBase::GetDataController() const
{
	return this;
}


int CObjectCollectionBase::GetOperationFlags(const Id& objectId) const
{
	QReadLocker locker(&m_lock);

	if (!objectId.isEmpty()){
		for (const ObjectInfo& objectInfo : m_objects){
			if (objectInfo.id == objectId){
				return objectInfo.flags;
			}
		}
	}

	return OF_ALL;
}


ICollectionInfo::Id CObjectCollectionBase::InsertNewObject(
			const QByteArray& typeId,
			const QString& name,
			const QString& description,
			DataPtr defaultValuePtr,
			const Id& proposedElementId,
			const idoc::IDocumentMetaInfo* dataMetaInfoPtr,
			const idoc::IDocumentMetaInfo* elementMetaInfoPtr,
			const IOperationContext* /*operationContextPtr*/)
{
	ObjectInfo info;

	if (!proposedElementId.isEmpty()){
		if (GetObjectInfo(proposedElementId) == nullptr){
			info.id = proposedElementId;
		}
	}

	info.objectPtr = CreateObjectInstance(typeId);
	if (info.objectPtr.IsValid()){
		if (defaultValuePtr.IsValid()){
			if (!info.objectPtr->CopyFrom(*defaultValuePtr)){
				return QByteArray();
			}
		}

		info.description = description;
		info.name = name;
		info.typeId = typeId;
		info.flags = GetItemDefaultFlags();

		if (elementMetaInfoPtr != nullptr){
			info.collectionItemMetaInfo.CopyFrom(*elementMetaInfoPtr);
		}

		if (dataMetaInfoPtr != nullptr){
			info.contentsMetaInfoPtr.SetCastedOrRemove(dataMetaInfoPtr->CloneMe());
		}

		if (InsertObjectIntoCollection(info)){
			istd::IChangeable::ChangeSet changeSet(CF_ADDED);
			changeSet.SetChangeInfo(CN_ELEMENT_INSERTED, info.id);
			istd::CChangeNotifier changeNotifier(this, &changeSet);

			return info.id;
		}
	}

	return QByteArray();
}


bool CObjectCollectionBase::RemoveElement(const Id& elementId, const IOperationContext* /*operationContextPtr*/)
{
	QReadLocker locker(&m_lock);
	for (Objects::iterator iter = m_objects.begin(); iter != m_objects.end(); ++iter){
		if ((*iter).id == elementId){
			istd::IChangeable::ChangeSet changeSet(CF_REMOVED);
			changeSet.SetChangeInfo(CN_ELEMENT_REMOVED, elementId);

			locker.unlock();

			istd::CChangeNotifier changeNotifier(this, &changeSet);

			m_lock.lockForWrite();

			imod::IModel* modelPtr = dynamic_cast<imod::IModel*>((*iter).objectPtr.GetPtr());
			if (modelPtr != nullptr){
				modelPtr->DetachObserver(&m_modelUpdateBridge);
			}

			m_objects.erase(iter);

			m_lock.unlock();

			break;
		}
	}

	return true;
}


const istd::IChangeable* CObjectCollectionBase::GetObjectPtr(const Id& objectId) const
{
	QReadLocker locker(&m_lock);

	for (const ObjectInfo& objectInfo : m_objects){
		if (objectInfo.id == objectId){
			return objectInfo.objectPtr.GetPtr();
		}
	}

	return nullptr;
}


bool CObjectCollectionBase::GetObjectData(const Id& objectId, DataPtr& dataPtr) const
{
	QReadLocker locker(&m_lock);

	for (const ObjectInfo& objectInfo : m_objects){
		if ((objectInfo.id == objectId) && objectInfo.objectPtr.IsValid()){
			if (!dataPtr.IsValid()){
				DataPtr newInstancePtr = CreateObjectInstance(objectInfo.typeId);
				if (newInstancePtr.IsValid()){
					if (newInstancePtr->CopyFrom(*objectInfo.objectPtr, objectInfo.copyMode)){
						dataPtr = newInstancePtr;

						return true;
					}
				}
			}
			else{
				return dataPtr->CopyFrom(*objectInfo.objectPtr, objectInfo.copyMode);
			}
		}
	}

	return false;
}


bool CObjectCollectionBase::SetObjectData(
			const Id& objectId,
			const istd::IChangeable& object,
			CompatibilityMode mode,
			const IOperationContext* /*operationContextPtr*/)
{
	QReadLocker locker(&m_lock);

	for (ObjectInfo& objectInfo : m_objects){
		if ((objectInfo.id == objectId) && objectInfo.objectPtr.IsValid()){
			istd::IChangeable::ChangeSet changeSet(CF_UPDATED);
			changeSet.SetChangeInfo(CN_ELEMENT_UPDATED, objectId);

			locker.unlock();

			istd::CChangeNotifier changeNotifier(this, &changeSet);

			m_lock.lockForWrite();

			objectInfo.copyMode = mode;
			bool retVal = objectInfo.objectPtr->CopyFrom(object, mode);

			m_lock.unlock();

			return retVal;
		}
	}

	return false;
}


IObjectCollection* CObjectCollectionBase::CreateSubCollection(
			int offset,
			int count,
			const iprm::IParamsSet* /*selectionParamsPtr*/) const
{
	imtbase::IObjectCollection* collectionPtr = new imtbase::CObjectCollection;

	Q_ASSERT(offset >= 0);

	QWriteLocker locker(&m_lock);

	int objectsCount = count >= 0 ? qMin(count, m_objects.count()) : m_objects.count();

	for (int i = offset; i < objectsCount; i++){
		collectionPtr->InsertNewObject(m_objects[i].typeId, m_objects[i].name, m_objects[i].description, m_objects[i].objectPtr, m_objects[i].id);
	}

	return collectionPtr;
}


imtbase::IObjectCollectionIterator* CObjectCollectionBase::CreateObjectCollectionIterator(
			int /*offset*/,
			int /*count*/,
			const iprm::IParamsSet* /*selectionParamsPtr*/) const
{
	return nullptr;
}


// reimplemented (IObjectCollectionInfo)

const iprm::IOptionsList* CObjectCollectionBase::GetObjectTypesInfo() const
{
	return nullptr;
}


ICollectionInfo::Id CObjectCollectionBase::GetObjectTypeId(const Id& objectId) const
{
	QReadLocker locker(&m_lock);

	for (const ObjectInfo& objectInfo : m_objects){
		if (objectInfo.id == objectId){
			return objectInfo.typeId;
		}
	}
	
	return QByteArray();
}


idoc::MetaInfoPtr CObjectCollectionBase::GetDataMetaInfo(const Id& objectId) const
{
	idoc::MetaInfoPtr metaInfoPtr;

	QReadLocker locker(&m_lock);

	for (const ObjectInfo& objectInfo : m_objects){
		if (objectInfo.id == objectId){
			if (objectInfo.contentsMetaInfoPtr.IsValid()){
				metaInfoPtr.SetCastedOrRemove(objectInfo.contentsMetaInfoPtr->CloneMe());
			}
		}
	}

	return metaInfoPtr;
}


// reimplemented (ICollectionInfo)

int CObjectCollectionBase::GetElementsCount(
			const iprm::IParamsSet* /*selectionParamPtr*/,
			ilog::IMessageConsumer* /*logPtr*/) const
{
	QReadLocker locker(&m_lock);

	return m_objects.count();
}


ICollectionInfo::Ids CObjectCollectionBase::GetElementIds(
			int offset,
			int count,
			const iprm::IParamsSet* /*selectionParamsPtr*/,
			ilog::IMessageConsumer* /*logPtr*/) const
{
	Ids retVal;

	Q_ASSERT(offset >= 0);

	QReadLocker locker(&m_lock);

	int objectsCount = count >= 0 ? qMin(count, m_objects.count()) : m_objects.count();

	for (int i = offset; i < objectsCount; i++){
		retVal.push_back(m_objects[i].id);
	}

	return retVal;
}


bool CObjectCollectionBase::GetSubsetInfo(
			ICollectionInfo& /*subsetInfo*/,
			int /*offset*/,
			int /*count*/,
			const iprm::IParamsSet* /*selectionParamsPtr*/,
			ilog::IMessageConsumer* /*logPtr*/) const
{
	return false;
}


QVariant CObjectCollectionBase::GetElementInfo(const Id& elementId, int infoType, ilog::IMessageConsumer* /*logPtr*/) const
{
	QReadLocker locker(&m_lock);

	int objectIndex = -1;
	for (int i = 0; i < m_objects.count(); ++i){
		if (m_objects[i].id == elementId){
			 objectIndex = i;
			 break;
		}
	}

	if (objectIndex >= 0){
		switch (infoType){
		case EIT_DESCRIPTION:
			return m_objects[objectIndex].description;

		case EIT_NAME:
			return m_objects[objectIndex].name;

		case EIT_ENABLED:
			return m_objects[objectIndex].isEnabled;
		}
	}

	return QVariant();
}


idoc::MetaInfoPtr CObjectCollectionBase::GetElementMetaInfo(const Id& elementId, ilog::IMessageConsumer* /*logPtr*/) const
{
	QReadLocker locker(&m_lock);

	idoc::MetaInfoPtr metaInfoPtr;

	for (const ObjectInfo& objectInfo : m_objects){
		if (objectInfo.id == elementId){
			metaInfoPtr.SetCastedOrRemove(objectInfo.collectionItemMetaInfo.CloneMe());
		}
	}

	return metaInfoPtr;
}


bool CObjectCollectionBase::SetElementName(const Id& elementId, const QString& objectName, ilog::IMessageConsumer* /*logPtr*/)
{
	QReadLocker locker(&m_lock);

	for (ObjectInfo& objectInfo : m_objects){
		if (objectInfo.id == elementId){
			if (objectInfo.name != objectName){
				istd::IChangeable::ChangeSet changeSet(CF_UPDATED);
				changeSet.SetChangeInfo(CN_ELEMENT_UPDATED, elementId);

				locker.unlock();

				istd::CChangeNotifier changeNotifier(this, &changeSet);

				m_lock.lockForWrite();

				objectInfo.name = objectName;

				m_lock.unlock();
			}

			return true;
		}
	}

	return false;
}


bool CObjectCollectionBase::SetElementDescription(const Id& elementId, const QString& objectDescription, ilog::IMessageConsumer* /*logPtr*/)
{
	QReadLocker locker(&m_lock);

	for (ObjectInfo& objectInfo : m_objects){
		if (objectInfo.id == elementId){
			if (objectInfo.description != objectDescription){
				istd::IChangeable::ChangeSet changeSet(CF_UPDATED);
				changeSet.SetChangeInfo(CN_ELEMENT_UPDATED, elementId);

				locker.unlock();

				istd::CChangeNotifier changeNotifier(this, &changeSet);

				m_lock.lockForWrite();

				objectInfo.description = objectDescription;

				m_lock.unlock();
			}

			return true;
		}
	}

	return false;
}


bool CObjectCollectionBase::SetElementEnabled(const Id& elementId, bool isEnabled, ilog::IMessageConsumer* /*logPtr*/)
{
	QReadLocker locker(&m_lock);

	for (ObjectInfo& objectInfo : m_objects){
		if (objectInfo.id == elementId){
			if (objectInfo.isEnabled != isEnabled){
				istd::IChangeable::ChangeSet changeSet(CF_UPDATED);
				changeSet.SetChangeInfo(CN_ELEMENT_UPDATED, elementId);

				locker.unlock();

				istd::CChangeNotifier changeNotifier(this, &changeSet);

				m_lock.lockForWrite();

				objectInfo.isEnabled = isEnabled;

				m_lock.unlock();
			}

			return true;
		}
	}

	return false;
}


// (reimplemented from ISerializable)

bool CObjectCollectionBase::Serialize(iser::IArchive& archive)
{
	if (archive.IsStoring()){
		m_lock.lockForRead();
	}
	else{
		m_lock.lockForWrite();
	}
	int objectCount = m_objects.count();

	istd::CChangeNotifier changeNotifier(archive.IsStoring() ? nullptr : this);

	if (!archive.IsStoring()){
		objectCount = 0;

		RemoveAllObjects();
	}

	iser::CArchiveTag objectListTag("ObjectsList", "List of objects", iser::CArchiveTag::TT_MULTIPLE);
	iser::CArchiveTag objectTag("Object", "Object item", iser::CArchiveTag::TT_GROUP, &objectListTag);

	bool retVal = true;

	retVal = retVal && archive.BeginMultiTag(objectListTag, objectTag, objectCount);

	for (int i = 0; i < objectCount; i++){
		retVal = retVal && archive.BeginTag(objectTag);

		ObjectInfo elementInfo;
		if (archive.IsStoring()){
			elementInfo = m_objects[i];
		}

		iser::CArchiveTag objectIdTag("ID", "Object ID", iser::CArchiveTag::TT_LEAF, &objectTag);
		retVal = retVal && archive.BeginTag(objectIdTag);
		retVal = retVal && archive.Process(elementInfo.id);
		retVal = retVal && archive.EndTag(objectIdTag);

		iser::CArchiveTag descriptionTag("Description", "Object description", iser::CArchiveTag::TT_LEAF, &objectTag);
		retVal = retVal && archive.BeginTag(descriptionTag);
		retVal = retVal && archive.Process(elementInfo.description);
		retVal = retVal && archive.EndTag(descriptionTag);

		iser::CArchiveTag typeIdTag("TypeId", "Object type-ID", iser::CArchiveTag::TT_LEAF, &objectTag);
		retVal = retVal && archive.BeginTag(typeIdTag);
		retVal = retVal && archive.Process(elementInfo.typeId);
		retVal = retVal && archive.EndTag(typeIdTag);

		iser::CArchiveTag objectNameTag("Name", "Object name", iser::CArchiveTag::TT_LEAF, &objectTag);
		retVal = retVal && archive.BeginTag(objectNameTag);
		retVal = retVal && archive.Process(elementInfo.name);
		retVal = retVal && archive.EndTag(objectNameTag);

		quint32 imtCoreVersion;
		bool imtCoreVersionExists = archive.GetVersionInfo().GetVersionNumber(imtcore::VI_IMTCORE, imtCoreVersion);
		if (imtCoreVersionExists && imtCoreVersion >= 3401){
			iser::CArchiveTag collectionMetaInfoTag("CollectionItemMeta", "Collection item meta information", iser::CArchiveTag::TT_GROUP, &objectTag);
			retVal = retVal && archive.BeginTag(collectionMetaInfoTag);
			retVal = retVal && elementInfo.collectionItemMetaInfo.Serialize(archive);
			retVal = retVal && archive.EndTag(collectionMetaInfoTag);
		}

		iser::CArchiveTag objectDataTag("Data", "Object data", iser::CArchiveTag::TT_GROUP, &objectTag);
		retVal = retVal && archive.BeginTag(objectDataTag);

		istd::IChangeable* objectPtr = nullptr;
		if (!archive.IsStoring()){
			elementInfo.objectPtr = CreateObjectInstance(elementInfo.typeId);

			objectPtr = elementInfo.objectPtr.GetPtr();
		}
		else{
			objectPtr = m_objects[i].objectPtr.GetPtr();
		}

		iser::ISerializable* serializablePtr = dynamic_cast<iser::ISerializable*>(objectPtr);
		if (serializablePtr != nullptr){
			retVal = retVal && serializablePtr->Serialize(archive);
		}

		retVal = retVal && archive.EndTag(objectDataTag);

		if (retVal && !archive.IsStoring()){
			InsertObjectIntoCollection(elementInfo);
		}

		retVal = retVal && archive.EndTag(objectTag);
	}

	retVal = retVal && archive.EndTag(objectListTag);

	m_lock.unlock();
	return retVal;
}


// reimplemented (istd::IChangeable)

int CObjectCollectionBase::GetSupportedOperations() const
{
	return SO_COPY | SO_RESET;
}


bool CObjectCollectionBase::CopyFrom(const IChangeable& object, CompatibilityMode /*mode*/)
{
	const CObjectCollectionBase* sourcePtr = dynamic_cast<const CObjectCollectionBase*>(&object);
	if (sourcePtr != nullptr){
		istd::CChangeNotifier changeNotifier(this);

		ICollectionInfo::Ids sourceElementIds = sourcePtr->GetElementIds();

		QWriteLocker targetLock(&m_lock);
		// Remove non-existing objects from the target collection:
		QMutableVectorIterator<ObjectInfo> targetObjectsIter(m_objects);
		while (targetObjectsIter.hasNext()){
			const ObjectInfo& objectInfo = targetObjectsIter.next();

			if (!sourceElementIds.contains(objectInfo.id)){
				targetObjectsIter.remove();
			}
		}

		targetLock.unlock();

		QReadLocker sourceLock(&sourcePtr->m_lock);
		for (const ObjectInfo& sourceObjectInfo : sourcePtr->m_objects){
			sourceLock.unlock();
			ObjectInfo* targetObjectInfoPtr = GetObjectInfo(sourceObjectInfo.id);
			sourceLock.relock();

			if (targetObjectInfoPtr == nullptr){
				QByteArray newId = InsertNewObject(sourceObjectInfo.typeId, sourceObjectInfo.name, sourceObjectInfo.description, sourceObjectInfo.objectPtr.GetPtr());
				if (newId.isEmpty()){
					return false;
				}
			}
			else{
				sourceLock.unlock();
				const istd::IChangeable* dataPtr = sourcePtr->GetObjectPtr(sourceObjectInfo.id);
				sourceLock.relock();
				if (dataPtr != nullptr){
					targetLock.relock();
					if (targetObjectInfoPtr->objectPtr->CopyFrom(*dataPtr, sourceObjectInfo.copyMode)){
						targetObjectInfoPtr->copyMode = sourceObjectInfo.copyMode;
					}
					else{
						return false;
					}
					targetLock.unlock();
				}
				else{
					return false;
				}
			}
		}

		return true;
	}
	else{
		const IObjectCollection* sourceCollectionPtr = dynamic_cast<const IObjectCollection*>(&object);
		if (sourceCollectionPtr != nullptr){
			istd::CChangeNotifier changeNotifier(this);

			Ids sourceElementIds = sourceCollectionPtr->GetElementIds();
			Ids targetIds = GetElementIds();

			for (const QByteArray& elementId : sourceElementIds){
				QString name = sourceCollectionPtr->GetElementInfo(elementId, EIT_NAME).toString();
				QString description = sourceCollectionPtr->GetElementInfo(elementId, EIT_DESCRIPTION).toString();
				QByteArray typeId = sourceCollectionPtr->GetObjectTypeId(elementId);
				const istd::IChangeable* sourceObjectPtr = sourceCollectionPtr->GetObjectPtr(elementId);

				if (!targetIds.contains(elementId)){
					QByteArray newId = InsertNewObject(typeId, name, description, sourceObjectPtr);
					if (newId.isEmpty()){
						return false;
					}
				}
				else{
					ObjectInfo* targetInfoPtr = GetObjectInfo(elementId);
					Q_ASSERT(targetInfoPtr != nullptr);

					if (sourceObjectPtr != nullptr){
						if (!targetInfoPtr->objectPtr.IsValid()){
							targetInfoPtr->objectPtr = CreateObjectInstance(typeId);
						}

						if (targetInfoPtr->objectPtr.IsValid()){
							if (!targetInfoPtr->objectPtr->CopyFrom(*sourceObjectPtr)){
								return false;
							}
						}
					}
				}
			}

			return true;
		}
	}
	
	return false;
}


bool CObjectCollectionBase::ResetData(CompatibilityMode /*mode*/)
{
	RemoveAllObjects();

	return true;
}


// protected methods

bool CObjectCollectionBase::InsertObjectIntoCollection(ObjectInfo info)
{
	imod::IModel* modelPtr = dynamic_cast<imod::IModel*>(info.objectPtr.GetPtr());
	if (modelPtr != nullptr){
		if (!modelPtr->AttachObserver(&m_modelUpdateBridge)){
			qDebug("CObjectCollectionBase::InsertObjectIntoCollection: Attaching object's model to the internal observer failed");

			return false;
		}
	}

	Q_ASSERT(info.objectPtr.IsValid());

	QWriteLocker locker(&m_lock);

	m_objects.push_back(info);

	return true;
}


int CObjectCollectionBase::GetItemDefaultFlags() const
{
	return OF_ALL & ~OF_SUPPORT_PAGINATION;
}


CObjectCollectionBase::ObjectInfo* CObjectCollectionBase::GetObjectInfo(const QByteArray& id) const
{
	QReadLocker locker(&m_lock);

	for (int i = 0; i < m_objects.count(); ++i){
		if (m_objects[i].id == id){
			 return const_cast<CObjectCollectionBase::ObjectInfo*>(&m_objects[i]);
		}
	}

	return nullptr;
}


void CObjectCollectionBase::RemoveAllObjects()
{
	m_modelUpdateBridge.EnsureModelsDetached();

	istd::CChangeNotifier changeNotifier(this);

	QWriteLocker locker(&m_lock);

	m_objects.clear();

	locker.unlock();
}


} // namespace imtbase


