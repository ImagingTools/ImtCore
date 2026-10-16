// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include "CHttpDateToolsTest.h"


// Qt includes
#include <QtCore/QLocale>
#include <QtCore/QTimeZone>

// ImtCore includes
#include <imtrest/CHttpDateTools.h>


// private slots

void CHttpDateToolsTest::ToImfFixdateTest_data()
{
	QTest::addColumn<qint64>("secsSinceEpoch");
	QTest::addColumn<QByteArray>("date");

	// The example of RFC 9110, section 5.6.7
	QTest::newRow("RFC example") << qint64(784111777) << QByteArray("Sun, 06 Nov 1994 08:49:37 GMT");
	QTest::newRow("epoch") << qint64(0) << QByteArray("Thu, 01 Jan 1970 00:00:00 GMT");
	QTest::newRow("leap day") << qint64(951782400) << QByteArray("Tue, 29 Feb 2000 00:00:00 GMT");
	QTest::newRow("last second of the century") << qint64(946684799) << QByteArray("Fri, 31 Dec 1999 23:59:59 GMT");
	QTest::newRow("after 2038") << qint64(2147483648) << QByteArray("Tue, 19 Jan 2038 03:14:08 GMT");
}


void CHttpDateToolsTest::ToImfFixdateTest()
{
	QFETCH(qint64, secsSinceEpoch);
	QFETCH(QByteArray, date);

	QCOMPARE(imtrest::CHttpDateTools::ToImfFixdate(QDateTime::fromSecsSinceEpoch(secsSinceEpoch)), date);
}


void CHttpDateToolsTest::TimeZoneIsIgnoredTest()
{
	// The same moment in different zones gives the same date in GMT:
	const QDateTime utcTime = QDateTime::fromSecsSinceEpoch(784111777, QTimeZone::UTC);
	const QDateTime aheadTime = QDateTime::fromSecsSinceEpoch(784111777, QTimeZone::fromSecondsAheadOfUtc(5 * 3600));
	const QDateTime behindTime = QDateTime::fromSecsSinceEpoch(784111777, QTimeZone::fromSecondsAheadOfUtc(-8 * 3600));

	const QByteArray expected("Sun, 06 Nov 1994 08:49:37 GMT");

	QCOMPARE(imtrest::CHttpDateTools::ToImfFixdate(utcTime), expected);
	QCOMPARE(imtrest::CHttpDateTools::ToImfFixdate(aheadTime), expected);
	QCOMPARE(imtrest::CHttpDateTools::ToImfFixdate(behindTime), expected);
}


void CHttpDateToolsTest::MonthNamesTest()
{
	const QByteArrayList names = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};

	for (int month = 1; month <= 12; ++month){
		const QDateTime dateTime(QDate(2021, month, 15), QTime(12, 0), QTimeZone::UTC);

		// "Fri, 15 Jan 2021 12:00:00 GMT": the month is at the position 8
		QCOMPARE(imtrest::CHttpDateTools::ToImfFixdate(dateTime).mid(8, 3), names[month - 1]);
	}
}


void CHttpDateToolsTest::DayNamesTest()
{
	const QByteArrayList names = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};

	// 2021-01-03 is Sunday
	for (int day = 0; day < 7; ++day){
		const QDateTime dateTime(QDate(2021, 1, 3 + day), QTime(0, 0), QTimeZone::UTC);

		QCOMPARE(imtrest::CHttpDateTools::ToImfFixdate(dateTime).left(3), names[day]);
	}
}


void CHttpDateToolsTest::NamesAreNotLocalizedTest()
{
	const QLocale previousLocale = QLocale();

	// The names of the days and months of HTTP are always English:
	QLocale::setDefault(QLocale(QLocale::German, QLocale::Germany));

	const QByteArray date = imtrest::CHttpDateTools::ToImfFixdate(QDateTime::fromSecsSinceEpoch(784111777));

	QLocale::setDefault(previousLocale);

	QCOMPARE(date, QByteArray("Sun, 06 Nov 1994 08:49:37 GMT"));
}


void CHttpDateToolsTest::FixedLengthTest()
{
	// IMF-fixdate has a fixed length (29 characters), single digits are padded with zeros
	QCOMPARE(imtrest::CHttpDateTools::ToImfFixdate(QDateTime::fromSecsSinceEpoch(0)).size(), qsizetype(29));
	QCOMPARE(imtrest::CHttpDateTools::ToImfFixdate(QDateTime::fromSecsSinceEpoch(784111777)).size(), qsizetype(29));
}


