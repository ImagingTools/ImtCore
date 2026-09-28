// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtduckdb/CDuckDatabaseEngineComp.h>


// Qt includes
#include <QtCore/QFile>

// ImtCore includes
#include <imtduckdb/CDuckAppender.h>
#include <imtduckdb/CDuckReadConnectionEngine.h>
#include <imtduckdb/CDuckSqlResult.h>


namespace imtduckdb
{


// reimplemented (imtdb::IDatabaseEngine)

bool CDuckDatabaseEngineComp::BeginTransaction() const
{
	if (!EnsureDatabaseOpen()){
		return false;
	}

	std::lock_guard<std::mutex> lock(m_connectionMutex);

	auto resultPtr = m_connectionPtr->Query("BEGIN TRANSACTION");
	if (!resultPtr || resultPtr->HasError()){
		SendErrorMessage(0, QStringLiteral("Unable to begin transaction. Error: '%1'")
					.arg(resultPtr ? QString::fromStdString(resultPtr->GetError()) : QStringLiteral("unknown error")), __FILE__);

		return false;
	}

	return true;
}


bool CDuckDatabaseEngineComp::FinishTransaction() const
{
	if (!EnsureDatabaseOpen()){
		return false;
	}

	std::lock_guard<std::mutex> lock(m_connectionMutex);

	auto resultPtr = m_connectionPtr->Query("COMMIT");
	if (!resultPtr || resultPtr->HasError()){
		SendErrorMessage(0, QStringLiteral("Unable to commit transaction. Error: '%1'")
					.arg(resultPtr ? QString::fromStdString(resultPtr->GetError()) : QStringLiteral("unknown error")), __FILE__);

		return false;
	}

	return true;
}


bool CDuckDatabaseEngineComp::CancelTransaction() const
{
	if (!EnsureDatabaseOpen()){
		return false;
	}

	std::lock_guard<std::mutex> lock(m_connectionMutex);

	auto resultPtr = m_connectionPtr->Query("ROLLBACK");
	if (!resultPtr || resultPtr->HasError()){
		SendErrorMessage(0, QStringLiteral("Unable to rollback transaction. Error: '%1'")
					.arg(resultPtr ? QString::fromStdString(resultPtr->GetError()) : QStringLiteral("unknown error")), __FILE__);

		return false;
	}

	return true;
}


QByteArray CDuckDatabaseEngineComp::GetDatabaseDriverId() const
{
	return QByteArrayLiteral("DUCKDB");
}


QSqlQuery CDuckDatabaseEngineComp::ExecSqlQuery(const QByteArray& queryString, QSqlError* sqlErrorPtr, bool isForwardOnly) const
{
	if (!EnsureDatabaseOpen()){
		if (sqlErrorPtr != nullptr){
			*sqlErrorPtr = QSqlError(QString(), QStringLiteral("DuckDB database could not be opened"), QSqlError::ConnectionError);
		}

		return QSqlQuery();
	}

	std::lock_guard<std::mutex> lock(m_connectionMutex);

	QSqlQuery retVal(new CDuckSqlResult(m_driverPtr.get(), *m_connectionPtr));

	retVal.setForwardOnly(isForwardOnly);

	bool success = retVal.exec(QString::fromUtf8(queryString));

	const QSqlError queryError = retVal.lastError();
	if (sqlErrorPtr != nullptr){
		*sqlErrorPtr = queryError;
	}

	if (!success){
		SendErrorMessage(0, QStringLiteral("Database query failed: '%1', SQL-statement: '%2'").arg(queryError.text(), QString::fromUtf8(queryString)), __FILE__);
	}

	return retVal;
}


QSqlQuery CDuckDatabaseEngineComp::ExecSqlQuery(
			const QByteArray& queryString,
			const QVariantMap& bindValues,
			QSqlError* sqlErrorPtr,
			bool isForwardOnly) const
{
	if (!EnsureDatabaseOpen()){
		if (sqlErrorPtr != nullptr){
			*sqlErrorPtr = QSqlError(QString(), QStringLiteral("DuckDB database could not be opened"), QSqlError::ConnectionError);
		}

		return QSqlQuery();
	}

	std::lock_guard<std::mutex> lock(m_connectionMutex);

	QSqlQuery retVal(new CDuckSqlResult(m_driverPtr.get(), *m_connectionPtr));

	retVal.setForwardOnly(isForwardOnly);
	retVal.prepare(QString::fromUtf8(queryString));

	for (auto value = bindValues.cbegin(); value != bindValues.cend(); ++ value){
		retVal.bindValue(value.key(), *value);
	}

	retVal.exec();

	const QSqlError queryError = retVal.lastError();
	if (sqlErrorPtr != nullptr){
		*sqlErrorPtr = queryError;
	}

	if (queryError.type() != QSqlError::NoError){
		qCritical() << __FILE__ << __LINE__
					<< "\n\t| what(): sqlError Occured"
					<< "\n\t| Query error" << queryError.text()
					<< "\n\t| Executed query" << queryString
					<< "\n\t| Bind Values" << bindValues;
	}

	return retVal;
}


QSqlQuery CDuckDatabaseEngineComp::ExecSqlQueryFromFile(const QString& filePath, QSqlError* sqlErrorPtr, bool isForwardOnly) const
{
	QFile sqlQueryFile(filePath);
	if(!sqlQueryFile.open(QFile::ReadOnly)){
		qCritical() << __FILE__ << __LINE__
					<< "\n\t| what(): Could not open SQL file"
					<< "\n\t| File path" << filePath
					<< "\n\t| Error" << sqlQueryFile.errorString();

		return QSqlQuery();
	}

	QByteArray queryString = sqlQueryFile.readAll();

	sqlQueryFile.close();

	return ExecSqlQuery(queryString, sqlErrorPtr, isForwardOnly);
}


QSqlQuery CDuckDatabaseEngineComp::ExecSqlQueryFromFile(
			const QString& filePath,
			const QVariantMap& bindValues,
			QSqlError* sqlErrorPtr,
			bool isForwardOnly) const
{
	QFile sqlQueryFile(filePath);
	if(!sqlQueryFile.open(QFile::ReadOnly)){
		qCritical() << __FILE__ << __LINE__
					<< "\n\t| what(): Could not open SQL file"
					<< "\n\t| File path" << filePath
					<< "\n\t| Error" << sqlQueryFile.errorString();

		return QSqlQuery();
	}

	QByteArray queryString = sqlQueryFile.readAll();

	sqlQueryFile.close();

	return ExecSqlQuery(queryString, bindValues, sqlErrorPtr, isForwardOnly);
}


std::unique_ptr<IDuckAppender> CDuckDatabaseEngineComp::CreateAppender(const QString& tableName, const QString& schemaName, QString* errorMessagePtr) const
{
	if (!EnsureDatabaseOpen()){
		if (errorMessagePtr != nullptr){
			*errorMessagePtr = QStringLiteral("DuckDB database could not be opened");
		}

		return nullptr;
	}

	std::lock_guard<std::mutex> lock(m_connectionMutex);

	return CDuckAppender::Create(*m_connectionPtr, tableName, schemaName, errorMessagePtr);
}


std::unique_ptr<imtdb::IDatabaseEngine> CDuckDatabaseEngineComp::CreateReadConnection() const
{
	if (!EnsureDatabaseOpen()){
		return nullptr;
	}

	std::lock_guard<std::mutex> lock(m_connectionMutex);

	try{
		return std::make_unique<CDuckReadConnectionEngine>(*m_databasePtr);
	}
	catch (const std::exception& exception){
		SendErrorMessage(0, QStringLiteral("DuckDB read connection could not be created. Error: %1")
					.arg(QString::fromUtf8(exception.what())), __FILE__);

		return nullptr;
	}
}


bool CDuckDatabaseEngineComp::SwapTable(const QString& liveTableName, const QString& shadowTableName, QString* errorMessagePtr) const
{
	if (!EnsureDatabaseOpen()){
		if (errorMessagePtr != nullptr){
			*errorMessagePtr = QStringLiteral("DuckDB database could not be opened");
		}

		return false;
	}

	if (!BeginTransaction()){
		if (errorMessagePtr != nullptr){
			*errorMessagePtr = QStringLiteral("Unable to begin transaction");
		}

		return false;
	}

	QSqlError sqlError;
	QSqlQuery existsQuery = ExecSqlQuery(
				QStringLiteral("SELECT EXISTS (SELECT 1 FROM information_schema.tables WHERE table_name = '%1')")
							.arg(liveTableName).toUtf8(),
				&sqlError);

	const bool liveTableExists = sqlError.type() == QSqlError::NoError && existsQuery.next() && existsQuery.value(0).toBool();

	if (sqlError.type() == QSqlError::NoError && liveTableExists){
		// Renaming straight over an existing table is not supported, so park the old one under a backup name first.
		const QString backupTableName = liveTableName + QStringLiteral("__shadow_swap_backup");

		ExecSqlQuery(QStringLiteral("DROP TABLE IF EXISTS \"%1\"").arg(backupTableName).toUtf8(), &sqlError);

		if (sqlError.type() == QSqlError::NoError){
			ExecSqlQuery(QStringLiteral("ALTER TABLE \"%1\" RENAME TO \"%2\"").arg(liveTableName, backupTableName).toUtf8(), &sqlError);
		}

		if (sqlError.type() == QSqlError::NoError){
			ExecSqlQuery(QStringLiteral("ALTER TABLE \"%1\" RENAME TO \"%2\"").arg(shadowTableName, liveTableName).toUtf8(), &sqlError);
		}

		if (sqlError.type() == QSqlError::NoError){
			ExecSqlQuery(QStringLiteral("DROP TABLE \"%1\"").arg(backupTableName).toUtf8(), &sqlError);
		}
	}
	else if (sqlError.type() == QSqlError::NoError){
		ExecSqlQuery(QStringLiteral("ALTER TABLE \"%1\" RENAME TO \"%2\"").arg(shadowTableName, liveTableName).toUtf8(), &sqlError);
	}

	if (sqlError.type() != QSqlError::NoError){
		if (errorMessagePtr != nullptr){
			*errorMessagePtr = sqlError.text();
		}

		CancelTransaction();

		return false;
	}

	return FinishTransaction();
}


// reimplemented (icomp::CComponentBase)

void CDuckDatabaseEngineComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	EnsureDatabaseOpen();
}


