// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtduckdb/CDuckDatabaseEngineComp.h>


// Qt includes
#include <QtCore/QFile>

// ImtCore includes
#include <imtdb/imtdb.h>
#include <imtduckdb/CDuckAppender.h>
#include <imtduckdb/CDuckConnectionEngine.h>
#include <imtduckdb/CDuckTableOperations.h>
#include <imtduckdb/CDuckSqlResult.h>


namespace imtduckdb
{


// reimplemented (imtdb::IDatabaseEngine)

bool CDuckDatabaseEngineComp::BeginTransaction() const
{
	if (!EnsureDatabaseOpen()){
		return false;
	}

	// The lock is kept until FinishTransaction() or CancelTransaction(), so no other thread can use the connection inside the transaction.
	m_connectionMutex.lock();

	if (m_isTransactionActive){
		m_connectionMutex.unlock();

		SendErrorMessage(0, QStringLiteral("Unable to begin transaction. A transaction is already active"), __FILE__);

		return false;
	}

	auto resultPtr = m_connectionPtr->Query("BEGIN TRANSACTION");
	if (!resultPtr || resultPtr->HasError()){
		m_connectionMutex.unlock();

		SendErrorMessage(0, QStringLiteral("Unable to begin transaction. Error: '%1'")
								.arg(resultPtr ? QString::fromStdString(resultPtr->GetError()) : QStringLiteral("unknown error")), __FILE__);

		return false;
	}

	m_isTransactionActive = true;

	return true;
}


bool CDuckDatabaseEngineComp::FinishTransaction() const
{
	return EndTransaction("COMMIT", QStringLiteral("commit"));
}


bool CDuckDatabaseEngineComp::CancelTransaction() const
{
	return EndTransaction("ROLLBACK", QStringLiteral("rollback"));
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

	std::lock_guard<std::recursive_mutex> lock(m_connectionMutex);

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

	std::lock_guard<std::recursive_mutex> lock(m_connectionMutex);

	QSqlQuery retVal(new CDuckSqlResult(m_driverPtr.get(), *m_connectionPtr));

	retVal.setForwardOnly(isForwardOnly);

	bool success = retVal.prepare(QString::fromUtf8(queryString));
	if (success){
		for (auto value = bindValues.cbegin(); value != bindValues.cend(); ++ value){
			retVal.bindValue(value.key(), *value);
		}

		success = retVal.exec();
	}

	const QSqlError queryError = retVal.lastError();
	if (sqlErrorPtr != nullptr){
		*sqlErrorPtr = queryError;
	}

	if (!success){
		SendErrorMessage(0, QStringLiteral("Database query failed: '%1', SQL-statement: '%2'").arg(queryError.text(), QString::fromUtf8(queryString)), __FILE__);
	}

	return retVal;
}


QSqlQuery CDuckDatabaseEngineComp::ExecSqlQueryFromFile(const QString& filePath, QSqlError* sqlErrorPtr, bool isForwardOnly) const
{
	QByteArray queryString;
	if (!ReadSqlFile(filePath, queryString, sqlErrorPtr)){
		return QSqlQuery();
	}

	return ExecSqlQuery(queryString, sqlErrorPtr, isForwardOnly);
}


QSqlQuery CDuckDatabaseEngineComp::ExecSqlQueryFromFile(
	const QString& filePath,
	const QVariantMap& bindValues,
	QSqlError* sqlErrorPtr,
	bool isForwardOnly) const
{
	QByteArray queryString;
	if (!ReadSqlFile(filePath, queryString, sqlErrorPtr)){
		return QSqlQuery();
	}

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

	std::lock_guard<std::recursive_mutex> lock(m_connectionMutex);

	return CDuckAppender::Create(*m_connectionPtr, tableName, schemaName, errorMessagePtr);
}


std::unique_ptr<IDuckConnection> CDuckDatabaseEngineComp::CreateConnection() const
{
	if (!EnsureDatabaseOpen()){
		return nullptr;
	}

	std::lock_guard<std::recursive_mutex> lock(m_connectionMutex);

	try{
		return std::make_unique<CDuckConnectionEngine>(*m_databasePtr);
	}
	catch (const std::exception& exception){
		SendErrorMessage(0, QStringLiteral("DuckDB connection could not be created. Error: %1")
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

	// Keeps the whole swap atomic for other threads, including the check of the transaction state.
	std::lock_guard<std::recursive_mutex> lock(m_connectionMutex);

	return imtduckdb::SwapTable(*this, m_isTransactionActive, liveTableName, shadowTableName, errorMessagePtr);
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
		std::lock_guard<std::recursive_mutex> lock(m_connectionMutex);

		m_driverPtr.reset();
		m_connectionPtr.reset();
		m_databasePtr.reset();

		m_isTransactionActive = false;
	}

	BaseClass::OnComponentDestroyed();
}


// private methods

bool CDuckDatabaseEngineComp::EnsureDatabaseOpen() const
{
	// The lock is held during creation and migration, so other threads never see a connection that is not fully initialized.
	// The migration calls back into the engine on the same thread, which is why the mutex is recursive.
	std::lock_guard<std::recursive_mutex> lock(m_connectionMutex);

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

	if (!CreateDatabaseMetaInfo() || !ExecuteDatabasePatches()){
		SendErrorMessage(0, QStringLiteral("DuckDB database '%1' could not be migrated. Continuing with the current schema").arg(databasePath), __FILE__);
	}

	return true;
}


bool CDuckDatabaseEngineComp::EndTransaction(const char* statement, const QString& actionName) const
{
	if (!EnsureDatabaseOpen()){
		return false;
	}

	std::lock_guard<std::recursive_mutex> lock(m_connectionMutex);

	if (!m_isTransactionActive){
		SendErrorMessage(0, QStringLiteral("Unable to %1 transaction. No transaction is active").arg(actionName), __FILE__);

		return false;
	}

	auto resultPtr = m_connectionPtr->Query(statement);

	const bool isSuccessful = resultPtr && !resultPtr->HasError();
	if (!isSuccessful){
		SendErrorMessage(0, QStringLiteral("Unable to %1 transaction. Error: '%2'")
							 .arg(actionName, resultPtr ? QString::fromStdString(resultPtr->GetError()) : QStringLiteral("unknown error")), __FILE__);
	}

	m_isTransactionActive = false;

	// Release the lock taken in BeginTransaction().
	m_connectionMutex.unlock();

	return isSuccessful;
}


bool CDuckDatabaseEngineComp::ReadSqlFile(const QString& filePath, QByteArray& queryString, QSqlError* sqlErrorPtr) const
{
	QFile sqlQueryFile(filePath);
	if (!sqlQueryFile.open(QFile::ReadOnly)){
		const QString errorText = QStringLiteral("Could not open SQL file '%1'. Error: %2").arg(filePath, sqlQueryFile.errorString());

		SendErrorMessage(0, errorText, __FILE__);

		if (sqlErrorPtr != nullptr){
			*sqlErrorPtr = QSqlError(QString(), errorText, QSqlError::UnknownError);
		}

		return false;
	}

	queryString = sqlQueryFile.readAll();

	sqlQueryFile.close();

	return true;
}


bool CDuckDatabaseEngineComp::CreateDatabaseMetaInfo() const
{
	QSqlError sqlError;

	ExecSqlQuery(
		QByteArrayLiteral(
			"CREATE TABLE IF NOT EXISTS \"Revisions\" ("
			"Revision INTEGER NOT NULL PRIMARY KEY, "
			"CreationDate TIMESTAMP DEFAULT CURRENT_TIMESTAMP, "
			"Description VARCHAR"
			")"),
		&sqlError);

	if (sqlError.type() != QSqlError::NoError){
		SendErrorMessage(0, QStringLiteral("\n\t| Revision table could not be created\n\t| Error: %1").arg(sqlError.text()), __FILE__);

		return false;
	}

	return true;
}


bool CDuckDatabaseEngineComp::ExecuteDatabasePatches() const
{
	if (!m_migrationControllerCompPtr.IsValid()){
		return true;
	}

	int newRevision = -1;
	int databaseVersion = GetDatabaseVersion();

	if (!BeginTransaction()){
		return false;
	}

	bool retVal = m_migrationControllerCompPtr->DoMigration(newRevision, istd::CIntRange(databaseVersion + 1, -1));
	if (!retVal){
		CancelTransaction();

		return false;
	}

	if (newRevision >= 0){
		QSqlError sqlError;
		ExecSqlQuery(QStringLiteral(R"(INSERT OR REPLACE INTO "Revisions" (Revision) VALUES (%1))").arg(newRevision).toUtf8(), &sqlError);

		if (sqlError.type() != QSqlError::NoError){
			SendErrorMessage(0, QStringLiteral("Setting the database revision failed: '%1'").arg(sqlError.text()), __FILE__);

			CancelTransaction();

			return false;
		}
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