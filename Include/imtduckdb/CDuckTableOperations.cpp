// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtduckdb/CDuckTableOperations.h>


// Qt includes
#include <QtSql/QSqlError>
#include <QtSql/QSqlQuery>

// ImtCore includes
#include <imtdb/imtdb.h>


namespace imtduckdb
{


// public static methods

bool CDuckTableOperations::SwapTable(
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

	const QString liveTableIdentifier	= imtdb::QuoteIdentifier(liveTableName);
	const QString shadowTableIdentifier = imtdb::QuoteIdentifier(shadowTableName);

	QSqlError sqlError;
	QSqlQuery existsQuery = engine.ExecSqlQuery(
				QStringLiteral("SELECT EXISTS (SELECT 1 FROM information_schema.tables WHERE table_name = '%1' AND table_schema = current_schema())")
							.arg(imtdb::EscapeSql(liveTableName)).toUtf8(),
				&sqlError);

	const bool liveTableExists = sqlError.type() == QSqlError::NoError && existsQuery.next() && existsQuery.value(0).toBool();

	if (sqlError.type() == QSqlError::NoError && liveTableExists){
		/**
			Renaming straight over an existing table is not supported, so the old one is parked under a backup name first.
		*/
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

	if (!ownsTransaction){
		return true;
	}

	return engine.FinishTransaction();
}


bool CDuckTableOperations::Execute(imtdb::IDatabaseEngine& engine, const QString& query, QString& errorMessage)
{
	QSqlError sqlError;
	engine.ExecSqlQuery(query.toUtf8(), &sqlError);
	if (sqlError.type() != QSqlError::NoError){
		errorMessage = sqlError.text();

		return false;
	}

	return true;
}


bool CDuckTableOperations::ReadValue(imtdb::IDatabaseEngine& engine, const QString& query, QVariant& value, QString& errorMessage)
{
	QSqlError sqlError;
	QSqlQuery result = engine.ExecSqlQuery(query.toUtf8(), &sqlError);
	if (sqlError.type() != QSqlError::NoError){
		errorMessage = sqlError.text();

		return false;
	}

	value = result.next() ? result.value(0) : QVariant();

	return true;
}


} // namespace imtduckdb

