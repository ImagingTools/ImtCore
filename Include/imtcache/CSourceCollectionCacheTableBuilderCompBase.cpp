// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtcache/CSourceCollectionCacheTableBuilderCompBase.h>


// Qt includes
#include <QtCore/QElapsedTimer>
#include <QtCore/QSet>
#include <QtCore/QTimeZone>
#include <QtCore/QUuid>
#include <QtSql/QSqlError>
#include <QtSql/QSqlQuery>

// ACF includes
#include <idoc/IDocumentMetaInfo.h>
#include <iprm/CParamsSet.h>
#include <istd/TInterfacePtr.h>

// ImtCore includes
#include <imtbase/CComplexCollectionFilter.h>
#include <imtbase/ICollectionInfo.h>
#include <imtcol/CDocumentCollectionFilter.h>
#include <imtcache/CCacheChangeLog.h>
#include <imtcache/CLoadProgress.h>
#include <imtdb/imtdb.h>


namespace imtcache
{


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

	if (*m_remoteSourcePageSizeAttrPtr > 0){
		return LoadRemoteRows(*appenderPtr, tableName, since, result);
	}

	return LoadLocalRows(*appenderPtr, tableName, since, result);
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


// private methods

bool CSourceCollectionCacheTableBuilderCompBase::LoadLocalRows(
			imtduckdb::IDuckAppender& appender,
			const QString& tableName,
			const QDateTime& since,
			BuildResult& result) const
{
	/**
		Without a cutoff no filter is passed, and the source delegate returns active rows only, which is what a rebuild wants.
	*/
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

	/**
		The SQL iterator reads the whole result set here, so this is the time spent waiting on PostgreSQL.
	*/
	istd::TUniqueInterfacePtr<imtbase::IObjectCollectionIterator> iteratorPtr(m_sourceCollectionCompPtr->CreateObjectCollectionIterator(QByteArray(), 0, -1, filterParamsPtr));
	if (!iteratorPtr.IsValid()){
		result.errorMessage = QStringLiteral("Unable to read the source collection for %1").arg(tableName);

		return false;
	}

	CLoadProgress progress(tableName, iteratorPtr->GetElementsCount(), readTimer.elapsed());

	while (iteratorPtr->Next()){
		progress.RowProcessed();

		const QDateTime modificationTime = iteratorPtr->GetElementInfo(*m_modificationTimeFieldAttrPtr).toDateTime();
		if (!AppendRow(*iteratorPtr, appender, tableName, modificationTime, result)){
			progress.RowSkipped();
		}
	}

	progress.MappingDone();

	if (!appender.Close()){
		result.errorMessage = QStringLiteral("Unable to flush %1. Error: %2").arg(tableName, appender.GetLastError());

		return false;
	}

	progress.Finish(result.rowsWritten);

	return true;
}


bool CSourceCollectionCacheTableBuilderCompBase::LoadRemoteRows(
			imtduckdb::IDuckAppender& appender,
			const QString& tableName,
			const QDateTime& since,
			BuildResult& result) const
{
	QElapsedTimer readTimer;
	readTimer.start();

	/**
		The pages do not say how many are to come, so the total for the progress report is asked for up front.
	*/
	imtbase::CComplexCollectionFilter countFilter;
	iprm::CParamsSet countParams;
	const iprm::IParamsSet* countParamsPtr = nullptr;
	if (since.isValid()){
		FillAfterCursorFilter(countFilter, since.toString(Qt::ISODateWithMs), QByteArray());
		countParams.SetEditableParameter(QByteArrayLiteral("ComplexFilter"), &countFilter);
		countParamsPtr = &countParams;
	}

	CLoadProgress progress(tableName, qMax(m_sourceCollectionCompPtr->GetElementsCount(countParamsPtr), 0), readTimer.elapsed());

	QString readError;
	const bool isRead = ReadRemoteRows(
				since,
				false,
				[&](const imtbase::IObjectCollectionIterator& iterator, const QDateTime& modificationTime){
					progress.RowProcessed();

					if (!AppendRow(iterator, appender, tableName, modificationTime, result)){
						progress.RowSkipped();
					}
				},
				readError
	);
	if (!isRead){
		result.errorMessage = QStringLiteral("Unable to read the source collection for %1. Error: %2").arg(tableName, readError);

		return false;
	}

	progress.MappingDone();

	if (!appender.Close()){
		result.errorMessage = QStringLiteral("Unable to flush %1. Error: %2").arg(tableName, appender.GetLastError());

		return false;
	}

	progress.Finish(result.rowsWritten);

	return true;
}


bool CSourceCollectionCacheTableBuilderCompBase::AppendRow(
			const imtbase::IObjectCollectionIterator& iterator,
			imtduckdb::IDuckAppender& appender,
			const QString& tableName,
			const QDateTime& modificationTime,
			BuildResult& result) const
{
	QVariantList rowValues;
	if (!MapRow(iterator, rowValues)){
		return false;
	}

	if (!appender.AppendRow(rowValues)){
		SendErrorMessage(0, QStringLiteral("Unable to append a row to %1. Error: %2").arg(tableName, appender.GetLastError()), __func__);

		return false;
	}

	const bool isModificationTimeValid	= modificationTime.isValid();
	const bool isLastUpdateTimeValid	= result.lastSourceUpdateTime.isValid();
	const bool isNewer					= modificationTime > result.lastSourceUpdateTime;
	if (isModificationTimeValid && (!isLastUpdateTimeValid || isNewer)){
		result.lastSourceUpdateTime = modificationTime;
	}

	++result.rowsWritten;

	return true;
}


bool CSourceCollectionCacheTableBuilderCompBase::ReadRemoteRows(
			const QDateTime& since,
			bool disabledOnly,
			const std::function<void(const imtbase::IObjectCollectionIterator&, const QDateTime&)>& onObject,
			QString& errorMessage) const
{
	const int pageSize = *m_remoteSourcePageSizeAttrPtr;

	imtcol::CDocumentCollectionFilter documentFilter;
	documentFilter.AddDocumentState(imtcol::IDocumentCollectionFilter::DS_DISABLED);

	QString cursorTime = since.isValid() ? since.toString(Qt::ISODateWithMs) : QString();
	QByteArray cursorId;

	while (true){
		imtbase::CComplexCollectionFilter pageFilter;
		FillPageSorting(pageFilter);
		FillAfterCursorFilter(pageFilter, cursorTime, cursorId);

		iprm::CParamsSet pageParams;
		pageParams.SetEditableParameter(QByteArrayLiteral("ComplexFilter"), &pageFilter);
		if (disabledOnly){
			pageParams.SetEditableParameter(*m_stateFilterParamIdAttrPtr, &documentFilter);
		}

		QElapsedTimer pageTimer;
		pageTimer.start();

		imtbase::IObjectCollectionUniquePtr pagePtr = m_sourceCollectionCompPtr->CreateSubCollection(0, pageSize, &pageParams);
		if (!pagePtr.IsValid()){
			errorMessage = QStringLiteral("the request for a page of %1 objects after '%2' failed").arg(pageSize).arg(cursorTime);

			return false;
		}

		istd::TUniqueInterfacePtr<imtbase::IObjectCollectionIterator> iteratorPtr(pagePtr->CreateObjectCollectionIterator());
		if (!iteratorPtr.IsValid()){
			errorMessage = QStringLiteral("a page of the source collection cannot be iterated");

			return false;
		}

		const QString previousTime = cursorTime;
		const QByteArray previousId = cursorId;

		int pageRows = 0;
		while (iteratorPtr->Next()){
			++pageRows;

			const QDateTime modificationTime = GetRemoteModificationTime(*iteratorPtr);
			if (!modificationTime.isValid()){
				errorMessage = QStringLiteral("object '%1' has no modification time").arg(iteratorPtr->GetObjectId());

				return false;
			}

			onObject(*iteratorPtr, modificationTime);

			cursorTime = modificationTime.toString(Qt::ISODateWithMs);
			cursorId = iteratorPtr->GetObjectId();
		}

		if (pageRows >= pageSize){
			qDebug().noquote() << QStringLiteral("%1: read %2 source objects up to %3 in %4 ms")
						.arg(GetCacheTableName())
						.arg(pageRows)
						.arg(cursorTime)
						.arg(pageTimer.elapsed());
		}

		if (pageRows < pageSize){
			return true;
		}

		/**
			A source that ignores the cursor would send the same page for ever.
		*/
		if (cursorTime == previousTime && cursorId == previousId){
			errorMessage = QStringLiteral("the source did not move past '%1'").arg(cursorTime);

			return false;
		}
	}
}


bool CSourceCollectionCacheTableBuilderCompBase::RemoveRowsDeletedSince(
			imtduckdb::IDuckConnection& connection,
			const QDateTime& lastSourceUpdateTime,
			BuildResult& result) const
{
	/**
		This relies on the source delete changing the modification time, so the cutoff that finds edits
		finds removals too. That holds for address collections; document collections do not do it.
	*/
	QStringList deletedIds;
	const auto collectDeleted = [&](const imtbase::IObjectCollectionIterator& iterator, const QDateTime& lastModified){
		const QUuid documentUuid = QUuid(iterator.GetObjectId());
		if (documentUuid.isNull()){
			return;
		}

		deletedIds << documentUuid.toString(QUuid::WithoutBraces);

		if (lastModified.isValid() && (!result.lastSourceUpdateTime.isValid() || lastModified > result.lastSourceUpdateTime)){
			result.lastSourceUpdateTime = lastModified;
		}
	};

	if (*m_remoteSourcePageSizeAttrPtr > 0){
		QString readError;
		if (!ReadRemoteRows(lastSourceUpdateTime, true, collectDeleted, readError)){
			result.errorMessage = QStringLiteral("Unable to read soft-deleted rows for %1. Error: %2").arg(GetCacheTableName(), readError);

			return false;
		}

		return deletedIds.isEmpty() || DeleteRowsById(connection, deletedIds, result);
	}

	imtbase::CComplexCollectionFilter changedFilter;
	FillChangedFilter(changedFilter, *m_modificationTimeFieldAttrPtr, lastSourceUpdateTime);

	imtcol::CDocumentCollectionFilter documentFilter;
	documentFilter.AddDocumentState(imtcol::IDocumentCollectionFilter::DS_DISABLED);

	iprm::CParamsSet deletedParams;
	deletedParams.SetEditableParameter(QByteArrayLiteral("ComplexFilter"), &changedFilter);
	deletedParams.SetEditableParameter(*m_stateFilterParamIdAttrPtr, &documentFilter);

	istd::TUniqueInterfacePtr<imtbase::IObjectCollectionIterator> iteratorPtr(m_sourceCollectionCompPtr->CreateObjectCollectionIterator(QByteArray(), 0, -1, &deletedParams));
	if (!iteratorPtr.IsValid()){
		result.errorMessage = QStringLiteral("Unable to read soft-deleted rows for %1").arg(GetCacheTableName());

		return false;
	}

	while (iteratorPtr->Next()){
		collectDeleted(*iteratorPtr, iteratorPtr->GetElementInfo(*m_modificationTimeFieldAttrPtr).toDateTime());
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
	QSet<QString> activeIds;
	const auto collectActive = [&](const imtbase::IObjectCollectionIterator& iterator){
		const QUuid documentUuid = QUuid(iterator.GetObjectId());
		if (documentUuid.isNull()){
			return;
		}

		activeIds.insert(documentUuid.toString(QUuid::WithoutBraces));
	};

	if (*m_remoteSourcePageSizeAttrPtr > 0){
		QString readError;
		const bool isRead = ReadRemoteRows(
					QDateTime(),
					false,
					[&](const imtbase::IObjectCollectionIterator& iterator, const QDateTime&){
						collectActive(iterator);
					},
					readError
		);
		if (!isRead){
			/**
				Deleting on the strength of a failed read would empty the table.
			*/
			result.errorMessage = QStringLiteral("Unable to read the active rows of %1. Error: %2").arg(GetCacheTableName(), readError);

			return false;
		}
	}
	else{
		/**
			No filter parameters: the source returns its active rows only.
		*/
		istd::TUniqueInterfacePtr<imtbase::IObjectCollectionIterator> iteratorPtr(m_sourceCollectionCompPtr->CreateObjectCollectionIterator());
		if (!iteratorPtr.IsValid()){
			/**
				Deleting on the strength of a failed read would empty the table.
			*/
			result.errorMessage = QStringLiteral("Unable to read the active rows of %1").arg(GetCacheTableName());

			return false;
		}

		while (iteratorPtr->Next()){
			collectActive(*iteratorPtr);
		}
	}

	const QByteArray cachedQuery = QStringLiteral(R"(SELECT CAST(%1 AS VARCHAR) FROM %2)")
										.arg(
											imtdb::QuoteIdentifier(GetObjectIdColumn()),
											imtdb::QuoteIdentifier(GetCacheTableName()))
										.toUtf8();
	QSqlError sqlError;
	QSqlQuery cachedSqlQuery = connection.ExecSqlQuery(cachedQuery, &sqlError);
	if (sqlError.type() != QSqlError::NoError){
		result.errorMessage = QStringLiteral("Unable to list the cached rows of %1. Error: %2").arg(GetCacheTableName(), sqlError.text());

		return false;
	}

	QStringList removedIds;
	while (cachedSqlQuery.next()){
		const QString cachedId = cachedSqlQuery.value(0).toString();
		if (activeIds.contains(cachedId)){
			continue;
		}

		removedIds << cachedId;
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
	/**
		Ids are UUID text produced by QUuid or read back from a UUID column, so quoting them is enough.
	*/
	const QString quotedIds = QStringLiteral("'%1'").arg(documentIds.join(QStringLiteral("','")));
	const QString removedCondition = QStringLiteral(R"(%1 IN (%2))")
										 .arg(imtdb::QuoteIdentifier(GetObjectIdColumn()), quotedIds);
	QString logError;
	if (!CCacheChangeLog::RecordRemoved(connection, GetCacheTableName(), GetKeyColumn(), removedCondition, logError)){
		result.errorMessage = QStringLiteral("Unable to log the removed rows of %1. Error: %2").arg(GetCacheTableName(), logError);

		return false;
	}

	const QString deletionQuery = QStringLiteral(R"(DELETE FROM %1 WHERE %2)")
									.arg(imtdb::QuoteIdentifier(GetCacheTableName()), removedCondition);
	QSqlError sqlError;
	connection.ExecSqlQuery(deletionQuery.toUtf8(), &sqlError);
	if (sqlError.type() != QSqlError::NoError){
		result.errorMessage = QStringLiteral("Unable to delete removed rows from %1. Error: %2").arg(GetCacheTableName(), sqlError.text());

		return false;
	}

	result.rowsDeleted = documentIds.count();

	return true;
}


void CSourceCollectionCacheTableBuilderCompBase::FillChangedFilter(
			imtbase::CComplexCollectionFilter& filter,
			const QByteArray& modificationTimeField,
			const QDateTime& since)
{
	if (!since.isValid()){
		return;
	}

	/**
		The time goes as ISO text, the only form the filter carries to a remote source besides numbers.
	*/
	filter.AddFieldFilter(imtbase::IComplexCollectionFilter::FieldFilter(modificationTimeField, since.toString(Qt::ISODateWithMs), imtbase::IComplexCollectionFilter::FO_GREATER));
}


void CSourceCollectionCacheTableBuilderCompBase::FillPageSorting(imtbase::CComplexCollectionFilter& filter) const
{
	imtbase::IComplexCollectionFilter::FieldMetaInfo sortInfo;
	sortInfo.flags = imtbase::IComplexCollectionFilter::SO_SORT;
	sortInfo.sortingOrder = imtbase::IComplexCollectionFilter::SO_ASC;

	filter.SetFields({
		imtbase::IComplexCollectionFilter::FieldInfo(*m_modificationTimeFieldAttrPtr, sortInfo),
		imtbase::IComplexCollectionFilter::FieldInfo(*m_sourceIdFieldAttrPtr, sortInfo)
	});
}


void CSourceCollectionCacheTableBuilderCompBase::FillAfterCursorFilter(
			imtbase::CComplexCollectionFilter& filter,
			const QString& cursorTime,
			const QByteArray& cursorId) const
{
	if (cursorTime.isEmpty()){
		return;
	}

	using Filter = imtbase::IComplexCollectionFilter;

	const Filter::FieldFilter later(*m_modificationTimeFieldAttrPtr, cursorTime, Filter::FO_GREATER);
	if (cursorId.isEmpty()){
		filter.AddFieldFilter(later);

		return;
	}

	Filter::FilterExpression sameTime;
	sameTime.fieldFilters << Filter::FieldFilter(*m_modificationTimeFieldAttrPtr, cursorTime, Filter::FO_EQUAL);
	sameTime.fieldFilters << Filter::FieldFilter(*m_sourceIdFieldAttrPtr, cursorId, Filter::FO_GREATER);

	Filter::FilterExpression expression;
	expression.logicalOperation = Filter::LO_OR;
	expression.fieldFilters << later;
	expression.filterExpressions << sameTime;

	filter.SetFilterExpression(expression);
}


QDateTime CSourceCollectionCacheTableBuilderCompBase::GetRemoteModificationTime(const imtbase::IObjectCollectionIterator& iterator)
{
	const idoc::MetaInfoPtr metaInfoPtr = iterator.GetCollectionMetaInfo();
	if (!metaInfoPtr.IsValid()){
		return QDateTime();
	}

	const QDateTime time = metaInfoPtr->GetMetaInfo(imtbase::ICollectionInfo::MIT_LAST_OPERATION_TIME).toDateTime();

	/**
		The source keeps UTC without a zone, so the time is UTC whatever it came back tagged as.
	*/
	return time.isValid() ? QDateTime(time.date(), time.time(), QTimeZone::utc()) : QDateTime();
}


} // namespace imtcache

