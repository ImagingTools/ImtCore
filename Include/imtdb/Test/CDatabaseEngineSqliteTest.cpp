// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include "CDatabaseEngineSqliteTest.h"


// STL includes
#include <memory>

// Qt includes
#include <QtCore/QElapsedTimer>
#include <QtCore/QTemporaryDir>
#include <QtCore/QUuid>
#include <QtCore/QThread>
#include <QtCore/QVariantMap>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlError>
#include <QtSql/QSqlQuery>

// ACF includes
#include <iattr/TAttribute.h>
#include <icomp/CRegistryElement.h>

// ImtCore includes
#include <imtdb/CSqliteConnectionConfiguratorComp.h>
#include <imtdb/IDatabaseConnectionConfigurator.h>
#include <imtdb/IDatabaseEngine.h>
#include <GeneratedFiles/imtdbTest/CDatabaseEnginesTest.h>


// The SQL scripts of the engine (the revision tables) are resources of the static library
static void InitImtDbResources()
{
	Q_INIT_RESOURCE(imtdb);
}

static const bool s_isResourcesInitialized = (InitImtDbResources(), true);


namespace
{


typedef imtdb::CSqliteConnectionConfiguratorComp Configurator;


// The values of the attributes of the exported settings of the composition (the ids are the ExportId of the composition)
const QByteArray s_journalMode = QByteArrayLiteral("JournalMode");
const QByteArray s_synchronous = QByteArrayLiteral("Synchronous");
const QByteArray s_foreignKeys = QByteArrayLiteral("ForeignKeys");
const QByteArray s_busyTimeout = QByteArrayLiteral("BusyTimeoutMs");


template <class Value>
void SetSetting(icomp::CRegistryElement& element, const QByteArray& attributeId, const Value& value)
{
	icomp::IRegistryElement::AttributeInfo* infoPtr = element.InsertAttributeInfo(attributeId, iattr::TAttribute<Value>::GetTypeName());
	Q_ASSERT(infoPtr != nullptr);

	infoPtr->attributePtr.SetPtr(new iattr::TAttribute<Value>(value));
}


void SetSetting(icomp::CRegistryElement& element, const QByteArray& attributeId, const QVariant& value)
{
	switch (value.typeId()){
	case QMetaType::Bool:
		SetSetting<bool>(element, attributeId, value.toBool());
		break;

	case QMetaType::QString:
		SetSetting<QString>(element, attributeId, value.toString());
		break;

	default:
		SetSetting<int>(element, attributeId, value.toInt());
		break;
	}
}


/**
	The composition of the engines in a temporary directory. The settings (the paths of the databases and the values that the caller gives)
	are set before the components are got, the attributes that are not given have the defaults of the components. The connections
	of the engines are closed before the directory is removed.
*/
class CEngines
{
public:
	explicit CEngines(const QVariantMap& settings = QVariantMap())
	{
		SetSetting(m_settingsElement, "ConfiguredDatabasePath", m_dir.filePath(QStringLiteral("configured.sqlite")));
		SetSetting(m_settingsElement, "CommonDatabasePath", m_dir.filePath(QStringLiteral("common.sqlite")));

		for (auto iter = settings.constBegin(); iter != settings.constEnd(); ++iter){
			SetSetting(m_settingsElement, iter.key().toUtf8(), iter.value());
		}

		m_compositionPtr.reset(new CDatabaseEnginesTest(&m_settingsElement));
	}

	~CEngines()
	{
		m_compositionPtr.reset();

		const QStringList names = QSqlDatabase::connectionNames();
		for (const QString& name : names){
			if (name.startsWith(QStringLiteral("configured.sqlite")) || name.startsWith(QStringLiteral("common.sqlite"))){
				{
					QSqlDatabase database = QSqlDatabase::database(name, false);
					database.close();
				}

				QSqlDatabase::removeDatabase(name);
			}
		}
	}

	bool IsValid() const
	{
		return m_dir.isValid();
	}

	/**
		The engine with the configurator of the connections.
	*/
	imtdb::IDatabaseEngine* GetConfiguredEngine()
	{
		return m_compositionPtr->GetInterface<imtdb::IDatabaseEngine>("ConfiguredEngine");
	}

	/**
		The common engine without the configurator.
	*/
	imtdb::IDatabaseEngine* GetCommonEngine()
	{
		return m_compositionPtr->GetInterface<imtdb::IDatabaseEngine>("CommonEngine");
	}

	/**
		The engine whose database is in the memory: the WAL mode of the configurator cannot be set there, so the connection is not set up.
	*/
	imtdb::IDatabaseEngine* GetFailingEngine()
	{
		return m_compositionPtr->GetInterface<imtdb::IDatabaseEngine>("FailingEngine");
	}

