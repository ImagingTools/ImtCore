// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
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
#include <imtdb/CSqlDatabaseObjectCollectionIterator.h>


namespace imtdb
{


// public methods

CSqlDatabaseObjectCollectionComp::CSqlDatabaseObjectCollectionComp()
	:m_databaseAccessObserver(*this),
	m_isInitialized(false)
{
}


// reimplemented (ISqlDatabaseObjectCollection)

QByteArray CSqlDatabaseObjectCollectionComp::GetDatabaseId() const
{
	return QByteArrayLiteral("");
}


bool CSqlDatabaseObjectCollectionComp::AreInternalTransactionsEnabled() const
{
	QMutexLocker locker(&m_transactionDisableCountersMutex);

	Qt::HANDLE threadId = QThread::currentThreadId();

	return !m_transactionDisableCounters.contains(threadId);
}


bool CSqlDatabaseObjectCollectionComp::SetInternalTransactionsEnabled(bool isEnabled)
{
	QMutexLocker locker(&m_transactionDisableCountersMutex);

	Qt::HANDLE threadId = QThread::currentThreadId();

	if (m_transactionDisableCounters.contains(threadId)){
		Q_ASSERT(m_transactionDisableCounters[threadId] > 0);

		if (!isEnabled){
			m_transactionDisableCounters[threadId]++;
		}
		else{
			if (--m_transactionDisableCounters[threadId] == 0){
				m_transactionDisableCounters.remove(threadId);
			}
		}
	}
	else{
		if (!isEnabled){
			m_transactionDisableCounters[threadId] = 1;
		}
		else{
			return false;
		}
	}

	return true;
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
			const istd::IChangeable* defaultValuePtr,
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

	if (!IsObjectTypeSupported(typeId)){
		SendErrorMessage(0, QString("Object type ID \"%1\" not supported").arg(qPrintable(typeId)));

		return QByteArray();
	}

	imtdb::IDatabaseObjectDelegate::NewObjectQuery objectQuery = m_objectDelegateCompPtr->CreateNewObjectQuery(
				typeId,
				objectId,
				name,
				description,
				defaultValuePtr,
				operationContextPtr);
	if (objectQuery.query.isEmpty()){
		SendErrorMessage(0, "Database query could not be created", "Database collection");

		return nullptr;
	}

	istd::IChangeable::ChangeSet changeSet(CF_ADDED);
	changeSet.SetChangeInfo(CN_ELEMENT_INSERTED, objectId);

	if (operationContextPtr != nullptr){
		AddOperationContextToChangeSet(*operationContextPtr, changeSet);
	}

	istd::CChangeNotifier changeNotifier(this, &changeSet);

	QVariantMap bindValues = objectQuery.bindValues;
	bool transactionSuccess = false;

	if(bindValues.isEmpty()){
		transactionSuccess = ExecuteTransaction(objectQuery.query);
	}
	else{
		transactionSuccess = ExecuteTransaction(objectQuery.query, bindValues);
	}

	if (!transactionSuccess){
		changeNotifier.Abort();
	}
	else{
		if (dataMetaInfoPtr != nullptr){
			QByteArray metaQuery = m_objectDelegateCompPtr->CreateDataMetaInfoQuery(*this, objectId, dataMetaInfoPtr);
			if(!metaQuery.isEmpty()){
				if(bindValues.isEmpty()){
					ExecuteTransaction(metaQuery);
				}
				else{
					ExecuteTransaction(metaQuery, bindValues);
				}
			}
		}
		if (collectionItemMetaInfoPtr != nullptr){
			QByteArray collectionItemMetaQuery = m_objectDelegateCompPtr->CreateCollectionItemMetaInfoQuery(*this, objectId, collectionItemMetaInfoPtr);
			if(!collectionItemMetaQuery.isEmpty()){
				if(bindValues.isEmpty()){
					ExecuteTransaction(collectionItemMetaQuery);
				}
				else{
					ExecuteTransaction(collectionItemMetaQuery, bindValues);
				}
			}
		}

		return objectId;
	}

	return QByteArray();
}


bool CSqlDatabaseObjectCollectionComp::RemoveElements(const Ids& elementIds, const imtbase::IOperationContext* operationContextPtr)
{
	if (!m_objectDelegateCompPtr.IsValid()){
		return false;
	}

	QByteArray query = m_objectDelegateCompPtr->CreateDeleteObjectsQuery(*this, elementIds, operationContextPtr);
	if (query.isEmpty()){
		SendErrorMessage(0, "Database query could not be created", "Database collection");

		return false;
	}

	imtbase::ICollectionInfo::MultiElementNotifierInfo notifierInfo;
	notifierInfo.elementIds = elementIds;

	istd::IChangeable::ChangeSet changeSet(CF_REMOVED);
	changeSet.SetChangeInfo(CN_ELEMENTS_REMOVED, QVariant::fromValue(notifierInfo));

	if (operationContextPtr != nullptr){
		AddOperationContextToChangeSet(*operationContextPtr, changeSet);
	}

	istd::CChangeNotifier changeNotifier(this, &changeSet);

	if (ExecuteTransaction(query)){
		return true;
	}

	changeNotifier.Abort();

	return false;
}


bool CSqlDatabaseObjectCollectionComp::RemoveElementSet(
			const iprm::IParamsSet* selectionParamsPtr,
			const imtbase::IOperationContext* operationContextPtr)
{
	if (!m_objectDelegateCompPtr.IsValid()){
		return false;
	}

	QByteArray query = m_objectDelegateCompPtr->CreateDeleteObjectSetQuery(*this, selectionParamsPtr, operationContextPtr);
	if (query.isEmpty()){
		SendErrorMessage(0, "Database query could not be created", "Database collection");

		return false;
	}

	imtbase::ICollectionInfo::MultiElementNotifierInfo notifierInfo;
	notifierInfo.elementIds = GetElementIds(0, -1, selectionParamsPtr);

	istd::IChangeable::ChangeSet changeSet(CF_REMOVED);
	changeSet.SetChangeInfo(CN_ELEMENTS_REMOVED, QVariant::fromValue(notifierInfo));

	if (operationContextPtr != nullptr){
		AddOperationContextToChangeSet(*operationContextPtr, changeSet);
	}

	istd::CChangeNotifier changeNotifier(this, &changeSet);

	if (ExecuteTransaction(query)){
		return true;
	}

	changeNotifier.Abort();

	return false;
}


bool CSqlDatabaseObjectCollectionComp::RestoreObjects(
			const Ids& objectIds,
			const imtbase::IOperationContext* operationContextPtr)
{
	if (!m_objectDelegateCompPtr.IsValid()){
		return false;
	}

	QByteArray query = m_objectDelegateCompPtr->CreateRestoreObjectsQuery(*this, objectIds, operationContextPtr);
	if (query.isEmpty()){
		SendErrorMessage(0, "Database query could not be created", "Database collection");

		return false;
	}

	imtbase::ICollectionInfo::MultiElementNotifierInfo notifierInfo;
	notifierInfo.elementIds = objectIds;

	istd::IChangeable::ChangeSet changeSet(CF_RESTORED);
	changeSet.SetChangeInfo(CN_ELEMENTS_RESTORED, QVariant::fromValue(notifierInfo));

	if (operationContextPtr != nullptr){
		AddOperationContextToChangeSet(*operationContextPtr, changeSet);
	}

	istd::CChangeNotifier changeNotifier(this, &changeSet);

	if (ExecuteTransaction(query)){
		return true;
	}

	changeNotifier.Abort();

	return false;
}


bool CSqlDatabaseObjectCollectionComp::RestoreObjectSet(
			const iprm::IParamsSet* selectionParamsPtr,
			const imtbase::IOperationContext* operationContextPtr)
{
	if (!m_objectDelegateCompPtr.IsValid()){
		return false;
	}

	QByteArray query = m_objectDelegateCompPtr->CreateRestoreObjectSetQuery(*this, selectionParamsPtr, operationContextPtr);
	if (query.isEmpty()){
		SendErrorMessage(0, "Database query could not be created", "Database collection");

		return false;
	}

	imtbase::ICollectionInfo::MultiElementNotifierInfo notifierInfo;
	notifierInfo.elementIds = imtbase::ICollectionInfo::Ids();

	istd::IChangeable::ChangeSet changeSet(CF_RESTORED);
	changeSet.SetChangeInfo(CN_ALL_CHANGED, QVariant::fromValue(notifierInfo));

	if (operationContextPtr != nullptr){
		AddOperationContextToChangeSet(*operationContextPtr, changeSet);
	}

	istd::CChangeNotifier changeNotifier(this, &changeSet);

	if (ExecuteTransaction(query)){
		return true;
	}

	changeNotifier.Abort();

	return false;
}


const istd::IChangeable* CSqlDatabaseObjectCollectionComp::GetObjectPtr(const QByteArray& /*objectId*/) const
{
	return nullptr;
}


bool CSqlDatabaseObjectCollectionComp::GetObjectData(const QByteArray& objectId, DataPtr& dataPtr, const iprm::IParamsSet* dataConfigurationPtr) const
{
	if (!m_objectDelegateCompPtr.IsValid()){
		return false;
	}

	if (!m_dbEngineCompPtr.IsValid()){
		SendCriticalMessage(0, "Invalid component configuration: Database engine missing", "Database collection");

		return false;
	}

	if (objectId.isEmpty()){
		return false;
	}

	QByteArray objectSelectionQuery = m_objectDelegateCompPtr->GetSelectionQuery(objectId, -1, -1, dataConfigurationPtr);
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

	istd::IChangeableUniquePtr objectPtr = m_objectDelegateCompPtr->CreateObjectFromRecord(sqlQuery.record(), dataConfigurationPtr);
	dataPtr.FromUnique(objectPtr);

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

	istd::IChangeable::ChangeSet changeSet(CF_OBJECT_DATA_CHANGED);
	changeSet.SetChangeInfo(CN_OBJECT_DATA_CHANGED, objectId);

	if (operationContextPtr != nullptr){
		AddOperationContextToChangeSet(*operationContextPtr, changeSet);
	}

	istd::CChangeNotifier changeNotifier(this, &changeSet);

	if (ExecuteTransaction(query)){
		return true;
	}

	changeNotifier.Abort();

	return false;
}


imtbase::IObjectCollectionUniquePtr CSqlDatabaseObjectCollectionComp::CreateSubCollection(
			int offset,
			int count,
			const iprm::IParamsSet* selectionParamsPtr) const
{
	if (!m_objectCollectionFactoryCompPtr.IsValid()){
		return nullptr;
	}

	if (!m_dbEngineCompPtr.IsValid()){
		SendCriticalMessage(0, "Invalid component configuration: Database engine missing", "Database collection");

		return 0;
	}

	imtbase::IObjectCollectionUniquePtr collectionPtr = m_objectCollectionFactoryCompPtr.CreateInstance();

	if (m_objectDelegateCompPtr.IsValid()){
		QByteArray objectSelectionQuery = m_objectDelegateCompPtr->GetSelectionQuery(QByteArray(), offset, count, selectionParamsPtr);
		if (objectSelectionQuery.isEmpty()){
			return nullptr;
		}

		QSqlError sqlError;
		QSqlQuery sqlQuery = m_dbEngineCompPtr->ExecSqlQuery(objectSelectionQuery, &sqlError, true);

		while (sqlQuery.next()){
			istd::IChangeableUniquePtr dataPtr = m_objectDelegateCompPtr->CreateObjectFromRecord(sqlQuery.record());

			QByteArray objectId = m_objectDelegateCompPtr->GetObjectIdFromRecord(sqlQuery.record());
			QByteArray typeId = m_objectDelegateCompPtr->GetObjectTypeId(objectId);
			if (collectionPtr.IsValid()){
				collectionPtr->InsertNewObject(typeId, "", "", dataPtr.GetPtr(), objectId);
			}
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

	SendCriticalMessage(0, "Invalid component configuration: Object delegate missing", "Database collection");

	return nullptr;
}


imtbase::ICollectionInfo::Id CSqlDatabaseObjectCollectionComp::GetObjectTypeId(const QByteArray& objectId) const
{
	if (m_objectDelegateCompPtr.IsValid()){
		return m_objectDelegateCompPtr->GetObjectTypeId(objectId);
	}

	SendCriticalMessage(0, "Invalid component configuration: Object delegate missing", "Database collection");

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

int CSqlDatabaseObjectCollectionComp::GetElementsCount(const iprm::IParamsSet* selectionParamPtr, ilog::IMessageConsumer* /*logPtr*/) const
{
	if (!m_objectDelegateCompPtr.IsValid()){
		SendCriticalMessage(0, "Invalid component configuration: Object delegate missing", "Database collection");

		return 0;
	}

	if (!m_dbEngineCompPtr.IsValid()){
		SendCriticalMessage(0, "Invalid component configuration: Database engine missing", "Database collection");

		return 0;
	}

	QByteArray countQuery = m_objectDelegateCompPtr->GetCountQuery(selectionParamPtr);
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
			const iprm::IParamsSet* selectionParamsPtr,
			ilog::IMessageConsumer* /*logPtr*/) const
{
	Ids retVal;

	if (m_objectDelegateCompPtr.IsValid() && m_dbEngineCompPtr.IsValid()){
		QByteArray objectSelectionQuery = m_objectDelegateCompPtr->GetSelectionQuery(QByteArray(), offset, count, selectionParamsPtr);
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
			const iprm::IParamsSet* /*selectionParamsPtr*/,
			ilog::IMessageConsumer* /*logPtr*/) const
{
	return false;
}


imtbase::IObjectCollectionIterator* CSqlDatabaseObjectCollectionComp::CreateObjectCollectionIterator(
			const QByteArray& objectId,
			int offset,
			int count,
			const iprm::IParamsSet* selectionParamsPtr) const
{
	if (m_objectDelegateCompPtr.IsValid() && m_dbEngineCompPtr.IsValid()){
		QByteArray objectSelectionQuery = m_objectDelegateCompPtr->GetSelectionQuery(objectId, offset, count, selectionParamsPtr);
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


QVariant CSqlDatabaseObjectCollectionComp::GetElementInfo(const QByteArray& elementId, int infoType, ilog::IMessageConsumer* /*logPtr*/) const
{
	if (m_objectDelegateCompPtr.IsValid()){
		QSqlRecord record = GetObjectRecord(elementId);
		if (!record.isEmpty()){
			idoc::MetaInfoPtr objectMetaInfoPtr;
			idoc::MetaInfoPtr collectionMetaInfoPtr;

			if (bool isOk = m_objectDelegateCompPtr->CreateObjectInfoFromRecord(record, objectMetaInfoPtr, collectionMetaInfoPtr))
			{
				// #10856
				int metaInfoType = 0;
				QString result;

				switch (infoType){
				case EIT_NAME:
					metaInfoType = idoc::IDocumentMetaInfo::MIT_TITLE;
					break;
				case EIT_DESCRIPTION:
					metaInfoType = idoc::IDocumentMetaInfo::MIT_DESCRIPTION;
					break;
				default:
					return {};
				}

				if (collectionMetaInfoPtr.IsValid()){
					result = collectionMetaInfoPtr->GetMetaInfo(metaInfoType).toString();
				}
				if (result.isEmpty() && objectMetaInfoPtr.IsValid()){
					result = objectMetaInfoPtr->GetMetaInfo(metaInfoType).toString();
				}

				return result;
			}
		}
	}
	else{
		SendCriticalMessage(0, "Invalid component configuration: Object delegate missing", "Database collection");
	}

	return QVariant();
}


idoc::MetaInfoPtr CSqlDatabaseObjectCollectionComp::GetElementMetaInfo(const Id& elementId, ilog::IMessageConsumer* /*logPtr*/) const
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


bool CSqlDatabaseObjectCollectionComp::SetElementName(const Id& elementId, const QString& name, ilog::IMessageConsumer* /*logPtr*/)
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

	istd::IChangeable::ChangeSet changeSet(CF_ELEMENT_RENAMED);
	changeSet.SetChangeInfo(CN_ELEMENT_RENAMED, elementId);

	istd::CChangeNotifier changeNotifier(this, &changeSet);

	if (!ExecuteTransaction(query)){
		changeNotifier.Abort();

		return false;
	}

	return true;
}


bool CSqlDatabaseObjectCollectionComp::SetElementDescription(const Id& elementId, const QString& description, ilog::IMessageConsumer* /*logPtr*/)
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

	istd::IChangeable::ChangeSet changeSet(CF_ELEMENT_DESCRIPTION_CHANGED);
	changeSet.SetChangeInfo(CN_ELEMENT_DESCRIPTION_CHANGED, elementId);

	istd::CChangeNotifier changeNotifier(this, &changeSet);

	if (!ExecuteTransaction(query)){
		changeNotifier.Abort();

		return false;
	}

	return true;
}


bool CSqlDatabaseObjectCollectionComp::SetElementEnabled(const Id& /*objectId*/, bool /*isEnabled*/, ilog::IMessageConsumer* /*logPtr*/)
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

bool CSqlDatabaseObjectCollectionComp::IsObjectTypeSupported(const QByteArray& typeId) const
{
	//! \todo REMOVE IT and replace by checking if the typeId is empty!
	//! Adapt all controllers working with a single document type to use the empty type-ID.
	if (typeId == "DocumentInfo"){
		return true;
	}

	if (m_objectDelegateCompPtr.IsValid()){
		const iprm::IOptionsList* infosPtr = m_objectDelegateCompPtr->GetObjectTypeInfos();
		if (infosPtr == nullptr){
			return true;
		}

		int count = infosPtr->GetOptionsCount();
		for (int i = 0; i < count; i++){
			if (infosPtr->GetOptionId(i) == typeId){
				return true;
			}
		}

		return false;
	}

	return false;
}


bool CSqlDatabaseObjectCollectionComp::ExecuteTransaction(const QByteArray& sqlQuery) const
{
	if (!m_dbEngineCompPtr.IsValid()){
		SendCriticalMessage(0, "Invalid component configuration: Database engine missing", "Database collection");

		return false;
	}

	QStringList queryList = QString::fromUtf8(sqlQuery).split(';');

	if (AreInternalTransactionsEnabled()){
		if (!m_dbEngineCompPtr->BeginTransaction()){
			qCritical() << "Failed to begin SQL transaction with queries:" << sqlQuery;

			return false;
		}
	}

	for (QString& singleQuery: queryList){
		if (!singleQuery.isEmpty()){
			QSqlError error;
			singleQuery = singleQuery.replace('\b', ';');
			m_dbEngineCompPtr->ExecSqlQuery(singleQuery.toUtf8(), &error);
			if (error.type() != QSqlError::NoError){
				SendErrorMessage(0, error.text(), "Database collection");

				qDebug() << "SQL-error: " << singleQuery;

				if (AreInternalTransactionsEnabled()){
					m_dbEngineCompPtr->CancelTransaction();
				}

				return false;
			}
		}
	}

	if (AreInternalTransactionsEnabled()){
		if (!m_dbEngineCompPtr->FinishTransaction()){
			qCritical() << "Failed to finish SQL transaction with queries:" << sqlQuery;

			return false;
		}
	}

	return true;
}


bool CSqlDatabaseObjectCollectionComp::ExecuteTransaction(const QByteArray& sqlQuery, const QVariantMap& bindValues) const
{
	if (!m_dbEngineCompPtr.IsValid()){
		SendCriticalMessage(0, "Invalid component configuration: Database engine missing", "Database collection");

		return false;
	}

	if (AreInternalTransactionsEnabled()){
		if (!m_dbEngineCompPtr->BeginTransaction()){
			qCritical() << "Failed to begin SQL transaction with queries:" << sqlQuery;

			return false;
		}
	}

	QSqlError error;
	m_dbEngineCompPtr->ExecSqlQuery(sqlQuery, bindValues, &error);

	if (error.type() != QSqlError::NoError){
		SendErrorMessage(0, error.text(), "Database collection");
		qDebug() << "SQL-error: " << sqlQuery;

		if (AreInternalTransactionsEnabled()){
			m_dbEngineCompPtr->CancelTransaction();
		}

		return false;
	}

	if (AreInternalTransactionsEnabled()){
		if (!m_dbEngineCompPtr->FinishTransaction()){
			qCritical() << "Failed to finish SQL transaction with queries:" << sqlQuery;

			return false;
		}
	}

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

		if (sqlQuery.next()){
			return sqlQuery.record();
		}
	}

	return QSqlRecord();
}


void CSqlDatabaseObjectCollectionComp::OnDatabaseAccessChanged(
			const istd::IChangeable::ChangeSet& /*changeSet*/,
			const imtdb::IDatabaseLoginSettings* /*databaseAccessSettingsPtr*/)
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

	if (m_databaseAccessSettingsCompPtr.IsValid()){
		m_databaseAccessObserver.RegisterObject(m_databaseAccessSettingsCompPtr.GetPtr(), &CSqlDatabaseObjectCollectionComp::OnDatabaseAccessChanged);
	}

	m_isInitialized = true;
}


void CSqlDatabaseObjectCollectionComp::OnComponentDestroyed()
{
	m_databaseAccessObserver.UnregisterAllObjects();

	BaseClass::OnComponentDestroyed();
}


// private methods

void CSqlDatabaseObjectCollectionComp::AddOperationContextToChangeSet(const imtbase::IOperationContext& operationContext, istd::IChangeable::ChangeSet& changeSet) const
{
	imtbase::IOperationContext::OperationContextInfo info = operationContext.GetOperationOwnerId();
	changeSet.SetChangeInfo(
				imtbase::IOperationContext::OPERATION_CONTEXT_INFO,
				QVariant::fromValue<imtbase::IOperationContext::OperationContextInfo>(info));
}


} // namespace imtdb


