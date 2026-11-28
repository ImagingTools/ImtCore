// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include "CChangeSetJournalTest.h"


// STL includes
#include <memory>

// Qt includes
#include <QtCore/QFile>
#include <QtCore/QUuid>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlError>
#include <QtSql/QSqlQuery>

// ACF includes
#include <icomp/TSimComponentWrap.h>
#include <itest/CStandardTestExecutor.h>

// ImtCore includes
#include <imtbase/COperationContext.h>
#include <imtdb/CChangeSetJournalComp.h>
#include <imtdb/CChangeSetJournalQueries.h>
#include <imtdb/CSqliteXPathExtractorComp.h>
#include <imtdb/IDatabaseEngine.h>


// The script of the journal tables is a resource of the static library
static void InitChangeSetJournalResources()
{
	Q_INIT_RESOURCE(imtdb);
}

static const bool s_isJournalResourcesInitialized = (InitChangeSetJournalResources(), true);


namespace
{


const QByteArray s_tenantId = "tenant-a";
const QByteArray s_ownerId = "alice";
const QByteArray s_table = "Groups";


/**
	Engine of the test on a SQLite database in memory.
*/
class CTestEngineComp: public icomp::CComponentBase, virtual public imtdb::IDatabaseEngine
{
public:
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_COMPONENT(CTestEngineComp);
		I_REGISTER_INTERFACE(imtdb::IDatabaseEngine);
	I_END_COMPONENT;

	CTestEngineComp()
		:m_connectionName(QUuid::createUuid().toString())
	{
		QSqlDatabase database = QSqlDatabase::addDatabase(QStringLiteral("QSQLITE"), m_connectionName);
		database.setDatabaseName(QStringLiteral(":memory:"));
		database.open();
	}

	~CTestEngineComp()
	{
		{
			QSqlDatabase database = QSqlDatabase::database(m_connectionName, false);
			database.close();
		}

		QSqlDatabase::removeDatabase(m_connectionName);
	}

	// reimplemented (imtdb::IDatabaseEngine)
	virtual bool BeginTransaction() const override { return Database().transaction(); }
	virtual bool FinishTransaction() const override { return Database().commit(); }
	virtual bool CancelTransaction() const override { return Database().rollback(); }
	virtual QByteArray GetDatabaseDriverId() const override { return QByteArrayLiteral("QSQLITE"); }

	virtual QSqlQuery ExecSqlQuery(const QByteArray& queryString, QSqlError* sqlError = nullptr, bool /*isForwardOnly*/ = false) const override
	{
		QSqlQuery query(Database());
		query.exec(QString::fromUtf8(queryString));
		if (sqlError != nullptr){
			*sqlError = query.lastError();
		}

		return query;
	}

	virtual QSqlQuery ExecSqlQuery(const QByteArray& queryString, const QVariantMap& bindValues, QSqlError* sqlError = nullptr, bool /*isForwardOnly*/ = false) const override
	{
		QSqlQuery query(Database());
		query.prepare(QString::fromUtf8(queryString));
		for (QVariantMap::const_iterator iter = bindValues.constBegin(); iter != bindValues.constEnd(); ++iter){
			query.bindValue(iter.key(), iter.value());
		}
		query.exec();
		if (sqlError != nullptr){
			*sqlError = query.lastError();
		}

		return query;
	}

	virtual QSqlQuery ExecSqlQueryFromFile(const QString& /*filePath*/, QSqlError* /*sqlError*/ = nullptr, bool /*isForwardOnly*/ = false) const override
	{
		return QSqlQuery();
	}

	virtual QSqlQuery ExecSqlQueryFromFile(const QString& /*filePath*/, const QVariantMap& /*bindValues*/, QSqlError* /*sqlError*/ = nullptr, bool /*isForwardOnly*/ = false) const override
	{
		return QSqlQuery();
	}

	/**
		Run the statements as the database collection does: split at ';' in one transaction.
	*/
	bool ExecuteTransaction(const QString& statements) const
	{
		if (!BeginTransaction()){
			return false;
		}

		for (QString statement: statements.split(';')){
			if (statement.trimmed().isEmpty()){
				continue;
			}

			statement.replace('\b', ';');

			QSqlError error;
			ExecSqlQuery(statement.toUtf8(), &error);
			if (error.type() != QSqlError::NoError){
				qWarning() << error.text() << statement;
				CancelTransaction();

				return false;
			}
		}

		return FinishTransaction();
	}

