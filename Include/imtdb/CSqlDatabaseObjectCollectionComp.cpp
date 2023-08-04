#include <imtdb/CSqlDatabaseObjectCollectionComp.h>


// Qt includes
#include <QtSql/QSqlQuery>
#include <QtCore/QSharedPointer>

// ACF includes
#include <istd/CChangeNotifier.h>
#include <istd/CChangeGroup.h>
#include <idoc/CStandardDocumentMetaInfo.h>

// ImtCore includes
#include <imtbase/IRevisionController.h>
#include <imtbase/CParamsSetJoiner.h>
#include <imtbase/CObjectCollection.h>
#include <imtdb/CDatabaseEngineComp.h>
#include <imtdb/CSqlDatabaseObjectCollectionIterator.h>


namespace imtdb
{


// public methods

CSqlDatabaseObjectCollectionComp::CSqlDatabaseObjectCollectionComp()
	:m_filterParamsObserver(*this),
	m_databaseAccessObserver(*this),
	m_isInitialized(false)
{
}


// reimplemented (imtbase::IObjectCollection)

const imtbase::IRevisionController* CSqlDatabaseObjectCollectionComp::GetRevisionController() const
{
	if (m_objectDelegateCompPtr.IsValid()){
		return CompCastPtr<imtbase::IRevisionController>(m_objectDelegateCompPtr.GetPtr());
	}

	return nullptr;
}


const imtbase::ICollectionDataController* CSqlDatabaseObjectCollectionComp::GetDataController() const
{
	return m_collectionDataControllerCompPtr.GetPtr();
}


int CSqlDatabaseObjectCollectionComp::GetOperationFlags(const QByteArray& /*objectId*/) const
{
	return OF_ALL;
}


QByteArray CSqlDatabaseObjectCollectionComp::InsertNewObject(
			const QByteArray& typeId,
			const QString& name,
			const QString& description,
			DataPtr defaultValuePtr,
			const QByteArray& proposedObjectId,
			const idoc::IDocumentMetaInfo* dataMetaInfoPtr,
			const idoc::IDocumentMetaInfo* collectionItemMetaInfoPtr,
			const imtbase::IOperationContext* operationContextPtr)
{
	if (!m_objectDelegateCompPtr.IsValid()){
		return nullptr;
	}

	QByteArray objectId = proposedObjectId;
	if (objectId.isEmpty()){
		objectId = QUuid::createUuid().toByteArray(QUuid::WithoutBraces);
	}

	imtdb::IDatabaseObjectDelegate::NewObjectQuery objectQuery = m_objectDelegateCompPtr->CreateNewObjectQuery(
				typeId,
				objectId,
				name,
				description,
				defaultValuePtr.GetPtr(),
				operationContextPtr);
	if (objectQuery.query.isEmpty()){
		SendErrorMessage(0, "Database query could not be created", "Database collection");

		return nullptr;
	}

	istd::IChangeable::ChangeSet changeSet(CF_ADDED);
	changeSet.SetChangeInfo(CN_ELEMENT_INSERTED, objectId);

	istd::CChangeNotifier changeNotifier(this, &changeSet);

	if (ExecuteTransaction(objectQuery.query)){
		if (dataMetaInfoPtr != nullptr){
			QByteArray metaQuery = m_objectDelegateCompPtr->CreateDataMetaInfoQuery(*this, objectId, dataMetaInfoPtr);
			if(!metaQuery.isEmpty()){
				ExecuteTransaction(metaQuery);
			}
		}
		if (collectionItemMetaInfoPtr != nullptr){
			QByteArray collectionItemMetaQuery = m_objectDelegateCompPtr->CreateCollectionItemMetaInfoQuery(*this, objectId, collectionItemMetaInfoPtr);
			if(!collectionItemMetaQuery.isEmpty()){
				ExecuteTransaction(collectionItemMetaQuery);
			}
		}

		return objectId;
	}
	else {
		changeNotifier.Abort();
	}

	return QByteArray();
}


bool CSqlDatabaseObjectCollectionComp::RemoveElement(const Id& elementId, const imtbase::IOperationContext* operationContextPtr)
{
	if (!m_objectDelegateCompPtr.IsValid()){
		return false;
	}

	QByteArray query = m_objectDelegateCompPtr->CreateDeleteObjectQuery(*this, elementId, operationContextPtr);
	if (query.isEmpty()){
		SendErrorMessage(0, "Database query could not be created", "Database collection");

		return false;
	}

	istd::IChangeable::ChangeSet changeSet(CF_REMOVED);
	changeSet.SetChangeInfo(CN_ELEMENT_REMOVED, elementId);

	istd::CChangeNotifier changeNotifier(this, &changeSet);

	if (ExecuteTransaction(query)){
		return true;
	}
	else{
		changeNotifier.Abort();
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

	if (objectId.isEmpty()){
		return false;
	}

	QByteArray objectSelectionQuery = m_objectDelegateCompPtr->GetSelectionQuery(objectId, -1, -1, nullptr);
	if (objectSelectionQuery.isEmpty()){
		return false;
	}

	QSqlError sqlError;
	QSqlQuery sqlQuery = m_dbEngineCompPtr->ExecSqlQuery(objectSelectionQuery, &sqlError);
	if (sqlError.type() != QSqlError::NoError){
		SendErrorMessage(0, sqlError.text(), "Database collection");

		return false;
	}

	if (!sqlQuery.next()){
		return false;
	}

	istd::IChangeable* dataObjPtr = m_objectDelegateCompPtr->CreateObjectFromRecord(sqlQuery.record());
	dataPtr = DataPtr(DataPtr::RootObjectPtr(dataObjPtr), [dataObjPtr](){
		return dataObjPtr;
	});

	return dataPtr.IsValid();
}


bool CSqlDatabaseObjectCollectionComp::SetObjectData(
			const Id& objectId,
			const istd::IChangeable& object,
			CompatibilityMode /*mode*/,
			const imtbase::IOperationContext* operationContextPtr)
{
	if (!m_objectDelegateCompPtr.IsValid()){
		return false;
	}

	QByteArray query = m_objectDelegateCompPtr->CreateUpdateObjectQuery(*this, objectId, object, operationContextPtr);
	if (query.isEmpty()){
		SendErrorMessage(0, "Database query could not be created", "Database collection");

		return false;
	}

	istd::IChangeable::ChangeSet changeSet(CF_UPDATED);
	changeSet.SetChangeInfo(CN_ELEMENT_UPDATED, objectId);

	istd::CChangeNotifier changeNotifier(this, &changeSet);

	if (ExecuteTransaction(query)){
		return true;
	}
	else{
		changeNotifier.Abort();
	}

	return false;
}


imtbase::IObjectCollection* CSqlDatabaseObjectCollectionComp::CreateSubCollection(
			int offset,
			int count,
			const iprm::IParamsSet* selectionParamsPtr) const
{
	imtbase::IObjectCollection* collectionPtr = m_objectCollectionFactoryCompPtr.CreateInstance();
	imtbase::CParamsSetJoiner filterParams(selectionParamsPtr, m_filterParamsCompPtr.GetPtr());

	if (m_objectDelegateCompPtr.IsValid()){
		QByteArray objectSelectionQuery = m_objectDelegateCompPtr->GetSelectionQuery(QByteArray(), offset, count, &filterParams);
		if (objectSelectionQuery.isEmpty()){
			return nullptr;
		}

		QSqlError sqlError;
		QSqlQuery sqlQuery = m_dbEngineCompPtr->ExecSqlQuery(objectSelectionQuery, &sqlError, true);

		while (sqlQuery.next()){
			istd::IChangeable* dataObjPtr = m_objectDelegateCompPtr->CreateObjectFromRecord(sqlQuery.record());
            DataPtr dataPtr = DataPtr(DataPtr::RootObjectPtr(dataObjPtr), [dataObjPtr](){
				return dataObjPtr;
				});

			QByteArray objectId = m_objectDelegateCompPtr->GetObjectIdFromRecord(sqlQuery.record());

			QByteArray typeId = m_objectDelegateCompPtr->GetObjectTypeId(objectId);

			collectionPtr->InsertNewObject(typeId, "", "", dataPtr, objectId);
		}
	}
	return collectionPtr;
}


// reimplemented (IObjectCollectionInfo)

const iprm::IOptionsList* CSqlDatabaseObjectCollectionComp::GetObjectTypesInfo() const
{
	if (m_objectDelegateCompPtr.IsValid()){
		return m_objectDelegateCompPtr->GetObjectTypeInfos();
	}
	else {
		SendCriticalMessage(0, "Invalid component configuration: Object delegate missing", "Database collection");
	}

	return nullptr;
}


imtbase::ICollectionInfo::Id CSqlDatabaseObjectCollectionComp::GetObjectTypeId(const QByteArray& objectId) const
{
	if (m_objectDelegateCompPtr.IsValid()){
		return m_objectDelegateCompPtr->GetObjectTypeId(objectId);
	}
	else{
		SendCriticalMessage(0, "Invalid component configuration: Object delegate missing", "Database collection");
	}

	return QByteArray();
}


idoc::MetaInfoPtr CSqlDatabaseObjectCollectionComp::GetDataMetaInfo(const Id& objectId) const
{
	if (m_objectDelegateCompPtr.IsValid()){
		QSqlRecord record = GetObjectRecord(objectId);
		if (!record.isEmpty()){
			idoc::MetaInfoPtr objectMetaInfoPtr;
			idoc::MetaInfoPtr collectionMetaInfoPtr;

			m_objectDelegateCompPtr->CreateObjectInfoFromRecord(record, objectMetaInfoPtr, collectionMetaInfoPtr);

			return objectMetaInfoPtr;
		}
	}
	else{
		SendCriticalMessage(0, "Invalid component configuration: Object delegate missing", "Database collection");
	}

	return idoc::MetaInfoPtr();
}


// reimplemented (ICollectionInfo)

int CSqlDatabaseObjectCollectionComp::GetElementsCount(const iprm::IParamsSet* selectionParamPtr) const
{
	if (!m_objectDelegateCompPtr.IsValid()){
		SendCriticalMessage(0, "Invalid component configuration: Object delegate missing", "Database collection");

		return 0;
	}

	if (!m_dbEngineCompPtr.IsValid()){
		SendCriticalMessage(0, "Invalid component configuration: Database engine missing", "Database collection");

		return 0;
	}

	imtbase::CParamsSetJoiner filterParams(selectionParamPtr, m_filterParamsCompPtr.GetPtr());

	QByteArray countQuery = m_objectDelegateCompPtr->GetCountQuery(&filterParams);
	if (!countQuery.isEmpty()){
		QSqlError sqlError;
		QSqlQuery result = m_dbEngineCompPtr->ExecSqlQuery(countQuery, &sqlError);

		if (sqlError.type() == QSqlError::NoError){
			if (result.first()){
				return result.value(0).toInt();
			}
		}
		else{
			SendErrorMessage(0, sqlError.text(), "Database collection");
		}
	}
	else{
		SendErrorMessage(0, "Database query could not be created", "Database collection");
	}

	return 0;
}


imtbase::ICollectionInfo::Ids CSqlDatabaseObjectCollectionComp::GetElementIds(
			int offset,
			int count,
			const iprm::IParamsSet* selectionParamsPtr) const
{
	Ids retVal;

	imtbase::CParamsSetJoiner filterParams(selectionParamsPtr, m_filterParamsCompPtr.GetPtr());

	if (m_objectDelegateCompPtr.IsValid() && m_dbEngineCompPtr.IsValid()){
		QByteArray objectSelectionQuery = m_objectDelegateCompPtr->GetSelectionQuery(QByteArray(), offset, count, &filterParams);
		if (objectSelectionQuery.isEmpty()){
			return Ids();
		}

		QSqlError sqlError;
		QSqlQuery sqlQuery = m_dbEngineCompPtr->ExecSqlQuery(objectSelectionQuery, &sqlError, true);

		while (sqlQuery.next()){
			QByteArray objectId = m_objectDelegateCompPtr->GetObjectIdFromRecord(sqlQuery.record());
			Q_ASSERT(!objectId.isEmpty());

			retVal.push_back(objectId);
		}
	}

	return retVal;
}


bool CSqlDatabaseObjectCollectionComp::GetSubsetInfo(
			imtbase::ICollectionInfo& /*subsetInfo*/,
			int /*offset*/,
			int /*count*/,
			const iprm::IParamsSet* /*selectionParamsPtr*/) const
{
	return false;
}


imtbase::IObjectCollectionIterator* CSqlDatabaseObjectCollectionComp::CreateObjectCollectionIterator(
			int offset,
			int count,
			const iprm::IParamsSet* selectionParamsPtr) const
{
	imtbase::CParamsSetJoiner filterParams(selectionParamsPtr, m_filterParamsCompPtr.GetPtr());

	if (m_objectDelegateCompPtr.IsValid() && m_dbEngineCompPtr.IsValid()){
		QByteArray objectSelectionQuery = m_objectDelegateCompPtr->GetSelectionQuery(QByteArray(), offset, count, &filterParams);
		if (objectSelectionQuery.isEmpty()){
			return nullptr;
		}

		QSqlError sqlError;
		QSqlQuery sqlQuery = m_dbEngineCompPtr->ExecSqlQuery(objectSelectionQuery, &sqlError, true);

		if (sqlError.type() != QSqlError::NoError){
			SendErrorMessage(0, sqlError.text(), "Database collection");

			qDebug() << "SQL-error" << objectSelectionQuery;

			return nullptr;
		}
		
		return new CSqlDatabaseObjectCollectionIterator(sqlQuery, m_objectDelegateCompPtr.GetPtr());
	}

	return nullptr;
}


QVariant CSqlDatabaseObjectCollectionComp::GetElementInfo(const QByteArray& elementId, int infoType) const
{
	if (m_objectDelegateCompPtr.IsValid()){
		QSqlRecord record = GetObjectRecord(elementId);
		if (!record.isEmpty()){
			idoc::MetaInfoPtr objectMetaInfoPtr;
			idoc::MetaInfoPtr collectionMetaInfoPtr;
			bool isOk = m_objectDelegateCompPtr->CreateObjectInfoFromRecord(record, objectMetaInfoPtr, collectionMetaInfoPtr);
			if (isOk){
				switch (infoType){
				case EIT_NAME:
					return collectionMetaInfoPtr.IsValid() ? collectionMetaInfoPtr->GetMetaInfo(idoc::IDocumentMetaInfo::MIT_TITLE).toString() : QString();

				case EIT_DESCRIPTION:
					return collectionMetaInfoPtr.IsValid() ? collectionMetaInfoPtr->GetMetaInfo(idoc::IDocumentMetaInfo::MIT_DESCRIPTION).toString() : QString();
				}
			}
		}
	}
	else{
		SendCriticalMessage(0, "Invalid component configuration: Object delegate missing", "Database collection");
	}

	return QVariant();
}


idoc::MetaInfoPtr CSqlDatabaseObjectCollectionComp::GetElementMetaInfo(const Id& elementId) const
{
	if (m_objectDelegateCompPtr.IsValid()){
		QSqlRecord record = GetObjectRecord(elementId);
		if (!record.isEmpty()){
			idoc::MetaInfoPtr objectMetaInfoPtr;
			idoc::MetaInfoPtr collectionMetaInfoPtr;

			m_objectDelegateCompPtr->CreateObjectInfoFromRecord(record, objectMetaInfoPtr, collectionMetaInfoPtr);

			return collectionMetaInfoPtr;
		}
	}
	else{
		SendCriticalMessage(0, "Invalid component configuration: Object delegate missing", "Database collection");
	}

	return idoc::MetaInfoPtr();
}


bool CSqlDatabaseObjectCollectionComp::SetElementName(const Id& elementId, const QString& name)
{
	if (!m_objectDelegateCompPtr.IsValid()){
		SendCriticalMessage(0, "Invalid component configuration: Object delegate missing", "Database collection");

		return false;
	}

	if (elementId.isEmpty()){
		return false;
	}

	QByteArray query = m_objectDelegateCompPtr->CreateRenameObjectQuery(*this, elementId, name, nullptr);
	if (query.isEmpty()){
		SendErrorMessage(0, "Database query could not be created", "Database collection");

		return false;
	}

	QByteArray objectSelectionQuery = m_objectDelegateCompPtr->GetSelectionQuery(elementId, -1, -1, nullptr);
	if (objectSelectionQuery.isEmpty()){
		return false;
	}

	istd::IChangeable::ChangeSet changeSet(CF_UPDATED);
	changeSet.SetChangeInfo(CN_ELEMENT_UPDATED, elementId);

	istd::CChangeNotifier changeNotifier(this, &changeSet);

	if (!ExecuteTransaction(query)){
		changeNotifier.Abort();

		return false;
	}

	return true;
}


bool CSqlDatabaseObjectCollectionComp::SetElementDescription(const Id& elementId, const QString& description)
{
	if (!m_objectDelegateCompPtr.IsValid()){
		SendCriticalMessage(0, "Invalid component configuration: Object delegate missing", "Database collection");
	
		return false;
	}

	QByteArray query = m_objectDelegateCompPtr->CreateDescriptionObjectQuery(*this, elementId, description, nullptr);
	if (query.isEmpty()){
		SendErrorMessage(0, "Database query could not be created", "Database collection");

		return false;
	}

	istd::IChangeable::ChangeSet changeSet(CF_UPDATED);
	changeSet.SetChangeInfo(CN_ELEMENT_UPDATED, elementId);

	istd::CChangeNotifier changeNotifier(this, &changeSet);

	if (!ExecuteTransaction(query)){
		changeNotifier.Abort();

		return false;
	}

	return true;
}


bool CSqlDatabaseObjectCollectionComp::SetElementEnabled(const Id& /*objectId*/, bool /*isEnabled*/)
{
	return false;
}


// reimplemented (istd::IChangeable)

bool CSqlDatabaseObjectCollectionComp::ResetData(CompatibilityMode /*mode*/)
{
	if (!m_objectDelegateCompPtr.IsValid()){
		SendCriticalMessage(0, "Invalid component configuration: Object delegate missing", "Database collection");

		return false;
	}

	QByteArray resetQuery = m_objectDelegateCompPtr->CreateResetQuery(*this);
	if (resetQuery.isEmpty()){
		SendErrorMessage(0, "Database query could not be created", "Database collection");

		return false;
	}
	if (ExecuteTransaction(resetQuery)){
		istd::CChangeNotifier changeNotifier(this);

		return true;
	}

	return false;
}


// protected methods

bool CSqlDatabaseObjectCollectionComp::ExecuteTransaction(const QByteArray& sqlQuery) const
{
	if (!m_dbEngineCompPtr.IsValid()){
		SendCriticalMessage(0, "Invalid component configuration: Database engine missing", "Database collection");

		return false;
	}

	QStringList queryList = QString::fromUtf8(sqlQuery).split(";");

	m_dbEngineCompPtr->BeginTransaction();

	for (QString& singleQuery: queryList){
		if (!singleQuery.isEmpty()){
			QSqlError error;
			singleQuery = singleQuery.replace('\b', ';');
			m_dbEngineCompPtr->ExecSqlQuery(singleQuery.toUtf8(), &error);
			if (error.type() != QSqlError::NoError){
				SendErrorMessage(0, error.text(), "Database collection");

				qDebug() << "SQL-error: " << singleQuery;

				m_dbEngineCompPtr->CancelTransaction();

				return false;
			}
		}
	}

	m_dbEngineCompPtr->FinishTransaction();

	return true;
}


QSqlRecord CSqlDatabaseObjectCollectionComp::GetObjectRecord(const QByteArray& objectId) const
{
	if (!m_objectDelegateCompPtr.IsValid()){
		SendCriticalMessage(0, "Invalid component configuration: Object delegate missing", "Database collection");

		return QSqlRecord();
	}

	if (!m_dbEngineCompPtr.IsValid()){
		SendCriticalMessage(0, "Invalid component configuration: Database engine missing", "Database collection");

		return QSqlRecord();
	}

	QByteArray objectSelectionQuery = m_objectDelegateCompPtr->GetSelectionQuery(objectId);
	if (!objectSelectionQuery.isEmpty()){
		QSqlError sqlError;
		QSqlQuery sqlQuery = m_dbEngineCompPtr->ExecSqlQuery(objectSelectionQuery, &sqlError, true);
		if (sqlError.type() != QSqlError::NoError){
			SendErrorMessage(0, sqlError.text(), "Database collection");
		}

		QSqlRecord record;
		if (sqlQuery.next()){
			return sqlQuery.record();
		}
	}

	return QSqlRecord();
}


void CSqlDatabaseObjectCollectionComp::OnFilterParamsChanged(const istd::IChangeable::ChangeSet& /*changeSet*/, const iprm::IParamsSet* /*filterParamsPtr*/)
{
	istd::CChangeNotifier changeNotifier(this);
}


void CSqlDatabaseObjectCollectionComp::OnDatabaseAccessChanged(const istd::IChangeable::ChangeSet& /*changeSet*/, const imtdb::IDatabaseLoginSettings* /*databaseAccessSettingsPtr*/)
{
	istd::CChangeNotifier changeNotifier(this);
}


// reimplemented (icomp::CComponentBase)

void CSqlDatabaseObjectCollectionComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	m_dbEngineCompPtr.EnsureInitialized();
	m_objectDelegateCompPtr.EnsureInitialized();

	m_isInitialized = false;

	m_filterParamsObserver.RegisterObject(m_filterParamsCompPtr.GetPtr(), &CSqlDatabaseObjectCollectionComp::OnFilterParamsChanged);
	m_databaseAccessObserver.RegisterObject(m_databaseAccessSettingsCompPtr.GetPtr(), &CSqlDatabaseObjectCollectionComp::OnDatabaseAccessChanged);
	
	m_isInitialized = true;
}


void CSqlDatabaseObjectCollectionComp::OnComponentDestroyed()
{
	m_filterParamsObserver.UnregisterAllObjects();
	m_databaseAccessObserver.UnregisterAllObjects();

	BaseClass::OnComponentDestroyed();
}

} // namespace imtdb


