// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QMap>
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
	regular QSqlQuery API (next(), value(), etc.). Cells are converted on access. Scrollable queries
	retain native chunks for seeking. Forward-only scans release chunks as the cursor advances.
	Requesting the exact size reads any remaining chunks and preserves the current position.

	Values keep their Qt type: UUIDs become text without braces, DATE a QDate, TIME a QTime and
	finite timestamps within the Qt range a QDateTime in UTC. Exceptional timestamps and integers
	wider than the Qt integer range retain their DuckDB text. Columns containing an unsupported
	type (INTERVAL, MAP, UNION, BIT, ...) must be cast in the query.
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
	virtual void detachFromResultSet() override;
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
		Validates the query outcome and declared column types, including nested types, before any rows are fetched.
		\return true if the result is ready to read. On failure, releases the result and sets the SQL error.
	*/
	bool AcceptResult(duckdb_state state);
	/**
		Releases the result, chunks and column types and resets the cursor. Keeps the prepared statement for reuse.
	*/
	void ClearResult();
	/**
		Releases cached chunks whose first row precedes \a firstRow. The caller must drop any cursor into those chunks.
	*/
	void DiscardChunksBefore(idx_t firstRow);
	/**
		Fetches and stores the next non-empty native chunk. If \a releasePrevious is true, releases older chunks
		after a successful fetch. Pass false when reading ahead for size() to preserve the current row.
		\return true if a chunk was stored, or false at end of result or after reporting a fetch error.
	*/
	bool FetchNextChunk(bool releasePrevious);
	/**
		Converts \a column of the current row on demand, or returns an invalid QVariant if the cursor or column is invalid.
	*/
	QVariant CurrentValue(int column) const;
	/**
		Converts the value at \a row in a vector with a validated logical \a type. SQL NULL becomes an invalid QVariant.
		LIST and ARRAY become QVariantList, and STRUCT becomes QVariantMap keyed by field name.
	*/
	static QVariant ReadValue(duckdb_vector vector, duckdb_logical_type type, idx_t row);
	/**
		Copies DuckDB's text representation and releases both the returned C string and the temporary \a value.
	*/
	static QString ReadValueText(duckdb_value value);
	/**
		Reads the value in \a row of the data of a vector of a scalar type.
	*/
	static QVariant ReadScalarValue(
				const void* dataPtr,
				duckdb_logical_type type,
				idx_t row);
	/**
		\return the Qt field type, or an invalid QMetaType if \a type or any nested member is unsupported.
	*/
	static QMetaType ConvertMetaType(duckdb_logical_type type);
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
	duckdb_result m_result{};
	QSqlRecord m_record;
	QList<duckdb_logical_type> m_columnTypes;
	// Keys are the first row indices of the chunks. Chunk sizes may vary.
	QMap<idx_t, duckdb_data_chunk> m_chunks;
	idx_t m_fetchedRows = 0;
	bool m_exhausted = true;
	duckdb_data_chunk m_currentChunk = nullptr;
	idx_t m_chunkStart = 0;
};


} // namespace imtduckdb