	int CountRows(const QString& table) const
	{
		QSqlQuery query = ExecSqlQuery(QStringLiteral(R"(SELECT COUNT(*) FROM "%1")").arg(table).toUtf8());

		return query.next() ? query.value(0).toInt() : -1;
	}

private:
	QSqlDatabase Database() const
	{
		return QSqlDatabase::database(m_connectionName);
	}

	QString m_connectionName;
};


typedef icomp::TSimSharedComponentPtr<CTestEngineComp> TestEngine;
typedef icomp::TSimSharedComponentPtr<imtdb::CSqliteXPathExtractorComp> Extractor;
typedef icomp::TSimSharedComponentPtr<imtdb::CChangeSetJournalComp> Journal;


struct Fixture
{
	Fixture()
	{
		engine->InitComponent();
		extractor->InitComponent();
		journal->SetRef("DatabaseEngine", engine);
		journal->SetRef("SqlJsonXPathExtractor", extractor);
		journal->SetIntAttr("RetentionDays", 90);
		journal->InitComponent();

		// A document table with revisions as the document delegate has it
		engine->ExecSqlQuery(QByteArrayLiteral(R"(CREATE TABLE "Groups" ("Id" TEXT PRIMARY KEY, "DocumentId" TEXT, "RevisionInfo" TEXT, "State" TEXT))"));
	}

	QString RevisionExpression()
	{
		return extractor->ExtractXPath(QStringLiteral("RevisionInfo"), QStringLiteral("RevisionNumber"), QMetaType::Int);
	}

	std::unique_ptr<imtbase::COperationContext> CreateContext(const QByteArray& changeSetId, const QByteArray& tenantId = s_tenantId) const
	{
		std::unique_ptr<imtbase::COperationContext> retVal(new imtbase::COperationContext);
		retVal->SetOperationOwnerId(imtbase::IOperationContext::IdentifableObjectInfo(s_ownerId));
		retVal->SetTenantId(tenantId);
		retVal->SetChangeSetId(changeSetId);

		return retVal;
	}

	QString Entry(const imtbase::IOperationContext& context, const QByteArray& documentId, imtbase::IChangeSetJournal::EntryOperation operation)
	{
		return imtdb::CChangeSetJournalQueries::CreateDocumentEntryQuery(&context, QByteArray(), s_table, documentId, operation, RevisionExpression());
	}

	// The statements of the document delegate, written after the entry
	bool Create(const imtbase::IOperationContext& context, const QByteArray& documentId)
	{
		return engine->ExecuteTransaction(
					Entry(context, documentId, imtbase::IChangeSetJournal::EO_CREATED) +
					QStringLiteral(R"(INSERT INTO "Groups" VALUES ('%1', '%2', json_object('RevisionNumber', 1), 'Active');)")
								.arg(QUuid::createUuid().toString(QUuid::WithoutBraces), QString::fromUtf8(documentId)));
	}

	bool Update(const imtbase::IOperationContext& context, const QByteArray& documentId)
	{
		const QString nextRevision = QStringLiteral(R"((SELECT MAX(%1) + 1 FROM "Groups" WHERE "DocumentId" = '%2'))").arg(RevisionExpression(), QString::fromUtf8(documentId));

		return engine->ExecuteTransaction(
					Entry(context, documentId, imtbase::IChangeSetJournal::EO_UPDATED) +
					QStringLiteral(R"(UPDATE "Groups" SET "State" = 'InActive' WHERE "DocumentId" = '%1';)").arg(QString::fromUtf8(documentId)) +
					QStringLiteral(R"(INSERT INTO "Groups" VALUES ('%1', '%2', json_object('RevisionNumber', %3), 'Active');)")
								.arg(QUuid::createUuid().toString(QUuid::WithoutBraces), QString::fromUtf8(documentId), nextRevision));
	}

	bool Remove(const imtbase::IOperationContext& context, const QByteArray& documentId)
	{
		return engine->ExecuteTransaction(
					Entry(context, documentId, imtbase::IChangeSetJournal::EO_REMOVED) +
					QStringLiteral(R"(UPDATE "Groups" SET "State" = 'Disabled' WHERE "State" = 'Active' AND "DocumentId" = '%1';)").arg(QString::fromUtf8(documentId)));
	}

