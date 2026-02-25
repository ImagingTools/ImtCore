// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtdb/CSqlJsonDatabaseDelegateComp.h>


// ACF includes
#include <iprm/TParamsPtr.h>
#include <iprm/ITextParam.h>
#include <iprm/IIdParam.h>
#include <istd/TOptDelPtr.h>
#include <istd/CCrcCalculator.h>
#include <iser/CJsonMemReadArchive.h>
#include <iser/CJsonMemWriteArchive.h>
#include <iser/ISerializable.h>
#include <imod/TModelWrap.h>
#include <idoc/CStandardDocumentMetaInfo.h>

// ImtCore includes
#include <imtbase/imtbase.h>
#include <imtbase/CObjectCollection.h>
#include <imtcol/IDocumentCollectionFilter.h>


namespace imtdb
{


// public methods

// reimplemented (imtdb::ISqlDatabaseObjectDelegate)

istd::IChangeableUniquePtr CSqlJsonDatabaseDelegateComp::CreateObjectFromRecord(
			const QSqlRecord& record,
			const iprm::IParamsSet* /*paramsPtr*/) const
{
	if (!m_databaseEngineCompPtr.IsValid()){
		return nullptr;
	}

	if (!m_documentFactoriesCompPtr.IsValid()){
		return nullptr;
	}

	istd::IChangeableUniquePtr documentPtr;

	int index = 0;
	QByteArray typeId = "DocumentInfo";
	if (record.contains("TypeId")){
		typeId = record.value("TypeId").toByteArray();
		index = -1;

		if (m_typesCompPtr.IsValid()){
			for (int i = 0; i < m_typesCompPtr->GetOptionsCount(); ++i){
				if (typeId == m_typesCompPtr->GetOptionId(i)){
					index = i;
					break;
				}
			}
		}
	}
	else{
		if (m_typesCompPtr.IsValid() && m_typesCompPtr->GetOptionsCount() > 0){
			typeId = m_typesCompPtr->GetOptionId(0);
			index = 0;
		}
	}

	if (m_documentFactoriesCompPtr.GetCount() > 0 && index >= 0){
		documentPtr = m_documentFactoriesCompPtr.CreateInstance(index);
	}

	if (!documentPtr.IsValid()){
		return nullptr;
	}

	if (record.contains("Document")){
		QByteArray documentContent = record.value(qPrintable("Document")).toByteArray();

		if (ReadDataFromMemory(typeId, documentContent, *documentPtr)){
			return documentPtr;
		}
	}

	return nullptr;
}


imtdb::IDatabaseObjectDelegate::NewObjectQuery CSqlJsonDatabaseDelegateComp::CreateNewObjectQuery(
			const QByteArray& typeId,
			const QByteArray& proposedObjectId,
			const QString& objectName,
			const QString& /*objectDescription*/,
			const istd::IChangeable* valuePtr,
			const imtbase::IOperationContext* operationContextPtr) const
{
	NewObjectQuery retVal;

	istd::TOptDelPtr<const istd::IChangeable> workingDocumentPtr;
	if (valuePtr != nullptr){
		workingDocumentPtr.SetPtr(valuePtr, false);
	}

	if (workingDocumentPtr.IsValid()){
		QByteArray documentContent;
		if (WriteDataToMemory(typeId, *workingDocumentPtr, documentContent)){
			QByteArray objectId = proposedObjectId.isEmpty() ? QUuid::createUuid().toByteArray(QUuid::WithoutBraces) : proposedObjectId;
			quint32 checksum = istd::CCrcCalculator::GetCrcFromData((const quint8*)documentContent.constData(), imtbase::narrow_cast<int>(documentContent.size()));

			int revisionVersion = 1;
			QString queryStr;
			if (*m_isMultiTypeAttrPtr){
				queryStr = QString("UPDATE \"%1\" SET \"IsActive\" = false WHERE \"DocumentId\" = '%2'; INSERT INTO \"%1\"(\"DocumentId\", \"Document\", \"RevisionNumber\", \"LastModified\", \"Checksum\", \"IsActive\", \"TypeId\") VALUES('%2', '%3', '%4', '%5', '%6', true, '%0');")
				.arg(QString(typeId));
			}
			else{
				queryStr = QString("UPDATE \"%1\" SET \"IsActive\" = false WHERE \"DocumentId\" = '%2'; INSERT INTO \"%1\"(\"DocumentId\", \"Document\", \"RevisionNumber\", \"LastModified\", \"Checksum\", \"IsActive\") VALUES('%2', '%3', '%4', '%5', '%6', true);");
			}

			retVal.query = queryStr
						.arg(qPrintable(*m_tableNameAttrPtr))
						.arg(qPrintable(objectId))
						.arg(SqlEncode(documentContent))
						.arg(revisionVersion)
						.arg(QDateTime::currentDateTimeUtc().toString(Qt::ISODateWithMs))
						.arg(checksum).toUtf8();

			retVal.query += CreateOperationDescriptionQuery(objectId, operationContextPtr);
			retVal.objectName = objectName;
		}
	}
	return retVal;
}


QByteArray CSqlJsonDatabaseDelegateComp::CreateDeleteObjectsQuery(
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
						"DELETE FROM \"%1\" WHERE \"%2\" IN (%3);")
						.arg(
							QString::fromUtf8(qPrintable(*m_tableNameAttrPtr)),
							QString::fromUtf8(qPrintable(*m_objectIdColumnAttrPtr)),
							quotedIds.join(", ")
							);

