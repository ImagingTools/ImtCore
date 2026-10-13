// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// STL includes
#include <memory>

// ImtCore includes
#include <imtduckdb/IDuckAppender.h>

// 3rdParty includes
#include <duckdb.hpp>


namespace imtduckdb
{


/**
	IDuckAppender implementation backed directly by duckdb::Appender.

	Constructing a duckdb::Appender and every operation on it can throw (unknown table, type
	mismatch, ...); this class converts all of that into GetLastError() + bool return values so
	callers never have to deal with DuckDB exceptions directly.
*/
class CDuckAppender: public IDuckAppender
{
public:
	/**
		Creates an appender for \a tableName (optionally schema-qualified via \a schemaName).
		\return nullptr if the duckdb::Appender could not be constructed (e.g. unknown table);
				in that case, \a errorMessagePtr (if not null) is filled with the error text.
	*/
	static std::unique_ptr<IDuckAppender> Create(
				duckdb::Connection& connection,
				const QString& tableName,
				const QString& schemaName = QString(),
				QString* errorMessagePtr = nullptr);

	// reimplemented (IDuckAppender)
	virtual bool AppendRow(const QVariantList& rowValues) override;
	virtual bool Flush() override;
	virtual bool Close() override;
	virtual QString GetLastError() const override;

private:
	explicit CDuckAppender(std::unique_ptr<duckdb::Appender> appenderPtr);

private:
	std::unique_ptr<duckdb::Appender> m_appenderPtr;
	QString m_lastError;
	bool m_closed = false;
};


} // namespace imtduckdb
