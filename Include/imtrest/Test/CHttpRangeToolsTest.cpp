// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include "CHttpRangeToolsTest.h"


// STL includes
#include <limits>

// ImtCore includes
#include <imtrest/CHttpRangeTools.h>


namespace
{


// "0-99,200-299" for the ranges 0-99 and 200-299
QByteArray ToText(const QList<imtrest::CHttpRangeTools::ByteRange>& ranges)
{
	QByteArrayList parts;

	for (const imtrest::CHttpRangeTools::ByteRange& range : ranges){
		parts.append(QByteArray::number(range.first) + '-' + QByteArray::number(range.last));
	}

	return parts.join(',');
}


QList<imtrest::CHttpRangeTools::ByteRange> FromText(const QByteArray& text)
{
	QList<imtrest::CHttpRangeTools::ByteRange> retVal;

	for (const QByteArray& part : text.split(',')){
		if (part.isEmpty()){
			continue;
		}

		const QList<QByteArray> positions = part.split('-');

		imtrest::CHttpRangeTools::ByteRange range;
		range.first = positions.at(0).toLongLong();
		range.last = positions.at(1).toLongLong();

		retVal.append(range);
	}

	return retVal;
}


} // namespace


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


void CHttpRangeToolsTest::ParseRangeTest_data()
{
	QTest::addColumn<QByteArray>("value");
	QTest::addColumn<qint64>("completeLength");
	QTest::addColumn<QByteArray>("expected");

	// The representation of 1000 bytes has the positions 0-999
	QTest::newRow("first 500 bytes") << QByteArray("bytes=0-499") << qint64(1000) << QByteArray("0-499");
	QTest::newRow("last 500 bytes by positions") << QByteArray("bytes=500-999") << qint64(1000) << QByteArray("500-999");
	QTest::newRow("single first byte") << QByteArray("bytes=0-0") << qint64(1000) << QByteArray("0-0");
	QTest::newRow("single last byte") << QByteArray("bytes=999-999") << qint64(1000) << QByteArray("999-999");
	QTest::newRow("all bytes") << QByteArray("bytes=0-999") << qint64(1000) << QByteArray("0-999");

	// Open range
	QTest::newRow("from the middle to the end") << QByteArray("bytes=500-") << qint64(1000) << QByteArray("500-999");
	QTest::newRow("from the start to the end") << QByteArray("bytes=0-") << qint64(1000) << QByteArray("0-999");
	QTest::newRow("only the last byte by position") << QByteArray("bytes=999-") << qint64(1000) << QByteArray("999-999");

	// Suffix range
	QTest::newRow("suffix of 500 bytes") << QByteArray("bytes=-500") << qint64(1000) << QByteArray("500-999");
	QTest::newRow("suffix of one byte") << QByteArray("bytes=-1") << qint64(1000) << QByteArray("999-999");
	QTest::newRow("suffix as long as the representation") << QByteArray("bytes=-1000") << qint64(1000) << QByteArray("0-999");
	QTest::newRow("suffix longer than the representation") << QByteArray("bytes=-5000") << qint64(1000) << QByteArray("0-999");

	// The last position beyond the end is cut
	QTest::newRow("last beyond the end") << QByteArray("bytes=0-5000") << qint64(1000) << QByteArray("0-999");
	QTest::newRow("last is equal to the length") << QByteArray("bytes=900-1000") << qint64(1000) << QByteArray("900-999");
	QTest::newRow("first is the last byte, last beyond the end") << QByteArray("bytes=999-5000") << qint64(1000) << QByteArray("999-999");

	// Numbers out of the range of qint64 mean the largest number
	QTest::newRow("huge last position") << QByteArray("bytes=0-99999999999999999999999") << qint64(1000) << QByteArray("0-999");
	QTest::newRow("huge suffix") << QByteArray("bytes=-99999999999999999999") << qint64(1000) << QByteArray("0-999");

	// Syntax details
	QTest::newRow("unit is case-insensitive") << QByteArray("BYTES=0-9") << qint64(1000) << QByteArray("0-9");
	QTest::newRow("unit with mixed case") << QByteArray("Bytes=0-9") << qint64(1000) << QByteArray("0-9");
	QTest::newRow("surrounding whitespace") << QByteArray(" \tbytes=0-9 \t") << qint64(1000) << QByteArray("0-9");
	QTest::newRow("leading zeros") << QByteArray("bytes=0007-0009") << qint64(1000) << QByteArray("7-9");

	// Large representation
	QTest::newRow("positions above 4 GiB") << QByteArray("bytes=5000000000-") << qint64(6000000000) << QByteArray("5000000000-5999999999");
	QTest::newRow("suffix of a large representation") << QByteArray("bytes=-1000000") << qint64(6000000000) << QByteArray("5999000000-5999999999");

	// Several ranges are returned in the order of the request and they are not merged
	QTest::newRow("two ranges") << QByteArray("bytes=0-99,200-299") << qint64(1000) << QByteArray("0-99,200-299");
	QTest::newRow("two ranges with spaces") << QByteArray("bytes=0-99 , 200-299") << qint64(1000) << QByteArray("0-99,200-299");
	QTest::newRow("empty list elements") << QByteArray("bytes=,0-99,,200-299,") << qint64(1000) << QByteArray("0-99,200-299");
	QTest::newRow("order of the request") << QByteArray("bytes=500-599,0-99") << qint64(1000) << QByteArray("500-599,0-99");
	QTest::newRow("overlapping ranges") << QByteArray("bytes=0-99,50-149") << qint64(1000) << QByteArray("0-99,50-149");
	QTest::newRow("range, open range and suffix") << QByteArray("bytes=0-9,100-,-10") << qint64(1000) << QByteArray("0-9,100-999,990-999");

	// The ranges that are not satisfiable are skipped if there is a satisfiable range
	QTest::newRow("skipped range after the end") << QByteArray("bytes=0-99,2000-2999") << qint64(1000) << QByteArray("0-99");
	QTest::newRow("skipped range before") << QByteArray("bytes=2000-2999,0-99") << qint64(1000) << QByteArray("0-99");
	QTest::newRow("skipped empty suffix") << QByteArray("bytes=-0,0-9") << qint64(1000) << QByteArray("0-9");
}


