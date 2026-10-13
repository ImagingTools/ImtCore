#include <imtcache/CSqlQueryCacheTableBuilderCompBase.h>


// Qt includes
#include <QtCore/QElapsedTimer>
#include <QtSql/QSqlError>

// ImtCore includes
#include <imtcache/CLoadProgress.h>


namespace imtcache
{


// protected methods

QDateTime CSqlQueryCacheTableBuilderCompBase::GetRecordTime(const QSqlQuery& /*query*/) const
{
	return QDateTime();
}


QString CSqlQueryCacheTableBuilderCompBase::FormatSourceTime(const QDateTime& time)
{
	// The source stores UTC without a zone and the cache keeps the times as it read them, so the wall time is used as is.
	return time.toString(QStringLiteral("yyyy-MM-dd'T'HH:mm:ss.zzz"));
}


// reimplemented (imtcache::CCacheTableBuilderCompBase)

bool CSqlQueryCacheTableBuilderCompBase::LoadRows(
			imtduckdb::IDuckConnection& connection,
			const QString& tableName,
			const QDateTime& since,
			BuildResult& result) const
{
	if (!m_sourceEngineCompPtr.IsValid()){
		result.errorMessage = QStringLiteral("Invalid component configuration: SourceDatabaseEngine reference missing");

		return false;
	}

	QString appenderError;
	std::unique_ptr<imtduckdb::IDuckAppender> appenderPtr = connection.CreateAppender(tableName, QString(), &appenderError);
	if (!appenderPtr){
		result.errorMessage = QStringLiteral("Unable to create an appender for %1. Error: %2").arg(tableName, appenderError);

		return false;
	}

	QElapsedTimer readTimer;
	readTimer.start();

	QSqlError sqlError;
	QSqlQuery query = m_sourceEngineCompPtr->ExecSqlQuery(GetLoadQuery(since), &sqlError);
	if (sqlError.type() != QSqlError::NoError){
		result.errorMessage = QStringLiteral("Unable to read the source rows for %1. Error: %2").arg(tableName, sqlError.text());

		return false;
	}

	// A driver that cannot tell the size up front reports -1; the progress is then simply not shown.
	CLoadProgress progress(tableName, qMax(0, query.size()), readTimer.elapsed());

	while (query.next()){
		progress.RowProcessed();

		QVariantList rowValues;
		if (!MapRecord(query, rowValues)){
			progress.RowSkipped();

			continue;
		}

		if (!appenderPtr->AppendRow(rowValues)){
			SendErrorMessage(0, QStringLiteral("Unable to append a row to %1. Error: %2").arg(tableName, appenderPtr->GetLastError()), __func__);

			continue;
		}

		const QDateTime recordTime = GetRecordTime(query);
		if (recordTime.isValid() && (!result.lastSourceUpdateTime.isValid() || recordTime > result.lastSourceUpdateTime)){
			result.lastSourceUpdateTime = recordTime;
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


} // namespace imtcache
