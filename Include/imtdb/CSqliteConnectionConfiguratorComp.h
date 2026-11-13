// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QString>
#include <QtCore/QStringList>

// ACF includes
#include <ilog/TLoggerCompWrap.h>

// ImtCore includes
#include <imtdb/IDatabaseConnectionConfigurator.h>


namespace imtdb
{


/**
	Sets up the connections to a SQLite database (see CDatabaseEngineComp, the reference \c ConnectionConfigurator) so that many threads
	can work with one database file. The engine has a connection for every thread, and every connection is set up when it is opened.

	By default (the defaults of the attributes) the connection gets:

	- \c journal_mode=WAL: the readers and the writer do not block each other (with the default journal a writer waits for the readers,
	  an unfinished query of one thread makes the writes of the others fail with "database is locked");
	- \c synchronous=NORMAL: with WAL it needs less \c fsync, a power failure can lose the last commits but cannot damage the database;
	- \c foreign_keys=ON: the references (\c REFERENCES) between the tables are checked, SQLite ignores them by default;
	- \c busy_timeout=5000: a connection waits for a lock of another connection instead of failing at once.

	Every setting is an attribute. The defaults of the SQLite driver of Qt are \c DELETE, \c FULL, no check of the references and 5000 ms.

	The result of the setup is checked: SQLite does not report an error for a setting that it cannot apply (for example, WAL on a network
	file system or in a memory database leaves the previous mode), so every setting is read back. The setup fails (the function gives
	\c false and the reason), if a setting is not applied, a value of an attribute is not allowed (it is reported to the log once, when the
	component is created) or the connection is not a SQLite one (the attribute \c DbType of the engine). The engine does not use such a
	connection.

	The journal mode is stored in the database file (WAL stays until it is switched back) and WAL needs a local file system. The other
	settings are settings of the connection.
*/
class CSqliteConnectionConfiguratorComp:
			public ilog::CLoggerComponentBase,
			virtual public IDatabaseConnectionConfigurator
{
public:
	typedef ilog::CLoggerComponentBase BaseClass;

	/**
		Journal mode of the database (\c PRAGMA \c journal_mode). The values are the numbers of the attribute \c JournalMode.
	*/
	enum JournalMode
	{
		JM_DELETE = 0,
		JM_TRUNCATE = 1,
		JM_PERSIST = 2,
		JM_MEMORY = 3,
		JM_WAL = 4,
		JM_OFF = 5
	};
	I_DECLARE_ENUM(JournalMode, JM_DELETE, JM_TRUNCATE, JM_PERSIST, JM_MEMORY, JM_WAL, JM_OFF)

	/**
		Synchronization with the disk (\c PRAGMA \c synchronous). The values are the numbers of the attribute \c Synchronous, they are
		the numbers of SQLite.
	*/
	enum Synchronous
	{
		SYNC_OFF = 0,
		SYNC_NORMAL = 1,
		SYNC_FULL = 2,
		SYNC_EXTRA = 3
	};
	I_DECLARE_ENUM(Synchronous, SYNC_OFF, SYNC_NORMAL, SYNC_FULL, SYNC_EXTRA)

	I_BEGIN_COMPONENT(CSqliteConnectionConfiguratorComp);
		I_REGISTER_INTERFACE(IDatabaseConnectionConfigurator);
		I_ASSIGN(m_journalModeAttrPtr, "JournalMode", "Journal mode of the database file.\nPossible values:\n0 - DELETE (the default of the driver);\n1 - TRUNCATE;\n2 - PERSIST;\n3 - MEMORY;\n4 - WAL (the readers and the writer do not block each other, needs a local file system);\n5 - OFF", true, JM_WAL);
		I_ASSIGN(m_synchronousAttrPtr, "Synchronous", "Synchronization of the writes with the disk.\nPossible values:\n0 - OFF;\n1 - NORMAL (with WAL: a power failure can lose the last commits, but cannot damage the database);\n2 - FULL (the default of the driver);\n3 - EXTRA", true, SYNC_NORMAL);
		I_ASSIGN(m_foreignKeysAttrPtr, "ForeignKeys", "If enabled, the references between the tables (REFERENCES) are checked, SQLite does not check them by default", true, true);
		I_ASSIGN(m_busyTimeoutAttrPtr, "BusyTimeoutMs", "Time in milliseconds that a connection waits for a lock of another connection before it fails with 'database is locked'. 0 - does not wait", true, 5000);
	I_END_COMPONENT;

	// reimplemented (imtdb::IDatabaseConnectionConfigurator)
	virtual bool ConfigureConnection(QSqlDatabase& databaseConnection, QString& errorMessage) const override;

protected:
	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;

private:
	I_ATTR(int, m_journalModeAttrPtr);
	I_ATTR(int, m_synchronousAttrPtr);
	I_ATTR(bool, m_foreignKeysAttrPtr);
	I_ATTR(int, m_busyTimeoutAttrPtr);

	// The problems of the values of the attributes, they are checked once, when the component is created. Empty if the values are allowed
	QString m_attributesError;
};


} // namespace imtdb