	return query.toUtf8();
}


QByteArray CSqlJsonDatabaseDelegateComp::CreateUpdateObjectQuery(
			const imtbase::IObjectCollection& /*collection*/,
			const QByteArray& objectId,
			const istd::IChangeable& object,
			const imtbase::IOperationContext* operationContextPtr,
			bool /*useExternDelegate*/) const
{
	QByteArray retVal;

	QByteArray documentContent;
	if (WriteDataToMemory("DocumentInfo", object, documentContent)){
		quint32 checksum = istd::CCrcCalculator::GetCrcFromData((const quint8*)documentContent.constData(), imtbase::narrow_cast<int>(documentContent.size()));

		QString queryStr;
		if (*m_isMultiTypeAttrPtr){
			queryStr =  QString("UPDATE \"%1\" SET \"IsActive\" = false WHERE \"DocumentId\" = '%2'; INSERT INTO \"%1\" (\"DocumentId\", \"Document\", \"LastModified\", \"Checksum\", \"IsActive\", \"RevisionNumber\", \"TypeId\") VALUES('%2', '%3', '%4', '%5', true, "
								" (SELECT MAX(\"RevisionNumber\") FROM \"%1\" WHERE \"DocumentId\" = '%2') + 1,"
								" (SELECT \"TypeId\" FROM \"%1\" WHERE \"DocumentId\" = '%2' LIMIT 1) );" );
		}
		else{
			queryStr = QString("UPDATE \"%1\" SET \"IsActive\" = false WHERE \"DocumentId\" = '%2'; INSERT INTO \"%1\" (\"DocumentId\", \"Document\", \"LastModified\", \"Checksum\", \"IsActive\", \"RevisionNumber\") VALUES('%2', '%3', '%4', '%5', true, (SELECT MAX(\"RevisionNumber\") FROM \"%1\" WHERE \"DocumentId\" = '%2') + 1 );");
		}
		retVal = queryStr
				.arg(qPrintable(*m_tableNameAttrPtr))
				.arg(qPrintable(objectId))
				.arg(SqlEncode(documentContent))
				.arg(QDateTime::currentDateTimeUtc().toString(Qt::ISODateWithMs))
				.arg(checksum).toUtf8();

		retVal += CreateOperationDescriptionQuery(objectId, operationContextPtr);
	}

	return retVal;
}


QByteArray CSqlJsonDatabaseDelegateComp::CreateDescriptionObjectQuery(
			const imtbase::IObjectCollection& /*collection*/,
			const QByteArray& objectId,
			const QString& description,
			const imtbase::IOperationContext* /*operationContextPtr*/) const
{
	QByteArray retVal = QString("UPDATE \"%1\" SET \"Document\" = jsonb_set(\"Document\", '{Description}', '\"%2\"', true), \"LastModified\" = '%3' WHERE \"%4\" ='%5' AND \"IsActive\" = true;")
				.arg(qPrintable(*m_tableNameAttrPtr))
				.arg(SqlEncode(description))
				.arg(QDateTime::currentDateTimeUtc().toString(Qt::ISODateWithMs))
				.arg(qPrintable(*m_objectIdColumnAttrPtr))
				.arg(qPrintable(objectId)).toUtf8();

	return retVal;
}


