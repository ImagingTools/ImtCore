// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtSql/QSqlQuery>

// ImtCore includes
#include <imtcache/CCacheTableBuilderCompBase.h>
#include <imtdb/IDatabaseEngine.h>


namespace imtcache
{


/**
	Fills a cache table from the result of a SQL query run against the source database, for data that
	is not stored as a collection of objects, such as the readings in an archive.

	A subclass supplies the query (all rows, or only those after a moment) and how to turn one result
	row into a cache row. The table handling is in the base class.
*/
class CSqlQueryCacheTableBuilderCompBase: public CCacheTableBuilderCompBase
{
public:
	using BaseClass = CCacheTableBuilderCompBase;

	I_BEGIN_BASE_COMPONENT(CSqlQueryCacheTableBuilderCompBase)
		I_ASSIGN(m_sourceEngineCompPtr, "SourceDatabaseEngine", "Database the rows are read from", true, "SourceDatabaseEngine");
	I_END_COMPONENT;

protected:
	/**
		Query returning the rows to load: those newer than \a since, or all of them when \a since is not set.
		Use FormatSourceTime() to put \a since into the text.
	*/
	virtual QByteArray GetLoadQuery(const QDateTime& since) const = 0;

	/**
		Converts the result row \a query points at into one cache row.
		\return false to skip the row.
	*/
	virtual bool MapRecord(const QSqlQuery& query, QVariantList& rowValues) const = 0;

	/**
		Time of the result row \a query points at, in the same terms as the \a since of GetLoadQuery().
		The newest one is kept as the starting point of the next update. The default is none.
	*/
	virtual QDateTime GetRecordTime(const QSqlQuery& query) const;

	/// \a time as a literal the source database reads as a zone-less timestamp, without the zone designator that would make it shift.
	static QString FormatSourceTime(const QDateTime& time);

	// reimplemented (imtcache::CCacheTableBuilderCompBase)
	virtual bool LoadRows(
				imtduckdb::IDuckConnection& connection,
				const QString& tableName,
				const QDateTime& since,
				BuildResult& result) const override;

private:
	I_REF(imtdb::IDatabaseEngine, m_sourceEngineCompPtr);
};


} // namespace imtcache
