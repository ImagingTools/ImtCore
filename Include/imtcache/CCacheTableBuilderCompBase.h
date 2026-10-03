// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <ilog/TLoggerCompWrap.h>

// ImtCore includes
#include <imtcache/ICacheTableBuilder.h>


namespace imtcache
{


/**
	Everything that is the same for every table the cache builds from an outside source: the shadow
	table and swap for a rebuild, the staging table and upsert for an incremental run, and the check
	that an existing table still has the columns the code expects. SQL is generated from
	GetColumnNames(), so adding a column means changing a list rather than editing several statements.

	A subclass supplies where the rows come from (LoadRows) and, when the source can lose rows, how to
	find them (RemoveDeletedRows).
*/
class CCacheTableBuilderCompBase:
			public ilog::CLoggerComponentBase,
			virtual public ICacheTableBuilder
{
public:
	using BaseClass = ilog::CLoggerComponentBase;

	I_BEGIN_BASE_COMPONENT(CCacheTableBuilderCompBase)
		I_REGISTER_INTERFACE(ICacheTableBuilder)
		I_ASSIGN(m_tableNameAttrPtr, "TableName", "Name of the cache table", true, "");
		I_ASSIGN(m_createTableScriptPathAttrPtr, "CreateTableScriptPath", "QRC path of the SQL script creating the cache table. ${TableName} is substituted", true, "");
		I_ASSIGN(m_objectIdColumnAttrPtr, "ObjectIdColumn", "Column identifying a row in the source, used as the upsert conflict target", true, "DocumentId");
	I_END_COMPONENT;

	// reimplemented (imtcache::ICacheTableBuilder)
	virtual QString GetCacheTableName() const override;
	virtual QStringList GetRequiredCacheTables() const override;
	virtual BuildResult Rebuild(imtduckdb::IDuckConnection& connection) const override;
	virtual BuildResult ApplyChanges(imtduckdb::IDuckConnection& connection, const QDateTime& lastSourceUpdateTime) const override;

protected:
	/// Cache table columns, in the order the create script declares them and a loaded row fills them.
	virtual QStringList GetColumnNames() const = 0;

	/**
		Loads the source rows into \a tableName through a bulk appender: the rows changed after
		\a since, or all of them when \a since is not set. Updates \a result.
	*/
	virtual bool LoadRows(
				imtduckdb::IDuckConnection& connection,
				const QString& tableName,
				const QDateTime& since,
				BuildResult& result) const = 0;

	/// Removes cache rows whose source row is gone. The default removes nothing.
	virtual bool RemoveDeletedRows(
				imtduckdb::IDuckConnection& connection,
				const QDateTime& lastSourceUpdateTime,
				BuildResult& result) const;

	/**
		Condition under which an incoming row may replace the one already cached, written against
		\c excluded (the incoming row) and the table. Empty means always.
	*/
	virtual QString GetReplaceCondition() const;

	/// Column identifying a row in the source.
	QString GetObjectIdColumn() const;

private:
	bool CreateTable(imtduckdb::IDuckConnection& connection, const QString& tableName, QString& errorMessage) const;

	/// False when the live table's columns differ from GetColumnNames(), i.e. it predates a schema change.
	bool HasExpectedColumns(imtduckdb::IDuckConnection& connection) const;

	QString GetUpsertQuery(const QString& stagingTableName) const;

private:
	I_ATTR(QByteArray, m_tableNameAttrPtr);
	I_ATTR(QByteArray, m_createTableScriptPathAttrPtr);
	I_ATTR(QByteArray, m_objectIdColumnAttrPtr);
};


} // namespace imtcache
