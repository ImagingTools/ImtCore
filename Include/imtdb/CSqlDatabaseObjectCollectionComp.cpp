#include <imtdb/CSqlDatabaseObjectCollectionComp.h>


// Qt includes
#include <QtSql/QSqlQuery>

// ACF includes
#include <istd/CChangeNotifier.h>
#include <istd/CChangeGroup.h>
#include <idoc/CStandardDocumentMetaInfo.h>

// ImtCore includes
#include <imtbase/CParamsSetJoiner.h>
#include <imtdb/CDatabaseEngineComp.h>


namespace imtdb
{


// public methods

CSqlDatabaseObjectCollectionComp::CSqlDatabaseObjectCollectionComp()
	:m_filterParamsObserver(*this),
	m_databaseAccessObserver(*this),
	m_isInitialized(false)
{
}


// reimplemented (ICollectionInfo)

int CSqlDatabaseObjectCollectionComp::GetElementsCount(const iprm::IParamsSet* selectionParamPtr) const
{
	imtbase::CParamsSetJoiner filterParams(selectionParamPtr, m_filterParamsCompPtr.GetPtr());

	if (m_objectDelegateCompPtr.IsValid()){
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

	if (m_objectDelegateCompPtr.IsValid()){
		QByteArray objectSelectionQuery = m_objectDelegateCompPtr->GetSelectionQuery(QByteArray(), offset, count, &filterParams);
		if (objectSelectionQuery.isEmpty()){
			return Ids();
		}

		QSqlError sqlError;
		QSqlQuery sqlQuery = m_dbEngineCompPtr->ExecSqlQuery(objectSelectionQuery, &sqlError, true);

		while (sqlQuery.next()){
			QByteArray objectId = m_objectDelegateCompPtr->GetObjectIdFromRecord(*m_typeIdAttrPtr, sqlQuery.record());
			Q_ASSERT(!objectId.isEmpty());

			retVal.push_back(objectId);
		}
	}

	return retVal;
}


QVariant CSqlDatabaseObjectCollectionComp::GetElementInfo(const QByteArray& elementId, int infoType) const
{
	if (m_objectDelegateCompPtr.IsValid()){
		QSqlRecord record = GetObjectRecord(elementId);
		if (!record.isEmpty()){
			imtbase::IMetaInfoCreator::MetaInfoPtr objectMetaInfoPtr;
			imtbase::IMetaInfoCreator::MetaInfoPtr collectionMetaInfoPtr;
			bool isOk = m_objectDelegateCompPtr->CreateObjectInfoFromRecord(*m_typeIdAttrPtr, record, objectMetaInfoPtr, collectionMetaInfoPtr);
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

	return QVariant();
}


// reimplemented (IObjectCollectionInfo)

bool CSqlDatabaseObjectCollectionComp::GetCollectionItemMetaInfo(const QByteArray& objectId, idoc::IDocumentMetaInfo& metaInfo) const
{
	if (m_objectDelegateCompPtr.IsValid()){
		QSqlRecord record = GetObjectRecord(objectId);
		if (!record.isEmpty()){
			imtbase::IMetaInfoCreator::MetaInfoPtr objectMetaInfoPtr;
			imtbase::IMetaInfoCreator::MetaInfoPtr collectionMetaInfoPtr;
			m_objectDelegateCompPtr->CreateObjectInfoFromRecord(*m_typeIdAttrPtr, record, objectMetaInfoPtr, collectionMetaInfoPtr);
			if (collectionMetaInfoPtr.IsValid()){
				return metaInfo.CopyFrom(*collectionMetaInfoPtr);
			}
		}
	}

	return false;
}


const iprm::IOptionsList* CSqlDatabaseObjectCollectionComp::GetObjectTypesInfo() const
{
	return &m_typesInfo;
}


imtbase::ICollectionInfo::Id CSqlDatabaseObjectCollectionComp::GetObjectTypeId(const QByteArray& /*objectId*/) const
{
	return *m_typeIdAttrPtr;
}


// reimplemented (imtbase::IObjectCollection)

QByteArray CSqlDatabaseObjectCollectionComp::InsertNewObject(
			const QByteArray& typeId,
			const QString& name,
			const QString& description,
			DataPtr defaultValuePtr,
			const QByteArray& proposedObjectId,
			const idoc::IDocumentMetaInfo* /*dataMetaInfoPtr*/,
			const idoc::IDocumentMetaInfo* /*collectionItemMetaInfoPtr*/)
{
	if (!m_objectDelegateCompPtr.IsValid()){
		return nullptr;
	}

	QByteArray objectId = proposedObjectId;
	if (objectId.isEmpty()){
		objectId = QUuid::createUuid().toByteArray(QUuid::WithoutBraces);
	}

	imtdb::IDatabaseObjectDelegate::NewObjectQuery objectQuery = m_objectDelegateCompPtr->CreateNewObjectQuery(typeId, objectId, name, description, defaultValuePtr.GetPtr());
	if (objectQuery.query.isEmpty()){
		SendErrorMessage(0, "Database query could not be created", "Database collection");

		return nullptr;
	}

	istd::IChangeable::ChangeSet changeSet(CF_ADDED);
	changeSet.SetChangeInfo(CN_OBJECT_ADDED, objectId);

	istd::CChangeNotifier changeNotifier(this, &changeSet);

	if (ExecuteTransaction(objectQuery.query)){
		return objectId;
	}
	else {
		changeNotifier.Abort();
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

	istd::IChangeable::ChangeSet changeSet(CF_REMOVED);
	changeSet.SetChangeInfo(CN_OBJECT_REMOVED, objectId);

	istd::CChangeNotifier changeNotifier(this, &changeSet);

	if (ExecuteTransaction(query)){
		return true;
	}
	else {
		changeNotifier.Abort();
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

	istd::IChangeable::ChangeSet changeSet(CF_UPDATED);
	changeSet.SetChangeInfo(CN_OBJECT_UPDATED, objectId);

	istd::CChangeNotifier changeNotifier(this, &changeSet);

	if (ExecuteTransaction(query)){
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

	istd::IChangeable::ChangeSet changeSet(CF_UPDATED);
	changeSet.SetChangeInfo(CN_OBJECT_UPDATED, objectId);

	istd::CChangeNotifier changeNotifier(this, &changeSet);

	if (!ExecuteTransaction(query)){
		changeNotifier.Abort();
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

	istd::IChangeable::ChangeSet changeSet(CF_UPDATED);
	changeSet.SetChangeInfo(CN_OBJECT_UPDATED, objectId);

	istd::CChangeNotifier changeNotifier(this, &changeSet);

	if (!ExecuteTransaction(query)){
		changeNotifier.Abort();
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
	if (m_objectDelegateCompPtr.IsValid()){
		QSqlRecord record = GetObjectRecord(objectId);
		if (!record.isEmpty()){
			imtbase::IMetaInfoCreator::MetaInfoPtr objectMetaInfoPtr;
			imtbase::IMetaInfoCreator::MetaInfoPtr collectionMetaInfoPtr;
			
			return m_objectDelegateCompPtr->CreateObjectInfoFromRecord(*m_typeIdAttrPtr, record, metaInfoPtr, collectionMetaInfoPtr);
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

	if (!sqlQuery.last()){
		return false;
	}

	istd::IChangeable* dataObjPtr = m_objectDelegateCompPtr->CreateObjectFromRecord(*m_typeIdAttrPtr, sqlQuery.record());
	dataPtr = DataPtr(DataPtr::RootObjectPtr(dataObjPtr), [dataObjPtr](){
		return dataObjPtr;
	});

	return dataPtr.IsValid();
}


void CSqlDatabaseObjectCollectionComp::SetObjectEnabled(const QByteArray& /*objectId*/, bool /*isEnabled*/)
{
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


QSqlRecord CSqlDatabaseObjectCollectionComp::GetObjectRecord(const QByteArray& objectId) const
{
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

	m_typesInfo.InsertOption(*m_typeNameAttrPtr, *m_typeIdAttrPtr);

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