void CHttpRangeToolsTest::ParseRangeTest()
{
	QFETCH(QByteArray, value);
	QFETCH(qint64, completeLength);
	QFETCH(QByteArray, expected);

	QList<imtrest::CHttpRangeTools::ByteRange> ranges;

	QCOMPARE(imtrest::CHttpRangeTools::ParseRange(value, completeLength, ranges), imtrest::CHttpRangeTools::RS_SATISFIABLE);
	QCOMPARE(ToText(ranges), expected);
}


void CHttpRangeToolsTest::ParseRangeNotSatisfiableTest_data()
{
	QTest::addColumn<QByteArray>("value");
	QTest::addColumn<qint64>("completeLength");

	QTest::newRow("first is equal to the length") << QByteArray("bytes=1000-") << qint64(1000);
	QTest::newRow("range after the end") << QByteArray("bytes=1000-2000") << qint64(1000);
	QTest::newRow("single byte after the end") << QByteArray("bytes=1000-1000") << qint64(1000);
	QTest::newRow("huge first position") << QByteArray("bytes=99999999999999999999-") << qint64(1000);
	QTest::newRow("empty suffix") << QByteArray("bytes=-0") << qint64(1000);
	QTest::newRow("several ranges after the end") << QByteArray("bytes=2000-,3000-4000") << qint64(1000);
	QTest::newRow("range and empty suffix") << QByteArray("bytes=2000-,-0") << qint64(1000);
	QTest::newRow("empty representation, open range") << QByteArray("bytes=0-") << qint64(0);
	QTest::newRow("empty representation, range") << QByteArray("bytes=0-0") << qint64(0);
	QTest::newRow("empty representation, suffix") << QByteArray("bytes=-5") << qint64(0);
}


