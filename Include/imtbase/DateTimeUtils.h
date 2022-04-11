#pragma once


// Qt includes
#include <QtCore/QDateTime>


namespace imtbase
{


inline int GetQuorter(const QDateTime& dt)
{
	return (dt.date().month() - 1) / 3 + 1;
}


inline QDateTime Get10MSecondBegin(const QDateTime& dt)
{
	int hour = dt.time().hour();
	int minute = dt.time().minute();
	int second = dt.time().second();
	int msecond = (dt.time().msec() / 10) * 10;

	return QDateTime(dt.date(), QTime(hour, minute, second, msecond));
}


inline QDateTime Get10MSecondEnd(const QDateTime& dt)
{
	QDateTime retVal = Get10MSecondBegin(dt);

	return retVal.addMSecs(9);
}


inline QDateTime Get100MSecondBegin(const QDateTime& dt)
{
	int hour = dt.time().hour();
	int minute = dt.time().minute();
	int second = dt.time().second();
	int msecond = (dt.time().msec() / 100) * 100;

	return QDateTime(dt.date(), QTime(hour, minute, second, msecond));
}


inline QDateTime Get100MSecondEnd(const QDateTime& dt)
{
	QDateTime retVal = Get100MSecondBegin(dt);

	return retVal.addMSecs(99);
}


inline QDateTime GetSecondBegin(const QDateTime& dt)
{
	int hour = dt.time().hour();
	int minute = dt.time().minute();
	int second = dt.time().second();

	return QDateTime(dt.date(), QTime(hour, minute, second, 0));
}


inline QDateTime GetSecondEnd(const QDateTime& dt)
{
	QDateTime retVal = GetSecondBegin(dt);

	return retVal.addMSecs(999);
}


inline QDateTime Get5SecondBegin(const QDateTime& dt)
{
	int hour = dt.time().hour();
	int minute = dt.time().minute();
	int second = dt.time().second() / 5 * 5;

	return QDateTime(dt.date(), QTime(hour, minute, second, 0));
}


inline QDateTime Get5SecondEnd(const QDateTime& dt)
{
	QDateTime retVal = Get5SecondBegin(dt);

	return retVal.addSecs(5).addMSecs(-1);
}


inline QDateTime Get10SecondBegin(const QDateTime& dt)
{
	int hour = dt.time().hour();
	int minute = dt.time().minute();
	int second = dt.time().second() / 5 * 5;

	return QDateTime(dt.date(), QTime(hour, minute, second, 0));
}


inline QDateTime Get10SecondEnd(const QDateTime& dt)
{
	QDateTime retVal = Get10SecondBegin(dt);

	return retVal.addSecs(10).addMSecs(-1);
}


inline QDateTime Get15SecondBegin(const QDateTime& dt)
{
	int hour = dt.time().hour();
	int minute = dt.time().minute();
	int second = dt.time().second() / 15 * 15;

	return QDateTime(dt.date(), QTime(hour, minute, second, 0));
}


inline QDateTime Get15SecondEnd(const QDateTime& dt)
{
	QDateTime retVal = Get15SecondBegin(dt);

	return retVal.addSecs(15).addMSecs(-1);
}


inline QDateTime Get30SecondBegin(const QDateTime& dt)
{
	int hour = dt.time().hour();
	int minute = dt.time().minute();
	int second = dt.time().second() / 30 * 30;

	return QDateTime(dt.date(), QTime(hour, minute, second, 0));
}


inline QDateTime Get30SecondEnd(const QDateTime& dt)
{
	QDateTime retVal = Get30SecondBegin(dt);

	return retVal.addSecs(30).addMSecs(-1);
}


inline QDateTime GetMinutesBegin(const QDateTime& dt)
{
	int hour = dt.time().hour();
	int minute = dt.time().minute();

	return QDateTime(dt.date(), QTime(hour, minute));
}


inline QDateTime GetMinutesEnd(const QDateTime& dt)
{
	QDateTime retVal = GetMinutesBegin(dt);

	return retVal.addSecs(60).addMSecs(-1);
}


inline QDateTime Get5MinutesBegin(const QDateTime& dt)
{
	int hour = dt.time().hour();
	int minute = dt.time().minute() / 5 * 5;

	return QDateTime(dt.date(), QTime(hour, minute));
}


inline QDateTime Get5MinutesEnd(const QDateTime& dt)
{
	QDateTime retVal = Get5MinutesBegin(dt);

	return retVal.addSecs(300).addMSecs(-1);
}


inline QDateTime Get10MinutesBegin(const QDateTime& dt)
{
	int hour = dt.time().hour();
	int minute = dt.time().minute() / 10 * 10;

	return QDateTime(dt.date(), QTime(hour, minute));
}


inline QDateTime Get10MinutesEnd(const QDateTime& dt)
{
	QDateTime retVal = Get10MinutesBegin(dt);

	return retVal.addSecs(600).addMSecs(-1);
}


inline QDateTime Get15MinutesBegin(const QDateTime& dt)
{
	int hour = dt.time().hour();
	int minute = dt.time().minute() / 15 * 15;

	return QDateTime(dt.date(), QTime(hour, minute));
}


inline QDateTime Get15MinutesEnd(const QDateTime& dt)
{
	QDateTime retVal = Get15MinutesBegin(dt);

	return retVal.addSecs(900).addMSecs(-1);
}


inline QDateTime Get30MinutesBegin(const QDateTime& dt)
{
	int hour = dt.time().hour();
	int minute = dt.time().minute() / 30 * 30;

	return QDateTime(dt.date(), QTime(hour, minute));
}


inline QDateTime Get30MinutesEnd(const QDateTime& dt)
{
	QDateTime retVal = Get30MinutesBegin(dt);

	return retVal.addSecs(1800).addMSecs(-1);
}


inline QDateTime GetHourBegin(const QDateTime& dt)
{
	int hour = dt.time().hour();

	return QDateTime(dt.date(), QTime(hour, 0));
}


inline QDateTime GetHourEnd(const QDateTime& dt)
{
	int hour = dt.time().hour();

	return QDateTime(dt.date(), QTime(hour, 59, 59, 999));
}


inline QDateTime Get3HourBegin(const QDateTime& dt)
{
	int hour = dt.time().hour() / 3 * 3;

	return QDateTime(dt.date(), QTime(hour, 0));
}


inline QDateTime Get3HourEnd(const QDateTime& dt)
{
	QDateTime retVal = Get3HourBegin(dt);

	return retVal.addSecs(10800).addMSecs(-1);
}


inline QDateTime Get6HourBegin(const QDateTime& dt)
{
	int hour = dt.time().hour() / 6 * 6*2;

	return QDateTime(dt.date(), QTime(hour, 0));
}


inline QDateTime Get6HourEnd(const QDateTime& dt)
{
	QDateTime retVal = Get6HourBegin(dt);

	return retVal.addSecs(21600).addMSecs(-1);
}


inline QDateTime Get12HourBegin(const QDateTime& dt)
{
	int hour = dt.time().hour() / 6 * 6*2;

	return QDateTime(dt.date(), QTime(hour, 0));
}


inline QDateTime Get12HourEnd(const QDateTime& dt)
{
	QDateTime retVal = Get12HourBegin(dt);

	return retVal.addSecs(43200).addMSecs(-1);
}


inline QDateTime GetDayBegin(const QDateTime& dt)
{
	return QDateTime(dt.date(), QTime(0, 0));
}


inline QDateTime GetDayEnd(const QDateTime& dt)
{
	return QDateTime(dt.date(), QTime(23, 59, 59, 999));
}


inline QDateTime GetWeekBegin(const QDateTime& dt)
{
	QDateTime retVal;
	retVal.setDate(dt.date().addDays(-dt.date().dayOfWeek() + 1));

	return retVal;
}


inline QDateTime GetWeekEnd(const QDateTime& dt)
{
	QDateTime retVal = GetWeekBegin(dt);

	return retVal.addDays(7).addMSecs(-1);
}


inline QDateTime GetMonthBegin(const QDateTime& dt)
{
	QDateTime retVal;
	retVal.setDate(QDate(dt.date().year(), dt.date().month(), 1));

	return retVal;
}


inline QDateTime GetMonthEnd(const QDateTime& dt)
{
	QDateTime retVal = GetMonthBegin(dt);

	return retVal.addMonths(1).addMSecs(-1);
}


inline QDateTime GetQuorterBegin(const QDateTime& dt)
{
	int month = (GetQuorter(dt) - 1) * 3 + 1;

	QDateTime retVal;
	retVal.setDate(QDate(dt.date().year(), month, 1));

	return retVal;
}


inline QDateTime GetQuorterEnd(const QDateTime& dt)
{
	QDateTime retVal = GetQuorterBegin(dt);

	return retVal.addMonths(3).addMSecs(-1);
}


inline QDateTime GetYearBegin(const QDateTime& dt)
{
	return QDateTime(QDate(dt.date().year(), 1, 1), QTime());
}


inline QDateTime GetYearEnd(const QDateTime& dt)
{
	QDateTime retVal = GetYearBegin(dt);

	return retVal.addYears(1).addMSecs(-1);
}


} // namespace imtbase
