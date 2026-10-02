// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtSql/QSqlDriver>

// 3rdParty includes
#include <duckdb.hpp>


namespace imtduckdb
{


/**
	Minimal QSqlDriver implementation that lets a duckdb::Connection be exposed through the
	standard QSqlQuery/QSqlError API without registering a Qt SQL plugin.

	Instances are created and owned directly by CDuckDatabaseEngineComp - they are never
	registered with QSqlDatabase::addDatabase().
*/
class CDuckSqlDriver: public QSqlDriver
{
public:
	explicit CDuckSqlDriver(duckdb::Connection& connection);

	// reimplemented (QSqlDriver)
	virtual bool hasFeature(DriverFeature feature) const override;
	/**
		\brief The duckdb::Connection is opened and owned by \sa CDuckDatabaseEngineComp.
				This driver is never registered with QSqlDatabase, so there is nothing to do here.
		\return BaseClass::isOpen() - true if the duckdb::Connection is open, false otherwise.
	*/
	virtual bool open(
				const QString& db,
				const QString& user = QString(),
				const QString& password = QString(),
				const QString& host = QString(),
				int port = -1,
				const QString& connOpts = QString()) override;
	virtual void close() override;
	virtual QSqlResult* createResult() const override;

	duckdb::Connection& GetConnection() const;

private:
	duckdb::Connection& m_connection;
};


} // namespace imtduckdb