	imtdb::IDatabaseConnectionConfigurator* GetConfigurator()
	{
		return m_compositionPtr->GetInterface<imtdb::IDatabaseConnectionConfigurator>("ConnectionConfigurator");
	}

	/**
		The path of the database file of the configured engine, the test can open a connection to it itself.
	*/
	QString GetConfiguredDatabasePath() const
	{
		return m_dir.filePath(QStringLiteral("configured.sqlite"));
	}

private:
	QTemporaryDir m_dir;
	icomp::CRegistryElement m_settingsElement;
	std::unique_ptr<CDatabaseEnginesTest> m_compositionPtr;
};


/**
	A connection that the test opens itself (the configurator is called with it directly, without the engine).
*/
class CTestConnection
{
public:
	explicit CTestConnection(const QString& databasePath)
		:m_name(QStringLiteral("test-connection-") + QUuid::createUuid().toString(QUuid::WithoutBraces))
	{
		m_database = QSqlDatabase::addDatabase(QStringLiteral("QSQLITE"), m_name);
		m_database.setDatabaseName(databasePath);
		m_isOpened = m_database.open();
	}

	~CTestConnection()
	{
		// No handle may exist when the connection is removed
		m_database.close();
		m_database = QSqlDatabase();

		QSqlDatabase::removeDatabase(m_name);
	}

	bool IsOpened() const
	{
		return m_isOpened;
	}

	QSqlDatabase& Get()
	{
		return m_database;
	}

	QString GetPragma(const QString& name)
	{
		QSqlQuery query(m_database);
		query.exec(QStringLiteral("PRAGMA ") + name);

		return query.next() ? query.value(0).toString() : QString();
	}

private:
	QString m_name;
	QSqlDatabase m_database;
	bool m_isOpened = false;
};


/**
	Value of a PRAGMA of the connection of the engine for the current thread.
*/
QString GetPragma(imtdb::IDatabaseEngine& engine, const QString& name)
{
	QSqlError error;
	QSqlQuery query = engine.ExecSqlQuery(("PRAGMA " + name).toUtf8(), &error);

	return query.next() ? query.value(0).toString() : QStringLiteral("<%1>").arg(error.text());
}


/**
	Run a function in a new thread (the engine opens a connection of its own for every thread) and wait for it.
*/
template <class Function>
void RunInThread(Function function)
{
	std::unique_ptr<QThread> threadPtr(QThread::create(function));
	threadPtr->start();
	threadPtr->wait(60000);
}


} // namespace


void CDatabaseEngineSqliteTest::CommonEngineKeepsDriverDefaultsTest()
{
	CEngines engines;
	QVERIFY(engines.IsValid());

	imtdb::IDatabaseEngine* enginePtr = engines.GetCommonEngine();
	QVERIFY(enginePtr != nullptr);

	QCOMPARE(GetPragma(*enginePtr, "journal_mode"), QStringLiteral("delete"));
	QCOMPARE(GetPragma(*enginePtr, "foreign_keys"), QStringLiteral("0"));
	QCOMPARE(GetPragma(*enginePtr, "synchronous"), QStringLiteral("2"));
	QCOMPARE(GetPragma(*enginePtr, "busy_timeout"), QStringLiteral("5000"));
}


// The attributes of the configurator are not set: it works with the defaults of its attributes
void CDatabaseEngineSqliteTest::ConfiguratorDefaultsTest()
{
	CEngines engines;
	QVERIFY(engines.IsValid());

	imtdb::IDatabaseEngine* enginePtr = engines.GetConfiguredEngine();
	QVERIFY(enginePtr != nullptr);

	QCOMPARE(GetPragma(*enginePtr, "journal_mode"), QStringLiteral("wal"));
	QCOMPARE(GetPragma(*enginePtr, "synchronous"), QStringLiteral("1"));
	QCOMPARE(GetPragma(*enginePtr, "foreign_keys"), QStringLiteral("1"));
	QCOMPARE(GetPragma(*enginePtr, "busy_timeout"), QStringLiteral("5000"));
}