QByteArray CSqlJsonDatabaseDelegateComp::GetCountQuery(const iprm::IParamsSet* paramsPtr) const
{
	QString baseQuery = GetSelectionQuery(QByteArray(), 0, -1, paramsPtr);

	return QString("SELECT COUNT(*) FROM (%1) as t").arg(baseQuery).toUtf8();
}


// reimplemented (imtdb::CSqlDatabaseDocumentDelegateComp)

QString CSqlJsonDatabaseDelegateComp::GetBaseSelectionQuery() const
{
	if (*m_isMultiTypeAttrPtr){
		return QString("SELECT root.*,"
					   "(SELECT \"LastModified\" FROM \"%2\" as t1 WHERE \"RevisionNumber\" = 1 AND root.\"%1\" = t1.\"%1\" LIMIT 1) as \"Added\" FROM \"%2\""
					   " as root WHERE \"IsActive\" = true")
				.arg(qPrintable(*m_objectIdColumnAttrPtr))
				.arg(qPrintable(*m_tableNameAttrPtr));
	}

	return QString("SELECT \"Id\", \"%1\", \"Document\", \"RevisionNumber\", \"LastModified\","
					"(SELECT \"LastModified\" FROM \"%2\" as t1 WHERE \"RevisionNumber\" = 1 AND root.\"%1\" = t1.\"%1\" LIMIT 1) as \"Added\" FROM \"%2\""
					" as root WHERE \"IsActive\" = true")
			.arg(qPrintable(*m_objectIdColumnAttrPtr))
			.arg(qPrintable(*m_tableNameAttrPtr));
}


bool CSqlJsonDatabaseDelegateComp::SetCollectionItemMetaInfoFromRecord(const QSqlRecord& record, idoc::IDocumentMetaInfo& metaInfo) const
{
	QByteArray objectId;
	if (record.contains(*m_objectIdColumnAttrPtr)){
		objectId = record.value(qPrintable(*m_objectIdColumnAttrPtr)).toByteArray();
	}

	if (!objectId.isEmpty()){
		QByteArray query = QString("SELECT * FROM \"%1\" WHERE \"%2\" = '%3' AND \"RevisionNumber\" = 1;")
				.arg(qPrintable(*m_tableNameAttrPtr))
				.arg(qPrintable(*m_objectIdColumnAttrPtr))
				.arg(qPrintable(objectId)).toUtf8();

		QSqlError error;
		QSqlQuery sqlQuery = m_databaseEngineCompPtr->ExecSqlQuery(query, &error);

		if (sqlQuery.next()){
			QSqlRecord orderRecord = sqlQuery.record();

			if (orderRecord.contains("LastModified")){
				QDateTime insertionTime = orderRecord.value("LastModified").toDateTime();

				metaInfo.SetMetaInfo(idoc::IDocumentMetaInfo::MIT_CREATION_TIME, insertionTime);
				metaInfo.SetMetaInfo(imtbase::IObjectCollection::MIT_INSERTION_TIME, insertionTime);
			}
		}
	}

	if (record.contains("LastModified")){
		QDateTime lastModificationTime = record.value("LastModified").toDateTime();

		metaInfo.SetMetaInfo(idoc::IDocumentMetaInfo::MIT_MODIFICATION_TIME, lastModificationTime);
		metaInfo.SetMetaInfo(imtbase::IObjectCollection::MIT_LAST_OPERATION_TIME, lastModificationTime);
	}

	if (record.contains("RevisionNumber")){
		qlonglong revisionNumber = record.value("RevisionNumber").toLongLong();

		metaInfo.SetMetaInfo(imtbase::ICollectionInfo::MIT_REVISION, revisionNumber);
	}

	return true;
}


bool CSqlJsonDatabaseDelegateComp::SetObjectMetaInfoFromRecord(const QSqlRecord& record, idoc::IDocumentMetaInfo& metaInfo) const
{
	if (record.contains("LastModified")){
		QDateTime lastModificationTime = record.value("LastModified").toDateTime();

		metaInfo.SetMetaInfo(idoc::IDocumentMetaInfo::MIT_MODIFICATION_TIME, lastModificationTime);
		metaInfo.SetMetaInfo(imtbase::IObjectCollection::MIT_LAST_OPERATION_TIME, lastModificationTime);
	}

	if (record.contains("RevisionNumber")){
		qlonglong revisionNumber = record.value("RevisionNumber").toLongLong();

		metaInfo.SetMetaInfo(imtbase::ICollectionInfo::MIT_REVISION, revisionNumber);
	}

	return true;
}


