// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imttest/CSimDatabaseEngineComp.h>


// Qt includes
#include <QtCore/QCoreApplication>
#include <QtCore/QUuid>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlError>
#include <QtSql/QSqlQuery>


namespace imttest
{


// public methods

void CSimDatabaseEngineComp::AddRule(const QString& queryPart, const QStringList& columnNames, const QList<QVariantList>& rows)
{
	m_rules.append({queryPart, columnNames, rows});
}


// reimplemented (imtdb::IDatabaseEngine)

bool CSimDatabaseEngineComp::BeginTransaction() const
{
	return true;
}


bool CSimDatabaseEngineComp::FinishTransaction() const
{
	return true;
}


bool CSimDatabaseEngineComp::CancelTransaction() const
{
	return true;
}


QByteArray CSimDatabaseEngineComp::GetDatabaseDriverId() const
{
	return QByteArrayLiteral("QSQLITE");
}


QSqlQuery CSimDatabaseEngineComp::ExecSqlQuery(const QByteArray& queryString, QSqlError* sqlError, bool /*isForwardOnly*/) const
{
	if(sqlError != nullptr){
		*sqlError = QSqlError();
	}

	return CreateResult(queryString);
}


QSqlQuery CSimDatabaseEngineComp::ExecSqlQuery(const QByteArray& queryString, const QVariantMap& /*bindValues*/, QSqlError* sqlError, bool isForwardOnly) const
{
	return ExecSqlQuery(queryString, sqlError, isForwardOnly);
}


QSqlQuery CSimDatabaseEngineComp::ExecSqlQueryFromFile(const QString& filePath, QSqlError* sqlError, bool isForwardOnly) const
{
	return ExecSqlQuery(filePath.toUtf8(), sqlError, isForwardOnly);
}


QSqlQuery CSimDatabaseEngineComp::ExecSqlQueryFromFile(const QString& filePath, const QVariantMap& /*bindValues*/, QSqlError* sqlError, bool isForwardOnly) const
{
	return ExecSqlQuery(filePath.toUtf8(), sqlError, isForwardOnly);
}


// protected methods

// reimplemented (icomp::CComponentBase)

void CSimDatabaseEngineComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	m_connectionName = QUuid::createUuid().toString(QUuid::WithoutBraces);

	QSqlDatabase database = QSqlDatabase::addDatabase(QStringLiteral("QSQLITE"), m_connectionName);
	database.setDatabaseName(QStringLiteral(":memory:"));
	database.open();
}


void CSimDatabaseEngineComp::OnComponentDestroyed()
{
	/**
		ACF test objects are static (\sa I_ADD_TEST), so the engine of the last test is destroyed after the application
		of main(). QSqlDatabase must not be called then; the in-memory connection ends with the process.
	*/
	if(QCoreApplication::instance() != nullptr){
		/**
			The database handle is released before the connection is removed,
			otherwise Qt warns that the connection is still in use.
		*/
		{
			QSqlDatabase database = QSqlDatabase::database(m_connectionName, false);
			database.close();
		}
		QSqlDatabase::removeDatabase(m_connectionName);
	}

	BaseClass::OnComponentDestroyed();
}


// private methods

QSqlQuery CSimDatabaseEngineComp::CreateResult(const QString& queryString) const
{
	executedQueries.append(queryString);

	QSqlQuery retVal(QSqlDatabase::database(m_connectionName));

	const Rule* rulePtr = nullptr;
	for(const Rule& rule : m_rules){
		if(queryString.contains(rule.queryPart)){
			rulePtr = &rule;

			break;
		}
	}

	if(rulePtr == nullptr || rulePtr->rows.isEmpty()){
		retVal.exec(QStringLiteral("SELECT 1 WHERE 0"));

		return retVal;
	}

	QStringList selects;
	QVariantList values;
	for(const QVariantList& row : rulePtr->rows){
		QStringList columns;
		for(int column = 0; column < rulePtr->columnNames.size(); ++column){
			columns.append(QStringLiteral(R"(? AS "%1")").arg(rulePtr->columnNames[column]));
			values.append(row.value(column));
		}

		selects.append(QStringLiteral("SELECT ") + columns.join(QStringLiteral(", ")));
	}

	retVal.prepare(selects.join(QStringLiteral(" UNION ALL ")));
	for(const QVariant& value : values){
		retVal.addBindValue(value);
	}
	retVal.exec();

	return retVal;
}


} // namespace imttest

