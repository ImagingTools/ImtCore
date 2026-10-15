// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include "CHttpRangeToolsTest.h"


// STL includes
#include <limits>

// ImtCore includes
#include <imtrest/CHttpRangeTools.h>


// private slots

void CHttpRangeToolsTest::ParseContentRangeTest_data()
{
	QTest::addColumn<QByteArray>("value");
	QTest::addColumn<qint64>("first");
	QTest::addColumn<qint64>("last");
	QTest::addColumn<qint64>("completeLength");

	// The positions are inclusive: the first 1000 bytes of a file of 1000 bytes
	QTest::newRow("whole representation") << QByteArray("bytes 0-999/1000") << qint64(0) << qint64(999) << qint64(1000);
	QTest::newRow("single byte") << QByteArray("bytes 0-0/1") << qint64(0) << qint64(0) << qint64(1);
	QTest::newRow("last byte") << QByteArray("bytes 999-999/1000") << qint64(999) << qint64(999) << qint64(1000);
	QTest::newRow("middle part") << QByteArray("bytes 1000-1999/5000") << qint64(1000) << qint64(1999) << qint64(5000);
	QTest::newRow("unknown length") << QByteArray("bytes 500-999/*") << qint64(500) << qint64(999) << qint64(-1);
	QTest::newRow("unsatisfied range") << QByteArray("bytes */1000") << qint64(-1) << qint64(-1) << qint64(1000);
	QTest::newRow("unit is case-insensitive") << QByteArray("BYTES 0-9/10") << qint64(0) << qint64(9) << qint64(10);
	QTest::newRow("unit with mixed case") << QByteArray("Bytes 0-9/10") << qint64(0) << qint64(9) << qint64(10);
	QTest::newRow("surrounding whitespace") << QByteArray(" \tbytes 0-9/10 \t") << qint64(0) << qint64(9) << qint64(10);
	QTest::newRow("leading zeros") << QByteArray("bytes 007-009/010") << qint64(7) << qint64(9) << qint64(10);
	QTest::newRow("positions above 4 GiB") << QByteArray("bytes 5000000000-5000999999/6000000000")
				<< qint64(5000000000) << qint64(5000999999) << qint64(6000000000);
	QTest::newRow("the largest position") << QByteArray("bytes 0-9223372036854775806/9223372036854775807")
				<< qint64(0) << qint64(9223372036854775806) << qint64(9223372036854775807);
}


void CHttpRangeToolsTest::ParseContentRangeTest()
{
	QFETCH(QByteArray, value);
	QFETCH(qint64, first);
	QFETCH(qint64, last);
	QFETCH(qint64, completeLength);

	imtrest::CHttpRangeTools::ContentRange range;

	QVERIFY(imtrest::CHttpRangeTools::ParseContentRange(value, range));
	QCOMPARE(range.first, first);
	QCOMPARE(range.last, last);
	QCOMPARE(range.completeLength, completeLength);
}


void CHttpRangeToolsTest::ParseContentRangeInvalidTest_data()
{
	QTest::addColumn<QByteArray>("value");

	QTest::newRow("empty") << QByteArray();
	QTest::newRow("only unit") << QByteArray("bytes");
	QTest::newRow("unit and space") << QByteArray("bytes ");
	QTest::newRow("no unit") << QByteArray("0-9/10");
	QTest::newRow("other unit") << QByteArray("items 0-9/10");
	QTest::newRow("no space after unit") << QByteArray("bytes0-9/10");
	QTest::newRow("two spaces after unit") << QByteArray("bytes  0-9/10");
	QTest::newRow("no length part") << QByteArray("bytes 0-9");
	QTest::newRow("empty length") << QByteArray("bytes 0-9/");
	QTest::newRow("no range part") << QByteArray("bytes /10");
	QTest::newRow("no last position") << QByteArray("bytes 0-/10");
	QTest::newRow("no first position") << QByteArray("bytes -9/10");
	QTest::newRow("no dash") << QByteArray("bytes 09/10");
	QTest::newRow("first is greater than last") << QByteArray("bytes 9-0/10");
	QTest::newRow("last is equal to the length") << QByteArray("bytes 0-10/10");
	QTest::newRow("last is greater than the length") << QByteArray("bytes 0-11/10");
	QTest::newRow("both unknown") << QByteArray("bytes */*");
	QTest::newRow("unsatisfied range without length") << QByteArray("bytes */");
	QTest::newRow("star as a position") << QByteArray("bytes *-5/10");
	QTest::newRow("spaces around dash") << QByteArray("bytes 0 - 9/10");
	QTest::newRow("space before slash") << QByteArray("bytes 0-9 /10");
	QTest::newRow("space after slash") << QByteArray("bytes 0-9/ 10");
	QTest::newRow("plus sign") << QByteArray("bytes +0-9/10");
	QTest::newRow("hexadecimal number") << QByteArray("bytes 0x1-9/10");
	QTest::newRow("exponent") << QByteArray("bytes 0-9/1e3");
	QTest::newRow("negative length") << QByteArray("bytes 0-9/-1");
	QTest::newRow("double dash") << QByteArray("bytes 0--9/10");
	QTest::newRow("two slashes") << QByteArray("bytes 0-9/10/11");
	QTest::newRow("two ranges") << QByteArray("bytes 0-4,5-9/10");
	QTest::newRow("trailing text") << QByteArray("bytes 0-9/10 bytes");
	QTest::newRow("position overflow") << QByteArray("bytes 0-99999999999999999999/*");
	QTest::newRow("length overflow") << QByteArray("bytes 0-9/99999999999999999999");
	QTest::newRow("the first position overflow") << QByteArray("bytes 9223372036854775808-9223372036854775809/*");
	QTest::newRow("non-ASCII digits") << QByteArray("bytes \xd9\xa0-\xd9\xa9/10");
}


