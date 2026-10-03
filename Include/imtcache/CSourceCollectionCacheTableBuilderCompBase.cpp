#include <imtcache/CSourceCollectionCacheTableBuilderCompBase.h>


// Qt includes
#include <QtCore/QElapsedTimer>
#include <QtCore/QSet>
#include <QtCore/QUuid>
#include <QtSql/QSqlError>
#include <QtSql/QSqlQuery>

// ACF includes
#include <iprm/CParamsSet.h>
#include <istd/TInterfacePtr.h>

// ImtCore includes
#include <imtbase/CComplexCollectionFilter.h>
#include <imtcol/CDocumentCollectionFilter.h>
#include <imtcache/CLoadProgress.h>
#include <imtdb/imtdb.h>


namespace imtcache
{


namespace
{


/// Selects the source rows changed after \a since, or all of them when \a since is not set.
void FillChangedFilter(imtbase::CComplexCollectionFilter& filter, const QByteArray& modificationTimeField, const QDateTime& since)
{
	if (!since.isValid()){
		return;
	}

	filter.AddFieldFilter(imtbase::IComplexCollectionFilter::FieldFilter(
				modificationTimeField,
				since,
				imtbase::IComplexCollectionFilter::FO_GREATER));
}

} // anonymous namespace


// protected methods

const imtbase::IObjectCollection* CSourceCollectionCacheTableBuilderCompBase::GetSourceCollection() const
{
	return m_sourceCollectionCompPtr.GetPtr();
}


bool CSourceCollectionCacheTableBuilderCompBase::LoadRows(
			imtduckdb::IDuckConnection& connection,
			const QString& tableName,
			const QDateTime& since,
			BuildResult& result) const
{
	if (!m_sourceCollectionCompPtr.IsValid()){
		result.errorMessage = QStringLiteral("Invalid component configuration: SourceCollection reference missing");

		return false;
	}

	QString appenderError;
	std::unique_ptr<imtduckdb::IDuckAppender> appenderPtr = connection.CreateAppender(tableName, QString(), &appenderError);
	if (!appenderPtr){
		result.errorMessage = QStringLiteral("Unable to create an appender for %1. Error: %2").arg(tableName, appenderError);

		return false;
	}

	// Without a cutoff no filter is passed, and the source delegate returns active rows only, which is what a rebuild wants.
	imtbase::CComplexCollectionFilter changedFilter;
	iprm::CParamsSet changedParams;
	const iprm::IParamsSet* filterParamsPtr = nullptr;
	if (since.isValid()){
		FillChangedFilter(changedFilter, *m_modificationTimeFieldAttrPtr, since);
		changedParams.SetEditableParameter(QByteArrayLiteral("ComplexFilter"), &changedFilter);
		filterParamsPtr = &changedParams;
	}

	QElapsedTimer readTimer;
	readTimer.start();

	// The SQL iterator reads the whole result set here, so this is the time spent waiting on PostgreSQL.
	istd::TUniqueInterfacePtr<imtbase::IObjectCollectionIterator> iteratorPtr(
				m_sourceCollectionCompPtr->CreateObjectCollectionIterator(QByteArray(), 0, -1, filterParamsPtr));
	if (!iteratorPtr.IsValid()){
		result.errorMessage = QStringLiteral("Unable to read the source collection for %1").arg(tableName);

		return false;
	}

	CLoadProgress progress(tableName, iteratorPtr->GetElementsCount(), readTimer.elapsed());

	while (iteratorPtr->Next()){
		progress.RowProcessed();

		QVariantList rowValues;
		if (!MapRow(*iteratorPtr, rowValues)){
			progress.RowSkipped();

			continue;
		}

		if (!appenderPtr->AppendRow(rowValues)){
			SendErrorMessage(0, QStringLiteral("Unable to append a row to %1. Error: %2").arg(tableName, appenderPtr->GetLastError()), __func__);

			continue;
		}

		const QDateTime lastModified = iteratorPtr->GetElementInfo(*m_modificationTimeFieldAttrPtr).toDateTime();
		if (lastModified.isValid() && (!result.lastSourceUpdateTime.isValid() || lastModified > result.lastSourceUpdateTime)){
			result.lastSourceUpdateTime = lastModified;
		}

		++result.rowsWritten;
	}

	progress.MappingDone();

	if (!appenderPtr->Close()){
		result.errorMessage = QStringLiteral("Unable to flush %1. Error: %2").arg(tableName, appenderPtr->GetLastError());

		return false;
	}

	progress.Finish(result.rowsWritten);

	return true;
}


bool CSourceCollectionCacheTableBuilderCompBase::RemoveDeletedRows(
			imtduckdb::IDuckConnection& connection,
			const QDateTime& lastSourceUpdateTime,
			BuildResult& result) const
{
	if (m_detectDeletionsAttrPtr.IsValid() && !*m_detectDeletionsAttrPtr){
		return true;
	}

	if (*m_reconcileDeletionsAttrPtr){
		return RemoveRowsMissingFromSource(connection, result);
	}

	return RemoveRowsDeletedSince(connection, lastSourceUpdateTime, result);
}


bool CSourceCollectionCacheTableBuilderCompBase::RemoveRowsDeletedSince(
			imtduckdb::IDuckConnection& connection,
			const QDateTime& lastSourceUpdateTime,
			BuildResult& result) const
{
	// This relies on the source delete changing the modification time, so the cutoff that finds edits
	// finds removals too. That holds for address collections; document collections do not do it.
	imtbase::CComplexCollectionFilter changedFilter;
	FillChangedFilter(changedFilter, *m_modificationTimeFieldAttrPtr, lastSourceUpdateTime);

	imtcol::CDocumentCollectionFilter documentFilter;
	documentFilter.AddDocumentState(imtcol::IDocumentCollectionFilter::DS_DISABLED);

	iprm::CParamsSet deletedParams;
	deletedParams.SetEditableParameter(QByteArrayLiteral("ComplexFilter"), &changedFilter);
	deletedParams.SetEditableParameter(*m_stateFilterParamIdAttrPtr, &documentFilter);

	istd::TUniqueInterfacePtr<imtbase::IObjectCollectionIterator> iteratorPtr(
				m_sourceCollectionCompPtr->CreateObjectCollectionIterator(QByteArray(), 0, -1, &deletedParams));
	if (!iteratorPtr.IsValid()){
		result.errorMessage = QStringLiteral("Unable to read soft-deleted rows for %1").arg(GetCacheTableName());

		return false;
	}

	QStringList deletedIds;
	while (iteratorPtr->Next()){
		const QUuid documentUuid = QUuid(QString::fromUtf8(iteratorPtr->GetObjectId()));
		if (documentUuid.isNull()){
			continue;
		}

		deletedIds << QString::fromUtf8(documentUuid.toByteArray(QUuid::WithoutBraces));

		const QDateTime lastModified = iteratorPtr->GetElementInfo(*m_modificationTimeFieldAttrPtr).toDateTime();
		if (lastModified.isValid() && (!result.lastSourceUpdateTime.isValid() || lastModified > result.lastSourceUpdateTime)){
			result.lastSourceUpdateTime = lastModified;
		}
	}

	if (deletedIds.isEmpty()){
		return true;
	}

	return DeleteRowsById(connection, deletedIds, result);
}


bool CSourceCollectionCacheTableBuilderCompBase::RemoveRowsMissingFromSource(
			imtduckdb::IDuckConnection& connection,
			BuildResult& result) const
{
	// No filter parameters: the source returns its active rows only.
	istd::TUniqueInterfacePtr<imtbase::IObjectCollectionIterator> iteratorPtr(
				m_sourceCollectionCompPtr->CreateObjectCollectionIterator(QByteArray(), 0, -1, nullptr));
	if (!iteratorPtr.IsValid()){
		// Deleting on the strength of a failed read would empty the table.
		result.errorMessage = QStringLiteral("Unable to read the active rows of %1").arg(GetCacheTableName());

		return false;
	}

	QSet<QString> activeIds;
	while (iteratorPtr->Next()){
		const QUuid documentUuid = QUuid(QString::fromUtf8(iteratorPtr->GetObjectId()));
		if (!documentUuid.isNull()){
			activeIds.insert(QString::fromUtf8(documentUuid.toByteArray(QUuid::WithoutBraces)));
		}
	}

	QSqlError sqlError;
	QSqlQuery cachedQuery = connection.ExecSqlQuery(
				QStringLiteral(R"(SELECT CAST(%1 AS VARCHAR) FROM %2)")
							.arg(imtdb::QuoteIdentifier(GetObjectIdColumn()),
								 imtdb::QuoteIdentifier(GetCacheTableName()))
							.toUtf8(),
				&sqlError);
	if (sqlError.type() != QSqlError::NoError){
		result.errorMessage = QStringLiteral("Unable to list the cached rows of %1. Error: %2").arg(GetCacheTableName(), sqlError.text());

		return false;
	}

	QStringList removedIds;
	while (cachedQuery.next()){
		const QString cachedId = cachedQuery.value(0).toString();
		if (!activeIds.contains(cachedId)){
			removedIds << cachedId;
		}
	}

	if (removedIds.isEmpty()){
		return true;
	}

	return DeleteRowsById(connection, removedIds, result);
}


bool CSourceCollectionCacheTableBuilderCompBase::DeleteRowsById(
			imtduckdb::IDuckConnection& connection,
			const QStringList& documentIds,
			BuildResult& result) const
{
	// Ids are UUID text produced by QUuid or read back from a UUID column, so quoting them is enough.
	QStringList quotedIds;
	for (const QString& documentId : documentIds){
		quotedIds << QStringLiteral("'%1'").arg(documentId);
	}

	QSqlError sqlError;
	connection.ExecSqlQuery(QStringLiteral(R"(DELETE FROM %1 WHERE %2 IN (%3))")
								.arg(imtdb::QuoteIdentifier(GetCacheTableName()),
									 imtdb::QuoteIdentifier(GetObjectIdColumn()),
									 quotedIds.join(','))
								.toUtf8(), &sqlError);

	if (sqlError.type() != QSqlError::NoError){
		result.errorMessage = QStringLiteral("Unable to delete removed rows from %1. Error: %2").arg(GetCacheTableName(), sqlError.text());

		return false;
	}

	result.rowsDeleted = documentIds.count();

	return true;
}


} // namespace imtcache