void CDatabaseEngineSqliteTest::ConfiguratorSetsValuesTest_data()
{
	QTest::addColumn<int>("journalMode");
	QTest::addColumn<int>("synchronous");
	QTest::addColumn<bool>("foreignKeys");
	QTest::addColumn<int>("busyTimeout");
	QTest::addColumn<QString>("expectedJournalMode");
	QTest::addColumn<QString>("expectedForeignKeys");
	QTest::addColumn<QString>("expectedBusyTimeout");

	// The numbers of the attributes are the values of the enumerations of the configurator
	QTest::newRow("DELETE, OFF, no foreign keys, no waiting") << int(Configurator::JM_DELETE) << int(Configurator::SYNC_OFF) << false << 0 << "delete" << "0" << "0";
	QTest::newRow("TRUNCATE, NORMAL") << int(Configurator::JM_TRUNCATE) << int(Configurator::SYNC_NORMAL) << true << 1234 << "truncate" << "1" << "1234";
	QTest::newRow("PERSIST, FULL") << int(Configurator::JM_PERSIST) << int(Configurator::SYNC_FULL) << true << 5000 << "persist" << "1" << "5000";
	QTest::newRow("MEMORY, EXTRA") << int(Configurator::JM_MEMORY) << int(Configurator::SYNC_EXTRA) << true << 10 << "memory" << "1" << "10";
	QTest::newRow("WAL, FULL") << int(Configurator::JM_WAL) << int(Configurator::SYNC_FULL) << false << 60000 << "wal" << "0" << "60000";
	QTest::newRow("OFF, NORMAL") << int(Configurator::JM_OFF) << int(Configurator::SYNC_NORMAL) << true << 5000 << "off" << "1" << "5000";
}


void CDatabaseEngineSqliteTest::ConfiguratorSetsValuesTest()
{
	QFETCH(int, journalMode);
	QFETCH(int, synchronous);
	QFETCH(bool, foreignKeys);
	QFETCH(int, busyTimeout);
	QFETCH(QString, expectedJournalMode);
	QFETCH(QString, expectedForeignKeys);
	QFETCH(QString, expectedBusyTimeout);

	QVariantMap settings;
	settings[s_journalMode] = journalMode;
	settings[s_synchronous] = synchronous;
	settings[s_foreignKeys] = foreignKeys;
	settings[s_busyTimeout] = busyTimeout;

	CEngines engines(settings);
	QVERIFY(engines.IsValid());

	imtdb::IDatabaseEngine* enginePtr = engines.GetConfiguredEngine();
	QVERIFY(enginePtr != nullptr);

	QCOMPARE(GetPragma(*enginePtr, "journal_mode"), expectedJournalMode);
	QCOMPARE(GetPragma(*enginePtr, "synchronous"), QString::number(synchronous));
	QCOMPARE(GetPragma(*enginePtr, "foreign_keys"), expectedForeignKeys);
	QCOMPARE(GetPragma(*enginePtr, "busy_timeout"), expectedBusyTimeout);
}


// The setup of a connection reports its result: the configurator gives true and no reason if all the settings are applied
void CDatabaseEngineSqliteTest::ConfiguratorReportsSuccessTest()
{
	CEngines engines;
	QVERIFY(engines.IsValid());

	imtdb::IDatabaseConnectionConfigurator* configuratorPtr = engines.GetConfigurator();
	QVERIFY(configuratorPtr != nullptr);

	CTestConnection connection(engines.GetConfiguredDatabasePath());
	QVERIFY(connection.IsOpened());

	QString errorMessage = QStringLiteral("not empty");
	QVERIFY(configuratorPtr->ConfigureConnection(connection.Get(), errorMessage));
	QVERIFY2(errorMessage.isEmpty(), qPrintable(errorMessage));

	QCOMPARE(connection.GetPragma("journal_mode"), QStringLiteral("wal"));
	QCOMPARE(connection.GetPragma("foreign_keys"), QStringLiteral("1"));
}


// A value that is not allowed is not ignored: the configurator reports every problem, the engine does not use the connection
void CDatabaseEngineSqliteTest::NotAllowedValuesFailTheSetupTest()
{
	QVariantMap settings;
	settings[s_journalMode] = 99;
	settings[s_synchronous] = 99;
	settings[s_busyTimeout] = -1;
	settings[s_foreignKeys] = true;

	CEngines engines(settings);
	QVERIFY(engines.IsValid());

	imtdb::IDatabaseConnectionConfigurator* configuratorPtr = engines.GetConfigurator();
	QVERIFY(configuratorPtr != nullptr);

	{
		CTestConnection connection(engines.GetConfiguredDatabasePath());
		QVERIFY(connection.IsOpened());

		QString errorMessage;
		QVERIFY(!configuratorPtr->ConfigureConnection(connection.Get(), errorMessage));
		QVERIFY2(errorMessage.contains(QStringLiteral("JournalMode")), qPrintable(errorMessage));
		QVERIFY2(errorMessage.contains(QStringLiteral("Synchronous")), qPrintable(errorMessage));
		QVERIFY2(errorMessage.contains(QStringLiteral("BusyTimeoutMs")), qPrintable(errorMessage));

		// Nothing is applied to the connection
		QCOMPARE(connection.GetPragma("journal_mode"), QStringLiteral("delete"));
		QCOMPARE(connection.GetPragma("foreign_keys"), QStringLiteral("0"));
	}

	// The engine does not work: the caller gets the error, not an empty result
	imtdb::IDatabaseEngine* enginePtr = engines.GetConfiguredEngine();
	QVERIFY(enginePtr != nullptr);

	QSqlError error;
	enginePtr->ExecSqlQuery("SELECT 1", &error);
	QVERIFY(error.isValid());
	QCOMPARE(error.type(), QSqlError::ConnectionError);
}


