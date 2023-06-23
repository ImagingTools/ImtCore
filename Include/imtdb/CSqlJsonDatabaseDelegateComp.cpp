#include <imtdb/CSqlJsonDatabaseDelegateComp.h>


// ACF includes
#include <iprm/TParamsPtr.h>
#include <iprm/ITextParam.h>
#include <istd/TOptDelPtr.h>
#include <istd/CCrcCalculator.h>
#include <iser/CJsonMemReadArchive.h>
#include <iser/CJsonMemWriteArchive.h>
#include <iser/ISerializable.h>


namespace imtdb
{


// public methods

// reimplemented (imtdb::ISqlDatabaseObjectDelegate)

QByteArray CSqlJsonDatabaseDelegateComp::GetSelectionQuery(
			const QByteArray& objectId,
			int offset,
			int count,
			const iprm::IParamsSet* paramsPtr) const
{
	if (!objectId.isEmpty()){
		return QString("SELECT * FROM \"%1\" WHERE \"IsActive\" = true AND \"%2\" = '%3'")
				.arg(qPrintable(*m_tableNameAttrPtr))
				.arg(qPrintable(*m_objectIdColumnAttrPtr))
				.arg(qPrintable(objectId)).toLocal8Bit();
	}

	QByteArray selectionQuery = BaseClass::GetSelectionQuery(objectId, offset, count, paramsPtr);

	return selectionQuery;
}


istd::IChangeable* CSqlJsonDatabaseDelegateComp::CreateObjectFromRecord(const QSqlRecord& record) const
{
	if (!m_databaseEngineCompPtr.IsValid()){
		return nullptr;
	}

	if (!m_documentFactoriesCompPtr.IsValid()){
		return nullptr;
	}

	istd::TDelPtr<istd::IChangeable> documentPtr;

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
		if (m_typesCompPtr->GetOptionsCount() > 0){
			typeId = m_typesCompPtr->GetOptionId(0);
			index = 0;
		}
	}

	if (m_documentFactoriesCompPtr.GetCount() > 0 && index >= 0){
		documentPtr.SetPtr(m_documentFactoriesCompPtr.CreateInstance(index));
	}

	if (!documentPtr.IsValid()){
		return nullptr;
	}