	bool Restore(const imtbase::IOperationContext& context, const QByteArray& documentId)
	{
		return engine->ExecuteTransaction(
					Entry(context, documentId, imtbase::IChangeSetJournal::EO_RESTORED) +
					QStringLiteral(R"(UPDATE "Groups" SET "State" = 'Active' WHERE "State" = 'Disabled' AND "DocumentId" = '%1';)").arg(QString::fromUtf8(documentId)));
	}

	TestEngine engine;
	Extractor extractor;
	Journal journal;
};


std::unique_ptr<Fixture> s_fixturePtr;


} // anonymous namespace


void CChangeSetJournalTest::init()
{
	s_fixturePtr.reset(new Fixture);
}


void CChangeSetJournalTest::cleanup()
{
	s_fixturePtr.reset();
}


void CChangeSetJournalTest::ChangeSetsOfTenantTest()
{
	Fixture& fixture = *s_fixturePtr;

	const QByteArray firstId = fixture.journal->BeginChangeSet(s_tenantId, s_ownerId, "assistant", QString(600, QChar('x')));
	QVERIFY(!firstId.isEmpty());
	QTest::qWait(5);
	const QByteArray secondId = fixture.journal->BeginChangeSet(s_tenantId, "bob", "assistant", QStringLiteral("Second"));
	const QByteArray otherTenantId = fixture.journal->BeginChangeSet("tenant-b", s_ownerId, "assistant", QStringLiteral("Other"));
	QVERIFY(fixture.journal->BeginChangeSet(QByteArray(), s_ownerId, "assistant", QString()).isEmpty());

	imtbase::IChangeSetJournal::ChangeSetInfo info;
	QVERIFY(fixture.journal->GetChangeSetInfo(s_tenantId, firstId, info));
	QCOMPARE(info.state, imtbase::IChangeSetJournal::CSS_APPLYING);
	QCOMPARE(info.ownerId, s_ownerId);
	QCOMPARE(info.origin, QByteArray("assistant"));
	QCOMPARE(info.summary.size(), int(imtdb::CChangeSetJournalComp::MAX_SUMMARY_LENGTH));
	QVERIFY(info.createdAt.isValid());

	// A change set of another tenant is not found
	QVERIFY(!fixture.journal->GetChangeSetInfo(s_tenantId, otherTenantId, info));
	QVERIFY(!fixture.journal->SetChangeSetState(s_tenantId, otherTenantId, imtbase::IChangeSetJournal::CSS_APPLIED));

	QVERIFY(fixture.journal->SetChangeSetState(s_tenantId, firstId, imtbase::IChangeSetJournal::CSS_APPLIED));
	QVERIFY(fixture.journal->GetChangeSetInfo(s_tenantId, firstId, info));
	QCOMPARE(info.state, imtbase::IChangeSetJournal::CSS_APPLIED);

	// Newest first, of one owner if given
	const QList<imtbase::IChangeSetJournal::ChangeSetInfo> all = fixture.journal->GetChangeSets(s_tenantId, QByteArray(), 10);
	QCOMPARE(all.size(), 2);
	QCOMPARE(all.at(0).id, secondId);
	QCOMPARE(all.at(1).id, firstId);

	const QList<imtbase::IChangeSetJournal::ChangeSetInfo> own = fixture.journal->GetChangeSets(s_tenantId, s_ownerId, 10);
	QCOMPARE(own.size(), 1);
	QCOMPARE(own.at(0).id, firstId);

	QCOMPARE(fixture.journal->GetChangeSets(s_tenantId, QByteArray(), 1).size(), 1);
}


