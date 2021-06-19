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


namespace imtbase
{


// public methods

CObjectCollectionBase::CObjectCollectionBase()
	:m_modelUpdateBridge(this)
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


bool CObjectCollectionBase::ExportFile(const imtbase::IObjectCollection& /*collection*/, const QByteArray& objectId, const QString& targetFilePath) const
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


QByteArray CObjectCollectionBase::ImportFile(imtbase::IObjectCollection& /*collection*/, const QByteArray& typeId, const QString& sourceFilePath) const
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


int CObjectCollectionBase::GetOperationFlags(const QByteArray& objectId) const
{
	if (!objectId.isEmpty()){
		for (const ObjectInfo& objectInfo : m_objects){
			if (objectInfo.id == objectId){
				return objectInfo.flags;
			}
		}
	}

	return OF_ALL;
}


bool CObjectCollectionBase::GetDataMetaInfo(const QByteArray& /*objectId*/, MetaInfoPtr& /*metaInfoPtr*/) const
{
	return false;
}


QByteArray CObjectCollectionBase::InsertNewObject(
			const QByteArray& typeId,
			const QString& name,
			const QString& description,
			const istd::IChangeable* defaultValuePtr,
			const QByteArray& proposedObjectId)
{
	ObjectInfo info;

	if (!proposedObjectId.isEmpty()){
		if (GetObjectInfo(proposedObjectId) == nullptr){
			info.id = proposedObjectId;
		}
	}

	info.objectPtr.SetPtr(CreateObjectInstance(typeId), true);
	if (info.objectPtr.IsValid()){
		istd::CChangeNotifier changeNotifier(this);

		if (defaultValuePtr != nullptr){
			if (!info.objectPtr->CopyFrom(*defaultValuePtr)){
				return QByteArray();
			}
		}

		info.description = description;
		info.name = name;
		info.typeId = typeId;
		info.flags = GetItemDefaultFlags();

		if (InsertObjectIntoCollection(info)){
			return info.id;
		}
	}

	return QByteArray();
}


bool CObjectCollectionBase::RemoveObject(const QByteArray& objectId)
{
	for (Objects::iterator iter = m_objects.begin(); iter != m_objects.end(); ++iter){
		if ((*iter).id == objectId){
			imod::IModel* modelPtr = dynamic_cast<imod::IModel*>((*iter).objectPtr.GetPtr());
			if (modelPtr != nullptr){
				modelPtr->DetachObserver(&m_modelUpdateBridge);
			}

			istd::CChangeNotifier changeNotifier(this);

			DestroyObjectInstance((*iter).objectPtr.PopPtr());

			m_objects.erase(iter);

			break;
		}
	}

	return true;
}


const istd::IChangeable* CObjectCollectionBase::GetObjectPtr(const QByteArray& objectId) const
{
	for (const ObjectInfo& objectInfo : m_objects){
		if (objectInfo.id == objectId){
			return objectInfo.objectPtr.GetPtr();
		}
	}

	return nullptr;
}


bool CObjectCollectionBase::GetObjectData(const QByteArray& objectId, DataPtr& dataPtr) const
{
	for (const ObjectInfo& objectInfo : m_objects){
		if ((objectInfo.id == objectId) && objectInfo.objectPtr.IsValid()){
			if (!dataPtr.IsValid()){
				istd::TDelPtr<istd::IChangeable> newInstancePtr(CreateObjectInstance(objectInfo.typeId));
				if (newInstancePtr.IsValid()){
					if (newInstancePtr->CopyFrom(*objectInfo.objectPtr)){
						dataPtr.SetPtr(newInstancePtr.PopPtr());

						return true;
					}
				}
			}
			else{
				return dataPtr->CopyFrom(*objectInfo.objectPtr);
			}
		}
	}

	return false;
}


bool CObjectCollectionBase::SetObjectData(const QByteArray& objectId, const istd::IChangeable& object, CompatibilityMode mode)
{
	for (const ObjectInfo& objectInfo : m_objects){
		if ((objectInfo.id == objectId) && objectInfo.objectPtr.IsValid()){
			return objectInfo.objectPtr->CopyFrom(object, mode);
		}
	}

	return false;
}


void CObjectCollectionBase::SetObjectName(const QByteArray& objectId, const QString& objectName)
{
	for (ObjectInfo& objectInfo : m_objects){
		if (objectInfo.id == objectId){
			istd::CChangeNotifier changeNotifier(this);

			objectInfo.name = objectName;
		}
	}
}


void CObjectCollectionBase::SetObjectDescription(const QByteArray& objectId, const QString& objectDescription)
{
	for (ObjectInfo& objectInfo : m_objects){
		if (objectInfo.id == objectId){
			istd::CChangeNotifier changeNotifier(this);

			objectInfo.description = objectDescription;
		}
	}
}


void CObjectCollectionBase::SetObjectEnabled(const QByteArray& objectId, bool isEnabled)
{
	for (ObjectInfo& objectInfo : m_objects){
		if (objectInfo.id == objectId){
			istd::CChangeNotifier changeNotifier(this);

			objectInfo.isEnabled = isEnabled;
		}
	}
}


bool CObjectCollectionBase::RegisterEventHandler(IObjectCollectionEventHandler* eventHandler)
{
	if (!m_eventHandlerList.contains(eventHandler)){
		m_eventHandlerList.append(eventHandler);
		return true;
	}

	return false;
}


bool CObjectCollectionBase::UnregisterEventHandler(IObjectCollectionEventHandler* eventHandler)
{
	int index = m_eventHandlerList.indexOf(eventHandler, 0);
	if (index >= 0){
		m_eventHandlerList.removeAt(index);
		return true;
	}

	return false;
}


// reimplemented (IObjectCollectionInfo)

bool CObjectCollectionBase::GetCollectionItemMetaInfo(const QByteArray& objectId, idoc::IDocumentMetaInfo& metaInfo) const
{
	for (const ObjectInfo& objectInfo : m_objects){
		if (objectInfo.id == objectId){
			return metaInfo.CopyFrom(objectInfo.metaInfo);
		}
	}
	
	return false;
}


const iprm::IOptionsList* CObjectCollectionBase::GetObjectTypesInfo() const
{
	return nullptr;
}


ICollectionInfo::Id CObjectCollectionBase::GetObjectTypeId(const QByteArray& objectId) const
{
	for (const ObjectInfo& objectInfo : m_objects){
		if (objectInfo.id == objectId){
			return objectInfo.typeId;
		}
	}
	
	return QByteArray();
}


// reimplemented (ICollectionInfo)

ICollectionInfo::Ids CObjectCollectionBase::GetElementIds() const
{
	Ids retVal;

	for (const ObjectInfo& objectInfo : m_objects){
		retVal.push_back(objectInfo.id);
	}

	return retVal;
}


QVariant CObjectCollectionBase::GetElementInfo(const QByteArray& elementId, int infoType) const
{
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

		case EIT_TYPE_ID:
			return m_objects[objectIndex].typeId;

		case EIT_ENABLED:
			return m_objects[objectIndex].isEnabled;
		}
	}

	return QVariant();
}


