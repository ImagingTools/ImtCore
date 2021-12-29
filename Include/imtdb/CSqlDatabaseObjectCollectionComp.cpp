#include <imtdb/CSqlDatabaseObjectCollectionComp.h>


// Qt includes
#include <QtSql/QSqlQuery>

// ACF includes
#include <istd/CChangeNotifier.h>
#include <istd/CChangeGroup.h>
#include <idoc/CStandardDocumentMetaInfo.h>

// ImtCore includes
#include <imtdb/CDatabaseEngineComp.h>


namespace imtdb
{


// public methods

CSqlDatabaseObjectCollectionComp::CSqlDatabaseObjectCollectionComp()
	:m_objectInfoMapMutex(QReadWriteLock::Recursive),
	m_filterParamsObserver(*this)
{
}


// reimplemented (ICollectionInfo)

imtbase::ICollectionInfo::Ids CSqlDatabaseObjectCollectionComp::GetElementIds() const
{
	QReadLocker readLock(&m_objectInfoMapMutex);

	return m_objectInfoMap.keys().toVector();
}


QVariant CSqlDatabaseObjectCollectionComp::GetElementInfo(const QByteArray& elementId, int infoType) const
{
	QReadLocker readLock(&m_objectInfoMapMutex);

	if (m_objectInfoMap.contains(elementId)){
		const ObjectInfo& item = m_objectInfoMap[elementId];

		switch (infoType){
		case EIT_NAME:
			return item.collectionMetaInfoPtr.IsValid() ? item.collectionMetaInfoPtr->GetMetaInfo(idoc::IDocumentMetaInfo::MIT_TITLE).toString() : QString();

		case EIT_DESCRIPTION:
			return item.collectionMetaInfoPtr.IsValid() ? item.collectionMetaInfoPtr->GetMetaInfo(idoc::IDocumentMetaInfo::MIT_DESCRIPTION).toString() : QString();

		case EIT_TYPE_ID:
			return item.typeId;
			break;
		}
	}

	return QVariant();
}


// reimplemented (IObjectCollectionInfo)

bool CSqlDatabaseObjectCollectionComp::GetCollectionItemMetaInfo(const QByteArray& objectId, idoc::IDocumentMetaInfo& metaInfo) const
{
	QReadLocker readLock(&m_objectInfoMapMutex);

	if (m_objectInfoMap.contains(objectId)){
		imtbase::IMetaInfoCreator::MetaInfoPtr objectMetaInfoPtr = m_objectInfoMap[objectId].collectionMetaInfoPtr;
		if (objectMetaInfoPtr.IsValid()){
			return metaInfo.CopyFrom(*objectMetaInfoPtr);
		}
	}

	return false;
}


const iprm::IOptionsList* CSqlDatabaseObjectCollectionComp::GetObjectTypesInfo() const
{
	return &m_typesInfo;
}


imtbase::ICollectionInfo::Id CSqlDatabaseObjectCollectionComp::GetObjectTypeId(const QByteArray& objectId) const
{
	QReadLocker readLock(&m_objectInfoMapMutex);

	if (m_objectInfoMap.contains(objectId)){
		return m_objectInfoMap[objectId].typeId;
	}

	return Id();
}


// reimplemented (imtbase::IObjectCollection)

QByteArray CSqlDatabaseObjectCollectionComp::InsertNewObject(
			const QByteArray& typeId,
			const QString& name,
			const QString& description,
			const istd::IChangeable* defaultValuePtr,
			const QByteArray& proposedObjectId,
			const idoc::IDocumentMetaInfo* dataMetaInfoPtr,
			const idoc::IDocumentMetaInfo* collectionItemMetaInfoPtr)
{
	if (!m_objectDelegateCompPtr.IsValid()){
		return nullptr;
	}

	QByteArray objectId = proposedObjectId;
	if (objectId.isEmpty()){
		objectId = QUuid::createUuid().toByteArray(QUuid::WithoutBraces);
	}

	QByteArray query = m_objectDelegateCompPtr->CreateNewObjectQuery(typeId, objectId, name, description, defaultValuePtr);
	if (query.isEmpty()){
		SendErrorMessage(0, "Database query could not be created", "Database collection");

		return nullptr;
	}

	if (ExecuteTransaction(query)){
		istd::CChangeNotifier changeNotifier(this);

		idoc::CStandardDocumentMetaInfo collectionMetaInfo;

		if (collectionItemMetaInfoPtr == nullptr){
			collectionMetaInfo.SetMetaInfo(IObjectCollection::MIT_INSERTION_TIME, QDateTime::currentDateTime());
			collectionMetaInfo.SetMetaInfo(idoc::IDocumentMetaInfo::MIT_MODIFICATION_TIME, QDateTime::currentDateTime());
			collectionMetaInfo.SetMetaInfo(imtbase::IObjectCollection::MIT_LAST_OPERATION_TIME, QDateTime::currentDateTime());
	
			collectionItemMetaInfoPtr = &collectionMetaInfo;
		}

		imtbase::IMetaInfoCreator::MetaInfoPtr metaInfoPtr;

		if (dataMetaInfoPtr == nullptr){
			if (m_metaInfoCreatorCompPtr.IsValid() && (defaultValuePtr != nullptr)){
				if (m_metaInfoCreatorCompPtr->CreateMetaInfo(defaultValuePtr, *m_typeIdAttrPtr, metaInfoPtr)){
					dataMetaInfoPtr = metaInfoPtr.GetPtr();
				}
			}
		}

		ObjectInfo objectInfo;

		objectInfo.typeId = typeId;
		objectInfo.collectionMetaInfoPtr.SetCastedOrRemove(collectionItemMetaInfoPtr->CloneMe());
		objectInfo.collectionMetaInfoPtr->SetMetaInfo(idoc::IDocumentMetaInfo::MIT_TITLE, name);
		objectInfo.collectionMetaInfoPtr->SetMetaInfo(idoc::IDocumentMetaInfo::MIT_DESCRIPTION, description);

		if (dataMetaInfoPtr != nullptr){
			objectInfo.metaInfoPtr.SetCastedOrRemove(dataMetaInfoPtr->CloneMe());
		}

		{
			QWriteLocker writeLock(&m_objectInfoMapMutex);

			m_objectInfoMap[objectId] = objectInfo;
		}

		return objectId;
	}

	return QByteArray();
}


bool CSqlDatabaseObjectCollectionComp::RemoveObject(const QByteArray& objectId)
{
	if (!m_objectDelegateCompPtr.IsValid()){
		return false;
	}

	QByteArray query = m_objectDelegateCompPtr->CreateDeleteObjectQuery(*this, objectId);
	if (query.isEmpty()){
		SendErrorMessage(0, "Database query could not be created", "Database collection");

		return false;
	}

	if (ExecuteTransaction(query)){
		istd::CChangeNotifier changeNotifier(this);

		{
			QWriteLocker writeLock(&m_objectInfoMapMutex);

			m_objectInfoMap.remove(objectId);
		}

		return true;
	}

	return false;
}


bool CSqlDatabaseObjectCollectionComp::SetObjectData(
			const QByteArray& objectId,
			const istd::IChangeable& object,
			CompatibilityMode /*mode*/)
{
	if (!m_objectDelegateCompPtr.IsValid()){
		return false;
	}

	QByteArray query = m_objectDelegateCompPtr->CreateUpdateObjectQuery(*this, objectId, object);
	if (query.isEmpty()){
		SendErrorMessage(0, "Database query could not be created", "Database collection");

		return false;
	}

	istd::CChangeNotifier changeNotifier(this);

	if (ExecuteTransaction(query)){
		m_metaInfoCreatorCompPtr->CreateMetaInfo(&object, *m_typeIdAttrPtr, m_objectInfoMap[objectId].metaInfoPtr);

		return true;
	}
	else{
		changeNotifier.Abort();
	}

	return false;
}


void CSqlDatabaseObjectCollectionComp::SetObjectName(const QByteArray& objectId, const QString& objectName)
{
	if (!m_objectDelegateCompPtr.IsValid()){
		return;
	}

	QByteArray query = m_objectDelegateCompPtr->CreateRenameObjectQuery(*this, objectId, objectName);
	if (query.isEmpty()){
		SendErrorMessage(0, "Database query could not be created", "Database collection");

		return;
	}

	if (ExecuteTransaction(query)){
		QWriteLocker writeLock(&m_objectInfoMapMutex);

		if (m_objectInfoMap.contains(objectId)){
			istd::CChangeNotifier changeNotifier(this);

			if (m_objectInfoMap[objectId].collectionMetaInfoPtr.IsValid()){
				m_objectInfoMap[objectId].collectionMetaInfoPtr->SetMetaInfo(idoc::IDocumentMetaInfo::MIT_TITLE, objectName);
			}

			writeLock.unlock();
		}
	}
}


void CSqlDatabaseObjectCollectionComp::SetObjectDescription(const QByteArray& objectId, const QString& objectDescription)
{
	if (!m_objectDelegateCompPtr.IsValid()){
		return;
	}

	QByteArray query = m_objectDelegateCompPtr->CreateDescriptionObjectQuery(*this, objectId, objectDescription);
	if (query.isEmpty()){
		SendErrorMessage(0, "Database query could not be created", "Database collection");

		return;
	}

	if (ExecuteTransaction(query)){
		QWriteLocker writeLock(&m_objectInfoMapMutex);

		if (m_objectInfoMap.contains(objectId)){
			istd::CChangeNotifier changeNotifier(this);

			if (m_objectInfoMap[objectId].collectionMetaInfoPtr.IsValid()){
				m_objectInfoMap[objectId].collectionMetaInfoPtr->SetMetaInfo(idoc::IDocumentMetaInfo::MIT_DESCRIPTION, objectDescription);
			}

			writeLock.unlock();
		}
	}
}


const imtbase::IRevisionController * CSqlDatabaseObjectCollectionComp::GetRevisionController() const
{
	return nullptr;
}


const imtbase::ICollectionDataController * CSqlDatabaseObjectCollectionComp::GetDataController() const
{
	return nullptr;
}


int CSqlDatabaseObjectCollectionComp::GetOperationFlags(const QByteArray& /*objectId*/) const
{
	return OF_ALL;
}


bool CSqlDatabaseObjectCollectionComp::GetDataMetaInfo(const QByteArray& objectId, MetaInfoPtr& metaInfoPtr) const
{
	QReadLocker readLock(&m_objectInfoMapMutex);

	if (m_objectInfoMap.contains(objectId)){
		imtbase::IMetaInfoCreator::MetaInfoPtr objectMetaInfoPtr = m_objectInfoMap[objectId].metaInfoPtr;
		if (objectMetaInfoPtr.IsValid()){
			metaInfoPtr.SetCastedOrRemove(objectMetaInfoPtr->CloneMe());

			return metaInfoPtr.IsValid();
		}
	}

	return false;
}


const istd::IChangeable* CSqlDatabaseObjectCollectionComp::GetObjectPtr(const QByteArray& /*objectId*/) const
{
	return nullptr;
}


bool CSqlDatabaseObjectCollectionComp::GetObjectData(const QByteArray& objectId, DataPtr& dataPtr) const
{
	if (!m_objectDelegateCompPtr.IsValid()){
		return false;
	}

	QByteArray objectSelectionQuery = m_objectDelegateCompPtr->GetSelectionQueryForObject(objectId, nullptr);
	if (objectSelectionQuery.isEmpty()){
		return false;
	}

	QSqlError sqlError;
	QSqlQuery sqlQuery = m_dbEngineCompPtr->ExecSqlQuery(objectSelectionQuery, &sqlError);
	if (sqlError.type() != QSqlError::NoError){
		SendErrorMessage(0, sqlError.text(), "Database collection");

		return false;
	}

	if (!sqlQuery.last()){
		return false;
	}

	dataPtr.SetPtr(m_objectDelegateCompPtr->CreateObjectFromRecord(*m_typeIdAttrPtr, sqlQuery.record()));

	return dataPtr.IsValid();
}


void CSqlDatabaseObjectCollectionComp::SetObjectEnabled(const QByteArray& /*objectId*/, bool /*isEnabled*/)
{
}


bool CSqlDatabaseObjectCollectionComp::RegisterEventHandler(imtbase::IObjectCollectionEventHandler* /*eventHandler*/)
{
	return false;
}


bool CSqlDatabaseObjectCollectionComp::UnregisterEventHandler(imtbase::IObjectCollectionEventHandler* /*eventHandler*/)
{
	return false;
}


// protected methods

bool CSqlDatabaseObjectCollectionComp::ExecuteTransaction(const QByteArray& sqlQuery) const
{
	QStringList queryList = QString(qPrintable(sqlQuery)).split(";");

	m_dbEngineCompPtr->BeginTransaction();

	for (const QString& singleQuery: queryList){
		if (!singleQuery.isEmpty()){
			QSqlError error;
			m_dbEngineCompPtr->ExecSqlQuery(singleQuery.toLocal8Bit(), &error);
			if (error.type() != QSqlError::NoError){
				SendErrorMessage(0, error.text(), "Database collection");

				m_dbEngineCompPtr->CancelTransaction();

				return false;
			}
		}
	}

	m_dbEngineCompPtr->FinishTransaction();

	return true;
}


void CSqlDatabaseObjectCollectionComp::CreateCollectionFromDatabase()
{
	if (!m_dbEngineCompPtr.IsValid()){
		return;
	}

	if (!m_objectDelegateCompPtr.IsValid()){
		return;
	}

	QByteArray objectSelectionQuery = m_objectDelegateCompPtr->GetSelectionQueryForObject(QByteArray(), m_filterParamsCompPtr.GetPtr());
	if (objectSelectionQuery.isEmpty()){
		return;
	}

	istd::CChangeNotifier changeNotifier(this);

	QWriteLocker writeLock(&m_objectInfoMapMutex);

	m_objectInfoMap.clear();

	writeLock.unlock();

	QSqlError sqlError;
	QSqlQuery sqlQuery = m_dbEngineCompPtr->ExecSqlQuery(objectSelectionQuery, &sqlError);

	while (sqlQuery.next()){
		QByteArray objectId = m_objectDelegateCompPtr->GetObjectIdFromRecord(*m_typeIdAttrPtr, sqlQuery.record());

		imtbase::IMetaInfoCreator::MetaInfoPtr objectMetaInfoPtr;
		imtbase::IMetaInfoCreator::MetaInfoPtr collectionMetaInfoPtr;
		bool isOk = m_objectDelegateCompPtr->CreateObjectInfoFromRecord(*m_typeIdAttrPtr, sqlQuery.record(), objectMetaInfoPtr, collectionMetaInfoPtr);
		if (isOk){
			ObjectInfo objectInfo;
			objectInfo.typeId = *m_typeIdAttrPtr;
			objectInfo.collectionMetaInfoPtr = collectionMetaInfoPtr;
			objectInfo.metaInfoPtr = objectMetaInfoPtr;

			writeLock.relock();

			m_objectInfoMap[objectId] = objectInfo;

			writeLock.unlock();
		}
	}
}


void CSqlDatabaseObjectCollectionComp::OnFilterParamsChanged(const istd::IChangeable::ChangeSet& /*changeSet*/, const iprm::IParamsSet* /*filterParamsPtr*/)
{
	CreateCollectionFromDatabase();
}


// reimplemented (icomp::CComponentBase)

void CSqlDatabaseObjectCollectionComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	m_typesInfo.InsertOption(*m_typeNameAttrPtr, *m_typeIdAttrPtr);

	m_filterParamsObserver.RegisterObject(m_filterParamsCompPtr.GetPtr(), &CSqlDatabaseObjectCollectionComp::OnFilterParamsChanged);

	CreateCollectionFromDatabase();
}


void CSqlDatabaseObjectCollectionComp::OnComponentDestroyed()
{
	m_filterParamsObserver.UnregisterAllObjects();

	BaseClass::OnComponentDestroyed();
}


} // namespace imtdb