void CChangeSetJournalTest::EntriesOfDocumentWritesTest()
{
	Fixture& fixture = *s_fixturePtr;

	const QByteArray changeSetId = fixture.journal->BeginChangeSet(s_tenantId, s_ownerId, "assistant", QStringLiteral("Groups"));
	const std::unique_ptr<imtbase::COperationContext> contextPtr = fixture.CreateContext(changeSetId);

	QVERIFY(fixture.Create(*contextPtr, "g1"));
	QVERIFY(fixture.Update(*contextPtr, "g1"));
	QVERIFY(fixture.Update(*contextPtr, "g1"));
	QVERIFY(fixture.Create(*contextPtr, "g2"));
	QVERIFY(fixture.Remove(*contextPtr, "g2"));
	QVERIFY(fixture.Restore(*contextPtr, "g2"));

	QList<imtbase::IChangeSetJournal::Entry> entries;
	QVERIFY(fixture.journal->GetEntries(s_tenantId, changeSetId, entries));
	QCOMPARE(entries.size(), 6);

	QCOMPARE(entries.at(0).operation, imtbase::IChangeSetJournal::EO_CREATED);
	QCOMPARE(entries.at(0).documentId, QByteArray("g1"));
	QCOMPARE(entries.at(0).tableName, s_table);
	QCOMPARE(entries.at(0).revisionBefore, qlonglong(-1));
	QCOMPARE(entries.at(0).revisionAfter, qlonglong(1));

	QCOMPARE(entries.at(1).operation, imtbase::IChangeSetJournal::EO_UPDATED);
	QCOMPARE(entries.at(1).revisionBefore, qlonglong(1));
	QCOMPARE(entries.at(1).revisionAfter, qlonglong(2));

	QCOMPARE(entries.at(2).revisionBefore, qlonglong(2));
	QCOMPARE(entries.at(2).revisionAfter, qlonglong(3));

	QCOMPARE(entries.at(4).operation, imtbase::IChangeSetJournal::EO_REMOVED);
	QCOMPARE(entries.at(4).documentId, QByteArray("g2"));
	QCOMPARE(entries.at(4).revisionBefore, qlonglong(1));
	QCOMPARE(entries.at(4).revisionAfter, qlonglong(1));

	QCOMPARE(entries.at(5).operation, imtbase::IChangeSetJournal::EO_RESTORED);
	QCOMPARE(entries.at(5).revisionAfter, qlonglong(1));

	// Removing a document that is not active writes no entry
	QVERIFY(fixture.Remove(*contextPtr, "unknown"));
	QVERIFY(fixture.journal->GetEntries(s_tenantId, changeSetId, entries));
	QCOMPARE(entries.size(), 6);

	imtbase::IChangeSetJournal::ChangeSetInfo info;
	QVERIFY(fixture.journal->GetChangeSetInfo(s_tenantId, changeSetId, info));
	QCOMPARE(info.entryCount, 6);
}


void CChangeSetJournalTest::EntriesOnlyForOpenChangeSetOfTenantTest()
{
	Fixture& fixture = *s_fixturePtr;

	const QByteArray changeSetId = fixture.journal->BeginChangeSet(s_tenantId, s_ownerId, "assistant", QString());

	// Without a change set, with an ID that is not a UUID: no statement at all
	QVERIFY(fixture.Entry(*fixture.CreateContext(QByteArray()), "g1", imtbase::IChangeSetJournal::EO_CREATED).isEmpty());
	QVERIFY(fixture.Entry(*fixture.CreateContext("x'); DROP TABLE \"Groups\"; --"), "g1", imtbase::IChangeSetJournal::EO_CREATED).isEmpty());
	QVERIFY(imtdb::CChangeSetJournalQueries::CreateDocumentEntryQuery(nullptr, QByteArray(), s_table, "g1", imtbase::IChangeSetJournal::EO_CREATED, fixture.RevisionExpression()).isEmpty());

	// An unknown change set, a change set of another tenant: the write happens, no entry
	QVERIFY(fixture.Create(*fixture.CreateContext(QUuid::createUuid().toByteArray(QUuid::WithoutBraces)), "g1"));
	QVERIFY(fixture.Update(*fixture.CreateContext(changeSetId, "tenant-b"), "g1"));
	QCOMPARE(fixture.engine->CountRows(QStringLiteral("ChangeSetEntries")), 0);
	QCOMPARE(fixture.engine->CountRows(QStringLiteral("Groups")), 2);

	// A finished change set takes no more entries
	QVERIFY(fixture.Update(*fixture.CreateContext(changeSetId), "g1"));
	QCOMPARE(fixture.engine->CountRows(QStringLiteral("ChangeSetEntries")), 1);
	QVERIFY(fixture.journal->SetChangeSetState(s_tenantId, changeSetId, imtbase::IChangeSetJournal::CSS_APPLIED));
	QVERIFY(fixture.Update(*fixture.CreateContext(changeSetId), "g1"));
	QCOMPARE(fixture.engine->CountRows(QStringLiteral("ChangeSetEntries")), 1);

	// A document ID with quotes and ';' is kept as a value
	const QByteArray strangeId = "g'; DELETE FROM \"Groups\"; --";
	const QByteArray secondId = fixture.journal->BeginChangeSet(s_tenantId, s_ownerId, "assistant", QString());
	QVERIFY(fixture.engine->ExecuteTransaction(fixture.Entry(*fixture.CreateContext(secondId), strangeId, imtbase::IChangeSetJournal::EO_CREATED)));
	QList<imtbase::IChangeSetJournal::Entry> entries;
	QVERIFY(fixture.journal->GetEntries(s_tenantId, secondId, entries));
	QCOMPARE(entries.size(), 1);
	QCOMPARE(entries.at(0).documentId, strangeId);
	QCOMPARE(fixture.engine->CountRows(QStringLiteral("Groups")), 4);
}