bool CObjectCollectionBase::Serialize(iser::IArchive& archive)
{
	int objectCount = m_objects.count();

	istd::CChangeNotifier changeNotifier(archive.IsStoring() ? nullptr : this);

	if (!archive.IsStoring()){
		objectCount = 0;

		RemoveAllObjects();
	}

	static iser::CArchiveTag objectListTag("ObjectsList", "List of objects", iser::CArchiveTag::TT_MULTIPLE);
	static iser::CArchiveTag objectTag("Object", "Object item", iser::CArchiveTag::TT_GROUP, &objectListTag);

	bool retVal = true;

	retVal = retVal && archive.BeginMultiTag(objectListTag, objectTag, objectCount);

	for (int i = 0; i < objectCount; i++){
		retVal = retVal && archive.BeginTag(objectTag);

		ObjectInfo elementInfo;
		if (archive.IsStoring()){
			elementInfo = m_objects[i];
		}

		static iser::CArchiveTag objectIdTag("ID", "Object ID", iser::CArchiveTag::TT_LEAF, &objectTag);
		retVal = retVal && archive.BeginTag(objectIdTag);
		retVal = retVal && archive.Process(elementInfo.id);
		retVal = retVal && archive.EndTag(objectIdTag);

		static iser::CArchiveTag descriptionTag("Description", "Object description", iser::CArchiveTag::TT_LEAF, &objectTag);
		retVal = retVal && archive.BeginTag(descriptionTag);
		retVal = retVal && archive.Process(elementInfo.description);
		retVal = retVal && archive.EndTag(descriptionTag);

		static iser::CArchiveTag typeIdTag("TypeId", "Object type-ID", iser::CArchiveTag::TT_LEAF, &objectTag);
		retVal = retVal && archive.BeginTag(typeIdTag);
		retVal = retVal && archive.Process(elementInfo.typeId);
		retVal = retVal && archive.EndTag(typeIdTag);

		static iser::CArchiveTag objectNameTag("Name", "Object name", iser::CArchiveTag::TT_LEAF, &objectTag);
		retVal = retVal && archive.BeginTag(objectNameTag);
		retVal = retVal && archive.Process(elementInfo.name);
		retVal = retVal && archive.EndTag(objectNameTag);

		quint32 imtCoreVersion;
		bool imtCoreVersionExists = archive.GetVersionInfo().GetVersionNumber(imtcore::VI_IMTCORE, imtCoreVersion);
		if (imtCoreVersionExists && imtCoreVersion >= 3401){
			static iser::CArchiveTag collectionMetaInfoTag("CollectionItemMeta", "Collection item meta information", iser::CArchiveTag::TT_GROUP, &objectTag);
			retVal = retVal && archive.BeginTag(collectionMetaInfoTag);
			retVal = retVal && elementInfo.metaInfo.Serialize(archive);
			retVal = retVal && archive.EndTag(collectionMetaInfoTag);
		}

		static iser::CArchiveTag objectDataTag("Data", "Object data", iser::CArchiveTag::TT_GROUP, &objectTag);
		retVal = retVal && archive.BeginTag(objectDataTag);

		istd::IChangeable* objectPtr = nullptr;
		if (!archive.IsStoring()){
			elementInfo.objectPtr.SetPtr(CreateObjectInstance(elementInfo.typeId), true);

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

	return retVal;
}


// reimplemented (istd::IChangeable)

int CObjectCollectionBase::GetSupportedOperations() const
{
	return SO_COPY | SO_RESET;
}


bool CObjectCollectionBase::CopyFrom(const IChangeable& object, CompatibilityMode mode)
{
	const CObjectCollectionBase* sourcePtr = dynamic_cast<const CObjectCollectionBase*>(&object);
	if (sourcePtr != nullptr){
		istd::CChangeNotifier changeNotifier(this);

		ICollectionInfo::Ids sourceElementIds = sourcePtr->GetElementIds();

		// Remove non-existing objects from the target collection:
		QMutableVectorIterator<ObjectInfo> targetObjectsIter(m_objects);
		while (targetObjectsIter.hasNext()){
			const ObjectInfo& objectInfo = targetObjectsIter.next();

			if (!sourceElementIds.contains(objectInfo.id)){
				targetObjectsIter.remove();
			}
		}

		for (const ObjectInfo& sourceObjectInfo : sourcePtr->m_objects){
			ObjectInfo* targetObjectInfoPtr = GetObjectInfo(sourceObjectInfo.id);

			if (targetObjectInfoPtr == nullptr){
				QByteArray newId = InsertNewObject(sourceObjectInfo.typeId, sourceObjectInfo.name, sourceObjectInfo.description, sourceObjectInfo.objectPtr.GetPtr());
				if (newId.isEmpty()){
					return false;
				}
			}
			else{
				const istd::IChangeable* dataPtr = sourcePtr->GetObjectPtr(sourceObjectInfo.id);
				if (dataPtr != nullptr){
					if (!targetObjectInfoPtr->objectPtr->CopyFrom(*dataPtr, mode)){
						return false;
					}
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
							targetInfoPtr->objectPtr.SetPtr(CreateObjectInstance(typeId));
						}

						if (targetInfoPtr->objectPtr.IsValid()){
							if (!targetInfoPtr->objectPtr->CopyFrom(*sourceObjectPtr)){
								return false;
							}
						}
					}
					else{
						if (targetInfoPtr->objectPtr.IsToRelase()){
							targetInfoPtr->objectPtr.Reset();
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

bool CObjectCollectionBase::InsertObjectIntoCollection(const ObjectInfo& info)
{
	imod::IModel* modelPtr = dynamic_cast<imod::IModel*>(info.objectPtr.GetPtr());
	if (modelPtr != nullptr){
		if (!modelPtr->AttachObserver(&m_modelUpdateBridge)){
			qDebug("CObjectCollectionBase::InsertObjectIntoCollection: Attaching object's model to the internal observer failed");

			return false;
		}
	}

	Q_ASSERT(info.objectPtr.IsValid());

	m_objects.push_back(info);

	return true;
}


void CObjectCollectionBase::DestroyObjectInstance(istd::IChangeable* objectPtr) const
{
	if (objectPtr != nullptr){
		delete objectPtr;
	}
}


int CObjectCollectionBase::GetItemDefaultFlags() const
{
	return OF_ALL;
}


CObjectCollectionBase::ObjectInfo* CObjectCollectionBase::GetObjectInfo(const QByteArray& id) const
{
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

	for (ObjectInfo& info : m_objects){
		if (info.objectPtr.IsToRelase()){
			DestroyObjectInstance(info.objectPtr.PopPtr());
		}
	}

	m_objects.clear();
}


} // namespace imtbase


