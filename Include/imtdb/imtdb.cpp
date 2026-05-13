// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtdb/imtdb.h>

// Qt includes
#include <QtCore/QDateTime>


namespace imtdb
{


QString SqlEncode(const QString& sqlQuery)
{
	QString retVal = sqlQuery;
	return retVal.replace('\'', "''").replace(';', '\b');
}


QString GetSqlResourcePath(const IDatabaseEngine& databaseEngine, const QString& fileName)
{
	const QByteArray driverName = databaseEngine.GetDatabaseDriverId();
	const bool isSqlite = driverName.compare(QByteArrayLiteral("QSQLITE"), Qt::CaseInsensitive) == 0;
	const QString prefix = isSqlite ? QStringLiteral(":/SQL/SQLite/") : QStringLiteral(":/SQL/Postgres/");
	return prefix + fileName;
}


QString UtcNow()
{
	return QDateTime::currentDateTimeUtc().toString(Qt::ISODateWithMs);
}


QString EscapeSql(const QString& value)
{
	QString escaped = value;
	escaped.replace('\'', "''");
	return escaped;
}


} // namespace imtdb