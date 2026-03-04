// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include "imthype/IJobOutput.h"
#include <imthypedb/CJobTicketDatabaseDelegateComp.h>


// ACF includes
#include <iser/CJsonMemReadArchive.h>
#include <iser/CJsonMemWriteArchive.h>


namespace imthypedb
{


// public methods

// reimplemented (imtdb::ISqlDatabaseObjectDelegate)

istd::IChangeableUniquePtr CJobTicketDatabaseDelegateComp::CreateObjectFromRecord(const QSqlRecord& record, const iprm::IParamsSet* /*dataConfigurationPtr*/) const
{
	if (!m_databaseEngineCompPtr.IsValid()){
		return nullptr;
	}

	if (!m_jobTicketFactCompPtr.IsValid()){
		return nullptr;
	}

	if (!m_typesCompPtr.IsValid()){
		return nullptr;
	}

	QByteArray typeId = record.value("TypeId").toByteArray();
	if (typeId.isEmpty()){
		return nullptr;
	}

	int factoryIndex = iprm::FindOptionIndexById(typeId, *m_typesCompPtr);

	if ((factoryIndex < 0) || (factoryIndex >= m_jobTicketFactCompPtr.GetCount())){
		return nullptr;
	}

	istd::TUniqueInterfacePtr jobTicketPtr = m_jobTicketFactCompPtr.CreateInstance(factoryIndex);

	if (!jobTicketPtr.IsValid()){
		return nullptr;
	}

	if (record.contains("TypeId")){

		jobTicketPtr->SetTypeId(typeId);
	}

	if (record.contains("Id")){
		QByteArray uuid = record.value("Id").toByteArray();
		jobTicketPtr->SetUuid(uuid);
	}

	if (record.contains("JobNumber")){
		QString name = jobTicketPtr->GetTypeId() + " - " + record.value("JobNumber").toString();
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
		jobTicketPtr->SetProcessingStatus(static_cast<imthype::IJobQueueManager::ProcessingStatus>(status));
	}

	// Deserialize Params from JSON
	if (record.contains("Params")){
		QByteArray paramsData = record.value("Params").toByteArray();
		if (!paramsData.isEmpty()){
			iprm::IParamsSetSharedPtr paramsPtr = jobTicketPtr->GetParams();
			if (!paramsPtr.IsValid()){
				paramsPtr = jobTicketPtr->CreateParams();
			}
			if (paramsPtr.IsValid()){
				iser::CJsonMemReadArchive archive(paramsData);
				paramsPtr->Serialize(archive);
				jobTicketPtr->SetParams(paramsPtr);
			}
		}
	}

	// Deserialize Results from JSON
	if (record.contains("Results")){
		QByteArray resultsData = record.value("Results").toByteArray();
		if (!resultsData.isEmpty()){
			const imthype::IJobOutput* resultsPtr = jobTicketPtr->GetResults();
			if (resultsPtr != nullptr){
				iser::ISerializable* serializablePtr = const_cast<iser::ISerializable*>(dynamic_cast<const iser::ISerializable*>(resultsPtr));
				if (serializablePtr != nullptr){
					iser::CJsonMemReadArchive archive(resultsData);
					serializablePtr->Serialize(archive);
				}
			}
		}
	}

	// Deserialize Input from JSON
	if (record.contains("Input")){
		QByteArray inputData = record.value("Input").toByteArray();
		if (!inputData.isEmpty()){
			const imtbase::IReferenceCollection* inputPtr = jobTicketPtr->GetInput();
			if (inputPtr != nullptr){
				iser::ISerializable* serializablePtr = const_cast<iser::ISerializable*>(dynamic_cast<const iser::ISerializable*>(inputPtr));
				if (serializablePtr != nullptr){
					iser::CJsonMemReadArchive archive(inputData);
					serializablePtr->Serialize(archive);
				}
			}
		}
	}

	istd::IChangeableUniquePtr retVal;
	retVal.MoveCastedPtr(jobTicketPtr);

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
	const imthype::IJobTicket* jobTicketPtr = dynamic_cast<const imthype::IJobTicket*>(valuePtr);
	if (jobTicketPtr == nullptr){
		return NewObjectQuery();
	}

	QByteArray typeId = jobTicketPtr->GetTypeId();
	QByteArray uuid = jobTicketPtr->GetUuid();
	QString name = jobTicketPtr->GetJobName();
	QByteArray contextId = jobTicketPtr->GetContextId();
	double progress = jobTicketPtr->GetProgress();
	imthype::IJobQueueManager::ProcessingStatus processingStatus = jobTicketPtr->GetProcessingStatus();

	if (!proposedObjectId.isEmpty()){
		uuid = proposedObjectId;
	}

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
	const imthype::IJobOutput* resultsPtr = jobTicketPtr->GetResults();
	if (resultsPtr != nullptr){
		iser::ISerializable* serializablePtr = const_cast<iser::ISerializable*>(dynamic_cast<const iser::ISerializable*>(resultsPtr));
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
	retVal.query = QString("INSERT INTO \"JobTickets\"(\"Id\", \"TypeId\", \"Name\", \"ContextId\", \"Progress\", \"ProcessingStatus\", \"Params\", \"Results\", \"Input\", \"Added\") VALUES('%1', '%2', '%3', '%4', %5, %6, '%7', '%8', '%9', '%10');")
				.arg(qPrintable(proposedObjectId))
				.arg(qPrintable(typeId))
				.arg(name)
				.arg(qPrintable(contextId))
				.arg(progress)
				.arg(static_cast<int>(processingStatus))
				.arg(QString::fromUtf8(paramsData))
				.arg(QString::fromUtf8(resultsData))
				.arg(QString::fromUtf8(inputData))
				.arg(QDateTime::currentDateTimeUtc().toString(Qt::ISODateWithMs))
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
	const imthype::IJobTicket* jobTicketPtr = dynamic_cast<const imthype::IJobTicket*>(&object);
	if (jobTicketPtr == nullptr || objectId.isEmpty()){
		return QByteArray();
	}

	QByteArray typeId = jobTicketPtr->GetTypeId();
	QString name = jobTicketPtr->GetJobName();
	QByteArray contextId = jobTicketPtr->GetContextId();
	double progress = jobTicketPtr->GetProgress();
	imthype::IJobQueueManager::ProcessingStatus processingStatus = jobTicketPtr->GetProcessingStatus();

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
	const imthype::IJobOutput* resultsPtr = jobTicketPtr->GetResults();
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

	QByteArray retVal = QString("UPDATE \"JobTickets\" SET \"TypeId\" = '%1', \"Name\" = '%3', \"ContextId\" = '%4', \"Progress\" = %5, \"ProcessingStatus\" = %6, \"Params\" = '%7', \"Results\" = '%8', \"Input\" = '%9', \"TimeStamp\" = '%10' WHERE \"Id\" ='%2';")
				.arg(qPrintable(typeId))
				.arg(qPrintable(objectId))
				.arg(name)
				.arg(qPrintable(contextId))
				.arg(progress)
				.arg(static_cast<int>(processingStatus))
				.arg(QString::fromUtf8(paramsData))
				.arg(QString::fromUtf8(resultsData))
				.arg(QString::fromUtf8(inputData))
				.arg(QDateTime::currentDateTimeUtc().toString(Qt::ISODateWithMs))
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

	const imthype::IJobTicket* jobTicketPtr = nullptr;
	imtbase::IObjectCollection::DataPtr objectPtr;
	if (collection.GetObjectData(objectId, objectPtr)){
		jobTicketPtr = dynamic_cast<const imthype::IJobTicket*>(objectPtr.GetPtr());
	}

	if (jobTicketPtr == nullptr){
		return QByteArray();
	}

	QByteArray retVal = QString("UPDATE \"JobTickets\" SET \"Name\" = '%1', \"TimeStamp\" = '%3' WHERE \"Id\" = '%2';")
			.arg(newObjectName)
			.arg(qPrintable(objectId))
			.arg(QDateTime::currentDateTimeUtc().toString(Qt::ISODateWithMs)).toLocal8Bit();

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


} // namespace imthypedb


