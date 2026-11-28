// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QDateTime>
#include <QtCore/QMutex>
#include <QtSql/QSqlQuery>

// ACF includes
#include <ilog/TLoggerCompWrap.h>

// ImtCore includes
#include <imtbase/IChangeSetJournal.h>
#include <imtdb/IDatabaseEngine.h>
#include <imtdb/ISqlJsonXPathExtractor.h>


namespace imtdb
{


/**
	Change set journal in the tables "ChangeSets" and "ChangeSetEntries" of PostgreSQL or SQLite
	(script CreateChangeSetTables.sql, created before the first change set).
	The tables must be in the database of the document tables: the document delegates write the entries
	in the transaction of the revision (see CChangeSetJournalQueries).
	Change sets older than RetentionDays are removed with their entries; the documents keep their revisions.
*/
class CChangeSetJournalComp:
			public ilog::CLoggerComponentBase,
			virtual public imtbase::IChangeSetJournal
{
public:
	typedef ilog::CLoggerComponentBase BaseClass;

	enum
	{
		MAX_SUMMARY_LENGTH = 500,
		MAX_CHANGE_SETS = 100
	};

	I_BEGIN_COMPONENT(CChangeSetJournalComp);
		I_REGISTER_INTERFACE(imtbase::IChangeSetJournal);
		I_ASSIGN(m_databaseEngineCompPtr, "DatabaseEngine", "Database engine of the document tables", true, "DatabaseEngine");
		I_ASSIGN(m_jsonXPathExtractorCompPtr, "SqlJsonXPathExtractor", "Extractor of the revision number from the revision info of the document tables", true, "SqlJsonXPathExtractor");
		I_ASSIGN(m_retentionDaysAttrPtr, "RetentionDays", "Days a change set is kept for a rollback; 0 keeps the change sets", true, 90);
	I_END_COMPONENT;

	// reimplemented (imtbase::IChangeSetJournal)
	virtual QByteArray BeginChangeSet(
				const QByteArray& tenantId,
				const QByteArray& ownerId,
				const QByteArray& origin,
				const QString& summary,
				const QByteArray& referenceId = QByteArray()) override;
	virtual bool SetChangeSetState(const QByteArray& tenantId, const QByteArray& changeSetId, ChangeSetState state) override;
	virtual bool GetChangeSetInfo(const QByteArray& tenantId, const QByteArray& changeSetId, ChangeSetInfo& info) const override;
	virtual QList<ChangeSetInfo> GetChangeSets(const QByteArray& tenantId, const QByteArray& ownerId, int maxCount) const override;
	virtual bool GetEntries(const QByteArray& tenantId, const QByteArray& changeSetId, QList<Entry>& entries) const override;
	virtual bool GetDocumentState(
				const QByteArray& tableSchema,
				const QByteArray& tableName,
				const QByteArray& documentId,
				DocumentState& state) const override;

private:
	bool EnsureTables();
	void RemoveExpired();
	bool ExecQuery(const QString& query, const QVariantMap& bindValues, QSqlQuery& sqlQuery) const;
	bool ReadChangeSet(const QSqlQuery& sqlQuery, ChangeSetInfo& info) const;
	static bool IsIdentifier(const QByteArray& name);

private:
	I_REF(IDatabaseEngine, m_databaseEngineCompPtr);
	I_REF(ISqlJsonXPathExtractor, m_jsonXPathExtractorCompPtr);
	I_ATTR(int, m_retentionDaysAttrPtr);

	QMutex m_tablesMutex;
	bool m_areTablesCreated = false;
	QDateTime m_lastCleanup;
};


} // namespace imtdb