	if (record.contains("Document")){
		QByteArray documentContent = record.value(qPrintable("Document")).toByteArray();

		if (ReadDataFromMemory(typeId, documentContent, *documentPtr)){
			return documentPtr.PopPtr();
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
		const imtbase::IOperationContext* /*operationContextPtr*/) const
{
	NewObjectQuery retVal;

	istd::TOptDelPtr<const istd::IChangeable> workingDocumentPtr;
	if (valuePtr != nullptr){
		workingDocumentPtr.SetPtr(valuePtr, false);
	}

	if (workingDocumentPtr.IsValid()){
		QByteArray documentContent;
		if (WriteDataToMemory(typeId, *workingDocumentPtr, documentContent)){
			QByteArray objectId = proposedObjectId.isEmpty() ? QUuid::createUuid().toString(QUuid::WithoutBraces).toUtf8() : proposedObjectId;
			quint32 checksum = istd::CCrcCalculator::GetCrcFromData((const quint8*)documentContent.constData(), documentContent.size());

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
					.arg(QDateTime::currentDateTimeUtc().toString(Qt::ISODate))
					.arg(checksum).toLocal8Bit();

			retVal.objectName = objectName;
		}
	}
	return retVal;
}


QByteArray CSqlJsonDatabaseDelegateComp::CreateDeleteObjectQuery(
		const imtbase::IObjectCollection& /*collection*/,
		const QByteArray& objectId,
		const imtbase::IOperationContext* /*operationContextPtr*/) const
{
	QByteArray retVal = QString("DELETE FROM \"%1\" WHERE \"%2\" = '%3';").arg(qPrintable(*m_tableNameAttrPtr)).arg(qPrintable(*m_objectIdColumnAttrPtr)).arg(qPrintable(objectId)).toLocal8Bit();

	return retVal;
}


QByteArray CSqlJsonDatabaseDelegateComp::CreateUpdateObjectQuery(
		const imtbase::IObjectCollection& /*collection*/,
		const QByteArray& objectId,
		const istd::IChangeable& object,
		const imtbase::IOperationContext* /*operationContextPtr*/,
		bool /*useExternDelegate*/) const
{
	QByteArray retVal;

	QByteArray documentContent;
	if (WriteDataToMemory("DocumentInfo", object, documentContent)){
		quint32 checksum = istd::CCrcCalculator::GetCrcFromData((const quint8*)documentContent.constData(), documentContent.size());

		QString queryStr;
		if (*m_isMultiTypeAttrPtr){
			queryStr = QString("UPDATE \"%1\" SET \"IsActive\" = false WHERE \"DocumentId\" = '%2'; INSERT INTO \"%1\" (\"DocumentId\", \"Document\", \"LastModified\", \"Checksum\", \"IsActive\", \"RevisionNumber\", \"TypeId\") VALUES('%2', '%3', '%4', '%5', true, "
								" (SELECT COUNT(\"Id\") FROM \"%1\" WHERE \"DocumentId\" = '%2') + 1 ),"
								" (SELECT \"TypeId\" FROM \"%1\" WHERE \"DocumentId\" = '%2' LIMIT 1) )," );

		}
		else{
			queryStr = QString("UPDATE \"%1\" SET \"IsActive\" = false WHERE \"DocumentId\" = '%2'; INSERT INTO \"%1\" (\"DocumentId\", \"Document\", \"LastModified\", \"Checksum\", \"IsActive\", \"RevisionNumber\") VALUES('%2', '%3', '%4', '%5', true, (SELECT COUNT(\"Id\") FROM \"%1\" WHERE \"DocumentId\" = '%2') + 1 );");
		}
		retVal = queryStr
				.arg(qPrintable(*m_tableNameAttrPtr))
				.arg(qPrintable(objectId))
				.arg(SqlEncode(documentContent))
				.arg(QDateTime::currentDateTimeUtc().toString(Qt::ISODate))
				.arg(checksum).toLocal8Bit();
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
			.arg(QDateTime::currentDateTimeUtc().toString(Qt::ISODate))
			.arg(qPrintable(*m_objectIdColumnAttrPtr))
			.arg(qPrintable(objectId)).toLocal8Bit();

	return retVal;
}


QByteArray CSqlJsonDatabaseDelegateComp::GetCountQuery(const iprm::IParamsSet* paramsPtr) const
{
	QString baseQuery = GetSelectionQuery(QByteArray(), 0, -1, paramsPtr);

	return QString("SELECT COUNT(*) FROM (%1) as t").arg(baseQuery).toLocal8Bit();
}


// reimplemented (imtdb::CSqlDatabaseDocumentDelegateComp)

QString CSqlJsonDatabaseDelegateComp::GetBaseSelectionQuery() const
{
	return QString("SELECT \"Id\", \"%1\", \"Document\", \"RevisionNumber\", \"LastModified\","
				"(SELECT \"LastModified\" FROM \"%2\" as t1 WHERE \"RevisionNumber\" = 1 AND t2.\"%1\" = t1.\"%1\" LIMIT 1) as \"Added\" FROM \"%2\""
				" as t2 WHERE \"IsActive\" = true")
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


bool CSqlJsonDatabaseDelegateComp::CreateSortQuery(const imtbase::ICollectionFilter& collectionFilter, QString& sortQuery) const
{
	QByteArray columnId;
	QByteArray sortOrder;

	if (!collectionFilter.GetSortingInfoIds().isEmpty()){
		columnId = collectionFilter.GetSortingInfoIds().first();
	}

	switch (collectionFilter.GetSortingOrder()){
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
	iprm::TParamsPtr<iprm::IParamsSet> objectFilterParamPtr(&filterParams, "ObjectFilter");
	if (objectFilterParamPtr.IsValid()){
		iprm::TParamsPtr<iprm::ITextParam> textParamPtr(objectFilterParamPtr.GetPtr(), "ObjectFilter");
		retVal = CreateObjectFilterQuery(*objectFilterParamPtr, objectFilterQuery);
		if (!retVal){
			return false;
		}
	}

	QString textFilterQuery;
	iprm::TParamsPtr<imtbase::ICollectionFilter> collectionFilterParamPtr(&filterParams, "Filter");
	if (collectionFilterParamPtr.IsValid()){
		retVal = CreateTextFilterQuery(*collectionFilterParamPtr, textFilterQuery);
		if (!retVal){
			return false;
		}
	}

	if (!objectFilterQuery.isEmpty() || !textFilterQuery.isEmpty()){
		filterQuery = " AND ";
	}

	filterQuery += objectFilterQuery;
	if (!objectFilterQuery.isEmpty() && !textFilterQuery.isEmpty()){
		filterQuery += " AND ";
	}

	if (!textFilterQuery.isEmpty()){
		filterQuery += "(" + textFilterQuery + ")";
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

			const iprm::ITextParam* textParamPtr = dynamic_cast<const iprm::ITextParam*>(filterParams.GetParameter(key));
			if (textParamPtr == nullptr){
				return false;
			}

			if (i > 0){
				filterQuery += " OR ";
			}

			QString value = textParamPtr->GetText();
			filterQuery += QString("\"Document\"->>'%1' = '%2'").arg(qPrintable(key)).arg(value);
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


bool CSqlJsonDatabaseDelegateComp::WriteDataToMemory(
			const QByteArray& /*typeId*/,
			const istd::IChangeable& object,
			QByteArray& data) const
{
	iser::ISerializable* serializableObjectPtr = const_cast<iser::ISerializable*>(dynamic_cast<const iser::ISerializable*>(&object));
	if (serializableObjectPtr == nullptr){
		Q_ASSERT(0);
		return false;
	}

	iser::CJsonMemWriteArchive archive(data, m_versionInfoCompPtr.GetPtr());

	if (!serializableObjectPtr->Serialize(archive)){
		return false;
	}
	
	return true;
}


bool CSqlJsonDatabaseDelegateComp::ReadDataFromMemory(
			const QByteArray& /*typeId*/,
			const QByteArray& data,
			istd::IChangeable& object) const
{
	iser::ISerializable* serializableObjectPtr = dynamic_cast<iser::ISerializable*>(&object);
	if (serializableObjectPtr == nullptr){
		Q_ASSERT(0);
		return false;
	}
	iser::CJsonMemReadArchive archive(data);

	if (!serializableObjectPtr->Serialize(archive)){
		return false;
	}

	return true;
}


} // namespace imtdb


