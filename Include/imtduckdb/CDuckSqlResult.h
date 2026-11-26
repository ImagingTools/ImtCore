// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtSql/QSqlRecord>
#include <QtSql/QSqlResult>

// 3rdParty includes
#include <duckdb.h>


namespace imtduckdb
{


class CDuckSqlDriver;


/**
	QSqlResult implementation over the DuckDB C API.

	Created by CDuckSqlDriver::createResult() so DuckDB query results can be consumed through the
	regular QSqlQuery API (next(), value(), etc.). The rows of a result are read once, when the
	query has finished, so that values can be read in any order.

	Values keep their Qt type: UUIDs become text without braces, DATE a QDate, TIME a QTime and
	TIMESTAMP a QDateTime in UTC. A column of a type without a Qt counterpart (INTERVAL, MAP, UNION,
	BIT, ...) fails the query; cast it in SQL instead.
*/
class CDuckSqlResult: public QSqlResult
{
public:
	CDuckSqlResult(const CDuckSqlDriver* driverPtr, duckdb_connection connection);
	virtual ~CDuckSqlResult() override;

protected:
	// reimplemented (QSqlResult)
	virtual bool reset(const QString& sqlQuery) override;
	virtual bool prepare(const QString& sqlQuery) override;
	virtual bool exec() override;
	virtual bool fetch(int index) override;
	virtual bool fetchFirst() override;
	virtual bool fetchLast() override;
	virtual QVariant data(int index) override;
	virtual bool isNull(int index) override;
	virtual int size() override;
	virtual int numRowsAffected() override;
	virtual QSqlRecord record() const override;

private:
	/**
		Reads the outcome of a finished query and destroys \a result.
		\return false and sets the last error if \a state reports a failure or a column cannot be read.
	*/
	bool AcceptResult(duckdb_state state, duckdb_result& result);
	/**
		Appends \a count values of \a vector, starting at row \a offset, to \a values. A NULL becomes an invalid QVariant.
		\return false if \a type, or the type of a nested element, has no Qt counterpart.
	*/
	static bool ReadValues(
				duckdb_vector vector,
				duckdb_logical_type type,
				idx_t offset,
				idx_t count,
				QVariantList& values);
	/**
		ReadValues() for a LIST or ARRAY vector. Every row becomes a QVariantList.
	*/
	static bool ReadListValues(
				duckdb_vector vector,
				duckdb_logical_type type,
				idx_t offset,
				idx_t count,
				QVariantList& values);
	/**
		ReadValues() for a STRUCT vector. Every row becomes a QVariantMap by field name.
	*/
	static bool ReadStructValues(
				duckdb_vector vector,
				duckdb_logical_type type,
				idx_t offset,
				idx_t count,
				QVariantList& values);
	/**
		Reads the value in \a row of the data of a vector of a scalar type.
	*/
	static QVariant ReadScalarValue(
				const void* dataPtr,
				duckdb_logical_type type,
				idx_t row);
	/**
		\return the Qt type values of \a typeId are read as, or an invalid QMetaType if the type has no Qt counterpart.
	*/
	static QMetaType ConvertMetaType(duckdb_type typeId);
	/**
		Translates Qt's Oracle-style ':name' placeholders into DuckDB's '$name' named-parameter syntax,
		leaving quoted string/identifier literals and '::type' casts untouched.
	*/
	static QString ToDuckDbNamedPlaceholders(const QString& query);

private:
	duckdb_connection m_connection;
	duckdb_prepared_statement m_preparedStatement = nullptr;

	/**
		DUCKDB_RESULT_TYPE_INVALID while there is no result.
	*/
	duckdb_result_type m_resultType = DUCKDB_RESULT_TYPE_INVALID;
	idx_t m_rowsChanged = 0;
	qsizetype m_rowCount = 0;
	/**
		Names and types of the columns, without values.
	*/
	QSqlRecord m_record;
	/**
		Values by column, then by row.
	*/
	QList<QVariantList> m_columns;
};


} // namespace imtduckdb

