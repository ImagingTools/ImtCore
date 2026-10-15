// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QDateTime>

// STL includes
#include <functional>

// ImtCore includes
#include <imtbase/CComplexCollectionFilter.h>
#include <imtbase/IObjectCollection.h>
#include <imtbase/IObjectCollectionIterator.h>
#include <imtcache/CCacheTableBuilderCompBase.h>
#include <imtduckdb/IDuckAppender.h>


namespace imtcache
{


/**
	Mirrors one PostgreSQL collection into one cache table.

	The table handling is in the base class; what is here is reading the collection and finding the
	rows it has lost: soft-deleted ones by the modification time, or any by comparing ids.

	A local collection is read through an iterator. A remote one has no iterator, and one request for
	all of it would run past the request time limit, so when RemoteSourcePageSize is set it is read
	through CreateSubCollection, a page at a time, in (modification time, id) order. Each page starts
	after the last row of the previous one, so rows changed while the read is going on cannot shift
	the pages and make it skip a row.

	A subclass supplies the entity-specific parts only: the column list and MapRow().
*/
class CSourceCollectionCacheTableBuilderCompBase: public CCacheTableBuilderCompBase
{
public:
	using BaseClass = CCacheTableBuilderCompBase;

	I_BEGIN_BASE_COMPONENT(CSourceCollectionCacheTableBuilderCompBase)
		I_ASSIGN(m_sourceCollectionCompPtr, "SourceCollection", "PostgreSQL collection this cache table mirrors", true, "SourceCollection");
		I_ASSIGN(m_modificationTimeFieldAttrPtr, "ModificationTimeField", "Field of the source collection holding the time a row was last changed. Address collections use LastModified, document collections use TimeStamp", true, "LastModified");
		I_ASSIGN(m_stateFilterParamIdAttrPtr, "StateFilterParamId", "Selection parameter id under which the source collection reads its document state filter. Address collections use State, document collections use DocumentFilter", true, "State");
		I_ASSIGN(m_reconcileDeletionsAttrPtr, "ReconcileDeletions", "Find removed rows by comparing the ids of the active source rows with the cache instead of by modification time. Required when deleting a source row does not change its modification time, and it also catches rows deleted for good. Costs a scan of all active source rows per update", true, false);
		I_ASSIGN(m_detectDeletionsAttrPtr, "DetectDeletions", "Remove cache rows whose source row was deleted. Turn off when nothing reads a removed row's leftovers and a full rebuild is an acceptable time to drop them, e.g. a collection that is not a document collection and deletes rows outright", false, true);
		I_ASSIGN(m_remoteSourcePageSizeAttrPtr, "RemoteSourcePageSize", "Set when the source collection is remote: the most objects to ask it for in one request. A remote collection has no iterator and answers a request for everything too slowly, so it is read in pages of this size. 0 means the source is local", true, 0);
		I_ASSIGN(m_sourceIdFieldAttrPtr, "SourceIdField", "Field of the source collection holding the object id. Orders rows that share a modification time when a remote source is read in pages", true, "Id");
	I_END_COMPONENT;

protected:
	/**
		Converts the source object the iterator currently points at into one cache row.
		\return false to skip the row.
	*/
	virtual bool MapRow(const imtbase::IObjectCollectionIterator& iterator, QVariantList& rowValues) const = 0;

	/// Source collection, for subclasses that need it beyond the standard iteration.
	const imtbase::IObjectCollection* GetSourceCollection() const;

	// reimplemented (imtcache::CCacheTableBuilderCompBase)
	virtual bool LoadRows(
				imtduckdb::IDuckConnection& connection,
				const QString& tableName,
				const QDateTime& since,
				BuildResult& result) const override;
	virtual bool RemoveDeletedRows(
				imtduckdb::IDuckConnection& connection,
				const QDateTime& lastSourceUpdateTime,
				BuildResult& result) const override;

private:
	/// The loop over the local source, through an iterator.
	bool LoadLocalRows(
				imtduckdb::IDuckAppender& appender,
				const QString& tableName,
				const QDateTime& since,
				BuildResult& result) const;

	/// The loop over the remote source, page by page.
	bool LoadRemoteRows(
				imtduckdb::IDuckAppender& appender,
				const QString& tableName,
				const QDateTime& since,
				BuildResult& result) const;

	/// Maps the row the iterator points at and appends it. Returns false when the row was not written.
	bool AppendRow(
				const imtbase::IObjectCollectionIterator& iterator,
				imtduckdb::IDuckAppender& appender,
				const QString& tableName,
				const QDateTime& modificationTime,
				BuildResult& result) const;

	/**
		Calls \a onObject for the remote source rows after \a since, in pages, ending with the last. When
		\a disabledOnly is set only soft-deleted rows are read. The iterator passed to \a onObject is
		over one page, and the page is gone after the call.
		\return false when a page could not be read, leaving \a errorMessage set.
	*/
	bool ReadRemoteRows(
				const QDateTime& since,
				bool disabledOnly,
				const std::function<void(const imtbase::IObjectCollectionIterator&, const QDateTime&)>& onObject,
				QString& errorMessage) const;

	/// Selects the source rows changed after \a since, or all of them when \a since is not set.
	void FillChangedFilter(imtbase::CComplexCollectionFilter& filter, const QDateTime& since) const;

	/// Orders by modification time, then id, so that every row has one place in the sequence.
	void FillPageSorting(imtbase::CComplexCollectionFilter& filter) const;

	/// Selects the rows after the cursor: changed later than its time, or at its time with a greater id. Times are ISO text.
	void FillAfterCursorFilter(imtbase::CComplexCollectionFilter& filter, const QString& cursorTime, const QByteArray& cursorId) const;

	/// The time the remote source row the iterator points at was last changed, as UTC. Invalid when it has none.
	static QDateTime GetRemoteModificationTime(const imtbase::IObjectCollectionIterator& iterator);

	/// Finds removals as sources soft-deleted after \a lastSourceUpdateTime. Needs deletion to change the modification time.
	bool RemoveRowsDeletedSince(
				imtduckdb::IDuckConnection& connection,
				const QDateTime& lastSourceUpdateTime,
				BuildResult& result) const;

	/// Finds removals as cache rows whose id is not among the active source rows.
	bool RemoveRowsMissingFromSource(imtduckdb::IDuckConnection& connection, BuildResult& result) const;

	/// Deletes the cache rows with the given normalized document ids.
	bool DeleteRowsById(imtduckdb::IDuckConnection& connection, const QStringList& documentIds, BuildResult& result) const;

private:
	I_REF(imtbase::IObjectCollection, m_sourceCollectionCompPtr);
	I_ATTR(QByteArray, m_modificationTimeFieldAttrPtr);
	I_ATTR(QByteArray, m_stateFilterParamIdAttrPtr);
	I_ATTR(bool, m_reconcileDeletionsAttrPtr);
	I_ATTR(bool, m_detectDeletionsAttrPtr);
	I_ATTR(int, m_remoteSourcePageSizeAttrPtr);
	I_ATTR(QByteArray, m_sourceIdFieldAttrPtr);
};


} // namespace imtcache
