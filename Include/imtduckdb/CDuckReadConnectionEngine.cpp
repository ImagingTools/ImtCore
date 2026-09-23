// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtduckdb/CDuckReadConnectionEngine.h>


// Qt includes
#include <QtCore/QFile>
#include <QtCore/QLoggingCategory>

// ImtCore includes
#include <imtduckdb/CDuckSqlResult.h>


namespace imtduckdb
{


CDuckReadConnectionEngine::CDuckReadConnectionEngine(duckdb::DuckDB& database)
	:m_connectionPtr(std::make_unique<duckdb::Connection>(database))
{
	m_driverPtr = std::make_unique<CDuckSqlDriver>(*m_connectionPtr);
}


// reimplemented (imtdb::IDatabaseEngine)

bool CDuckReadConnectionEngine::BeginTransaction() const
{
	auto resultPtr = m_connectionPtr->Query("BEGIN TRANSACTION");

	return resultPtr && !resultPtr->HasError();
}


bool CDuckReadConnectionEngine::FinishTransaction() const
{
	auto resultPtr = m_connectionPtr->Query("COMMIT");

	return resultPtr && !resultPtr->HasError();
}


bool CDuckReadConnectionEngine::CancelTransaction() const
{
	auto resultPtr = m_connectionPtr->Query("ROLLBACK");

	return resultPtr && !resultPtr->HasError();
}


QByteArray CDuckReadConnectionEngine::GetDatabaseDriverId() const
{
	return QByteArrayLiteral("DUCKDB");
}


QSqlQuery CDuckReadConnectionEngine::ExecSqlQuery(const QByteArray& queryString, QSqlError* sqlErrorPtr, bool isForwardOnly) const
{
	QSqlQuery retVal(new CDuckSqlResult(m_driverPtr.get(), *m_connectionPtr));

	retVal.setForwardOnly(isForwardOnly);

	bool success = retVal.exec(QString::fromUtf8(queryString));

	const QSqlError queryError = retVal.lastError();
	if (sqlErrorPtr != nullptr){
		*sqlErrorPtr = queryError;
	}

	if (!success){
		qCritical() << __FILE__ << __LINE__
					<< "\n\t| what(): Database query failed"
					<< "\n\t| Error" << queryError.text()
					<< "\n\t| SQL-statement" << queryString;
	}

	return retVal;
}


QSqlQuery CDuckReadConnectionEngine::ExecSqlQuery(
			const QByteArray& queryString,
			const QVariantMap& bindValues,
			QSqlError* sqlErrorPtr,
			bool isForwardOnly) const
{
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


QSqlQuery CDuckReadConnectionEngine::ExecSqlQueryFromFile(const QString& filePath, QSqlError* sqlErrorPtr, bool isForwardOnly) const
{
	QFile sqlQueryFile(filePath);
	if (!sqlQueryFile.open(QFile::ReadOnly)){
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


QSqlQuery CDuckReadConnectionEngine::ExecSqlQueryFromFile(
			const QString& filePath,
			const QVariantMap& bindValues,
			QSqlError* sqlErrorPtr,
			bool isForwardOnly) const
{
	QFile sqlQueryFile(filePath);
	if (!sqlQueryFile.open(QFile::ReadOnly)){
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


} // namespace imtduckdb
