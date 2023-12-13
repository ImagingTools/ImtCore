#include <imtdb/CSqlStructureDelegateCompBase.h>

// Qt includes
#include <QtCore/QUuid>


namespace imtdb
{


// public methods

// reimplemented (IGqlStructureDelegate)

imtbase::ICollectionStructureInfo::Id CSqlStructureDelegateCompBase::InsertNewNode(
			const QString& name,
			const QString& description,
			const Id& proposedNodeId,
			const Id& parentNodeId,
			const idoc::IDocumentMetaInfo* metaInfoPtr,
			const imtbase::IOperationContext* operationContextPtr)
{
	QByteArray nodeId = proposedNodeId;
	if (nodeId.isEmpty()){
		nodeId = QUuid::createUuid().toByteArray(QUuid::WithoutBraces);
	}

	QByteArray query = CreateInsertNewNodeQuery(
			name,
			description,
			nodeId,
			parentNodeId,
			metaInfoPtr,
			operationContextPtr);
	if (query.isEmpty()){
		SendErrorMessage(0, "Database query could not be created", "Hierarchical structure info");

		return QByteArray();
	}

	if (ExecuteTransaction(query)){
		return nodeId;
	}

	return QByteArray();
}


bool CSqlStructureDelegateCompBase::SetNodeName(
			const Id& nodeId,
			const QString& name,
			const imtbase::IOperationContext* operationContextPtr)
{
	QByteArray query = CreateSetNodeNameQuery(
					nodeId,
					name,
					operationContextPtr);
	if (query.isEmpty()){
		SendErrorMessage(0, "Database query could not be created", "Hierarchical structure info");

		return false;
	}

	if (ExecuteTransaction(query)){
		return true;
	}

	return false;
}


bool CSqlStructureDelegateCompBase::SetNodeDescription(const Id& nodeId, const QString& description, const imtbase::IOperationContext* operationContextPtr)
{
	QByteArray query = CreateSetNodeDescriptionQuery(
					nodeId,
					description,
					operationContextPtr);
	if (query.isEmpty()){
		SendErrorMessage(0, "Database query could not be created", "Hierarchical structure info");

		return false;
	}

	if (ExecuteTransaction(query)){
		return true;
	}

	return false;
}


bool CSqlStructureDelegateCompBase::SetNodeMetaInfo(
			const Id& nodeId,
			const idoc::IDocumentMetaInfo& metaInfo,
			const imtbase::IOperationContext* operationContextPtr)
{
	QByteArray query = CreateSetNodeMetaInfoQuery(
					nodeId,
					&metaInfo,
					operationContextPtr);
	if (query.isEmpty()){
		SendErrorMessage(0, "Database query could not be created", "Hierarchical structure info");

		return false;
	}

	if (ExecuteTransaction(query)){
		return true;
	}

	return false;
}


bool CSqlStructureDelegateCompBase::MoveNode(
			const Id& nodeId,
			const Id& parentNodeId,
			const imtbase::IOperationContext* operationContextPtr)
{
	QByteArray query = CreateMoveNodeQuery(
					nodeId,
					parentNodeId,
					operationContextPtr);
	if (query.isEmpty()){
		SendErrorMessage(0, "Database query could not be created", "Hierarchical structure info");

		return false;
	}

	if (ExecuteTransaction(query)){
		return true;
	}

	return false;
}


bool CSqlStructureDelegateCompBase::RemoveNode(const Id& nodeId, const imtbase::IOperationContext* operationContextPtr)
{
	QByteArray query = CreateRemoveNodeQuery(
					nodeId,
					operationContextPtr);
	if (query.isEmpty()){
		SendErrorMessage(0, "Database query could not be created", "Hierarchical structure info");

		return false;
	}

	if (ExecuteTransaction(query)){
		return true;
	}

	return false;
}


bool CSqlStructureDelegateCompBase::AssignObject(
			const Id& objectId,
			const Id& nodeId,
			const imtbase::IOperationContext* operationContextPtr)
{
	QByteArray query = CreateInsertNewObjectQuery(
					objectId,
					nodeId,
					operationContextPtr);
	if (query.isEmpty()){
		SendErrorMessage(0, "Database query could not be created", "Hierarchical structure info");

		return false;
	}

	if (ExecuteTransaction(query)){
		return true;
	}

	return false;
}


bool CSqlStructureDelegateCompBase::MoveObject(
			const Id& objectId,
			const Id& sourceNodeId,
			const Id& targetNodeId,
			const imtbase::IOperationContext* operationContextPtr)
{
	QByteArray query = CreateMoveObjectQuery(
					objectId,
					sourceNodeId,
					targetNodeId,
					operationContextPtr);
	if (query.isEmpty()){
		SendErrorMessage(0, "Database query could not be created", "Hierarchical structure info");

		return false;
	}

	if (ExecuteTransaction(query)){
		return true;
	}

	return false;
}


bool CSqlStructureDelegateCompBase::RemoveObject(
			const Id& objectId,
			const Id& nodeId,
			const imtbase::IOperationContext* operationContextPtr)
{
	QByteArray query = CreateRemoveObjectQuery(
					objectId,
					nodeId,
					operationContextPtr);
	if (query.isEmpty()){
		SendErrorMessage(0, "Database query could not be created", "Hierarchical structure info");

		return false;
	}

	if (ExecuteTransaction(query)){
		return true;
	}

	return false;
}


int CSqlStructureDelegateCompBase::GetNodeCount(const iprm::IParamsSet* selectionParamsPtr) const
{
	QByteArray query = CreateGetNodeCountQuery(selectionParamsPtr);
	if (query.isEmpty()){
		SendErrorMessage(0, "Database query could not be created", "Hierarchical structure info");

		return 0;
	}

	QSqlError sqlError;
	QSqlQuery sqlQuery = m_dbEngineCompPtr->ExecSqlQuery(query, &sqlError, true);
	if (sqlError.type() != QSqlError::NoError){
		SendErrorMessage(0, sqlError.text(), "Hierarchical structure info");
	}

	if (sqlQuery.next()){
		return sqlQuery.value("").toInt();
	}

	return 0;
}


imtbase::ICollectionStructureInfo::Ids CSqlStructureDelegateCompBase::GetNodeIds(
			int offset,
			int count,
			const iprm::IParamsSet* selectionParamsPtr) const
{
	QByteArray query = CreateGetNodeIdsQuery(
					offset,
					count,
					selectionParamsPtr);
	if (query.isEmpty()){
		SendErrorMessage(0, "Database query could not be created", "Hierarchical structure info");

		return Ids();
	}

	QSqlError sqlError;
	QSqlQuery sqlQuery = m_dbEngineCompPtr->ExecSqlQuery(query, &sqlError, true);
	if (sqlError.type() != QSqlError::NoError){
		SendErrorMessage(0, sqlError.text(), "Hierarchical structure info");
	}

	Ids retVal;
	while (sqlQuery.next()){
		retVal << sqlQuery.value("").toByteArray();
	}

	return retVal;
}


// imtbase::ICollectionStructureInfo::Ids CSqlStructureDelegateCompBase::GetObjectIds(
// 	int offset,
// 	int count,
// 	const iprm::IParamsSet* selectionParamsPtr) const
// {
// 	QByteArray query = CreateGetObjectIdsQuery(
// 		offset,
// 		count,
// 		selectionParamsPtr);
// 	if (query.isEmpty()){
// 		SendErrorMessage(0, "Database query could not be created", "Hierarchical structure info");

// 		return Ids();
// 	}

// 	QSqlError sqlError;
// 	QSqlQuery sqlQuery = m_dbEngineCompPtr->ExecSqlQuery(query, &sqlError, true);
// 	if (sqlError.type() != QSqlError::NoError){
// 		SendErrorMessage(0, sqlError.text(), "Hierarchical structure info");
// 	}

// 	Ids retVal;
// 	while (sqlQuery.next()){
// 		retVal << sqlQuery.value("").toByteArray();
// 	}

// 	return retVal;
// }


imtbase::ICollectionStructureInfo::NodeInfo CSqlStructureDelegateCompBase::GetNodeInfo(const Id& nodeId)
{
	QByteArray query = CreateGetNodeInfoQuery(nodeId);
	if (query.isEmpty()){
		SendErrorMessage(0, "Database query could not be created", "Hierarchical structure info");

		return NodeInfo();
	}

	QSqlError sqlError;
	QSqlQuery sqlQuery = m_dbEngineCompPtr->ExecSqlQuery(query, &sqlError, true);
	if (sqlError.type() != QSqlError::NoError){
		SendErrorMessage(0, sqlError.text(), "Hierarchical structure info");
	}

	if (sqlQuery.next()){
		imtbase::ICollectionStructureInfo::NodeInfo nodeInfo;
		nodeInfo.name = sqlQuery.value("Name").toString();
		nodeInfo.description = sqlQuery.value("Description").toString();
	}

	return NodeInfo();
}


QList<imtbase::ICollectionStructureInfo::PathElement> CSqlStructureDelegateCompBase::GetNodePath(const Id& nodeId) const
{
	NodePath retVal;
	QByteArray query = CreateGetObjectParentNodeIdsQuery(nodeId);
	if (query.isEmpty()){
		SendErrorMessage(0, "Database query could not be created", "Hierarchical structure info");

		return retVal;
	}

	QSqlError sqlError;
	QSqlQuery sqlQuery = m_dbEngineCompPtr->ExecSqlQuery(query, &sqlError, true);
	if (sqlError.type() != QSqlError::NoError){
		SendErrorMessage(0, sqlError.text(), "Hierarchical structure info");
	}

	while (sqlQuery.next()){
		PathElement pathElement;
		pathElement.id = sqlQuery.value("id").toByteArray();
		pathElement.name = sqlQuery.value("name").toString();
		retVal << pathElement;
	}

	return retVal;
}


imtbase::ICollectionStructureInfo::Ids CSqlStructureDelegateCompBase::GetObjectParentNodeIds(const Id& objectId) const
{
	QByteArray query = CreateGetObjectParentNodeIdsQuery(objectId);
	if (query.isEmpty()){
		SendErrorMessage(0, "Database query could not be created", "Hierarchical structure info");

		return Ids();
	}

	QSqlError sqlError;
	QSqlQuery sqlQuery = m_dbEngineCompPtr->ExecSqlQuery(query, &sqlError, true);
	if (sqlError.type() != QSqlError::NoError){
		SendErrorMessage(0, sqlError.text(), "Hierarchical structure info");
	}

	Ids retVal;
	while (sqlQuery.next()){
		retVal << sqlQuery.value("").toByteArray();
	}

	return retVal;
}


imtbase::ICollectionStructureController* CSqlStructureDelegateCompBase::GetHierarchicalStructureController()
{
	return nullptr;
}


bool CSqlStructureDelegateCompBase::ExecuteTransaction(const QByteArray& sqlQuery) const
{
	if (!m_dbEngineCompPtr.IsValid()){
		SendCriticalMessage(0, "Invalid component configuration: Database engine missing", "Hierarchical structure info");

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
				SendErrorMessage(0, error.text(), "Hierarchical structure info");

				qDebug() << "SQL-error: " << singleQuery;

				m_dbEngineCompPtr->CancelTransaction();

				return false;
			}
		}
	}

	m_dbEngineCompPtr->FinishTransaction();

	return true;
}


} // namespace imtdb