void CChangeSetJournalTest::UnrecordedWriteTest()
{
	Fixture& fixture = *s_fixturePtr;

	const QByteArray changeSetId = fixture.journal->BeginChangeSet(s_tenantId, s_ownerId, "assistant", QString());
	const std::unique_ptr<imtbase::COperationContext> contextPtr = fixture.CreateContext(changeSetId);

	QVERIFY(fixture.engine->ExecuteTransaction(imtdb::CChangeSetJournalQueries::CreateUnrecordedEntryQuery(contextPtr.get(), QByteArray(), "Tenants")));

	QList<imtbase::IChangeSetJournal::Entry> entries;
	QVERIFY(fixture.journal->GetEntries(s_tenantId, changeSetId, entries));
	QCOMPARE(entries.size(), 1);
	QCOMPARE(entries.at(0).operation, imtbase::IChangeSetJournal::EO_UNRECORDED);

	// The collection recognizes the statements of a delegate that records its documents
	QVERIFY(imtdb::CChangeSetJournalQueries::ContainsEntryQuery(fixture.Entry(*contextPtr, "g1", imtbase::IChangeSetJournal::EO_UPDATED).toUtf8()));
	QVERIFY(imtdb::CChangeSetJournalQueries::ContainsEntryQuery(imtdb::CChangeSetJournalQueries::CreateUnrecordedEntryQuery(contextPtr.get(), QByteArray(), "Tenants").toUtf8()));
	QVERIFY(!imtdb::CChangeSetJournalQueries::ContainsEntryQuery(QByteArrayLiteral(R"(UPDATE "Tenants" SET "Name" = 'x';)")));
	QCOMPARE(entries.at(0).tableName, QByteArray("Tenants"));
	QVERIFY(entries.at(0).documentId.isEmpty());
}


void CChangeSetJournalTest::DocumentStateTest()
{
	Fixture& fixture = *s_fixturePtr;

	const QByteArray changeSetId = fixture.journal->BeginChangeSet(s_tenantId, s_ownerId, "assistant", QString());
	const std::unique_ptr<imtbase::COperationContext> contextPtr = fixture.CreateContext(changeSetId);
	QVERIFY(fixture.Create(*contextPtr, "g1"));
	QVERIFY(fixture.Update(*contextPtr, "g1"));

	imtbase::IChangeSetJournal::DocumentState state;
	QVERIFY(fixture.journal->GetDocumentState(QByteArray(), s_table, "g1", state));
	QVERIFY(state.exists);
	QVERIFY(!state.isRemoved);
	QCOMPARE(state.revision, qlonglong(2));

	QVERIFY(fixture.Remove(*contextPtr, "g1"));
	QVERIFY(fixture.journal->GetDocumentState(QByteArray(), s_table, "g1", state));
	QVERIFY(state.isRemoved);
	QCOMPARE(state.revision, qlonglong(2));

	QVERIFY(fixture.journal->GetDocumentState(QByteArray(), s_table, "unknown", state));
	QVERIFY(!state.exists);

	// Only plain names of tables
	QVERIFY(!fixture.journal->GetDocumentState(QByteArray(), "Groups\" WHERE 1=1 --", "g1", state));
	QVERIFY(!fixture.journal->GetDocumentState("main;", s_table, "g1", state));
}


I_ADD_TEST(CChangeSetJournalTest);