bool CSqlJsonDatabaseDelegateComp::CreateSortQuery(const imtbase::ICollectionFilter& collectionFilter, QString& sortQuery) const
{
	QByteArray columnId;
	QByteArray sortOrder;

	if (!collectionFilter.GetSortingInfoIds().isEmpty()){
		columnId = collectionFilter.GetSortingInfoIds().first();
	}

	switch (collectionFilter.GetSortingOrder()){
	case imtbase::ICollectionFilter::SO_NO_ORDER:
		I_IF_DEBUG(qWarning() << __FILE__ << __LINE__ <<  "sorting order is not set, using default sorting order (ASC).";)
	case imtbase::ICollectionFilter::SO_ASC:
		sortOrder = "ASC";
		break;
	case imtbase::ICollectionFilter::SO_DESC:
		sortOrder = "DESC";
		break;
	}

	if (!columnId.isEmpty() && !sortOrder.isEmpty()){
		if (columnId == "LastModified" || columnId == "Added"){
			sortQuery = QString("ORDER BY \"%1\" %2").arg(qPrintable(columnId)).arg(qPrintable(sortOrder));
		}
		else{
			sortQuery = QString("ORDER BY \"Document\"->>'%1' %2").arg(qPrintable(columnId)).arg(qPrintable(sortOrder));
		}
	}

	return true;
}


bool CSqlJsonDatabaseDelegateComp::CreateFilterQuery(const iprm::IParamsSet& filterParams, QString& filterQuery) const
{
	bool retVal = true;
	QString objectFilterQuery;

	iprm::IParamsSet::Ids paramIds = filterParams.GetParamIds();

	if (paramIds.contains("ObjectFilter")){
		iprm::TParamsPtr<iprm::IParamsSet> objectFilterParamPtr(&filterParams, "ObjectFilter");
		if (objectFilterParamPtr.IsValid()){
			retVal = CreateObjectFilterQuery(*objectFilterParamPtr, objectFilterQuery);
			if (!retVal){
				return false;
			}
		}
	}

	QString textFilterQuery;
	if (paramIds.contains("Filter")){
		iprm::TParamsPtr<imtbase::ICollectionFilter> collectionFilterParamPtr(&filterParams, "Filter");
		if (collectionFilterParamPtr.IsValid()){
			retVal = CreateTextFilterQuery(*collectionFilterParamPtr, textFilterQuery);
			if (!retVal){
				return false;
			}
		}
	}

	QString timeFilterQuery;
	if (paramIds.contains("ComplexFilter")){
		iprm::TParamsPtr<imtbase::IComplexCollectionFilter> complexFilterParamPtr(&filterParams, "ComplexFilter");
		if (complexFilterParamPtr.IsValid()){
			BaseClass::CreateTextFilterQuery(*complexFilterParamPtr, textFilterQuery);
			BaseClass::CreateObjectFilterQuery(*complexFilterParamPtr, objectFilterQuery);
			BaseClass::CreateTimeFilterQuery(complexFilterParamPtr->GetTimeFilter(), timeFilterQuery);
		}
	}

	QString additionalFilters = CreateAdditionalFiltersQuery(filterParams);

	if (!objectFilterQuery.isEmpty()){
		filterQuery += " AND (" + objectFilterQuery + ")";
	}

	if (!textFilterQuery.isEmpty()){
		filterQuery += " AND (" + textFilterQuery + ")";
	}

	if (!timeFilterQuery.isEmpty()){
		filterQuery += " AND (" + timeFilterQuery + ")";
	}

	if(!additionalFilters.isEmpty()){
		filterQuery += " AND (" + additionalFilters + ")";
	}

	return true;
}


bool CSqlJsonDatabaseDelegateComp::CreateObjectFilterQuery(
			const iprm::IParamsSet& filterParams,
			QString& filterQuery) const
{
	iprm::IParamsSet::Ids paramIds = filterParams.GetParamIds();

	if (!paramIds.isEmpty()){
#if QT_VERSION < 0x060000
		QByteArrayList idsList(paramIds.toList());
#else
		QByteArrayList idsList(paramIds.cbegin(), paramIds.cend());
#endif

		for (int i = 0; i < idsList.size(); i++){
			QByteArray key = idsList[i];

			auto textParamPtr = dynamic_cast<const iprm::ITextParam*>(filterParams.GetParameter(key));
			if (textParamPtr == nullptr){
				continue;
			}

			if (i > 0 || (i == 0 && !filterQuery.isEmpty())){
				filterQuery += " AND ";
			}

			QString value = textParamPtr->GetText();
			filterQuery += QString("lower(\"Document\"->>'%1') = lower('%2')").arg(qPrintable(key)).arg(value);
		}
	}

	return true;
}


