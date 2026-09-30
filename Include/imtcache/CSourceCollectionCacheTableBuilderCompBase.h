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
		I_ASSIGN(m_modificationTimeFieldAttrPtr, "ModificationTimeField", "Field of the source collection holding the time a row was last changed. Address collections use LastModified, document collections use TimeStamp", true, "LastModified");
		I_ASSIGN(m_stateFilterParamIdAttrPtr, "StateFilterParamId", "Selection parameter id under which the source collection reads its document state filter. Address collections use State, document collections use DocumentFilter", true, "State");
		I_ASSIGN(m_reconcileDeletionsAttrPtr, "ReconcileDeletions", "Find removed rows by comparing the ids of the active source rows with the cache instead of by modification time. Required when deleting a source row does not change its modification time, and it also catches rows deleted for good. Costs a scan of all active source rows per update", true, false);
		I_ASSIGN(m_detectDeletionsAttrPtr, "DetectDeletions", "Remove cache rows whose source row was deleted. Turn off when nothing reads a removed row's leftovers and a full rebuild is an acceptable time to drop them, e.g. a collection that is not a document collection and deletes rows outright", false, true);
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

	/// Removes cache rows whose source row is gone, using the strategy selected by ReconcileDeletions.
	bool RemoveDeletedRows(
				imtduckdb::IDuckConnection& connection,
				const QDateTime& lastSourceUpdateTime,
				BuildResult& result) const;

	/// Finds removals as sources soft-deleted after \a lastSourceUpdateTime. Needs deletion to change the modification time.
	bool RemoveRowsDeletedSince(
				imtduckdb::IDuckConnection& connection,
				const QDateTime& lastSourceUpdateTime,
				BuildResult& result) const;

	/// Finds removals as cache rows whose id is not among the active source rows.
	bool RemoveRowsMissingFromSource(imtduckdb::IDuckConnection& connection, BuildResult& result) const;

	/// Deletes the cache rows with the given normalized document ids.
	bool DeleteRowsById(imtduckdb::IDuckConnection& connection, const QStringList& documentIds, BuildResult& result) const;

	bool CreateTable(imtduckdb::IDuckConnection& connection, const QString& tableName, QString& errorMessage) const;

	/// False when the live table's columns differ from GetColumnNames(), i.e. it predates a schema change.
	bool HasExpectedColumns(imtduckdb::IDuckConnection& connection) const;

	QString GetUpsertQuery(const QString& stagingTableName) const;

private:
	I_REF(imtbase::IObjectCollection, m_sourceCollectionCompPtr);
	I_ATTR(QByteArray, m_tableNameAttrPtr);
	I_ATTR(QByteArray, m_createTableScriptPathAttrPtr);
	I_ATTR(QByteArray, m_objectIdColumnAttrPtr);
	I_ATTR(QByteArray, m_modificationTimeFieldAttrPtr);
	I_ATTR(QByteArray, m_stateFilterParamIdAttrPtr);
	I_ATTR(bool, m_reconcileDeletionsAttrPtr);
	I_ATTR(bool, m_detectDeletionsAttrPtr);
};


} // namespace imtcache
