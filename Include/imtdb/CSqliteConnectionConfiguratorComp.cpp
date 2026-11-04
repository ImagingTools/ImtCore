// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtdb/CSqliteConnectionConfiguratorComp.h>


// Qt includes
#include <QtSql/QSqlError>
#include <QtSql/QSqlQuery>


namespace
{


/**
	\brief Set a PRAGMA and check the result by reading it back.
	\details The form with \c expected is needed when SQLite gives the setting back in another form than it was set. For example, if the
		synchronization is set by the name (\c PRAGMA \c synchronous=NORMAL), it is read back as the number (\c 1), so \c value is \c NORMAL
		and \c expected is \c 1. If the setting is read back as it was set, use the shorter form below.
	\param	value		Value that is set.
	\param	expected	Value that the connection must have after that (it is compared without the case).
	\param	errors		The description of the problem is added here if the setting is not applied.
*/
bool SetPragma(QSqlDatabase& databaseConnection, const QString& name, const QString& value, const QString& expected, QStringList& errors)
{
	QSqlQuery setQuery(databaseConnection);
	if (!setQuery.exec(QStringLiteral("PRAGMA %1=%2").arg(name, value))){
		errors << QStringLiteral("'PRAGMA %1=%2' failed: %3").arg(name, value, setQuery.lastError().text());

		return false;
	}

	// SQLite does not report an error for a setting that it cannot apply, so the result is read back
	QSqlQuery getQuery(databaseConnection);
	if (!getQuery.exec(QStringLiteral("PRAGMA %1").arg(name)) || !getQuery.next()){
		errors << QStringLiteral("'PRAGMA %1' cannot be read: %2").arg(name, getQuery.lastError().text());

		return false;
	}

	const QString actual = getQuery.value(0).toString();
	if (actual.compare(expected, Qt::CaseInsensitive) != 0){
		// WAL is the setting that SQLite most often leaves unchanged without an error, so the message gives the usual reasons.
		// Only the journal mode can be expected to be "wal", the other settings are numbers.
		const bool isWal = (expected.compare(QStringLiteral("wal"), Qt::CaseInsensitive) == 0);
		const QString walHint = isWal ? QStringLiteral(" (WAL cannot be enabled on a network file system, e.g. SMB, or for an in-memory database)") : QString();

		errors << QStringLiteral("%1 is '%2' instead of '%3'%4").arg(name, actual, expected, walHint);

		return false;
	}

	return true;
}


/**
	Set a PRAGMA and check the result by reading it back, if the setting is read back as it was set: the number (the settings of the
	connection) or the name of the journal mode (it is compared without the case). The connection must have \c value after that.
	\sa	The function above, if the setting is read back in another form.
*/
bool SetPragma(QSqlDatabase& databaseConnection, const QString& name, const QString& value, QStringList& errors)
{
	return SetPragma(databaseConnection, name, value, value, errors);
}


} // namespace


namespace imtdb
{


// public methods

// reimplemented (imtdb::IDatabaseConnectionConfigurator)

bool CSqliteConnectionConfiguratorComp::ConfigureConnection(QSqlDatabase& databaseConnection, QString& errorMessage) const
{
	errorMessage.clear();

	if (databaseConnection.driverName().compare(QStringLiteral("QSQLITE"), Qt::CaseInsensitive) != 0){
		errorMessage = QStringLiteral("The SQLite configurator is used with the driver '%1'").arg(databaseConnection.driverName());

		return false;
	}

	if (!m_attributesError.isEmpty()){
		errorMessage = m_attributesError;

		return false;
	}

	QStringList errors;

	// The timeout goes first: the switch of the journal mode needs a lock of the database
	const QString busyTimeout = QString::number(*m_busyTimeoutAttrPtr);
	SetPragma(databaseConnection, QStringLiteral("busy_timeout"), busyTimeout, errors);

	// SQLite takes the name of the journal mode only
	const QString journalMode = QString::fromLatin1(ToString(JournalMode(*m_journalModeAttrPtr)));
	SetPragma(databaseConnection, QStringLiteral("journal_mode"), journalMode, errors);

	// The values of the enumeration are the numbers of SQLite
	const QString synchronous = QString::number(*m_synchronousAttrPtr);
	SetPragma(databaseConnection, QStringLiteral("synchronous"), synchronous, errors);

	// The check of the foreign keys is a setting of the connection (not of the file), it is ignored inside a transaction
	const QString foreignKeys = QString::number(*m_foreignKeysAttrPtr ? 1 : 0);
	SetPragma(databaseConnection, QStringLiteral("foreign_keys"), foreignKeys, errors);

	if (!errors.isEmpty()){
		errorMessage = QStringLiteral("SQLite database '%1': %2").arg(databaseConnection.databaseName(), errors.join(QStringLiteral("; ")));

		return false;
	}

	return true;
}


// protected methods

// reimplemented (icomp::CComponentBase)

void CSqliteConnectionConfiguratorComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	QStringList errors;

	if (!JournalModeGetValues().contains(*m_journalModeAttrPtr)){
		errors << QStringLiteral("the attribute JournalMode has the value %1 that is not allowed").arg(*m_journalModeAttrPtr);
	}

	if (!SynchronousGetValues().contains(*m_synchronousAttrPtr)){
		errors << QStringLiteral("the attribute Synchronous has the value %1 that is not allowed").arg(*m_synchronousAttrPtr);
	}

	if (*m_busyTimeoutAttrPtr < 0){
		errors << QStringLiteral("the attribute BusyTimeoutMs has the negative value %1").arg(*m_busyTimeoutAttrPtr);
	}

	m_attributesError = errors.join(QStringLiteral("; "));
	if (!m_attributesError.isEmpty()){
		SendErrorMessage(0, QStringLiteral("The settings of the SQLite connections are not allowed, the connections will not be used: %1").arg(m_attributesError), __FILE__);
	}
}


} // namespace imtdb