bool CSqlJsonDatabaseDelegateComp::CreateTextFilterQuery(
			const imtbase::ICollectionFilter& collectionFilter,
			QString& textFilterQuery) const
{
	QByteArrayList filteringColumnIds = collectionFilter.GetFilteringInfoIds();
	if (filteringColumnIds.isEmpty()){
		return true;
	}

	QString textFilter = collectionFilter.GetTextFilter();
	if (!textFilter.isEmpty()){
		textFilterQuery = QString("\"Document\"->>'%1' ILIKE '%%2%'").arg(qPrintable(filteringColumnIds.first())).arg(textFilter);

		for (int i = 1; i < filteringColumnIds.count(); ++i){
			textFilterQuery += " OR ";

			textFilterQuery += QString("\"Document\"->>'%1' ILIKE '%%2%'").arg(qPrintable(filteringColumnIds[i])).arg(textFilter);
		}
	}

	return true;
}


bool CSqlJsonDatabaseDelegateComp::CreateTimeFilterQuery(const imtbase::ITimeFilterParam& timeFilter, QString& timeFilterQuery, const QString& /*timeFieldId*/) const
{
	QString addedStrQuery = QString(R"((SELECT "LastModified" FROM "%1" as temp WHERE "RevisionNumber" = 1 AND root."DocumentId" = temp."DocumentId" LIMIT 1))").arg(qPrintable(*m_tableNameAttrPtr));
	switch (timeFilter.GetTimeUnit()){
	case imtbase::ITimeFilterParam::TU_CUSTOM:
		break;
	case imtbase::ITimeFilterParam::TU_HOUR:
		switch(timeFilter.GetInterpretationMode()){
		case imtbase::ITimeFilterParam::IM_CURRENT:
			timeFilterQuery += QString(R"((date_trunc('hour', %1) = date_trunc('hour', current_timestamp at time zone 'utc')))").arg(addedStrQuery);
			break;
		case imtbase::ITimeFilterParam::IM_LAST:
			timeFilterQuery += QString(R"((date_trunc('hour', %1) = date_trunc('hour', current_timestamp at time zone 'utc') - interval '1 hour'))").arg(addedStrQuery);
			break;
		case imtbase::ITimeFilterParam::IM_FOR:
			timeFilterQuery += QString(R"((%1 >= current_timestamp at time zone 'utc' - interval '1 hour' and %1 <= current_timestamp at time zone 'utc'))").arg(addedStrQuery);
			break;
		}
		break;
	case imtbase::ITimeFilterParam::TU_DAY:
		switch(timeFilter.GetInterpretationMode()){
		case imtbase::ITimeFilterParam::IM_CURRENT:
			timeFilterQuery += QString(R"((date_trunc('day', %1) = date_trunc('day', current_timestamp at time zone 'utc')))").arg(addedStrQuery);
			break;
		case imtbase::ITimeFilterParam::IM_LAST:
			timeFilterQuery += QString(R"((date_trunc('day', %1) = date_trunc('day', current_timestamp at time zone 'utc') - interval '1 day'))").arg(addedStrQuery);
			break;
		case imtbase::ITimeFilterParam::IM_FOR:
			timeFilterQuery += QString(R"((%1 >= current_timestamp at time zone 'utc' - interval '1 day' and %1 <= current_timestamp at time zone 'utc'))").arg(addedStrQuery);
			break;
		}
		break;
	case imtbase::ITimeFilterParam::TU_WEEK:
		switch(timeFilter.GetInterpretationMode()){
		case imtbase::ITimeFilterParam::IM_CURRENT:
			timeFilterQuery += QString(R"((date_trunc('week', %1) = date_trunc('week', current_timestamp at time zone 'utc')))").arg(addedStrQuery);
			break;
		case imtbase::ITimeFilterParam::IM_LAST:
			timeFilterQuery += QString(R"((date_trunc('week', %1) = date_trunc('week', current_timestamp at time zone 'utc') - interval '1 week'))").arg(addedStrQuery);
			break;
		case imtbase::ITimeFilterParam::IM_FOR:
			timeFilterQuery += QString(R"((%1 >= current_timestamp at time zone 'utc' - interval '1 week' and %1 <= current_timestamp at time zone 'utc'))").arg(addedStrQuery);
			break;
		}
		break;
	case imtbase::ITimeFilterParam::TU_MONTH:
		switch(timeFilter.GetInterpretationMode()){
		case imtbase::ITimeFilterParam::IM_CURRENT:
			timeFilterQuery += QString(R"((date_trunc('month', %1) = date_trunc('month', current_timestamp at time zone 'utc')))").arg(addedStrQuery);
			break;
		case imtbase::ITimeFilterParam::IM_LAST:
			timeFilterQuery += QString(R"((date_trunc('month', %1) = date_trunc('month', current_timestamp at time zone 'utc') - interval '1 month'))").arg(addedStrQuery);
			break;
		case imtbase::ITimeFilterParam::IM_FOR:
			timeFilterQuery += QString(R"((%1 >= current_timestamp at time zone 'utc' - interval '1 month' and %1 <= current_timestamp at time zone 'utc'))").arg(addedStrQuery);
			break;
		}

		break;
	case imtbase::ITimeFilterParam::TU_YEAR:
		switch(timeFilter.GetInterpretationMode()){
		case imtbase::ITimeFilterParam::IM_CURRENT:
			timeFilterQuery += QString(R"((date_trunc('year', %1) = date_trunc('year', current_timestamp at time zone 'utc')))").arg(addedStrQuery);
			break;
		case imtbase::ITimeFilterParam::IM_LAST:
			timeFilterQuery += QString(R"((date_trunc('year', %1) = date_trunc('year', current_timestamp at time zone 'utc') - interval '1 year'))").arg(addedStrQuery);
			break;
		case imtbase::ITimeFilterParam::IM_FOR:
			timeFilterQuery += QString(R"((%1 >= current_timestamp at time zone 'utc' - interval '1 year' and %1 <= current_timestamp at time zone 'utc'))").arg(addedStrQuery);
			break;
		}
		break;
	}

	if (timeFilterQuery.isEmpty()){
		imtbase::CTimeRange timeRange = timeFilter.GetTimeRange();
		if (!timeRange.IsNull()){
			timeFilterQuery += QString(R"(date(%0) >= date('%1') AND date(%0) <= date('%2'))")
			.arg(addedStrQuery)
				.arg(timeRange.GetBeginTime().toString(Qt::ISODateWithMs))
				.arg(timeRange.GetEndTime().toString(Qt::ISODateWithMs));
		}
	}

	return true;
}


