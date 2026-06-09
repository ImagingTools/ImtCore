// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtdb/imtdb.h>

// Qt includes
#include <QtCore/QDateTime>
#include <QtCore/QUuid>
#include <QtCore/QVariant>


namespace imtdb
{


QString SqlEncode(const QString& sqlQuery)
{
	QString retVal = sqlQuery;

	return retVal.replace('\'', QStringLiteral("''")).replace(';', '\b');
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
	escaped.replace('\'', QStringLiteral("''"));

	return escaped;
}


QByteArray VariantToByteArray(const QVariant& value)
{
	if (value.typeId() == QMetaType::QUuid){
		return value.value<QUuid>().toByteArray(QUuid::WithoutBraces);
	}

	return value.toByteArray();
}


QString SqlValue(const QString& value, bool useDefaultIfEmpty)
{
	if (value.isEmpty()){
		return useDefaultIfEmpty ? DEFAULT_DATA_LITERAL : NULL_DATA_LITERAL;
	}
	return SqlEncode(value).prepend('\'').append('\'');
}


QString SqlValue(int value, bool useDefaultIfEmpty)
{
	if (value == 0){
		return useDefaultIfEmpty ? DEFAULT_DATA_LITERAL : NULL_DATA_LITERAL;
	}
	return QString::number(value);
}


QString SqlValue(double value, bool useDefaultIfEmpty)
{
	if (value == 0.0){
		return useDefaultIfEmpty ? DEFAULT_DATA_LITERAL : NULL_DATA_LITERAL;
	}
	return QString::number(value);
}


} // namespace imtdb