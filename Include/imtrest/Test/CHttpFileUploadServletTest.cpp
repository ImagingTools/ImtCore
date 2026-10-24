// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include "CHttpFileUploadServletTest.h"


// ImtCore includes
#include <imtrest/CHttpFileUploadServletComp.h>


// private slots

void CHttpFileUploadServletTest::GetChunkRangeTest_data()
{
	QTest::addColumn<QByteArray>("contentRange");
	QTest::addColumn<qint64>("chunkSize");
	QTest::addColumn<qint64>("first");
	QTest::addColumn<qint64>("last");
	QTest::addColumn<qint64>("fileSize");

	// The positions of the header are inclusive (RFC 9110, 14.4) and the range of the chunk is the same:
	QTest::newRow("first chunk") << QByteArray("bytes 0-999/5000") << qint64(1000) << qint64(0) << qint64(999) << qint64(5000);
	QTest::newRow("middle chunk") << QByteArray("bytes 1000-1999/5000") << qint64(1000) << qint64(1000) << qint64(1999) << qint64(5000);
	QTest::newRow("last chunk") << QByteArray("bytes 4000-4999/5000") << qint64(1000) << qint64(4000) << qint64(4999) << qint64(5000);
	QTest::newRow("short last chunk") << QByteArray("bytes 4500-4999/5000") << qint64(500) << qint64(4500) << qint64(4999) << qint64(5000);
	QTest::newRow("single byte") << QByteArray("bytes 0-0/1") << qint64(1) << qint64(0) << qint64(0) << qint64(1);

	// The range is 64-bit, there is no limit of 2 GiB:
	QTest::newRow("last byte of 2 GiB") << QByteArray("bytes 2147483647-2147483647/2147483648")
				<< qint64(1) << qint64(2147483647) << qint64(2147483647) << qint64(2147483648);
	QTest::newRow("chunk above 4 GiB") << QByteArray("bytes 5000000000-5000000999/6000000000")
				<< qint64(1000) << qint64(5000000000) << qint64(5000000999) << qint64(6000000000);
	QTest::newRow("the largest file") << QByteArray("bytes 9223372036854775806-9223372036854775806/9223372036854775807")
				<< qint64(1) << qint64(9223372036854775806) << qint64(9223372036854775806) << qint64(9223372036854775807);
}


void CHttpFileUploadServletTest::GetChunkRangeTest()
{
	QFETCH(QByteArray, contentRange);
	QFETCH(qint64, chunkSize);
	QFETCH(qint64, first);
	QFETCH(qint64, last);
	QFETCH(qint64, fileSize);

	imtservergql::IFileUploadHandler::ByteRange range;
	qint64 resultFileSize = -1;

	QVERIFY(imtrest::CHttpFileUploadServletComp::GetChunkRange(contentRange, chunkSize, range, resultFileSize));

	QCOMPARE(range.GetMinValue(), first);
	QCOMPARE(range.GetMaxValue(), last);
	QCOMPARE(resultFileSize, fileSize);

	// The number of bytes of the range is the size of the chunk:
	QCOMPARE(range.GetLength() + 1, chunkSize);
}


void CHttpFileUploadServletTest::GetChunkRangeInvalidTest_data()
{
	QTest::addColumn<QByteArray>("contentRange");
	QTest::addColumn<qint64>("chunkSize");

	QTest::newRow("no header") << QByteArray() << qint64(1000);
	QTest::newRow("wrong syntax") << QByteArray("bytes 0-999") << qint64(1000);
	QTest::newRow("other unit") << QByteArray("items 0-999/5000") << qint64(1000);

	// The chunk should fill the range exactly:
	QTest::newRow("chunk is shorter") << QByteArray("bytes 0-999/5000") << qint64(999);
	QTest::newRow("chunk is longer") << QByteArray("bytes 0-999/5000") << qint64(1001);
	QTest::newRow("empty chunk") << QByteArray("bytes 0-999/5000") << qint64(0);

	// The exclusive end of the previous implementation is not the standard form: the range 0-1000 has 1001 bytes
	QTest::newRow("range with the exclusive end") << QByteArray("bytes 0-1000/5000") << qint64(1000);

	QTest::newRow("range is out of the file") << QByteArray("bytes 4500-5000/5000") << qint64(501);
	QTest::newRow("first is greater than last") << QByteArray("bytes 999-0/5000") << qint64(1);

	// The file size is required:
	QTest::newRow("unknown file size") << QByteArray("bytes 0-999/*") << qint64(1000);
	QTest::newRow("unsatisfied range") << QByteArray("bytes */5000") << qint64(0);
}


void CHttpFileUploadServletTest::GetChunkRangeInvalidTest()
{
	QFETCH(QByteArray, contentRange);
	QFETCH(qint64, chunkSize);

	imtservergql::IFileUploadHandler::ByteRange range;
	qint64 fileSize = -1;

	QVERIFY2(!imtrest::CHttpFileUploadServletComp::GetChunkRange(contentRange, chunkSize, range, fileSize), contentRange.constData());
}


void CHttpFileUploadServletTest::GetChunkRangeKeepsResultOnErrorTest()
{
	imtservergql::IFileUploadHandler::ByteRange range(11, 22);
	qint64 fileSize = 33;

	QVERIFY(!imtrest::CHttpFileUploadServletComp::GetChunkRange("bytes 0-999/5000", 999, range, fileSize));

	QCOMPARE(range.GetMinValue(), qint64(11));
	QCOMPARE(range.GetMaxValue(), qint64(22));
	QCOMPARE(fileSize, qint64(33));
}


I_ADD_TEST(CHttpFileUploadServletTest);