// WAL is not possible in a memory database: SQLite does not report an error, it leaves the journal mode. The setup must notice it
void CDatabaseEngineSqliteTest::SettingThatIsNotAppliedFailsTheSetupTest()
{
	CEngines engines;
	QVERIFY(engines.IsValid());

	imtdb::IDatabaseConnectionConfigurator* configuratorPtr = engines.GetConfigurator();
	QVERIFY(configuratorPtr != nullptr);

	CTestConnection connection(QStringLiteral(":memory:"));
	QVERIFY(connection.IsOpened());

	QString errorMessage;
	QVERIFY(!configuratorPtr->ConfigureConnection(connection.Get(), errorMessage));
	QVERIFY2(errorMessage.contains(QStringLiteral("journal_mode is 'memory' instead of 'wal'")), qPrintable(errorMessage));
	QVERIFY2(errorMessage.contains(QStringLiteral(":memory:")), qPrintable(errorMessage));

	// The settings that can be applied are applied, the problem does not hide the others
	QCOMPARE(connection.GetPragma("foreign_keys"), QStringLiteral("1"));
}


// The engine that cannot set up its connection does not use it: every call reports the error, nothing works slowly or half-configured
void CDatabaseEngineSqliteTest::EngineDoesNotUseConnectionThatIsNotSetUpTest()
{
	CEngines engines;
	QVERIFY(engines.IsValid());

	imtdb::IDatabaseEngine* enginePtr = engines.GetFailingEngine();
	QVERIFY(enginePtr != nullptr);

	for (int i = 0; i < 2; ++i){
		QSqlError error;
		QSqlQuery query = enginePtr->ExecSqlQuery("SELECT 1", &error);

		QVERIFY(error.isValid());
		QCOMPARE(error.type(), QSqlError::ConnectionError);
		QVERIFY(!query.isActive());
	}

	QVERIFY(!enginePtr->BeginTransaction());

	// The same on the connection of another thread
	bool isError = false;
	RunInThread([&](){
		QSqlError threadError;
		enginePtr->ExecSqlQuery("SELECT 1", &threadError);

		isError = threadError.isValid();
	});
	QVERIFY(isError);
}


// The check of the foreign keys and the synchronization are settings of a connection (not of the file), the engine has a connection for
// every thread
void CDatabaseEngineSqliteTest::SettingsAreAppliedOnEveryThreadTest()
{
	CEngines engines;
	QVERIFY(engines.IsValid());

	imtdb::IDatabaseEngine* enginePtr = engines.GetConfiguredEngine();
	QVERIFY(enginePtr != nullptr);

	QString foreignKeys;
	QString synchronous;
	QString journalMode;
	QString busyTimeout;

	RunInThread([&](){
		foreignKeys = GetPragma(*enginePtr, "foreign_keys");
		synchronous = GetPragma(*enginePtr, "synchronous");
		journalMode = GetPragma(*enginePtr, "journal_mode");
		busyTimeout = GetPragma(*enginePtr, "busy_timeout");
	});

	QCOMPARE(foreignKeys, QStringLiteral("1"));
	QCOMPARE(synchronous, QStringLiteral("1"));
	QCOMPARE(journalMode, QStringLiteral("wal"));
	QCOMPARE(busyTimeout, QStringLiteral("5000"));
}


void CDatabaseEngineSqliteTest::ForeignKeysAreCheckedTest_data()
{
	QTest::addColumn<bool>("isConfiguredEngine");
	QTest::addColumn<bool>("foreignKeys");
	QTest::addColumn<bool>("isOrphanRefused");

	QTest::newRow("common engine: the reference to a missing record is accepted (driver default)") << false << false << false;
	QTest::newRow("configurator, ForeignKeys=true: the reference to a missing record is refused") << true << true << true;
	QTest::newRow("configurator, ForeignKeys=false: the reference to a missing record is accepted") << true << false << false;
}