void CDuckDatabaseEngineComp::OnComponentDestroyed()
{
	{
		std::lock_guard<std::mutex> lock(m_connectionMutex);

		m_driverPtr.reset();
		m_connectionPtr.reset();
		m_databasePtr.reset();
	}

	BaseClass::OnComponentDestroyed();
}


// private methods

bool CDuckDatabaseEngineComp::EnsureDatabaseOpen() const
{
	bool justCreated = false;

	{
		std::lock_guard<std::mutex> lock(m_connectionMutex);

		if (m_connectionPtr){
			return true;
		}

		QString databasePath = GetDatabasePath();
		const bool readOnly = m_readOnlyAttrPtr.IsValid() && *m_readOnlyAttrPtr;

		try{
			duckdb::case_insensitive_map_t<duckdb::Value> configOptions;

			const QByteArray memoryLimit = m_memoryLimitAttrPtr.IsValid() ? *m_memoryLimitAttrPtr : QByteArray();
			if (!memoryLimit.isEmpty()){
				configOptions.emplace("memory_limit", duckdb::Value(memoryLimit.toStdString()));
			}

			const int threadCount = m_threadCountAttrPtr.IsValid() ? *m_threadCountAttrPtr : 0;
			if (threadCount > 0){
				configOptions.emplace("threads", duckdb::Value::BIGINT(threadCount));
			}

			const QByteArray checkpointThreshold = m_checkpointThresholdAttrPtr.IsValid() ? *m_checkpointThresholdAttrPtr : QByteArray();
			if (!checkpointThreshold.isEmpty()){
				configOptions.emplace("checkpoint_threshold", duckdb::Value(checkpointThreshold.toStdString()));
			}

			duckdb::DBConfig config(configOptions, readOnly);

			m_databasePtr	= std::make_unique<duckdb::DuckDB>(databasePath.isEmpty() ? std::string() : databasePath.toStdString(), &config);
			m_connectionPtr = std::make_unique<duckdb::Connection>(*m_databasePtr);
			m_driverPtr		= std::make_unique<CDuckSqlDriver>(*m_connectionPtr);

			justCreated = true;
		}
		catch (const std::exception& exception){
			SendErrorMessage(0, QStringLiteral("DuckDB database '%1' could not be opened. Error: %2")
						.arg(databasePath, QString::fromUtf8(exception.what())), __FILE__);

			m_driverPtr.reset();
			m_connectionPtr.reset();
			m_databasePtr.reset();

			return false;
		}

		// A read-only database is expected to already be fully migrated; CREATE TABLE / DDL would fail on it.
		if (readOnly){
			return true;
		}
	}

	bool retVal = true;
	if (justCreated){
		retVal = CreateDatabaseMetaInfo();
	}

	if (retVal){
		retVal = ExecuteDatabasePatches();
	}

	return retVal;
}