void CHttpRangeToolsTest::ParseRangeNotSatisfiableTest()
{
	QFETCH(QByteArray, value);
	QFETCH(qint64, completeLength);

	QList<imtrest::CHttpRangeTools::ByteRange> ranges;

	QCOMPARE(imtrest::CHttpRangeTools::ParseRange(value, completeLength, ranges), imtrest::CHttpRangeTools::RS_NOT_SATISFIABLE);
	QVERIFY(ranges.isEmpty());
}


void CHttpRangeToolsTest::ParseRangeIgnoredTest_data()
{
	QTest::addColumn<QByteArray>("value");

	QTest::newRow("empty") << QByteArray();
	QTest::newRow("only unit") << QByteArray("bytes");
	QTest::newRow("unit with equal sign only") << QByteArray("bytes=");
	QTest::newRow("only commas") << QByteArray("bytes=,,");
	QTest::newRow("unknown unit") << QByteArray("items=0-9");
	QTest::newRow("unit that begins with bytes") << QByteArray("bytesx=0-9");
	QTest::newRow("no unit") << QByteArray("=0-9");
	QTest::newRow("space instead of equal sign") << QByteArray("bytes 0-9");
	QTest::newRow("space before equal sign") << QByteArray("bytes =0-9");
	QTest::newRow("only dash") << QByteArray("bytes=-");
	QTest::newRow("no dash") << QByteArray("bytes=9");
	QTest::newRow("letters") << QByteArray("bytes=a-b");
	QTest::newRow("letter after the number") << QByteArray("bytes=0-9a");
	QTest::newRow("sign") << QByteArray("bytes=+1-5");
	QTest::newRow("negative first") << QByteArray("bytes=--5");
	QTest::newRow("hexadecimal") << QByteArray("bytes=0x10-0x20");
	QTest::newRow("last is less than first") << QByteArray("bytes=5-1");
	QTest::newRow("last is less than first by one") << QByteArray("bytes=10-9");
	QTest::newRow("two dashes in the suffix") << QByteArray("bytes=-5-");
	QTest::newRow("space inside of the range") << QByteArray("bytes=0 - 9");
	QTest::newRow("space before the dash") << QByteArray("bytes=1 -9");
	QTest::newRow("space after the dash") << QByteArray("bytes=0- 9");
	QTest::newRow("parameter") << QByteArray("bytes=0-9;q=1");

	// A bad element makes the whole field invalid, even if the other elements are good
	QTest::newRow("bad element after a good one") << QByteArray("bytes=0-9,x");
	QTest::newRow("bad element before a good one") << QByteArray("bytes=x,0-9");
	QTest::newRow("last is less than first in the second range") << QByteArray("bytes=0-9,5-1");
	QTest::newRow("bad element after a range that is not satisfiable") << QByteArray("bytes=5000-,x");
}


void CHttpRangeToolsTest::ParseRangeIgnoredTest()
{
	QFETCH(QByteArray, value);

	QList<imtrest::CHttpRangeTools::ByteRange> ranges;

	QCOMPARE(imtrest::CHttpRangeTools::ParseRange(value, 1000, ranges), imtrest::CHttpRangeTools::RS_IGNORE);
	QVERIFY2(ranges.isEmpty(), value.constData());
}


void CHttpRangeToolsTest::ParseRangeIgnoredNegativeLengthTest()
{
	QList<imtrest::CHttpRangeTools::ByteRange> ranges;

	// The size of the representation is unknown, so there is nothing to check the range with
	QCOMPARE(imtrest::CHttpRangeTools::ParseRange("bytes=0-9", -1, ranges), imtrest::CHttpRangeTools::RS_IGNORE);
	QVERIFY(ranges.isEmpty());
}