QByteArray CSqlJsonDatabaseDelegateComp::GetObjectSelectionQuery(const QByteArray& objectId, const iprm::IParamsSet* paramsPtr) const
{
	QString stateDocumentFilter;
	if (paramsPtr != nullptr){
		iprm::IParamsSet::Ids paramIds = paramsPtr->GetParamIds();
		if (paramIds.contains("DocumentFilter")){
			iprm::TParamsPtr<imtcol::IDocumentCollectionFilter> documentFilterParamPtr(paramsPtr, "DocumentFilter");
			if (documentFilterParamPtr.IsValid()){
				imtcol::IDocumentCollectionFilter::DocumentStates states = documentFilterParamPtr->GetDocumentStates();

				if (states.contains(imtcol::IDocumentCollectionFilter::DS_ACTIVE)){
					stateDocumentFilter += QString("\"IsActive\" = true");
				}

				if (states.contains(imtcol::IDocumentCollectionFilter::DS_INACTIVE)){
					if (!stateDocumentFilter.isEmpty()){
						stateDocumentFilter += QString(" OR ");
					}

					stateDocumentFilter += QString("\"IsActive\" = false");
				}
			}
		}
	}

	if (stateDocumentFilter.isEmpty()){
		stateDocumentFilter = QString("\"IsActive\" = true");
	}

	QString schemaPrefix;
	if (m_tableSchemaAttrPtr.IsValid()){
		schemaPrefix = QString("%1.").arg(qPrintable(*m_tableSchemaAttrPtr));
	}

	return QString("(SELECT * FROM %0\"%1\" WHERE (%2) AND \"DocumentId\" = '%3') ORDER BY \"RevisionNumber\" DESC;")
		.arg(schemaPrefix)
		.arg(qPrintable(*m_tableNameAttrPtr))
		.arg(stateDocumentFilter)
		.arg(qPrintable(objectId)).toUtf8();
}