bool CDuckDatabaseEngineComp::CreateDatabaseMetaInfo() const
{
	QSqlError sqlError;

	ExecSqlQuery(
				QByteArrayLiteral(
					R"(CREATE TABLE IF NOT EXISTS "Revisions" ()"
					"Revision INTEGER NOT NULL PRIMARY KEY, "
					"CreationDate TIMESTAMP DEFAULT CURRENT_TIMESTAMP, "
					"Description VARCHAR"
					")"),
				&sqlError);

	if (sqlError.type() != QSqlError::NoError){
		SendErrorMessage(0, QStringLiteral("\n\t| Revision table could not be created""\n\t| Error: %1").arg(sqlError.text()), __FILE__);

		return false;
	}

	return true;
}


bool CDuckDatabaseEngineComp::ExecuteDatabasePatches() const
{
	if (!m_migrationControllerCompPtr.IsValid()){
		return true;
	}

	if (!BeginTransaction()){
		return false;
	}

	int newRevision = -1;
	int databaseVersion = GetDatabaseVersion();

	bool retVal = m_migrationControllerCompPtr->DoMigration(newRevision, istd::CIntRange(databaseVersion + 1, -1));
	if (!retVal){
		CancelTransaction();

		return false;
	}

	return FinishTransaction();
}


int CDuckDatabaseEngineComp::GetDatabaseVersion() const
{
	QSqlError sqlError;

	QSqlQuery queryGetRevision = ExecSqlQuery(QByteArrayLiteral(R"(SELECT * FROM "Revisions" ORDER BY Revision DESC LIMIT 1)"), &sqlError);
	if (sqlError.type() != QSqlError::NoError){
		return -1;
	}

	if (queryGetRevision.next()){
		return queryGetRevision.value(0).toInt();
	}

	return -1;
}


QString CDuckDatabaseEngineComp::GetDatabasePath() const
{
	if (!m_dbFilePathCompPtr.IsValid()){
		SendInfoMessageOnce(0, QObject::tr("Database file path was not set. Using an in-memory DuckDB database"));

		return QString();
	}

	return m_dbFilePathCompPtr->GetPath();
}


} // namespace imtduckdb