void CHttpRangeToolsTest::ParseRangeKeepsResultTest()
{
	QList<imtrest::CHttpRangeTools::ByteRange> ranges = FromText("11-22,33-44");

	QCOMPARE(imtrest::CHttpRangeTools::ParseRange("bytes=9-0", 1000, ranges), imtrest::CHttpRangeTools::RS_IGNORE);
	QCOMPARE(ToText(ranges), QByteArray("11-22,33-44"));

	QCOMPARE(imtrest::CHttpRangeTools::ParseRange("bytes=5000-", 1000, ranges), imtrest::CHttpRangeTools::RS_NOT_SATISFIABLE);
	QCOMPARE(ToText(ranges), QByteArray("11-22,33-44"));
}


void CHttpRangeToolsTest::ParseRangeReplacesResultTest()
{
	QList<imtrest::CHttpRangeTools::ByteRange> ranges = FromText("11-22,33-44");

	QCOMPARE(imtrest::CHttpRangeTools::ParseRange("bytes=0-9", 1000, ranges), imtrest::CHttpRangeTools::RS_SATISFIABLE);
	QCOMPARE(ToText(ranges), QByteArray("0-9"));
}


void CHttpRangeToolsTest::ParseRangeMaxCountTest()
{
	QList<imtrest::CHttpRangeTools::ByteRange> ranges;

	// By default 16 ranges are allowed
	QByteArray value("bytes=");
	for (int i = 0; i < 16; ++i){
		value += QByteArray::number(i * 10) + '-' + QByteArray::number(i * 10 + 5) + ',';
	}

	QCOMPARE(imtrest::CHttpRangeTools::ParseRange(value, 1000, ranges), imtrest::CHttpRangeTools::RS_SATISFIABLE);
	QCOMPARE(ranges.size(), 16);

	value += "500-599";

	QCOMPARE(imtrest::CHttpRangeTools::ParseRange(value, 1000, ranges), imtrest::CHttpRangeTools::RS_IGNORE);
	QCOMPARE(ranges.size(), 16);

	// The limit is a parameter
	QCOMPARE(imtrest::CHttpRangeTools::ParseRange("bytes=0-1,5-6", 1000, ranges, 2), imtrest::CHttpRangeTools::RS_SATISFIABLE);
	QCOMPARE(imtrest::CHttpRangeTools::ParseRange("bytes=0-1,5-6,9-10", 1000, ranges, 2), imtrest::CHttpRangeTools::RS_IGNORE);

	// The ranges that are not satisfiable count too
	QCOMPARE(imtrest::CHttpRangeTools::ParseRange("bytes=0-1,5000-,6000-", 1000, ranges, 2), imtrest::CHttpRangeTools::RS_IGNORE);

	// Empty list elements do not count
	QCOMPARE(imtrest::CHttpRangeTools::ParseRange("bytes=,,0-1,,,5-6,,", 1000, ranges, 2), imtrest::CHttpRangeTools::RS_SATISFIABLE);
}


void CHttpRangeToolsTest::ParseRangeContentRangeTest()
{
	QList<imtrest::CHttpRangeTools::ByteRange> ranges;

	// The answer 206 for the suffix range
	QCOMPARE(imtrest::CHttpRangeTools::ParseRange("bytes=-500", 1000, ranges), imtrest::CHttpRangeTools::RS_SATISFIABLE);
	QCOMPARE(ranges.size(), 1);
	QCOMPARE(imtrest::CHttpRangeTools::CreateContentRange(ranges.at(0).first, ranges.at(0).GetLength(), 1000), QByteArray("bytes 500-999/1000"));

	// The answer 206 for the cut range
	QCOMPARE(imtrest::CHttpRangeTools::ParseRange("bytes=900-5000", 1000, ranges), imtrest::CHttpRangeTools::RS_SATISFIABLE);
	QCOMPARE(imtrest::CHttpRangeTools::CreateContentRange(ranges.at(0).first, ranges.at(0).GetLength(), 1000), QByteArray("bytes 900-999/1000"));

	// The answer 416 has the unsatisfied range of Content-Range
	imtrest::CHttpRangeTools::ContentRange contentRange;
	QVERIFY(imtrest::CHttpRangeTools::ParseContentRange("bytes */1000", contentRange));
	QVERIFY(!contentRange.IsByteRange());
	QCOMPARE(contentRange.completeLength, qint64(1000));
}


