#include <imtduckdb/CDuckTableOperations.h>


// Qt includes
#include <QtSql/QSqlError>
#include <QtSql/QSqlQuery>

// ImtCore includes
#include <imtdb/imtdb.h>


namespace imtduckdb
{


bool SwapTable(
			imtdb::IDatabaseEngine& engine,
			bool isTransactionActive,
			const QString& liveTableName,
			const QString& shadowTableName,
			QString* errorMessagePtr)
{
	const bool ownsTransaction = !isTransactionActive;
	if (ownsTransaction && !engine.BeginTransaction()){
		if (errorMessagePtr != nullptr){
			*errorMessagePtr = QStringLiteral("Unable to begin transaction");
		}

		return false;
	}

	const QString liveTableIdentifier = imtdb::QuoteIdentifier(liveTableName);
	const QString shadowTableIdentifier = imtdb::QuoteIdentifier(shadowTableName);

	QSqlError sqlError;
	QSqlQuery existsQuery = engine.ExecSqlQuery(
		QStringLiteral("SELECT EXISTS (SELECT 1 FROM information_schema.tables WHERE table_name = '%1' AND table_schema = current_schema())")
			.arg(imtdb::EscapeSql(liveTableName)).toUtf8(),
		&sqlError);

	const bool liveTableExists = sqlError.type() == QSqlError::NoError && existsQuery.next() && existsQuery.value(0).toBool();

	if (sqlError.type() == QSqlError::NoError && liveTableExists){
		// Renaming straight over an existing table is not supported, so park the old one under a backup name first.
		const QString backupTableIdentifier = imtdb::QuoteIdentifier(liveTableName + QStringLiteral("__shadow_swap_backup"));

		engine.ExecSqlQuery(QStringLiteral("DROP TABLE IF EXISTS %1").arg(backupTableIdentifier).toUtf8(), &sqlError);

		if (sqlError.type() == QSqlError::NoError){
			engine.ExecSqlQuery(QStringLiteral("ALTER TABLE %1 RENAME TO %2").arg(liveTableIdentifier, backupTableIdentifier).toUtf8(), &sqlError);
		}

		if (sqlError.type() == QSqlError::NoError){
			engine.ExecSqlQuery(QStringLiteral("ALTER TABLE %1 RENAME TO %2").arg(shadowTableIdentifier, liveTableIdentifier).toUtf8(), &sqlError);
		}

		if (sqlError.type() == QSqlError::NoError){
			engine.ExecSqlQuery(QStringLiteral("DROP TABLE %1").arg(backupTableIdentifier).toUtf8(), &sqlError);
		}
	}
	else if (sqlError.type() == QSqlError::NoError){
		engine.ExecSqlQuery(QStringLiteral("ALTER TABLE %1 RENAME TO %2").arg(shadowTableIdentifier, liveTableIdentifier).toUtf8(), &sqlError);
	}

	if (sqlError.type() != QSqlError::NoError){
		if (errorMessagePtr != nullptr){
			*errorMessagePtr = sqlError.text();
		}

		if (ownsTransaction){
			engine.CancelTransaction();
		}

		return false;
	}

	if (ownsTransaction){
		return engine.FinishTransaction();
	}

	return true;
}


} // namespace imtduckdb
