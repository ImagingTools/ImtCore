// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtSql/QSqlDriver>

// 3rdParty includes
#include <duckdb.h>


namespace imtduckdb
{


/**
	Minimal QSqlDriver implementation that lets a DuckDB connection be exposed through the
	standard QSqlQuery/QSqlError API without registering a Qt SQL plugin.

	Instances are created and owned directly by CDuckDatabaseEngineComp - they are never
	registered with QSqlDatabase::addDatabase().
*/
class CDuckSqlDriver: public QSqlDriver
{
public:
	explicit CDuckSqlDriver(duckdb_connection connection);

	// reimplemented (QSqlDriver)
	virtual bool hasFeature(DriverFeature feature) const override;
	/**
		\brief The DuckDB connection is opened and owned by \sa CDuckDatabaseEngineComp.
				This driver is never registered with QSqlDatabase, so there is nothing to do here.
		\return BaseClass::isOpen() - true if the DuckDB connection is open, false otherwise.
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

	/**
		Runs a statement without a result, such as BEGIN TRANSACTION, COMMIT or ROLLBACK.
		\return false if the statement failed, in which case \a errorMessagePtr (if not null) is filled in.
	*/
	bool ExecuteStatement(const char* statement, QString* errorMessagePtr = nullptr) const;

private:
	duckdb_connection m_connection;
};


} // namespace imtduckdb

