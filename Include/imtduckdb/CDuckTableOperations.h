// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QVariant>

// ImtCore includes
#include <imtdb/IDatabaseEngine.h>


namespace imtduckdb
{


/**
	Operations on the tables of a DuckDB database that take more than one statement or read a result back.
*/
class CDuckTableOperations
{
public:
	/**
		Atomically replaces \a liveTableName with the freshly built \a shadowTableName by renaming,
		inside a single transaction on \a engine. Readers on other connections keep seeing the old table
		until the transaction commits, and the live table is never dropped before its replacement is in
		place; on any failure everything is rolled back and the live table is left untouched.

		Handles the first-ever build (no live table yet) by renaming the shadow table straight into
		place. DuckDB has no nested transactions, so pass \a isTransactionActive to join a transaction
		the caller already opened instead of opening a second one.
	*/
	static bool SwapTable(
				imtdb::IDatabaseEngine& engine,
				bool isTransactionActive,
				const QString& liveTableName,
				const QString& shadowTableName,
				QString* errorMessagePtr = nullptr);
	/**
		Runs \a query; \a errorMessage receives the error of a failed one.
	*/
	static bool Execute(imtdb::IDatabaseEngine& engine, const QString& query, QString& errorMessage);
	/**
		The first column of the first row of the result of \a query, or an invalid value when there is no row.
	*/
	static bool ReadValue(imtdb::IDatabaseEngine& engine, const QString& query, QVariant& value, QString& errorMessage);
	/**
		The columns of \a tableName in the current schema, in their order; empty when there is no such table.
	*/
	static bool ReadColumnNames(imtdb::IDatabaseEngine& engine, const QString& tableName, QStringList& columnNames, QString& errorMessage);
};


} // namespace imtduckdb

