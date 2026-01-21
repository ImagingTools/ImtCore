#include <imthype/CJobTicketDatabaseDelegateComp.h>


namespace imthype
{


// public methods

// reimplemented (imtdb::ISqlDatabaseObjectDelegate)

QByteArray CJobTicketDatabaseDelegateComp::GetObjectTypeId(const QByteArray& /*objectId*/) const
{
	return "JobTicket";
}


istd::IChangeableUniquePtr CJobTicketDatabaseDelegateComp::CreateObjectFromRecord(const QSqlRecord& record, const iprm::IParamsSet* /*dataConfigurationPtr*/) const
{
	if (!m_databaseEngineCompPtr.IsValid()){
		return nullptr;
	}

	if (!m_jobTicketFactCompPtr.IsValid()){
		return nullptr;
	}

	IJobTicketSharedPtr jobTicketPtr = m_jobTicketFactCompPtr.CreateInstance();
	if (!jobTicketPtr.IsValid()){
		return nullptr;
	}

	if (record.contains("TypeId")){
		QByteArray typeId = record.value("TypeId").toByteArray();
		jobTicketPtr->SetTypeId(typeId);
	}

	if (record.contains("Uuid")){
		QByteArray uuid = record.value("Uuid").toByteArray();
		jobTicketPtr->SetUuid(uuid);
	}

	if (record.contains("Name")){
		QString name = record.value("Name").toString();
		jobTicketPtr->SetJobName(name);
	}

	if (record.contains("ContextId")){
		QByteArray contextId = record.value("ContextId").toByteArray();
		jobTicketPtr->SetContextId(contextId);
	}

	if (record.contains("Progress")){
		double progress = record.value("Progress").toDouble();
		jobTicketPtr->SetProgress(progress);
	}

	if (record.contains("ProcessingStatus")){
		int status = record.value("ProcessingStatus").toInt();
		jobTicketPtr->SetProcessingStatus(static_cast<IJobQueueManager::ProcessingStatus>(status));
	}

	istd::IChangeableUniquePtr retVal;
	retVal.MoveCastedPtr<IJobTicket>(jobTicketPtr);

	return retVal;
}


imtdb::IDatabaseObjectDelegate::NewObjectQuery CJobTicketDatabaseDelegateComp::CreateNewObjectQuery(
			const QByteArray& /*typeId*/,
			const QByteArray& proposedObjectId,
			const QString& /*objectName*/,
			const QString& /*objectDescription*/,
			const istd::IChangeable* valuePtr,
			const imtbase::IOperationContext* /*operationContextPtr*/) const
{
	const IJobTicket* jobTicketPtr = dynamic_cast<const IJobTicket*>(valuePtr);
	if (jobTicketPtr == nullptr){
		return NewObjectQuery();
	}

	QByteArray typeId = jobTicketPtr->GetTypeId();
	QByteArray uuid = jobTicketPtr->GetUuid();
	QString name = jobTicketPtr->GetJobName();
	QByteArray contextId = jobTicketPtr->GetContextId();
	double progress = jobTicketPtr->GetProgress();
	int processingStatus = static_cast<int>(jobTicketPtr->GetProcessingStatus());

	NewObjectQuery retVal;
	retVal.query = QString("INSERT INTO \"JobTickets\"(\"Id\", \"TypeId\", \"Uuid\", \"Name\", \"ContextId\", \"Progress\", \"ProcessingStatus\") VALUES('%1', '%2', '%3', '%4', '%5', %6, %7);")
				.arg(qPrintable(proposedObjectId))
				.arg(qPrintable(typeId))
				.arg(qPrintable(uuid))
				.arg(name)
				.arg(qPrintable(contextId))
				.arg(progress)
				.arg(processingStatus)
				.toUtf8();
	retVal.objectName = name;

	return retVal;
}


QByteArray CJobTicketDatabaseDelegateComp::CreateDeleteObjectsQuery(
			const imtbase::IObjectCollection& /*collection*/,
			const imtbase::ICollectionInfo::Ids& objectIds,
			const imtbase::IOperationContext* /*operationContextPtr*/) const
{
	if (objectIds.isEmpty()){
		return QByteArray();
	}

	QStringList quotedIds;
	for (const QByteArray& objectId : objectIds){
		quotedIds << QString("'%1'").arg(qPrintable(objectId));
	}

	QString query = QString(
						"DELETE FROM \"JobTickets\" WHERE \"Id\" IN (%1);")
						.arg(
							quotedIds.join(", ")
							);

	return query.toUtf8();
}


QByteArray CJobTicketDatabaseDelegateComp::CreateDeleteObjectSetQuery(
			const imtbase::IObjectCollection& /*collection*/,
			const iprm::IParamsSet* /*paramsPtr*/,
			const imtbase::IOperationContext* /*operationContextPtr*/) const
{
	return QByteArray();
}


QByteArray CJobTicketDatabaseDelegateComp::CreateUpdateObjectQuery(
			const imtbase::IObjectCollection& /*collection*/,
			const QByteArray& objectId,
			const istd::IChangeable& object,
			const imtbase::IOperationContext* /*operationContextPtr*/,
			bool /*useExternDelegate*/) const
{
	const IJobTicket* jobTicketPtr = dynamic_cast<const IJobTicket*>(&object);
	if (jobTicketPtr == nullptr || objectId.isEmpty()){
		return QByteArray();
	}

	QByteArray typeId = jobTicketPtr->GetTypeId();
	QByteArray uuid = jobTicketPtr->GetUuid();
	QString name = jobTicketPtr->GetJobName();
	QByteArray contextId = jobTicketPtr->GetContextId();
	double progress = jobTicketPtr->GetProgress();
	int processingStatus = static_cast<int>(jobTicketPtr->GetProcessingStatus());

	QByteArray retVal = QString("UPDATE \"JobTickets\" SET \"TypeId\" = '%1', \"Uuid\" = '%2', \"Name\" = '%3', \"ContextId\" = '%4', \"Progress\" = %5, \"ProcessingStatus\" = %6 WHERE \"Id\" ='%7';")
				.arg(qPrintable(typeId))
				.arg(qPrintable(uuid))
				.arg(name)
				.arg(qPrintable(contextId))
				.arg(progress)
				.arg(processingStatus)
				.arg(qPrintable(objectId))
				.toUtf8();

	return retVal;
}


QByteArray CJobTicketDatabaseDelegateComp::CreateRenameObjectQuery(
			const imtbase::IObjectCollection& collection,
			const QByteArray& objectId,
			const QString& newObjectName,
			const imtbase::IOperationContext* /*operationContextPtr*/) const
{
	const IJobTicket* jobTicketPtr = nullptr;
	imtbase::IObjectCollection::DataPtr objectPtr;
	if (collection.GetObjectData(objectId, objectPtr)){
		jobTicketPtr = dynamic_cast<const IJobTicket*>(objectPtr.GetPtr());
	}

	if (jobTicketPtr == nullptr){
		return QByteArray();
	}

	if (objectId.isEmpty()){
		return QByteArray();
	}

	QByteArray retVal = QString("UPDATE \"JobTickets\" SET \"Name\" = '%1' WHERE \"Id\" = '%2';")
			.arg(newObjectName)
			.arg(qPrintable(objectId)).toLocal8Bit();

	return retVal;
}


QByteArray CJobTicketDatabaseDelegateComp::CreateDescriptionObjectQuery(
			const imtbase::IObjectCollection& /*collection*/,
			const QByteArray& /*objectId*/,
			const QString& /*description*/,
			const imtbase::IOperationContext* /*operationContextPtr*/) const
{
	return QByteArray();
}


} // namespace imthype


