// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imttest/CTestDatabaseTools.h>


// Qt includes
#include <QtCore/QDebug>
#include <QtCore/QUuid>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlError>
#include <QtSql/QSqlQuery>


namespace imttest
{


// public static methods

bool CTestDatabaseTools::DropDatabase(const imtdb::IDatabaseLoginSettings& settings)
{
	/// The maintenance database of the test compositions, from which the engine also creates the test database.
	static const QString maintenanceDatabaseName = QStringLiteral("postgres");

	if(!settings.GetDatabaseName().endsWith(QStringLiteral("_test"))){
		qWarning() << "Only a test database is dropped, whose name ends with _test:" << settings.GetDatabaseName();

		return false;
	}

	const QString connectionName = QUuid::createUuid().toString(QUuid::WithoutBraces);

	bool retVal = false;
	{
		QSqlDatabase maintenanceDatabase = QSqlDatabase::addDatabase(QStringLiteral("QPSQL"), connectionName);
		maintenanceDatabase.setHostName(settings.GetHost());
		maintenanceDatabase.setPort(settings.GetPort());
		maintenanceDatabase.setUserName(settings.GetUserName());
		maintenanceDatabase.setPassword(settings.GetPassword());
		maintenanceDatabase.setDatabaseName(maintenanceDatabaseName);
		if(!maintenanceDatabase.open()){
			qWarning() << "The maintenance database could not be opened:" << maintenanceDatabase.lastError().text();
		}
		else{
			QSqlQuery dropQuery(maintenanceDatabase);
			dropQuery.exec(QStringLiteral(R"(SELECT pg_terminate_backend(pid) FROM pg_stat_activity WHERE datname = '%1' AND pid != pg_backend_pid())").arg(settings.GetDatabaseName()));

			retVal = dropQuery.exec(QStringLiteral(R"(DROP DATABASE IF EXISTS "%1")").arg(settings.GetDatabaseName()));
			if(!retVal){
				qWarning() << "The test database could not be dropped:" << dropQuery.lastError().text();
			}
		}
	}

	QSqlDatabase::removeDatabase(connectionName);

	return retVal;
}


} // namespace imttest