void CDatabaseEngineSqliteTest::ForeignKeysAreCheckedTest()
{
	QFETCH(bool, isConfiguredEngine);
	QFETCH(bool, foreignKeys);
	QFETCH(bool, isOrphanRefused);

	QVariantMap settings;
	settings[s_foreignKeys] = foreignKeys;

	CEngines engines(settings);
	QVERIFY(engines.IsValid());

	imtdb::IDatabaseEngine* enginePtr = isConfiguredEngine ? engines.GetConfiguredEngine() : engines.GetCommonEngine();
	QVERIFY(enginePtr != nullptr);

	QSqlError error;
	enginePtr->ExecSqlQuery("CREATE TABLE Parent (Id INTEGER PRIMARY KEY)", &error);
	QVERIFY2(!error.isValid(), qPrintable(error.text()));
	enginePtr->ExecSqlQuery("CREATE TABLE Child (Id INTEGER PRIMARY KEY, ParentId INTEGER REFERENCES Parent (Id))", &error);
	QVERIFY2(!error.isValid(), qPrintable(error.text()));

	// The parent exists
	enginePtr->ExecSqlQuery("INSERT INTO Parent (Id) VALUES (1)", &error);
	QVERIFY2(!error.isValid(), qPrintable(error.text()));
	enginePtr->ExecSqlQuery("INSERT INTO Child (ParentId) VALUES (1)", &error);
	QVERIFY2(!error.isValid(), qPrintable(error.text()));

	// The parent does not exist
	enginePtr->ExecSqlQuery("INSERT INTO Child (ParentId) VALUES (999)", &error);
	QCOMPARE(error.isValid(), isOrphanRefused);
	if (isOrphanRefused){
		QVERIFY2(error.text().contains(QStringLiteral("FOREIGN KEY")), qPrintable(error.text()));
	}
}


void CDatabaseEngineSqliteTest::WriteIsNotBlockedByUnfinishedReadTest_data()
{
	QTest::addColumn<bool>("isConfiguredEngine");
	QTest::addColumn<bool>("isWriteDone");

	// About 5 s (the timeout of the driver) and then "database is locked"
	QTest::newRow("common engine: the writer waits for the reader and gives up") << false << false;
	QTest::newRow("configurator: the writer does not wait") << true << true;
}


// A reader with an unfinished query (a long list, a reconciliation) and a writer of another thread: that is how the server works
void CDatabaseEngineSqliteTest::WriteIsNotBlockedByUnfinishedReadTest()
{
	QFETCH(bool, isConfiguredEngine);
	QFETCH(bool, isWriteDone);

	CEngines engines;
	QVERIFY(engines.IsValid());

	imtdb::IDatabaseEngine* enginePtr = isConfiguredEngine ? engines.GetConfiguredEngine() : engines.GetCommonEngine();
	QVERIFY(enginePtr != nullptr);

	QSqlError error;
	enginePtr->ExecSqlQuery("CREATE TABLE Items (Id INTEGER PRIMARY KEY, Value TEXT)", &error);
	QVERIFY2(!error.isValid(), qPrintable(error.text()));
	for (int i = 0; i < 100; ++i){
		enginePtr->ExecSqlQuery("INSERT INTO Items (Value) VALUES ('x')", &error);
		QVERIFY2(!error.isValid(), qPrintable(error.text()));
	}

	// The reader of this thread: the query is not finished, so the transaction holds its lock
	QVERIFY(enginePtr->BeginTransaction());
	QSqlQuery reader = enginePtr->ExecSqlQuery("SELECT * FROM Items", &error);
	QVERIFY2(!error.isValid(), qPrintable(error.text()));
	QVERIFY(reader.next());

	bool isWritten = false;
	qint64 elapsedMs = 0;
	QString writeError;

	RunInThread([&](){
		QElapsedTimer timer;
		timer.start();

		QSqlError threadError;
		enginePtr->ExecSqlQuery("INSERT INTO Items (Value) VALUES ('from the writer')", &threadError);

		isWritten = !threadError.isValid();
		writeError = threadError.text();
		elapsedMs = timer.elapsed();
	});

	reader.finish();
	enginePtr->FinishTransaction();

	QCOMPARE(isWritten, isWriteDone);
	if (isWriteDone){
		QVERIFY2(elapsedMs < 2000, qPrintable(QStringLiteral("The writer has waited %1 ms").arg(elapsedMs)));
	}
	else{
		QVERIFY2(writeError.contains(QStringLiteral("locked")), qPrintable(writeError));
	}
}


I_ADD_TEST(CDatabaseEngineSqliteTest);