void CHttpDateToolsTest::FromHttpDateTest_data()
{
	QTest::addColumn<QByteArray>("value");
	QTest::addColumn<qint64>("secsSinceEpoch");

	// Three formats of HTTP-date (RFC 9110, section 5.6.7), the example of the standard
	QTest::newRow("IMF-fixdate") << QByteArray("Sun, 06 Nov 1994 08:49:37 GMT") << qint64(784111777);
	QTest::newRow("RFC 850") << QByteArray("Sunday, 06-Nov-94 08:49:37 GMT") << qint64(784111777);
	QTest::newRow("asctime") << QByteArray("Sun Nov  6 08:49:37 1994") << qint64(784111777);

	QTest::newRow("asctime, zero padded day") << QByteArray("Sun Nov 06 08:49:37 1994") << qint64(784111777);
	QTest::newRow("asctime, two-digit day") << QByteArray("Wed Nov 16 08:49:37 1994") << qint64(784975777);
	QTest::newRow("epoch") << QByteArray("Thu, 01 Jan 1970 00:00:00 GMT") << qint64(0);
	QTest::newRow("before the epoch") << QByteArray("Wed, 31 Dec 1969 23:59:59 GMT") << qint64(-1);
	QTest::newRow("leap day") << QByteArray("Tue, 29 Feb 2000 00:00:00 GMT") << qint64(951782400);
	QTest::newRow("last second of the century") << QByteArray("Fri, 31 Dec 1999 23:59:59 GMT") << qint64(946684799);
	QTest::newRow("after 2038") << QByteArray("Tue, 19 Jan 2038 03:14:08 GMT") << qint64(2147483648);
}


void CHttpDateToolsTest::FromHttpDateTest()
{
	QFETCH(QByteArray, value);
	QFETCH(qint64, secsSinceEpoch);

	const QDateTime now(QDate(2026, 10, 16), QTime(12, 0), QTimeZone::UTC);
	const QDateTime result = imtrest::CHttpDateTools::FromHttpDate(value, now);

	QVERIFY(result.isValid());
	QCOMPARE(result.toSecsSinceEpoch(), secsSinceEpoch);
}


void CHttpDateToolsTest::FromHttpDateIsUtcTest()
{
	const QDateTime result = imtrest::CHttpDateTools::FromHttpDate("Sun, 06 Nov 1994 08:49:37 GMT");

	// The time does not depend on the time zone of the computer:
	QCOMPARE(result.offsetFromUtc(), 0);
	QCOMPARE(result.time(), QTime(8, 49, 37));
	QCOMPARE(result.date(), QDate(1994, 11, 6));
}


void CHttpDateToolsTest::RoundTripTest()
{
	const QList<qint64> moments = {-1, 0, 1, 784111777, 946684799, 951782400, 1483228799, 2147483648, 4102444800, 253402300799};

	for (qint64 secsSinceEpoch : moments){
		const QDateTime dateTime = QDateTime::fromSecsSinceEpoch(secsSinceEpoch);

		const QByteArray value = imtrest::CHttpDateTools::ToImfFixdate(dateTime);
		const QDateTime parsed = imtrest::CHttpDateTools::FromHttpDate(value);

		QVERIFY2(parsed.isValid(), value.constData());
		QCOMPARE(parsed.toSecsSinceEpoch(), secsSinceEpoch);
	}
}


void CHttpDateToolsTest::Rfc850YearTest_data()
{
	QTest::addColumn<QByteArray>("value");
	QTest::addColumn<int>("year");

	// The current time is 2026-10-16 12:00:00. A year that is more than 50 years ahead (after 2076-10-16 12:00:00)
	// is the latest year in the past with the same last two digits.
	QTest::newRow("1994") << QByteArray("Sunday, 06-Nov-94 08:49:37 GMT") << 1994;
	QTest::newRow("1999") << QByteArray("Friday, 31-Dec-99 23:59:59 GMT") << 1999;
	QTest::newRow("2000") << QByteArray("Saturday, 01-Jan-00 00:00:00 GMT") << 2000;
	QTest::newRow("2026") << QByteArray("Friday, 16-Oct-26 12:00:00 GMT") << 2026;
	QTest::newRow("2070") << QByteArray("Wednesday, 01-Jan-70 00:00:00 GMT") << 2070;
	QTest::newRow("exactly 50 years ahead") << QByteArray("Wednesday, 16-Oct-76 12:00:00 GMT") << 2076;
	QTest::newRow("a second after 50 years") << QByteArray("Wednesday, 16-Oct-76 12:00:01 GMT") << 1976;
	QTest::newRow("1977") << QByteArray("Saturday, 01-Jan-77 00:00:00 GMT") << 1977;
}


