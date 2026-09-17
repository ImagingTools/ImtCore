// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtduckdb/CDuckDatabaseEngineComp.h>


// Qt includes
#include <QtCore/QFile>

// imtduckdb includes
#include <imtduckdb/CDuckSqlResult.h>


namespace imtduckdb
{


// public methods

CDuckDatabaseEngineComp::CDuckDatabaseEngineComp()
{
}


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
	sqlQueryFile.open(QFile::ReadOnly);

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
	sqlQueryFile.open(QFile::ReadOnly);

	QByteArray queryString = sqlQueryFile.readAll();

	sqlQueryFile.close();

	return ExecSqlQuery(queryString, bindValues, sqlErrorPtr, isForwardOnly);
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

		try{
			m_databasePtr = std::make_unique<duckdb::DuckDB>(databasePath.isEmpty() ? std::string() : databasePath.toStdString());
			m_connectionPtr = std::make_unique<duckdb::Connection>(*m_databasePtr);
			m_driverPtr = std::make_unique<CDuckSqlDriver>(*m_connectionPtr);

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
	}

	if (justCreated){
		ExecuteDatabasePatches();
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
	bool retVal = m_migrationControllerCompPtr->DoMigration(newRevision);
	if (!retVal){
		CancelTransaction();

		return false;
	}

	return FinishTransaction();
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
