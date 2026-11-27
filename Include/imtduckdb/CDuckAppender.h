// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// STL includes
#include <memory>

// 3rdParty includes
#include <duckdb.h>

// ImtCore includes
#include <imtduckdb/IDuckAppender.h>


namespace imtduckdb
{


/**
	IDuckAppender implementation backed directly by a DuckDB appender.

	Every call of the DuckDB appender reports failure through its state, and the error text is read
	from the appender into GetLastError(), so callers only deal with bool return values.
*/
class CDuckAppender: public IDuckAppender
{
public:
	/**
		Creates an appender for \a tableName (optionally schema-qualified via \a schemaName).
		\return nullptr if the DuckDB appender could not be created (e.g. unknown table);
				in that case, \a errorMessagePtr (if not null) is filled with the error text.
	*/
	static std::unique_ptr<IDuckAppender> Create(
				duckdb_connection connection,
				const QString& tableName,
				const QString& schemaName = QString(),
				QString* errorMessagePtr = nullptr);

	/**
		Flushes the rows that were not flushed yet, as closing does, and destroys the DuckDB appender.
	*/
	virtual ~CDuckAppender() override;

	// reimplemented (IDuckAppender)
	virtual bool AppendRow(const QVariantList& rowValues) override;
	virtual bool Flush() override;
	virtual bool Close() override;
	virtual QString GetLastError() const override;

private:
	explicit CDuckAppender(duckdb_appender appender);

	/**
		Reads the error of the last failed call of \a appender.
	*/
	static QString GetAppenderError(duckdb_appender appender);

private:
	duckdb_appender m_appender;
	QString m_lastError;
	bool m_closed = false;
};


} // namespace imtduckdb