void CHttpDateToolsTest::Rfc850YearTest()
{
	QFETCH(QByteArray, value);
	QFETCH(int, year);

	const QDateTime now(QDate(2026, 10, 16), QTime(12, 0), QTimeZone::UTC);
	const QDateTime result = imtrest::CHttpDateTools::FromHttpDate(value, now);

	QVERIFY(result.isValid());
	QCOMPARE(result.date().year(), year);
}


void CHttpDateToolsTest::Rfc850YearInNextCenturyTest()
{
	// Near the end of the century the next century is ahead by less than 50 years:
	const QDateTime now(QDate(2099, 12, 31), QTime(12, 0), QTimeZone::UTC);

	QCOMPARE(imtrest::CHttpDateTools::FromHttpDate("Monday, 01-Jan-05 00:00:00 GMT", now).date().year(), 2105);
	QCOMPARE(imtrest::CHttpDateTools::FromHttpDate("Friday, 06-Nov-94 08:49:37 GMT", now).date().year(), 2094);
	QCOMPARE(imtrest::CHttpDateTools::FromHttpDate("Friday, 06-Nov-49 08:49:37 GMT", now).date().year(), 2149);
	QCOMPARE(imtrest::CHttpDateTools::FromHttpDate("Friday, 06-Nov-50 08:49:37 GMT", now).date().year(), 2050);
}


void CHttpDateToolsTest::LeapSecondTest()
{
	// The second 60 is the next minute (RFC 9110, section 5.6.7)
	const QDateTime result = imtrest::CHttpDateTools::FromHttpDate("Sat, 31 Dec 2016 23:59:60 GMT");

	QVERIFY(result.isValid());
	QCOMPARE(result.toSecsSinceEpoch(), qint64(1483228800));
}


void CHttpDateToolsTest::SpacesAreIgnoredTest()
{
	// Spaces around a value of a header are not a part of it
	const QDateTime result = imtrest::CHttpDateTools::FromHttpDate("  Sun, 06 Nov 1994 08:49:37 GMT\t ");

	QVERIFY(result.isValid());
	QCOMPARE(result.toSecsSinceEpoch(), qint64(784111777));
}


void CHttpDateToolsTest::DayNameIsNotComparedWithDateTest()
{
	// 1994-11-06 is Sunday, but the name of the day is not a reason to reject a date that is otherwise correct
	const QDateTime result = imtrest::CHttpDateTools::FromHttpDate("Mon, 06 Nov 1994 08:49:37 GMT");

	QVERIFY(result.isValid());
	QCOMPARE(result.toSecsSinceEpoch(), qint64(784111777));
}