QByteArray CSqlJsonDatabaseDelegateComp::CreateOperationDescriptionQuery(
	const QByteArray& objectId,
	const imtbase::IOperationContext* operationContextPtr) const
{
	if (operationContextPtr != nullptr){
		auto operationPtr = const_cast<imtbase::IOperationContext*>(operationContextPtr);
		if (operationPtr != nullptr){
			auto changeCollectionPtr = dynamic_cast<imtbase::CObjectCollection*>(operationPtr->GetChangesCollection());

			QByteArray json;
			{
				iser::CJsonMemWriteArchive archive(m_versionInfoCompPtr.GetPtr());
				if (!changeCollectionPtr->Serialize(archive)){
					qDebug() << QString("Unable to serialize a change object collection");
				}
				else{
					json = archive.GetData();
				}
			}

			QString operationDescription = json;

			imtbase::IOperationContext::IdentifableObjectInfo objectInfo = operationPtr->GetOperationOwnerId();
			return QString(R"(UPDATE "%1" SET "OwnerId" = '%2', "OwnerName" = '%3', "OperationDescription" = '%4' WHERE "IsActive" = true AND "DocumentId" = '%5';)")
				.arg(qPrintable(*m_tableNameAttrPtr))
				.arg(qPrintable(objectInfo.id))
				.arg(objectInfo.name)
				.arg(SqlEncode(operationDescription))
				.arg(qPrintable(objectId))
				.toUtf8();
		}
	}

	return QByteArray();
}



idoc::MetaInfoPtr CSqlJsonDatabaseDelegateComp::CreateObjectMetaInfo(const QByteArray& /*typeId*/) const
{
	return idoc::MetaInfoPtr(new imod::TModelWrap<idoc::CStandardDocumentMetaInfo>);
}


bool CSqlJsonDatabaseDelegateComp::WriteDataToMemory(
			const QByteArray& /*typeId*/,
			const istd::IChangeable& object,
			QByteArray& data) const
{
	auto serializableObjectPtr = const_cast<iser::ISerializable*>(dynamic_cast<const iser::ISerializable*>(&object));
	if (serializableObjectPtr == nullptr){
		Q_ASSERT(0);
		return false;
	}

	iser::CJsonMemWriteArchive archive(m_versionInfoCompPtr.GetPtr());
	if (!serializableObjectPtr->Serialize(archive)){
		SendErrorMessage(0, "Unable to write data to memory. Error: Serialization failed", "CSqlJsonDatabaseDelegateComp");
		return false;
	}

	data = archive.GetData();

	return true;
}


bool CSqlJsonDatabaseDelegateComp::ReadDataFromMemory(
			const QByteArray& /*typeId*/,
			const QByteArray& data,
			istd::IChangeable& object) const
{
	auto serializableObjectPtr = dynamic_cast<iser::ISerializable*>(&object);
	if (serializableObjectPtr == nullptr){
		Q_ASSERT(0);
		return false;
	}

	iser::CJsonMemReadArchive archive(data);

	if (!serializableObjectPtr->Serialize(archive)){
		SendErrorMessage(0, "Unable to read data from memory. Error: Serialization failed", "CSqlJsonDatabaseDelegateComp");
		return false;
	}

	return true;
}


bool CSqlJsonDatabaseDelegateComp::TableExists(const QString& tableName) const
{
	if (!m_databaseEngineCompPtr.IsValid()){
		return false;
	}

	QString tableExistsQuery = QString(R"(SELECT EXISTS (SELECT FROM information_schema.tables WHERE table_name = '%1');)").arg(tableName);

	QSqlError sqlError;
	QSqlQuery sqlQuery = m_databaseEngineCompPtr->ExecSqlQuery(tableExistsQuery.toUtf8(), &sqlError);

	if (sqlError.type() != QSqlError::NoError){
		return false;
	}

	if (!sqlQuery.next()){
		return false;
	}

	QSqlRecord record = sqlQuery.record();
	if (record.contains("exists")){
		return record.value("exists").toBool();
	}

	return false;
}


} // namespace imtdb


