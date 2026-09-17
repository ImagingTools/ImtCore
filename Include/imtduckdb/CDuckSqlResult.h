// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtSql/QSqlResult>

// 3rdParty includes
#include <duckdb.hpp>

// std includes
#include <memory>


namespace imtduckdb
{


class CDuckSqlDriver;


/**
	QSqlResult implementation backed by a duckdb::MaterializedQueryResult.

	Created by CDuckSqlDriver::createResult() so DuckDB query results (executed via the
	DuckDB C++ API) can be consumed through the regular QSqlQuery API (next(), value(), etc.).
*/
class CDuckSqlResult: public QSqlResult
{
public:
	CDuckSqlResult(const CDuckSqlDriver* driverPtr, duckdb::Connection& connection);

protected:
	// reimplemented (QSqlResult)
	virtual bool reset(const QString& sqlQuery) override;
	virtual bool fetch(int index) override;
	virtual bool fetchFirst() override;
	virtual bool fetchLast() override;
	virtual QVariant data(int index) override;
	virtual bool isNull(int index) override;
	virtual int size() override;
	virtual int numRowsAffected() override;

private:
	static QVariant ConvertValue(const duckdb::Value& value);

private:
	duckdb::Connection& m_connection;
	std::unique_ptr<duckdb::MaterializedQueryResult> m_resultPtr;
};


} // namespace imtduckdb