void CHttpDateToolsTest::InvalidDateTest_data()
{
	QTest::addColumn<QByteArray>("value");

	QTest::newRow("empty") << QByteArray();
	QTest::newRow("spaces") << QByteArray("   ");
	QTest::newRow("text") << QByteArray("garbage");
	QTest::newRow("number") << QByteArray("784111777");
	QTest::newRow("ISO 8601") << QByteArray("1994-11-06T08:49:37Z");

	// IMF-fixdate
	QTest::newRow("zone UTC") << QByteArray("Sun, 06 Nov 1994 08:49:37 UTC");
	QTest::newRow("no zone") << QByteArray("Sun, 06 Nov 1994 08:49:37");
	QTest::newRow("text after GMT") << QByteArray("Sun, 06 Nov 1994 08:49:37 GMTX");
	QTest::newRow("zone offset") << QByteArray("Sun, 06 Nov 1994 08:49:37 +0100");
	QTest::newRow("day 32") << QByteArray("Sun, 32 Nov 1994 08:49:37 GMT");
	QTest::newRow("day 0") << QByteArray("Sun, 00 Nov 1994 08:49:37 GMT");
	QTest::newRow("30 February") << QByteArray("Sun, 30 Feb 1994 08:49:37 GMT");
	QTest::newRow("29 February, not a leap year") << QByteArray("Tue, 29 Feb 1994 08:49:37 GMT");
	QTest::newRow("31 April") << QByteArray("Sun, 31 Apr 1994 08:49:37 GMT");
	QTest::newRow("hour 24") << QByteArray("Sun, 06 Nov 1994 24:00:00 GMT");
	QTest::newRow("minute 60") << QByteArray("Sun, 06 Nov 1994 08:60:37 GMT");
	QTest::newRow("second 61") << QByteArray("Sun, 06 Nov 1994 08:49:61 GMT");
	QTest::newRow("unknown month") << QByteArray("Sun, 06 Foo 1994 08:49:37 GMT");
	QTest::newRow("unknown day name") << QByteArray("Xyz, 06 Nov 1994 08:49:37 GMT");
	QTest::newRow("lower case") << QByteArray("sun, 06 nov 1994 08:49:37 gmt");
	QTest::newRow("upper case") << QByteArray("SUN, 06 NOV 1994 08:49:37 GMT");
	QTest::newRow("one-digit day") << QByteArray("Sun, 6 Nov 1994 08:49:37 GMT");
	QTest::newRow("sign in day") << QByteArray("Sun, +6 Nov 1994 08:49:37 GMT");
	QTest::newRow("three-digit year") << QByteArray("Sun, 06 Nov 994 08:49:37 GMT");
	QTest::newRow("wrong time separators") << QByteArray("Sun, 06 Nov 1994 08-49-37 GMT");
	QTest::newRow("no comma") << QByteArray("Sun 06 Nov 1994 08:49:37 GMT");

	// RFC 850
	QTest::newRow("RFC 850, four-digit year") << QByteArray("Sunday, 06-Nov-1994 08:49:37 GMT");
	QTest::newRow("RFC 850, spaces instead of hyphens") << QByteArray("Sunday, 06 Nov 94 08:49:37 GMT");
	QTest::newRow("RFC 850, no comma") << QByteArray("Sunday 06-Nov-94 08:49:37 GMT");
	QTest::newRow("RFC 850, short day name") << QByteArray("Sun, 06-Nov-94 08:49:37 GMT");
	QTest::newRow("RFC 850, unknown day name") << QByteArray("Sundae, 06-Nov-94 08:49:37 GMT");
	QTest::newRow("RFC 850, no zone") << QByteArray("Sunday, 06-Nov-94 08:49:37");
	QTest::newRow("RFC 850, 30 February") << QByteArray("Sunday, 30-Feb-94 08:49:37 GMT");

	// asctime
	QTest::newRow("asctime, one space before the day") << QByteArray("Sun Nov 6 08:49:37 1994");
	QTest::newRow("asctime, two-digit year") << QByteArray("Sun Nov  6 08:49:37 94");
	QTest::newRow("asctime, zone") << QByteArray("Sun Nov  6 08:49:37 1994 GMT");
	QTest::newRow("asctime, unknown month") << QByteArray("Sun Foo  6 08:49:37 1994");
	QTest::newRow("asctime, hour 24") << QByteArray("Sun Nov  6 24:49:37 1994");
}


void CHttpDateToolsTest::InvalidDateTest()
{
	QFETCH(QByteArray, value);

	const QDateTime now(QDate(2026, 10, 16), QTime(12, 0), QTimeZone::UTC);

	QVERIFY2(!imtrest::CHttpDateTools::FromHttpDate(value, now).isValid(), value.constData());
}


void CHttpDateToolsTest::TruncatedValuesAreInvalidTest_data()
{
	QTest::addColumn<QByteArray>("value");

	QTest::newRow("IMF-fixdate") << QByteArray("Sun, 06 Nov 1994 08:49:37 GMT");
	QTest::newRow("RFC 850") << QByteArray("Sunday, 06-Nov-94 08:49:37 GMT");
	QTest::newRow("asctime") << QByteArray("Sun Nov  6 08:49:37 1994");
}


void CHttpDateToolsTest::TruncatedValuesAreInvalidTest()
{
	QFETCH(QByteArray, value);

	// A value that is cut at any position is not a date, the parser does not read after its end:
	for (qsizetype length = 0; length < value.size(); ++length){
		const QByteArray cutValue = value.left(length);

		QVERIFY2(!imtrest::CHttpDateTools::FromHttpDate(cutValue).isValid(), cutValue.constData());
	}
}


void CHttpDateToolsTest::CorruptedValuesTest()
{
	const QByteArrayList values = {
				"Sun, 06 Nov 1994 08:49:37 GMT",
				"Sunday, 06-Nov-94 08:49:37 GMT",
				"Sun Nov  6 08:49:37 1994"};

	const QByteArray replacements("0x ,-:\xff");

	// Any damage gives a valid UTC time or an invalid time, nothing else
	for (const QByteArray& value : values){
		for (qsizetype position = 0; position < value.size(); ++position){
			for (char replacement : replacements){
				QByteArray corruptedValue = value;
				corruptedValue[position] = replacement;

				const QDateTime result = imtrest::CHttpDateTools::FromHttpDate(corruptedValue);

				QVERIFY(!result.isValid() || (result.offsetFromUtc() == 0));
			}

			// The character NUL
			QByteArray nulValue = value;
			nulValue[position] = '\0';

			const QDateTime nulResult = imtrest::CHttpDateTools::FromHttpDate(nulValue);

			QVERIFY(!nulResult.isValid() || (nulResult.offsetFromUtc() == 0));
		}
	}
}


I_ADD_TEST(CHttpDateToolsTest);


