// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <ilog/TLoggerCompWrap.h>

// ImtCore includes
#include <imtbase/IObjectCollection.h>
#include <imtbase/IObjectCollectionIterator.h>

// IotPlatform includes
#include <imtcache/ICacheTableBuilder.h>


namespace imtcache
{


/**
	Mirrors one PostgreSQL collection into one cache table.

	Everything that is the same for every entity lives here: shadow table for a rebuild, staging
	table plus upsert for an incremental run, removal of soft-deleted rows, and the modification-time
	bookkeeping. SQL is generated from GetColumnNames() rather than written per entity, so adding a
	column means changing a list rather than editing statements in several places.

	A subclass supplies the entity-specific parts only: the column list and MapRow().
*/
class CSourceCollectionCacheTableBuilderCompBase:
			public ilog::CLoggerComponentBase,
			virtual public ICacheTableBuilder
{
public:
	using BaseClass = ilog::CLoggerComponentBase;

	I_BEGIN_BASE_COMPONENT(CSourceCollectionCacheTableBuilderCompBase)
		I_REGISTER_INTERFACE(ICacheTableBuilder)
		I_ASSIGN(m_sourceCollectionCompPtr, "SourceCollection", "PostgreSQL collection this cache table mirrors", true, "SourceCollection");
		I_ASSIGN(m_tableNameAttrPtr, "TableName", "Name of the cache table", true, "");
		I_ASSIGN(m_createTableScriptPathAttrPtr, "CreateTableScriptPath", "QRC path of the SQL script creating the cache table. ${TableName} is substituted", true, "");
		I_ASSIGN(m_objectIdColumnAttrPtr, "ObjectIdColumn", "Column holding the source document UUID, used as the upsert conflict target", true, "DocumentId");
	I_END_COMPONENT;

	// reimplemented (imtcache::ICacheTableBuilder)
	virtual QString GetCacheTableName() const override;
	virtual QStringList GetRequiredCacheTables() const override;
	virtual BuildResult Rebuild(imtduckdb::IDuckConnection& connection) const override;
	virtual BuildResult ApplyChanges(imtduckdb::IDuckConnection& connection, const QDateTime& lastSourceUpdateTime) const override;

protected:
	/// Cache table columns, in the order the create script declares them and MapRow() fills them.
	virtual QStringList GetColumnNames() const = 0;

	/**
		Converts the source object the iterator currently points at into one cache row.
		\return false to skip the row.
	*/
	virtual bool MapRow(const imtbase::IObjectCollectionIterator& iterator, QVariantList& rowValues) const = 0;

	/// Source collection, for subclasses that need it beyond the standard iteration.
	const imtbase::IObjectCollection* GetSourceCollection() const;

private:
	/// Loads source rows selected by \a filterParamsPtr into \a tableName through a bulk appender.
	bool LoadRows(
				imtduckdb::IDuckConnection& connection,
				const QString& tableName,
				const iprm::IParamsSet* filterParamsPtr,
				BuildResult& result) const;

	/// Removes cache rows for sources soft-deleted after \a lastSourceUpdateTime.
	bool RemoveDeletedRows(
				imtduckdb::IDuckConnection& connection,
				const QDateTime& lastSourceUpdateTime,
				BuildResult& result) const;

	bool CreateTable(imtduckdb::IDuckConnection& connection, const QString& tableName, QString& errorMessage) const;

	/// False when the live table's columns differ from GetColumnNames(), i.e. it predates a schema change.
	bool HasExpectedColumns(imtduckdb::IDuckConnection& connection) const;

	QString GetUpsertQuery(const QString& stagingTableName) const;

private:
	I_REF(imtbase::IObjectCollection, m_sourceCollectionCompPtr);
	I_ATTR(QByteArray, m_tableNameAttrPtr);
	I_ATTR(QByteArray, m_createTableScriptPathAttrPtr);
	I_ATTR(QByteArray, m_objectIdColumnAttrPtr);
};


} // namespace imtcache