void CHttpRangeToolsTest::ByteRangeLengthTest()
{
	imtrest::CHttpRangeTools::ByteRange range;

	range.first = 0;
	range.last = 0;
	QCOMPARE(range.GetLength(), qint64(1));

	range.first = 500;
	range.last = 999;
	QCOMPARE(range.GetLength(), qint64(500));

	range.first = 5000000000;
	range.last = 5000999999;
	QCOMPARE(range.GetLength(), qint64(1000000));
}


void CHttpRangeToolsTest::CoalesceRangesTest_data()
{
	QTest::addColumn<QByteArray>("ranges");
	QTest::addColumn<QByteArray>("expected");

	QTest::newRow("no ranges") << QByteArray() << QByteArray();
	QTest::newRow("single range") << QByteArray("10-19") << QByteArray("10-19");
	QTest::newRow("separate ranges") << QByteArray("0-9,20-29") << QByteArray("0-9,20-29");
	QTest::newRow("unsorted separate ranges") << QByteArray("20-29,0-9") << QByteArray("0-9,20-29");
	QTest::newRow("overlapping ranges") << QByteArray("0-99,50-149") << QByteArray("0-149");
	QTest::newRow("touching ranges") << QByteArray("0-99,100-199") << QByteArray("0-199");
	QTest::newRow("a gap of one byte") << QByteArray("0-98,100-199") << QByteArray("0-98,100-199");
	QTest::newRow("range inside of another one") << QByteArray("0-999,10-20") << QByteArray("0-999");
	QTest::newRow("same ranges") << QByteArray("5-9,5-9") << QByteArray("5-9");
	QTest::newRow("same first position") << QByteArray("5-9,5-20") << QByteArray("5-20");
	QTest::newRow("chain of ranges") << QByteArray("0-10,5-20,15-30") << QByteArray("0-30");
	QTest::newRow("unsorted overlapping ranges") << QByteArray("500-599,0-99,90-510") << QByteArray("0-599");
	QTest::newRow("merged and separate") << QByteArray("300-399,0-9,5-15,100-199") << QByteArray("0-15,100-199,300-399");
	QTest::newRow("single bytes") << QByteArray("3-3,1-1,2-2") << QByteArray("1-3");
}


void CHttpRangeToolsTest::CoalesceRangesTest()
{
	QFETCH(QByteArray, ranges);
	QFETCH(QByteArray, expected);

	QCOMPARE(ToText(imtrest::CHttpRangeTools::CoalesceRanges(FromText(ranges))), expected);
}


void CHttpRangeToolsTest::CoalesceRangesLargePositionsTest()
{
	const qint64 maxValue = std::numeric_limits<qint64>::max();

	QList<imtrest::CHttpRangeTools::ByteRange> ranges = FromText("9223372036854775798-9223372036854775807,9223372036854775778-9223372036854775797");

	const QList<imtrest::CHttpRangeTools::ByteRange> result = imtrest::CHttpRangeTools::CoalesceRanges(ranges);

	QCOMPARE(result.size(), 1);
	QCOMPARE(result.at(0).first, maxValue - 29);
	QCOMPARE(result.at(0).last, maxValue);

	// The positions are not changed by the method
	QCOMPARE(ranges.at(0).first, maxValue - 9);
}


I_ADD_TEST(CHttpRangeToolsTest);


