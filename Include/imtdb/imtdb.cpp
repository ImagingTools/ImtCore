// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtdb/imtdb.h>

// Qt includes
#include <QtCore/QDateTime>
#include <QtCore/QTimeZone>
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


QString QuoteIdentifier(const QString& identifier)
{
	QString escaped = identifier;

	return QStringLiteral(R"("%1")").arg(escaped.replace('"', QStringLiteral(R"("")")));
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


static QDateTime AddTimeUnits(const QDateTime& time, imtbase::ITimeFilterParam::TimeUnit unit, int count)
{
	switch (unit){
	case imtbase::ITimeFilterParam::TU_HOUR:
		return time.addSecs(qint64(count) * 3600);
	case imtbase::ITimeFilterParam::TU_DAY:
		return time.addDays(count);
	case imtbase::ITimeFilterParam::TU_WEEK:
		return time.addDays(qint64(count) * 7);
	case imtbase::ITimeFilterParam::TU_MONTH:
		return time.addMonths(count);
	case imtbase::ITimeFilterParam::TU_YEAR:
		return time.addYears(count);
	default:
		return time;
	}
}


static QDateTime GetTimeUnitStart(const QDateTime& time, imtbase::ITimeFilterParam::TimeUnit unit)
{
	const QDate date = time.date();

	switch (unit){
	case imtbase::ITimeFilterParam::TU_HOUR:
		return QDateTime(date, QTime(time.time().hour(), 0), QTimeZone::UTC);
	case imtbase::ITimeFilterParam::TU_DAY:
		return QDateTime(date, QTime(0, 0), QTimeZone::UTC);
	case imtbase::ITimeFilterParam::TU_WEEK:
		return QDateTime(date.addDays(1 - date.dayOfWeek()), QTime(0, 0), QTimeZone::UTC);
	case imtbase::ITimeFilterParam::TU_MONTH:
		return QDateTime(QDate(date.year(), date.month(), 1), QTime(0, 0), QTimeZone::UTC);
	case imtbase::ITimeFilterParam::TU_YEAR:
		return QDateTime(QDate(date.year(), 1, 1), QTime(0, 0), QTimeZone::UTC);
	default:
		return time;
	}
}


void GetTimeFilterBounds(const imtbase::ITimeFilterParam& timeFilter, const QDateTime& now, QDateTime& begin, QDateTime& end)
{
	begin = QDateTime();
	end = QDateTime();

	const imtbase::ITimeFilterParam::TimeUnit unit = timeFilter.GetTimeUnit();
	if (unit == imtbase::ITimeFilterParam::TU_CUSTOM){
		const imtbase::CTimeRange timeRange = timeFilter.GetTimeRange();
		if (timeRange.GetBeginTime().isValid()){
			begin = QDateTime(timeRange.GetBeginTime().date(), QTime(0, 0), QTimeZone::UTC);
		}

		if (timeRange.GetEndTime().isValid()){
			end = QDateTime(timeRange.GetEndTime().date().addDays(1), QTime(0, 0), QTimeZone::UTC);
		}

		return;
	}

	switch (timeFilter.GetInterpretationMode()){
	case imtbase::ITimeFilterParam::IM_FOR:
		begin = AddTimeUnits(now, unit, -qMax(1, timeFilter.GetUnitMultiplier()));
		break;
	case imtbase::ITimeFilterParam::IM_CURRENT:
		begin = GetTimeUnitStart(now, unit);
		break;
	case imtbase::ITimeFilterParam::IM_LAST:
		end = GetTimeUnitStart(now, unit);
		begin = AddTimeUnits(end, unit, -1);
		break;
	}
}


} // namespace imtdb