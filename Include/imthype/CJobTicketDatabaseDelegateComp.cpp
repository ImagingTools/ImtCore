#include "imthype/IJobOutput.h"
#include <imthype/CJobTicketDatabaseDelegateComp.h>


// ACF includes
#include <iser/CJsonMemReadArchive.h>
#include <iser/CJsonMemWriteArchive.h>


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

	// Deserialize Params from JSON
	if (record.contains("Params")){
		QString paramsDataBase64 = record.value("Params").toString();
		if (!paramsDataBase64.isEmpty()){
			QByteArray paramsData = QByteArray::fromBase64(paramsDataBase64.toUtf8());
			if (!paramsData.isEmpty()){
				iprm::IParamsSetSharedPtr paramsPtr = jobTicketPtr->GetParams();
				if (paramsPtr.IsValid()){
					iser::CJsonMemReadArchive archive(paramsData);
					paramsPtr->Serialize(archive);
				}
			}
		}
	}

	// Deserialize Results from JSON
	if (record.contains("Results")){
		QString resultsDataBase64 = record.value("Results").toString();
		if (!resultsDataBase64.isEmpty()){
			QByteArray resultsData = QByteArray::fromBase64(resultsDataBase64.toUtf8());
			if (!resultsData.isEmpty()){
				const IJobOutput* resultsPtr = jobTicketPtr->GetResults();
				if (resultsPtr != nullptr){
					auto serializablePtr = const_cast<iser::ISerializable*>(dynamic_cast<const iser::ISerializable*>(resultsPtr));
					if (serializablePtr != nullptr){
						iser::CJsonMemReadArchive archive(resultsData);
						serializablePtr->Serialize(archive);
					}
				}
			}
		}
	}

	// Deserialize Input from JSON
	if (record.contains("Input")){
		QString inputDataBase64 = record.value("Input").toString();
		if (!inputDataBase64.isEmpty()){
			QByteArray inputData = QByteArray::fromBase64(inputDataBase64.toUtf8());
			if (!inputData.isEmpty()){
				const imtbase::IReferenceCollection* inputPtr = jobTicketPtr->GetInput();
				if (inputPtr != nullptr){
					auto serializablePtr = const_cast<iser::ISerializable*>(dynamic_cast<const iser::ISerializable*>(inputPtr));
					if (serializablePtr != nullptr){
						iser::CJsonMemReadArchive archive(inputData);
						serializablePtr->Serialize(archive);
					}
				}
			}
		}
	}

	istd::IChangeableUniquePtr retVal;
	retVal.MoveCastedPtr<IJobTicket>(jobTicketPtr.GetPtr());

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
	// Note: Progress and ProcessingStatus are now managed separately via IJobStatus, not persisted with IJobTicket

	// Serialize Params to JSON
	QByteArray paramsData;
	iprm::IParamsSetSharedPtr paramsPtr = jobTicketPtr->GetParams();
	if (paramsPtr.IsValid()){
		iser::CJsonMemWriteArchive archive(nullptr);
		if (paramsPtr->Serialize(archive)){
			paramsData = archive.GetData();
		}
	}

	// Serialize Results to JSON
	QByteArray resultsData;
	const IJobOutput* resultsPtr = jobTicketPtr->GetResults();
	if (resultsPtr != nullptr){
		auto serializablePtr = const_cast<iser::ISerializable*>(dynamic_cast<const iser::ISerializable*>(resultsPtr));
		if (serializablePtr != nullptr){
			iser::CJsonMemWriteArchive archive(nullptr);
			if (serializablePtr->Serialize(archive)){
				resultsData = archive.GetData();
			}
		}
	}

	// Serialize Input to JSON
	QByteArray inputData;
	const imtbase::IReferenceCollection* inputPtr = jobTicketPtr->GetInput();
	if (inputPtr != nullptr){
		auto serializablePtr = const_cast<iser::ISerializable*>(dynamic_cast<const iser::ISerializable*>(inputPtr));
		if (serializablePtr != nullptr){
			iser::CJsonMemWriteArchive archive(nullptr);
			if (serializablePtr->Serialize(archive)){
				inputData = archive.GetData();
			}
		}
	}

	NewObjectQuery retVal;
	retVal.query = QString("INSERT INTO \"JobTickets\"(\"Id\", \"TypeId\", \"Uuid\", \"Name\", \"ContextId\", \"Progress\", \"ProcessingStatus\", \"Params\", \"Results\", \"Input\") VALUES('%1', '%2', '%3', '%4', '%5', %6, %7, '%8', '%9', '%10');")
				.arg(qPrintable(proposedObjectId))
				.arg(qPrintable(typeId))
				.arg(qPrintable(uuid))
				.arg(name)
				.arg(qPrintable(contextId))
				.arg(progress)
				.arg(static_cast<int>(status))
				.arg(QString::fromUtf8(paramsData.toBase64()))
				.arg(QString::fromUtf8(resultsData.toBase64()))
				.arg(QString::fromUtf8(inputData.toBase64()))
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
	// Note: Progress and ProcessingStatus are now managed separately via IJobStatus, not persisted with IJobTicket

	// Serialize Params to JSON
	QByteArray paramsData;
	iprm::IParamsSetSharedPtr paramsPtr = jobTicketPtr->GetParams();
	if (paramsPtr.IsValid()){
		iser::CJsonMemWriteArchive archive(nullptr);
		if (paramsPtr->Serialize(archive)){
			paramsData = archive.GetData();
		}
	}

	// Serialize Results to JSON
	QByteArray resultsData;
	const IJobOutput* resultsPtr = jobTicketPtr->GetResults();
	if (resultsPtr != nullptr){
		auto serializablePtr = const_cast<iser::ISerializable*>(dynamic_cast<const iser::ISerializable*>(resultsPtr));
		if (serializablePtr != nullptr){
			iser::CJsonMemWriteArchive archive(nullptr);
			if (serializablePtr->Serialize(archive)){
				resultsData = archive.GetData();
			}
		}
	}

	// Serialize Input to JSON
	QByteArray inputData;
	const imtbase::IReferenceCollection* inputPtr = jobTicketPtr->GetInput();
	if (inputPtr != nullptr){
		auto serializablePtr = const_cast<iser::ISerializable*>(dynamic_cast<const iser::ISerializable*>(inputPtr));
		if (serializablePtr != nullptr){
			iser::CJsonMemWriteArchive archive(nullptr);
			if (serializablePtr->Serialize(archive)){
				inputData = archive.GetData();
			}
		}
	}

	QByteArray retVal = QString("UPDATE \"JobTickets\" SET \"TypeId\" = '%1', \"Uuid\" = '%2', \"Name\" = '%3', \"ContextId\" = '%4', \"Progress\" = %5, \"ProcessingStatus\" = %6, \"Params\" = '%7', \"Results\" = '%8', \"Input\" = '%9' WHERE \"Id\" ='%10';")
				.arg(qPrintable(typeId))
				.arg(qPrintable(uuid))
				.arg(name)
				.arg(qPrintable(contextId))
				.arg(progress)
				.arg(static_cast<int>(status))
				.arg(QString::fromUtf8(paramsData.toBase64()))
				.arg(QString::fromUtf8(resultsData.toBase64()))
				.arg(QString::fromUtf8(inputData.toBase64()))
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
	if (objectId.isEmpty()){
		return QByteArray();
	}

	const IJobTicket* jobTicketPtr = nullptr;
	imtbase::IObjectCollection::DataPtr objectPtr;
	if (collection.GetObjectData(objectId, objectPtr)){
		jobTicketPtr = dynamic_cast<const IJobTicket*>(objectPtr.GetPtr());
	}

	if (jobTicketPtr == nullptr){
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


