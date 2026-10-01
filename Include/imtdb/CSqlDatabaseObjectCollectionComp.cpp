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
#include <imtdb/imtdb.h>


namespace imtdb
{


// public methods

CSqlDatabaseObjectCollectionComp::CSqlDatabaseObjectCollectionComp()
	:m_databaseAccessObserver(*this),
	m_isInitialized(false)
{
}


// reimplemented (imtbase::ITenantCollectionProvider)

bool CSqlDatabaseObjectCollectionComp::IsTenantSeparated() const
{
	return m_objectDelegateCompPtr.IsValid() && m_objectDelegateCompPtr->HasTenantStorage();
}


imtbase::ITenantObjectCollection* CSqlDatabaseObjectCollectionComp::GetTenantCollection(const QByteArray& tenantId) const
{
	QMutexLocker locker(&m_tenantCollectionsMutex);

	std::unique_ptr<CTenantCollection>& collectionPtr = m_tenantCollections[tenantId];
	if (!collectionPtr){
		collectionPtr.reset(new CTenantCollection(const_cast<CSqlDatabaseObjectCollectionComp&>(*this), tenantId));
	}

	return collectionPtr.get();
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
	return DoInsertNewObject(*this, QByteArray(), typeId, name, description, defaultValuePtr, proposedObjectId, dataMetaInfoPtr, collectionItemMetaInfoPtr, operationContextPtr);
}


bool CSqlDatabaseObjectCollectionComp::RemoveElements(const Ids& elementIds, const imtbase::IOperationContext* operationContextPtr)
{
	return DoRemoveElements(*this, QByteArray(), elementIds, operationContextPtr);
}


bool CSqlDatabaseObjectCollectionComp::RemoveElementSet(
			const iprm::IParamsSet* selectionParamsPtr,
			const imtbase::IOperationContext* operationContextPtr)
{
	return DoRemoveElementSet(*this, QByteArray(), selectionParamsPtr, operationContextPtr);
}


bool CSqlDatabaseObjectCollectionComp::RestoreObjects(
			const Ids& objectIds,
			const imtbase::IOperationContext* operationContextPtr)
{
	return DoRestoreObjects(*this, QByteArray(), objectIds, operationContextPtr);
}


bool CSqlDatabaseObjectCollectionComp::RestoreObjectSet(
			const iprm::IParamsSet* selectionParamsPtr,
			const imtbase::IOperationContext* operationContextPtr)
{
	return DoRestoreObjectSet(*this, QByteArray(), selectionParamsPtr, operationContextPtr);
}


const istd::IChangeable* CSqlDatabaseObjectCollectionComp::GetObjectPtr(const QByteArray& /*objectId*/) const
{
	return nullptr;
}


bool CSqlDatabaseObjectCollectionComp::GetObjectData(const QByteArray& objectId, DataPtr& dataPtr, const iprm::IParamsSet* dataConfigurationPtr) const
{
	return DoGetObjectData(QByteArray(), objectId, dataPtr, dataConfigurationPtr);
}


bool CSqlDatabaseObjectCollectionComp::SetObjectData(
			const Id& objectId,
			const istd::IChangeable& object,
			CompatibilityMode /*mode*/,
			const imtbase::IOperationContext* operationContextPtr)
{
	return DoSetObjectData(*this, QByteArray(), objectId, object, operationContextPtr);
}


imtbase::IObjectCollectionUniquePtr CSqlDatabaseObjectCollectionComp::CreateSubCollection(
			int offset,
			int count,
			const iprm::IParamsSet* selectionParamsPtr) const
{
	return DoCreateSubCollection(QByteArray(), offset, count, selectionParamsPtr);
}


imtbase::IObjectCollectionIterator* CSqlDatabaseObjectCollectionComp::CreateObjectCollectionIterator(
			const QByteArray& objectId,
			int offset,
			int count,
			const iprm::IParamsSet* selectionParamsPtr) const
{
	return DoCreateObjectCollectionIterator(QByteArray(), objectId, offset, count, selectionParamsPtr);
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
	return DoGetObjectTypeId(QByteArray(), objectId);
}


idoc::MetaInfoPtr CSqlDatabaseObjectCollectionComp::GetDataMetaInfo(const Id& objectId) const
{
	return DoGetDataMetaInfo(QByteArray(), objectId);
}


// reimplemented (ICollectionInfo)

int CSqlDatabaseObjectCollectionComp::GetElementsCount(const iprm::IParamsSet* selectionParamPtr, ilog::IMessageConsumer* /*logPtr*/) const
{
	return DoGetElementsCount(QByteArray(), selectionParamPtr);
}


imtbase::ICollectionInfo::Ids CSqlDatabaseObjectCollectionComp::GetElementIds(
			int offset,
			int count,
			const iprm::IParamsSet* selectionParamsPtr,
			ilog::IMessageConsumer* /*logPtr*/) const
{
	return DoGetElementIds(QByteArray(), offset, count, selectionParamsPtr);
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


QVariant CSqlDatabaseObjectCollectionComp::GetElementInfo(const QByteArray& elementId, int infoType, ilog::IMessageConsumer* /*logPtr*/) const
{
	return DoGetElementInfo(QByteArray(), elementId, infoType);
}


idoc::MetaInfoPtr CSqlDatabaseObjectCollectionComp::GetElementMetaInfo(const Id& elementId, ilog::IMessageConsumer* /*logPtr*/) const
{
	return DoGetElementMetaInfo(QByteArray(), elementId);
}


bool CSqlDatabaseObjectCollectionComp::SetElementName(const Id& elementId, const QString& name, ilog::IMessageConsumer* /*logPtr*/)
{
	return DoSetElementName(*this, QByteArray(), elementId, name);
}


bool CSqlDatabaseObjectCollectionComp::SetElementDescription(const Id& elementId, const QString& description, ilog::IMessageConsumer* /*logPtr*/)
{
	return DoSetElementDescription(*this, QByteArray(), elementId, description);
}


bool CSqlDatabaseObjectCollectionComp::SetElementEnabled(const Id& /*objectId*/, bool /*isEnabled*/, ilog::IMessageConsumer* /*logPtr*/)
{
	return false;
}


// reimplemented (istd::IChangeable)

bool CSqlDatabaseObjectCollectionComp::ResetData(CompatibilityMode /*mode*/)
{
	return DoResetData(*this, QByteArray());
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


QByteArray CSqlDatabaseObjectCollectionComp::DoInsertNewObject(
			imtbase::IObjectCollection& collection,
			const QByteArray& tenantId,
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
		SendErrorMessage(0, QStringLiteral(R"(Object type ID "%1" not supported)").arg(typeId));

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

	if (!ApplyTenant(objectQuery.query, tenantId, operationContextPtr)){
		return QByteArray();
	}

	istd::IChangeable::ChangeSet changeSet(CF_ADDED);
	changeSet.SetChangeInfo(CN_ELEMENT_INSERTED, objectId);

	if (operationContextPtr != nullptr){
		AddOperationContextToChangeSet(*operationContextPtr, changeSet);
	}

	istd::CChangeNotifier changeNotifier(&collection, &changeSet);

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
			QByteArray metaQuery = m_objectDelegateCompPtr->CreateDataMetaInfoQuery(collection, objectId, dataMetaInfoPtr);
			if(!metaQuery.isEmpty() && ApplyTenant(metaQuery, tenantId, operationContextPtr)){
				if(bindValues.isEmpty()){
					ExecuteTransaction(metaQuery);
				}
				else{
					ExecuteTransaction(metaQuery, bindValues);
				}
			}
		}
		if (collectionItemMetaInfoPtr != nullptr){
			QByteArray collectionItemMetaQuery = m_objectDelegateCompPtr->CreateCollectionItemMetaInfoQuery(collection, objectId, collectionItemMetaInfoPtr);
			if(!collectionItemMetaQuery.isEmpty() && ApplyTenant(collectionItemMetaQuery, tenantId, operationContextPtr)){
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


bool CSqlDatabaseObjectCollectionComp::DoRemoveElements(
			imtbase::IObjectCollection& collection,
			const QByteArray& tenantId,
			const Ids& elementIds,
			const imtbase::IOperationContext* operationContextPtr)
{
	if (!m_objectDelegateCompPtr.IsValid()){
		return false;
	}

	QByteArray query = m_objectDelegateCompPtr->CreateDeleteObjectsQuery(collection, elementIds, operationContextPtr);
	if (query.isEmpty()){
		SendErrorMessage(0, "Database query could not be created", "Database collection");

		return false;
	}

	if (!ApplyTenant(query, tenantId, operationContextPtr)){
		return false;
	}

	imtbase::ICollectionInfo::MultiElementNotifierInfo notifierInfo;
	notifierInfo.elementIds = elementIds;

	istd::IChangeable::ChangeSet changeSet(CF_REMOVED);
	changeSet.SetChangeInfo(CN_ELEMENTS_REMOVED, QVariant::fromValue(notifierInfo));

	if (operationContextPtr != nullptr){
		AddOperationContextToChangeSet(*operationContextPtr, changeSet);
	}

	istd::CChangeNotifier changeNotifier(&collection, &changeSet);

	if (ExecuteTransaction(query)){
		return true;
	}

	changeNotifier.Abort();

	return false;
}


bool CSqlDatabaseObjectCollectionComp::DoRemoveElementSet(
			imtbase::IObjectCollection& collection,
			const QByteArray& tenantId,
			const iprm::IParamsSet* selectionParamsPtr,
			const imtbase::IOperationContext* operationContextPtr)
{
	if (!m_objectDelegateCompPtr.IsValid()){
		return false;
	}

	QByteArray query = m_objectDelegateCompPtr->CreateDeleteObjectSetQuery(collection, selectionParamsPtr, operationContextPtr);
	if (query.isEmpty()){
		SendErrorMessage(0, "Database query could not be created", "Database collection");

		return false;
	}

	if (!ApplyTenant(query, tenantId, operationContextPtr)){
		return false;
	}

	imtbase::ICollectionInfo::MultiElementNotifierInfo notifierInfo;
	notifierInfo.elementIds = DoGetElementIds(tenantId, 0, -1, selectionParamsPtr);

	istd::IChangeable::ChangeSet changeSet(CF_REMOVED);
	changeSet.SetChangeInfo(CN_ELEMENTS_REMOVED, QVariant::fromValue(notifierInfo));

	if (operationContextPtr != nullptr){
		AddOperationContextToChangeSet(*operationContextPtr, changeSet);
	}

	istd::CChangeNotifier changeNotifier(&collection, &changeSet);

	if (ExecuteTransaction(query)){
		return true;
	}

	changeNotifier.Abort();

	return false;
}


bool CSqlDatabaseObjectCollectionComp::DoRestoreObjects(
			imtbase::IObjectCollection& collection,
			const QByteArray& tenantId,
			const Ids& objectIds,
			const imtbase::IOperationContext* operationContextPtr)
{
	if (!m_objectDelegateCompPtr.IsValid()){
		return false;
	}

	QByteArray query = m_objectDelegateCompPtr->CreateRestoreObjectsQuery(collection, objectIds, operationContextPtr);
	if (query.isEmpty()){
		SendErrorMessage(0, "Database query could not be created", "Database collection");

		return false;
	}

	if (!ApplyTenant(query, tenantId, operationContextPtr)){
		return false;
	}

	imtbase::ICollectionInfo::MultiElementNotifierInfo notifierInfo;
	notifierInfo.elementIds = objectIds;

	istd::IChangeable::ChangeSet changeSet(CF_RESTORED);
	changeSet.SetChangeInfo(CN_ELEMENTS_RESTORED, QVariant::fromValue(notifierInfo));

	if (operationContextPtr != nullptr){
		AddOperationContextToChangeSet(*operationContextPtr, changeSet);
	}

	istd::CChangeNotifier changeNotifier(&collection, &changeSet);

	if (ExecuteTransaction(query)){
		return true;
	}

	changeNotifier.Abort();

	return false;
}


bool CSqlDatabaseObjectCollectionComp::DoRestoreObjectSet(
			imtbase::IObjectCollection& collection,
			const QByteArray& tenantId,
			const iprm::IParamsSet* selectionParamsPtr,
			const imtbase::IOperationContext* operationContextPtr)
{
	if (!m_objectDelegateCompPtr.IsValid()){
		return false;
	}

	QByteArray query = m_objectDelegateCompPtr->CreateRestoreObjectSetQuery(collection, selectionParamsPtr, operationContextPtr);
	if (query.isEmpty()){
		SendErrorMessage(0, "Database query could not be created", "Database collection");

		return false;
	}

	if (!ApplyTenant(query, tenantId, operationContextPtr)){
		return false;
	}

	imtbase::ICollectionInfo::MultiElementNotifierInfo notifierInfo;
	notifierInfo.elementIds = imtbase::ICollectionInfo::Ids();

	istd::IChangeable::ChangeSet changeSet(CF_RESTORED);
	changeSet.SetChangeInfo(CN_ALL_CHANGED, QVariant::fromValue(notifierInfo));

	if (operationContextPtr != nullptr){
		AddOperationContextToChangeSet(*operationContextPtr, changeSet);
	}

	istd::CChangeNotifier changeNotifier(&collection, &changeSet);

	if (ExecuteTransaction(query)){
		return true;
	}

	changeNotifier.Abort();

	return false;
}


bool CSqlDatabaseObjectCollectionComp::DoGetObjectData(const QByteArray& tenantId, const QByteArray& objectId, DataPtr& dataPtr, const iprm::IParamsSet* dataConfigurationPtr) const
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
	if (objectSelectionQuery.isEmpty() || !ApplyTenant(objectSelectionQuery, tenantId)){
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
	dataPtr.FromUnique(std::move(objectPtr));

	return dataPtr.IsValid();
}


bool CSqlDatabaseObjectCollectionComp::DoSetObjectData(
			imtbase::IObjectCollection& collection,
			const QByteArray& tenantId,
			const Id& objectId,
			const istd::IChangeable& object,
			const imtbase::IOperationContext* operationContextPtr)
{
	if (!m_objectDelegateCompPtr.IsValid()){
		return false;
	}

	auto objectQuery = m_objectDelegateCompPtr->CreateUpdateObjectQueryWithParameters(collection, objectId, object, operationContextPtr);
	if (objectQuery.query.isEmpty()){
		SendErrorMessage(0, "Database query could not be created", "Database collection");

		return false;
	}

	if (!ApplyTenant(objectQuery.query, tenantId, operationContextPtr)){
		return false;
	}

	istd::IChangeable::ChangeSet changeSet(CF_OBJECT_DATA_CHANGED);
	changeSet.SetChangeInfo(CN_OBJECT_DATA_CHANGED, objectId);

	if (operationContextPtr != nullptr){
		AddOperationContextToChangeSet(*operationContextPtr, changeSet);
	}

	istd::CChangeNotifier changeNotifier(&collection, &changeSet);

	const bool transactionSuccess = objectQuery.bindValues.isEmpty()
										? ExecuteTransaction(objectQuery.query)
										: ExecuteTransaction(objectQuery.query, objectQuery.bindValues);
	if (transactionSuccess){
		return true;
	}

	changeNotifier.Abort();

	return false;
}


imtbase::IObjectCollectionUniquePtr CSqlDatabaseObjectCollectionComp::DoCreateSubCollection(
			const QByteArray& tenantId,
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
		if (objectSelectionQuery.isEmpty() || !ApplyTenant(objectSelectionQuery, tenantId)){
			return nullptr;
		}

		QSqlError sqlError;
		QSqlQuery sqlQuery = m_dbEngineCompPtr->ExecSqlQuery(objectSelectionQuery, &sqlError, true);

		while (sqlQuery.next()){
			istd::IChangeableUniquePtr dataPtr = m_objectDelegateCompPtr->CreateObjectFromRecord(sqlQuery.record());

			QByteArray objectId = m_objectDelegateCompPtr->GetObjectIdFromRecord(sqlQuery.record());
			QByteArray typeId = DoGetObjectTypeId(tenantId, objectId);
			if (collectionPtr.IsValid()){
				collectionPtr->InsertNewObject(typeId, "", "", dataPtr.GetPtr(), objectId);
			}
		}
	}

	return collectionPtr;
}


imtbase::IObjectCollectionIterator* CSqlDatabaseObjectCollectionComp::DoCreateObjectCollectionIterator(
			const QByteArray& tenantId,
			const QByteArray& objectId,
			int offset,
			int count,
			const iprm::IParamsSet* selectionParamsPtr) const
{
	if (!m_objectDelegateCompPtr.IsValid() || !m_dbEngineCompPtr.IsValid()){
		return nullptr;
	}

	QByteArray baseSelectionQuery =
			m_objectDelegateCompPtr->GetSelectionQuery(objectId, 0, -1, selectionParamsPtr);

	if (baseSelectionQuery.isEmpty() || !ApplyTenant(baseSelectionQuery, tenantId)){
		return nullptr;
	}

	QString queryWithTotalCount = QStringLiteral(
										"SELECT *, COUNT(*) OVER() AS \"TotalCount\" FROM (%1) AS _base")
										.arg(baseSelectionQuery);

	const QByteArray driverId = m_dbEngineCompPtr->GetDatabaseDriverId();
	const bool isSQLite = (driverId == "QSQLITE");

	if (count > 0){
		if (isSQLite){
			queryWithTotalCount += QStringLiteral(" LIMIT %1 OFFSET %2")
									.arg(QString::number(count), QString::number(qMax(0, offset)));
		}
		else{
			queryWithTotalCount += QStringLiteral(" OFFSET %1 ROWS FETCH NEXT %2 ROWS ONLY")
									.arg(QString::number(qMax(0, offset)), QString::number(count));
		}
	}
	else if (offset > 0 && isSQLite){
		queryWithTotalCount += QStringLiteral(" LIMIT -1 OFFSET %1")
								.arg(offset);
	}

	QSqlError sqlError;
	QSqlQuery sqlQuery = m_dbEngineCompPtr->ExecSqlQuery(queryWithTotalCount.toUtf8(), &sqlError, true);

	if (sqlError.type() != QSqlError::NoError){
		SendErrorMessage(0, sqlError.text(), "Database collection");
		qDebug() << "SQL-error" << queryWithTotalCount;
		return nullptr;
	}

	return new CSqlDatabaseObjectCollectionIterator(
				sqlQuery,
				m_objectDelegateCompPtr.GetPtr());
}


imtbase::ICollectionInfo::Id CSqlDatabaseObjectCollectionComp::DoGetObjectTypeId(const QByteArray& tenantId, const QByteArray& objectId) const
{
	if (!m_objectDelegateCompPtr.IsValid()){
		SendCriticalMessage(0, "Invalid component configuration: Object delegate missing", "Database collection");

		return QByteArray();
	}

	if (tenantId.isEmpty()){
		return m_objectDelegateCompPtr->GetObjectTypeId(objectId);
	}

	QSqlRecord record = GetObjectRecord(tenantId, objectId);
	if (record.isEmpty()){
		return QByteArray();
	}

	QByteArray typeId = m_objectDelegateCompPtr->GetObjectTypeIdFromRecord(record);
	if (typeId.isEmpty()){
		// a collection of a single object type does not store the type
		const iprm::IOptionsList* typesPtr = m_objectDelegateCompPtr->GetObjectTypeInfos();
		if ((typesPtr != nullptr) && (typesPtr->GetOptionsCount() == 1)){
			typeId = typesPtr->GetOptionId(0);
		}
	}

	return typeId;
}


idoc::MetaInfoPtr CSqlDatabaseObjectCollectionComp::DoGetDataMetaInfo(const QByteArray& tenantId, const Id& objectId) const
{
	if (m_objectDelegateCompPtr.IsValid()){
		QSqlRecord record = GetObjectRecord(tenantId, objectId);
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


int CSqlDatabaseObjectCollectionComp::DoGetElementsCount(const QByteArray& tenantId, const iprm::IParamsSet* selectionParamPtr) const
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
		if (!ApplyTenant(countQuery, tenantId)){
			return 0;
		}

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


imtbase::ICollectionInfo::Ids CSqlDatabaseObjectCollectionComp::DoGetElementIds(
			const QByteArray& tenantId,
			int offset,
			int count,
			const iprm::IParamsSet* selectionParamsPtr) const
{
	Ids retVal;

	if (m_objectDelegateCompPtr.IsValid() && m_dbEngineCompPtr.IsValid()){
		QByteArray objectSelectionQuery = m_objectDelegateCompPtr->GetSelectionQuery(QByteArray(), offset, count, selectionParamsPtr);
		if (objectSelectionQuery.isEmpty() || !ApplyTenant(objectSelectionQuery, tenantId)){
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


QVariant CSqlDatabaseObjectCollectionComp::DoGetElementInfo(const QByteArray& tenantId, const QByteArray& elementId, int infoType) const
{
	if (m_objectDelegateCompPtr.IsValid()){
		QSqlRecord record = GetObjectRecord(tenantId, elementId);
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


idoc::MetaInfoPtr CSqlDatabaseObjectCollectionComp::DoGetElementMetaInfo(const QByteArray& tenantId, const Id& elementId) const
{
	if (m_objectDelegateCompPtr.IsValid()){
		QSqlRecord record = GetObjectRecord(tenantId, elementId);
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


bool CSqlDatabaseObjectCollectionComp::DoSetElementName(imtbase::IObjectCollection& collection, const QByteArray& tenantId, const Id& elementId, const QString& name)
{
	if (!m_objectDelegateCompPtr.IsValid()){
		SendCriticalMessage(0, "Invalid component configuration: Object delegate missing", "Database collection");

		return false;
	}

	if (elementId.isEmpty()){
		return false;
	}

	QString escapedName = imtdb::SqlEncode(name);
	QByteArray query = m_objectDelegateCompPtr->CreateRenameObjectQuery(collection, elementId, escapedName, nullptr);
	if (query.isEmpty()){
		SendErrorMessage(0, "Database query could not be created", "Database collection");

		return false;
	}

	if (!ApplyTenant(query, tenantId)){
		return false;
	}

	istd::IChangeable::ChangeSet changeSet(CF_ELEMENT_RENAMED);
	changeSet.SetChangeInfo(CN_ELEMENT_RENAMED, elementId);

	istd::CChangeNotifier changeNotifier(&collection, &changeSet);

	if (!ExecuteTransaction(query)){
		changeNotifier.Abort();

		return false;
	}

	return true;
}


bool CSqlDatabaseObjectCollectionComp::DoSetElementDescription(imtbase::IObjectCollection& collection, const QByteArray& tenantId, const Id& elementId, const QString& description)
{
	if (!m_objectDelegateCompPtr.IsValid()){
		SendCriticalMessage(0, "Invalid component configuration: Object delegate missing", "Database collection");

		return false;
	}
	QString escapedDescription = imtdb::SqlEncode(description);
	if (escapedDescription.length() > *m_maxLengthCommentAttrPtr){
		escapedDescription = escapedDescription.left(*m_maxLengthCommentAttrPtr);
		// Ensure we don't split an escaped quote pair ('')
		while (escapedDescription.endsWith(QLatin1Char('\'')) && escapedDescription.count(QLatin1Char('\'')) % 2 != 0){
			escapedDescription.chop(1);
		}
	}

	QByteArray query = m_objectDelegateCompPtr->CreateDescriptionObjectQuery(collection, elementId, escapedDescription, nullptr);
	if (query.isEmpty()){
		SendErrorMessage(0, "Database query could not be created", "Database collection");

		return false;
	}

	if (!ApplyTenant(query, tenantId)){
		return false;
	}

	istd::IChangeable::ChangeSet changeSet(CF_ELEMENT_DESCRIPTION_CHANGED);
	changeSet.SetChangeInfo(CN_ELEMENT_DESCRIPTION_CHANGED, elementId);

	istd::CChangeNotifier changeNotifier(&collection, &changeSet);

	if (!ExecuteTransaction(query)){
		changeNotifier.Abort();

		return false;
	}

	return true;
}


bool CSqlDatabaseObjectCollectionComp::DoResetData(imtbase::IObjectCollection& collection, const QByteArray& tenantId)
{
	if (!m_objectDelegateCompPtr.IsValid()){
		SendCriticalMessage(0, "Invalid component configuration: Object delegate missing", "Database collection");

		return false;
	}

	QByteArray resetQuery = m_objectDelegateCompPtr->CreateResetQuery(collection);
	if (resetQuery.isEmpty()){
		SendErrorMessage(0, "Database query could not be created", "Database collection");

		return false;
	}

	if (!ApplyTenant(resetQuery, tenantId)){
		return false;
	}

	if (ExecuteTransaction(resetQuery)){
		istd::CChangeNotifier changeNotifier(&collection);

		return true;
	}

	return false;
}


QSqlRecord CSqlDatabaseObjectCollectionComp::GetObjectRecord(const QByteArray& tenantId, const QByteArray& objectId) const
{
	if (!m_objectDelegateCompPtr.IsValid()){
		SendCriticalMessage(0, "Invalid component configuration: Object delegate missing", "Database collection");

		return QSqlRecord();
	}

	if (!m_dbEngineCompPtr.IsValid()){
		SendCriticalMessage(0, "Invalid component configuration: Database engine missing", "Database collection");

		return QSqlRecord();
	}

	if (objectId.isEmpty()){
		return QSqlRecord();
	}

	QByteArray objectSelectionQuery = m_objectDelegateCompPtr->GetSelectionQuery(objectId);
	if (!objectSelectionQuery.isEmpty() && ApplyTenant(objectSelectionQuery, tenantId)){
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


bool CSqlDatabaseObjectCollectionComp::ApplyTenant(QByteArray& query, const QByteArray& tenantId, const imtbase::IOperationContext* operationContextPtr) const
{
	if ((operationContextPtr != nullptr) && query.contains(ISqlDatabaseObjectDelegate::s_tenantSchemePrefixPlaceholder)){
		const QByteArray operationTenantId = operationContextPtr->GetTenantId();
		if (!operationTenantId.isEmpty() && (operationTenantId != tenantId)){
			SendErrorMessage(0, QStringLiteral("Tenant storage access denied: operation of tenant '%1' on the data of tenant '%2'").arg(QString(operationTenantId), QString(tenantId)), "Database collection");

			return false;
		}
	}

	return m_objectDelegateCompPtr->ApplyTenantStorage(query, tenantId);
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

	{
		QMutexLocker locker(&m_tenantCollectionsMutex);

		m_tenantCollections.clear();
	}

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


// public methods of the embedded class CTenantCollection

CSqlDatabaseObjectCollectionComp::CTenantCollection::CTenantCollection(CSqlDatabaseObjectCollectionComp& parent, const QByteArray& tenantId)
	:m_parent(parent),
	m_tenantId(tenantId)
{
}


// reimplemented (imtbase::ITenantObjectCollection)

QByteArray CSqlDatabaseObjectCollectionComp::CTenantCollection::GetTenantId() const
{
	return m_tenantId;
}


// reimplemented (ISqlDatabaseObjectCollection)

QByteArray CSqlDatabaseObjectCollectionComp::CTenantCollection::GetDatabaseId() const
{
	return m_parent.GetDatabaseId();
}


bool CSqlDatabaseObjectCollectionComp::CTenantCollection::AreInternalTransactionsEnabled() const
{
	return m_parent.AreInternalTransactionsEnabled();
}


bool CSqlDatabaseObjectCollectionComp::CTenantCollection::SetInternalTransactionsEnabled(bool isEnabled)
{
	return m_parent.SetInternalTransactionsEnabled(isEnabled);
}


// reimplemented (imtbase::IObjectCollection)

const imtbase::IRevisionController* CSqlDatabaseObjectCollectionComp::CTenantCollection::GetRevisionController() const
{
	return m_parent.GetRevisionController();
}


const imtbase::ICollectionDataController* CSqlDatabaseObjectCollectionComp::CTenantCollection::GetDataController() const
{
	return m_parent.GetDataController();
}


int CSqlDatabaseObjectCollectionComp::CTenantCollection::GetOperationFlags(const QByteArray& objectId) const
{
	return m_parent.GetOperationFlags(objectId);
}


QByteArray CSqlDatabaseObjectCollectionComp::CTenantCollection::InsertNewObject(
			const QByteArray& typeId,
			const QString& name,
			const QString& description,
			const istd::IChangeable* defaultValuePtr,
			const QByteArray& proposedObjectId,
			const idoc::IDocumentMetaInfo* dataMetaInfoPtr,
			const idoc::IDocumentMetaInfo* elementMetaInfoPtr,
			const imtbase::IOperationContext* operationContextPtr)
{
	return m_parent.DoInsertNewObject(*this, m_tenantId, typeId, name, description, defaultValuePtr, proposedObjectId, dataMetaInfoPtr, elementMetaInfoPtr, operationContextPtr);
}


bool CSqlDatabaseObjectCollectionComp::CTenantCollection::RemoveElements(const Ids& elementIds, const imtbase::IOperationContext* operationContextPtr)
{
	return m_parent.DoRemoveElements(*this, m_tenantId, elementIds, operationContextPtr);
}


bool CSqlDatabaseObjectCollectionComp::CTenantCollection::RemoveElementSet(
			const iprm::IParamsSet* selectionParamsPtr,
			const imtbase::IOperationContext* operationContextPtr)
{
	return m_parent.DoRemoveElementSet(*this, m_tenantId, selectionParamsPtr, operationContextPtr);
}


bool CSqlDatabaseObjectCollectionComp::CTenantCollection::RestoreObjects(
			const Ids& objectIds,
			const imtbase::IOperationContext* operationContextPtr)
{
	return m_parent.DoRestoreObjects(*this, m_tenantId, objectIds, operationContextPtr);
}


bool CSqlDatabaseObjectCollectionComp::CTenantCollection::RestoreObjectSet(
			const iprm::IParamsSet* selectionParamsPtr,
			const imtbase::IOperationContext* operationContextPtr)
{
	return m_parent.DoRestoreObjectSet(*this, m_tenantId, selectionParamsPtr, operationContextPtr);
}


const istd::IChangeable* CSqlDatabaseObjectCollectionComp::CTenantCollection::GetObjectPtr(const QByteArray& /*objectId*/) const
{
	return nullptr;
}


bool CSqlDatabaseObjectCollectionComp::CTenantCollection::GetObjectData(const QByteArray& objectId, DataPtr& dataPtr, const iprm::IParamsSet* dataConfigurationPtr) const
{
	return m_parent.DoGetObjectData(m_tenantId, objectId, dataPtr, dataConfigurationPtr);
}


bool CSqlDatabaseObjectCollectionComp::CTenantCollection::SetObjectData(
			const Id& objectId,
			const istd::IChangeable& object,
			CompatibilityMode /*mode*/,
			const imtbase::IOperationContext* operationContextPtr)
{
	return m_parent.DoSetObjectData(*this, m_tenantId, objectId, object, operationContextPtr);
}


imtbase::IObjectCollectionUniquePtr CSqlDatabaseObjectCollectionComp::CTenantCollection::CreateSubCollection(
			int offset,
			int count,
			const iprm::IParamsSet* selectionParamsPtr) const
{
	return m_parent.DoCreateSubCollection(m_tenantId, offset, count, selectionParamsPtr);
}


imtbase::IObjectCollectionIterator* CSqlDatabaseObjectCollectionComp::CTenantCollection::CreateObjectCollectionIterator(
			const QByteArray& objectId,
			int offset,
			int count,
			const iprm::IParamsSet* selectionParamsPtr) const
{
	return m_parent.DoCreateObjectCollectionIterator(m_tenantId, objectId, offset, count, selectionParamsPtr);
}


// reimplemented (IObjectCollectionInfo)

const iprm::IOptionsList* CSqlDatabaseObjectCollectionComp::CTenantCollection::GetObjectTypesInfo() const
{
	return m_parent.GetObjectTypesInfo();
}


imtbase::ICollectionInfo::Id CSqlDatabaseObjectCollectionComp::CTenantCollection::GetObjectTypeId(const QByteArray& objectId) const
{
	return m_parent.DoGetObjectTypeId(m_tenantId, objectId);
}


idoc::MetaInfoPtr CSqlDatabaseObjectCollectionComp::CTenantCollection::GetDataMetaInfo(const Id& objectId) const
{
	return m_parent.DoGetDataMetaInfo(m_tenantId, objectId);
}


// reimplemented (ICollectionInfo)

int CSqlDatabaseObjectCollectionComp::CTenantCollection::GetElementsCount(const iprm::IParamsSet* selectionParamPtr, ilog::IMessageConsumer* /*logPtr*/) const
{
	return m_parent.DoGetElementsCount(m_tenantId, selectionParamPtr);
}


imtbase::ICollectionInfo::Ids CSqlDatabaseObjectCollectionComp::CTenantCollection::GetElementIds(
			int offset,
			int count,
			const iprm::IParamsSet* selectionParamsPtr,
			ilog::IMessageConsumer* /*logPtr*/) const
{
	return m_parent.DoGetElementIds(m_tenantId, offset, count, selectionParamsPtr);
}


bool CSqlDatabaseObjectCollectionComp::CTenantCollection::GetSubsetInfo(
			imtbase::ICollectionInfo& /*subsetInfo*/,
			int /*offset*/,
			int /*count*/,
			const iprm::IParamsSet* /*selectionParamsPtr*/,
			ilog::IMessageConsumer* /*logPtr*/) const
{
	return false;
}


QVariant CSqlDatabaseObjectCollectionComp::CTenantCollection::GetElementInfo(const QByteArray& elementId, int infoType, ilog::IMessageConsumer* /*logPtr*/) const
{
	return m_parent.DoGetElementInfo(m_tenantId, elementId, infoType);
}


idoc::MetaInfoPtr CSqlDatabaseObjectCollectionComp::CTenantCollection::GetElementMetaInfo(const Id& elementId, ilog::IMessageConsumer* /*logPtr*/) const
{
	return m_parent.DoGetElementMetaInfo(m_tenantId, elementId);
}


bool CSqlDatabaseObjectCollectionComp::CTenantCollection::SetElementName(const Id& elementId, const QString& name, ilog::IMessageConsumer* /*logPtr*/)
{
	return m_parent.DoSetElementName(*this, m_tenantId, elementId, name);
}


bool CSqlDatabaseObjectCollectionComp::CTenantCollection::SetElementDescription(const Id& elementId, const QString& description, ilog::IMessageConsumer* /*logPtr*/)
{
	return m_parent.DoSetElementDescription(*this, m_tenantId, elementId, description);
}


bool CSqlDatabaseObjectCollectionComp::CTenantCollection::SetElementEnabled(const Id& /*elementId*/, bool /*isEnabled*/, ilog::IMessageConsumer* /*logPtr*/)
{
	return false;
}


// reimplemented (istd::IChangeable)

bool CSqlDatabaseObjectCollectionComp::CTenantCollection::ResetData(CompatibilityMode /*mode*/)
{
	return m_parent.DoResetData(*this, m_tenantId);
}


void CSqlDatabaseObjectCollectionComp::CTenantCollection::BeginChanges(const ChangeSet& changeSet)
{
	m_parent.BeginChanges(CreateTenantChangeSet(changeSet));
}


void CSqlDatabaseObjectCollectionComp::CTenantCollection::EndChanges(const ChangeSet& changeSet)
{
	m_parent.EndChanges(CreateTenantChangeSet(changeSet));
}


void CSqlDatabaseObjectCollectionComp::CTenantCollection::BeginChangeGroup(const ChangeSet& changeSet)
{
	m_parent.BeginChangeGroup(CreateTenantChangeSet(changeSet));
}


void CSqlDatabaseObjectCollectionComp::CTenantCollection::EndChangeGroup(const ChangeSet& changeSet)
{
	m_parent.EndChangeGroup(CreateTenantChangeSet(changeSet));
}


// private methods of the embedded class CTenantCollection

istd::IChangeable::ChangeSet CSqlDatabaseObjectCollectionComp::CTenantCollection::CreateTenantChangeSet(const ChangeSet& changeSet) const
{
	ChangeSet tenantChangeSet(changeSet);
	tenantChangeSet.SetChangeInfo(CN_TENANT_ID, m_tenantId);

	return tenantChangeSet;
}


} // namespace imtdb


