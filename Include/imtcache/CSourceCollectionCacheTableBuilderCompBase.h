// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtbase/IObjectCollection.h>
#include <imtbase/IObjectCollectionIterator.h>
#include <imtcache/CCacheTableBuilderCompBase.h>


namespace imtcache
{


/**
	Mirrors one PostgreSQL collection into one cache table.

	The table handling is in the base class; what is here is reading the collection and finding the
	rows it has lost: soft-deleted ones by the modification time, or any by comparing ids.

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
};


} // namespace imtcache
