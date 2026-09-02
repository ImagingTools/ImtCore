// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QFutureWatcher>
#include <QtCore/QSet>
#include <QtCore/QTimer>

// ACF includes
#include <ifile/IFileNameParam.h>
#include <ilog/TLoggerCompWrap.h>

// ImtCore includes
#include <imtdb/IDatabaseEngine.h>


namespace imtdb
{


/**
	Garbage collector and integrity auditor for the content-addressed document file
	store written by CSqlDatabaseFileDocumentDelegateComp.

	Liveness is a single scan of the 'Document' column over ALL rows of the collection
	table - active documents, inactive revision rows and soft-deleted rows alike - which
	is correct only while the store root is used exclusively by that one table.
	An unreferenced file is deleted after it is older than the grace period; the grace
	period must exceed the longest running transaction and the backup window, since a
	file may legitimately be unreferenced while its inserting transaction is still open.
	If any row holds a non-descriptor 'Document' value, deletion is skipped entirely for
	that pass: liveness cannot be proven for content the collector cannot attribute.

	Referenced files that are missing or have an unexpected size are reported as errors
	(integrity audit). With 'AuditOnly' enabled (the default) nothing is ever deleted;
	candidates are only reported.
*/
class CFileDocumentGarbageCollectorComp:
			public QObject,
			public ilog::CLoggerComponentBase
{
	Q_OBJECT
public:
	typedef ilog::CLoggerComponentBase BaseClass;

	I_BEGIN_COMPONENT(CFileDocumentGarbageCollectorComp)
		I_ASSIGN(m_databaseEngineCompPtr, "DatabaseEngine", "Database engine used for the liveness scan of the collection table", true, "DatabaseEngine");
		I_ASSIGN(m_storageRootCompPtr, "StorageRoot", "Root folder of the document file store.\nMust be the same folder assigned to the collection's file document delegate, and used exclusively by that collection's table", true, "StorageRoot");
		I_ASSIGN(m_tableSchemaAttrPtr, "TableSchema", "Name of the schema containing the document table", false, "");
		I_ASSIGN(m_tableNameAttrPtr, "TableName", "Name of the document table referencing the store", true, "");
		I_ASSIGN(m_checkIntervalAttrPtr, "CheckInterval", "Interval of the collection pass (in msec)", true, 3600000);
		I_ASSIGN(m_gracePeriodHoursAttrPtr, "GracePeriodHours", "Minimum age of an unreferenced file before it may be deleted.\nMust exceed the longest running transaction and the backup window", true, 168);
		I_ASSIGN(m_auditOnlyAttrPtr, "AuditOnly", "If true - unreferenced files are only reported, nothing is deleted", true, true);
	I_END_COMPONENT;

protected:
	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;
	virtual void OnComponentDestroyed() override;

private:
	struct LivenessInfo
	{
		QSet<QByteArray> referencedHashes;

		/**
			Set if any row holds a 'Document' value that is not a store descriptor;
			deletion is disabled for the pass in this case.
		*/
		bool hasForeignContent = false;
	};

	bool GetLivenessInfo(LivenessInfo& livenessInfo) const;
	bool SweepStore(const LivenessInfo& livenessInfo, bool isDeletionAllowed);

private Q_SLOTS:
	void OnTimeout();

protected:
	QTimer m_timer;

protected:
	I_REF(imtdb::IDatabaseEngine, m_databaseEngineCompPtr);
	I_REF(ifile::IFileNameParam, m_storageRootCompPtr);
	I_ATTR(QByteArray, m_tableSchemaAttrPtr);
	I_ATTR(QByteArray, m_tableNameAttrPtr);
	I_ATTR(int, m_checkIntervalAttrPtr);
	I_ATTR(int, m_gracePeriodHoursAttrPtr);
	I_ATTR(bool, m_auditOnlyAttrPtr);

private:
	QFutureWatcher<bool> m_sweepWatcher;
};


} // namespace imtdb