void CHttpRangeToolsTest::ParseContentRangeInvalidTest()
{
	QFETCH(QByteArray, value);

	imtrest::CHttpRangeTools::ContentRange range;

	QVERIFY2(!imtrest::CHttpRangeTools::ParseContentRange(value, range), value.constData());
}


void CHttpRangeToolsTest::ParseContentRangeKeepsResultOnErrorTest()
{
	imtrest::CHttpRangeTools::ContentRange range;
	range.first = 11;
	range.last = 22;
	range.completeLength = 33;

	QVERIFY(!imtrest::CHttpRangeTools::ParseContentRange("bytes 9-0/10", range));

	QCOMPARE(range.first, qint64(11));
	QCOMPARE(range.last, qint64(22));
	QCOMPARE(range.completeLength, qint64(33));
}


void CHttpRangeToolsTest::ContentRangeLengthTest()
{
	imtrest::CHttpRangeTools::ContentRange range;

	// Not parsed range:
	QVERIFY(!range.IsByteRange());
	QCOMPARE(range.GetLength(), qint64(0));

	QVERIFY(imtrest::CHttpRangeTools::ParseContentRange("bytes 0-999/5000", range));
	QVERIFY(range.IsByteRange());
	QCOMPARE(range.GetLength(), qint64(1000));

	QVERIFY(imtrest::CHttpRangeTools::ParseContentRange("bytes 1000-1999/5000", range));
	QCOMPARE(range.GetLength(), qint64(1000));

	QVERIFY(imtrest::CHttpRangeTools::ParseContentRange("bytes 7-7/8", range));
	QCOMPARE(range.GetLength(), qint64(1));

	QVERIFY(imtrest::CHttpRangeTools::ParseContentRange("bytes */5000", range));
	QVERIFY(!range.IsByteRange());
	QCOMPARE(range.GetLength(), qint64(0));
}


void CHttpRangeToolsTest::CreateContentRangeTest_data()
{
	QTest::addColumn<qint64>("first");
	QTest::addColumn<qint64>("length");
	QTest::addColumn<qint64>("completeLength");
	QTest::addColumn<QByteArray>("expected");

	const qint64 maxValue = std::numeric_limits<qint64>::max();

	// The last position is inclusive:
	QTest::newRow("first part") << qint64(0) << qint64(1000) << qint64(2500) << QByteArray("bytes 0-999/2500");
	QTest::newRow("middle part") << qint64(1000) << qint64(1000) << qint64(2500) << QByteArray("bytes 1000-1999/2500");
	QTest::newRow("short last part") << qint64(2000) << qint64(500) << qint64(2500) << QByteArray("bytes 2000-2499/2500");
	QTest::newRow("single byte") << qint64(7) << qint64(1) << qint64(8) << QByteArray("bytes 7-7/8");
	QTest::newRow("whole representation") << qint64(0) << qint64(2500) << qint64(2500) << QByteArray("bytes 0-2499/2500");
	QTest::newRow("unknown length") << qint64(0) << qint64(10) << qint64(-1) << QByteArray("bytes 0-9/*");
	QTest::newRow("positions above 4 GiB") << qint64(5000000000) << qint64(1000000) << qint64(6000000000)
				<< QByteArray("bytes 5000000000-5000999999/6000000000");
	QTest::newRow("the largest position") << (maxValue - 9) << qint64(10) << qint64(-1)
				<< QByteArray("bytes 9223372036854775798-9223372036854775807/*");

	// No range is described:
	QTest::newRow("empty range") << qint64(0) << qint64(0) << qint64(10) << QByteArray();
	QTest::newRow("negative length") << qint64(0) << qint64(-3) << qint64(10) << QByteArray();
	QTest::newRow("negative first position") << qint64(-1) << qint64(5) << qint64(10) << QByteArray();
	QTest::newRow("last is equal to the length") << qint64(5) << qint64(6) << qint64(10) << QByteArray();
	QTest::newRow("range is out of the length") << qint64(0) << qint64(11) << qint64(10) << QByteArray();
	QTest::newRow("first is out of the length") << qint64(10) << qint64(1) << qint64(10) << QByteArray();
	QTest::newRow("last position overflow") << (maxValue - 8) << qint64(10) << qint64(-1) << QByteArray();
	QTest::newRow("length overflow") << qint64(5) << maxValue << qint64(-1) << QByteArray();
}


void CHttpRangeToolsTest::CreateContentRangeTest()
{
	QFETCH(qint64, first);
	QFETCH(qint64, length);
	QFETCH(qint64, completeLength);
	QFETCH(QByteArray, expected);

	QCOMPARE(imtrest::CHttpRangeTools::CreateContentRange(first, length, completeLength), expected);
}


void CHttpRangeToolsTest::CreateContentRangeRoundTripTest()
{
	const qint64 firstValues[] = {0, 1, 999, 1000, 4096, 1048576, 5000000000};
	const qint64 lengthValues[] = {1, 2, 1000, 1048576};

	for (qint64 first : firstValues){
		for (qint64 length : lengthValues){
			for (qint64 extra : {qint64(0), qint64(1), qint64(12345)}){
				const qint64 completeLength = first + length + extra;

				const QByteArray value = imtrest::CHttpRangeTools::CreateContentRange(first, length, completeLength);
				QVERIFY(!value.isEmpty());

				imtrest::CHttpRangeTools::ContentRange range;
				QVERIFY2(imtrest::CHttpRangeTools::ParseContentRange(value, range), value.constData());

				QCOMPARE(range.first, first);
				QCOMPARE(range.GetLength(), length);
				QCOMPARE(range.completeLength, completeLength);
			}
		}
	}
}


I_ADD_TEST(CHttpRangeToolsTest);


