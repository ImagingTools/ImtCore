// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include "CFileRangeBodySourceTest.h"


// STL includes
#include <limits>
#include <memory>

// Qt includes
#include <QtCore/QBuffer>
#include <QtCore/QFile>
#include <QtCore/QTemporaryDir>

// ImtCore includes
#include <imtrest/CFileRangeBodySource.h>


namespace
{


typedef imtrest::IResponseBodySource Source;


/**
	Data of the file at the given position: every byte depends on its position, so lost, repeated and moved parts are found.
*/
QByteArray CreatePattern(qint64 offset, qint64 size)
{
	QByteArray retVal(qsizetype(size), Qt::Uninitialized);

	for (qint64 i = 0; i < size; ++i){
		retVal[qsizetype(i)] = char(((offset + i) * 31 + 7) & 0xFF);
	}

	return retVal;
}


/**
	Buffer that tells when it is destroyed: the source owns the device, so it is the moment when the file is closed.
*/
class CTrackedBuffer: public QBuffer
{
public:
	explicit CTrackedBuffer(bool& isDestroyed)
		:m_isDestroyed(isDestroyed)
	{
		m_isDestroyed = false;
	}

	~CTrackedBuffer() override
	{
		m_isDestroyed = true;
	}

private:
	bool& m_isDestroyed;
};


std::unique_ptr<QIODevice> CreateBuffer(const QByteArray& data)
{
	std::unique_ptr<QBuffer> bufferPtr(new QBuffer());
	bufferPtr->setData(data);
	bufferPtr->open(QIODevice::ReadOnly);

	return bufferPtr;
}


/**
	Read the source to the end by the parts of the given size.
	\return The result that has finished the reading: \c RR_END or \c RR_ERROR.
*/
Source::ReadResult ReadAll(Source& source, qint64 partSize, QByteArray& body, qint64* maxPartPtr = nullptr)
{
	body.clear();

	while (true){
		QByteArray part;

		const Source::ReadResult result = source.Read(part, partSize);
		if (result != Source::RR_DATA){
			return result;
		}

		if (maxPartPtr != nullptr){
			*maxPartPtr = qMax(*maxPartPtr, qint64(part.size()));
		}

		if (part.isEmpty() || (part.size() > partSize)){
			// The contract of the source is broken
			return Source::RR_ERROR;
		}

		body.append(part);
	}
}


// A file in a temporary directory
class CFile
{
public:
	bool Create(qint64 size)
	{
		if (!m_dir.isValid()){
			return false;
		}

		m_path = m_dir.filePath(QStringLiteral("data.bin"));

		QFile file(m_path);
		if (!file.open(QIODevice::WriteOnly)){
			return false;
		}

		// By parts, so a large file does not need a large array
		const qint64 partSize = 1024 * 1024;
		for (qint64 position = 0; position < size; position += partSize){
			const QByteArray part = CreatePattern(position, qMin(partSize, size - position));
			if (file.write(part) != part.size()){
				return false;
			}
		}

		return true;
	}

	QString GetPath() const
	{
		return m_path;
	}

private:
	QTemporaryDir m_dir;
	QString m_path;
};


} // namespace


// private slots

void CFileRangeBodySourceTest::RangeTest_data()
{
	QTest::addColumn<qint64>("offset");
	QTest::addColumn<qint64>("length");

	QTest::newRow("whole") << qint64(0) << qint64(1000);
	QTest::newRow("beginning") << qint64(0) << qint64(100);
	QTest::newRow("middle") << qint64(250) << qint64(300);
	QTest::newRow("end") << qint64(900) << qint64(100);
	QTest::newRow("first byte") << qint64(0) << qint64(1);
	QTest::newRow("last byte") << qint64(999) << qint64(1);
	QTest::newRow("single byte in the middle") << qint64(500) << qint64(1);
}


void CFileRangeBodySourceTest::RangeTest()
{
	QFETCH(qint64, offset);
	QFETCH(qint64, length);

	const QByteArray data = CreatePattern(0, 1000);

	imtrest::IResponseBodySourcePtr sourcePtr = imtrest::CFileRangeBodySource::Create(CreateBuffer(data), offset, length);
	QVERIFY(sourcePtr);

	// The parts do not divide the range evenly
	QByteArray body;
	QCOMPARE(ReadAll(*sourcePtr, 77, body), Source::RR_END);

	QCOMPARE(body, data.mid(qsizetype(offset), qsizetype(length)));
}


void CFileRangeBodySourceTest::PartSizeTest()
{
	const QByteArray data = CreatePattern(0, 1000);

	imtrest::IResponseBodySourcePtr sourcePtr = imtrest::CFileRangeBodySource::Create(CreateBuffer(data), 0, 1000);
	QVERIFY(sourcePtr);

	// The source never gives more than it is asked, and does not give less than it can
	QByteArray part;
	QCOMPARE(sourcePtr->Read(part, 300), Source::RR_DATA);
	QCOMPARE(part.size(), 300);

	QCOMPARE(sourcePtr->Read(part, 5000), Source::RR_DATA);
	QCOMPARE(part.size(), 700);

	QCOMPARE(sourcePtr->Read(part, 5000), Source::RR_END);
	QVERIFY(part.isEmpty());
}


void CFileRangeBodySourceTest::EmptyRangeTest()
{
	imtrest::IResponseBodySourcePtr sourcePtr = imtrest::CFileRangeBodySource::Create(CreateBuffer(CreatePattern(0, 10)), 10, 0);
	QVERIFY(sourcePtr);

	QByteArray part("old");
	QCOMPARE(sourcePtr->Read(part, 100), Source::RR_END);
	QVERIFY(part.isEmpty());

	// A file of the size 0
	imtrest::IResponseBodySourcePtr emptySourcePtr = imtrest::CFileRangeBodySource::Create(CreateBuffer(QByteArray()), 0, 0);
	QVERIFY(emptySourcePtr);
	QCOMPARE(emptySourcePtr->Read(part, 100), Source::RR_END);
}


void CFileRangeBodySourceTest::LargeFileTest()
{
	// The file is larger than the parts, and the range is not aligned to them
	const qint64 fileSize = 3 * 1024 * 1024 + 5;

	CFile file;
	QVERIFY(file.Create(fileSize));

	const qint64 offset = 1000003;
	const qint64 length = fileSize - offset;

	imtrest::IResponseBodySourcePtr sourcePtr = imtrest::CFileRangeBodySource::CreateForFile(file.GetPath(), offset, length);
	QVERIFY(sourcePtr);

	const qint64 partSize = 256 * 1024;
	qint64 maxPart = 0;

	QByteArray body;
	QCOMPARE(ReadAll(*sourcePtr, partSize, body, &maxPart), Source::RR_END);

	QCOMPARE(qint64(body.size()), length);
	QVERIFY(maxPart <= partSize);
	QVERIFY(body == CreatePattern(offset, length));
}


void CFileRangeBodySourceTest::ReadsFromDevicePositionTest()
{
	// The device was read before: the range is counted from the beginning of the device
	std::unique_ptr<QIODevice> devicePtr = CreateBuffer(CreatePattern(0, 1000));
	devicePtr->read(500);

	imtrest::IResponseBodySourcePtr sourcePtr = imtrest::CFileRangeBodySource::Create(std::move(devicePtr), 100, 50);
	QVERIFY(sourcePtr);

	QByteArray body;
	QCOMPARE(ReadAll(*sourcePtr, 1000, body), Source::RR_END);
	QCOMPARE(body, CreatePattern(100, 50));
}


void CFileRangeBodySourceTest::CreateRejectsInvalidDeviceTest()
{
	QVERIFY(!imtrest::CFileRangeBodySource::Create(nullptr, 0, 0));

	// Not open
	std::unique_ptr<QBuffer> closedPtr(new QBuffer());
	closedPtr->setData(CreatePattern(0, 10));
	QVERIFY(!imtrest::CFileRangeBodySource::Create(std::move(closedPtr), 0, 10));

	// Cannot be read
	std::unique_ptr<QBuffer> writeOnlyPtr(new QBuffer());
	writeOnlyPtr->open(QIODevice::WriteOnly);
	QVERIFY(!imtrest::CFileRangeBodySource::Create(std::move(writeOnlyPtr), 0, 0));
}


void CFileRangeBodySourceTest::CreateRejectsInvalidRangeTest_data()
{
	QTest::addColumn<qint64>("offset");
	QTest::addColumn<qint64>("length");

	// The size of the data is 1000. The range outside of the file means that the file is shorter than the head of the response says.
	QTest::newRow("negative offset") << qint64(-1) << qint64(10);
	QTest::newRow("negative length") << qint64(0) << qint64(-1);
	QTest::newRow("offset after the end") << qint64(1001) << qint64(0);
	QTest::newRow("length beyond the end") << qint64(0) << qint64(1001);
	QTest::newRow("range beyond the end") << qint64(900) << qint64(101);
	QTest::newRow("the largest length") << qint64(0) << std::numeric_limits<qint64>::max();
	QTest::newRow("sum overflows") << qint64(10) << std::numeric_limits<qint64>::max();
}


void CFileRangeBodySourceTest::CreateRejectsInvalidRangeTest()
{
	QFETCH(qint64, offset);
	QFETCH(qint64, length);

	QVERIFY(!imtrest::CFileRangeBodySource::Create(CreateBuffer(CreatePattern(0, 1000)), offset, length));
}


void CFileRangeBodySourceTest::CreateForFileTest()
{
	QTemporaryDir dir;
	QVERIFY(dir.isValid());

	// No file
	QVERIFY(!imtrest::CFileRangeBodySource::CreateForFile(dir.filePath(QStringLiteral("missing.bin")), 0, 0));

	// A directory is not a file that can be read
	QVERIFY(!imtrest::CFileRangeBodySource::CreateForFile(dir.path(), 0, 0));

	CFile file;
	QVERIFY(file.Create(1000));

	imtrest::IResponseBodySourcePtr sourcePtr = imtrest::CFileRangeBodySource::CreateForFile(file.GetPath(), 10, 20);
	QVERIFY(sourcePtr);

	QByteArray body;
	QCOMPARE(ReadAll(*sourcePtr, 100, body), Source::RR_END);
	QCOMPARE(body, CreatePattern(10, 20));

	// The file is shorter than the range
	QVERIFY(!imtrest::CFileRangeBodySource::CreateForFile(file.GetPath(), 0, 1001));
}


void CFileRangeBodySourceTest::FileBecomesShorterTest()
{
	CFile file;
	QVERIFY(file.Create(1000));

	imtrest::IResponseBodySourcePtr sourcePtr = imtrest::CFileRangeBodySource::CreateForFile(file.GetPath(), 0, 1000);
	QVERIFY(sourcePtr);

	// The file is cut after the head of the response is made: the data are not enough for the announced length
	QFile cutFile(file.GetPath());
	QVERIFY(cutFile.open(QIODevice::ReadWrite));
	QVERIFY(cutFile.resize(100));
	cutFile.close();

	QByteArray body;
	QCOMPARE(ReadAll(*sourcePtr, 50, body), Source::RR_ERROR);

	QVERIFY(body.size() < 1000);
}


void CFileRangeBodySourceTest::NonPositiveMaxSizeTest()
{
	imtrest::IResponseBodySourcePtr sourcePtr = imtrest::CFileRangeBodySource::Create(CreateBuffer(CreatePattern(0, 10)), 0, 10);
	QVERIFY(sourcePtr);

	// The contract says that the size is positive: a mistake is an error, not an endless loop
	QByteArray part;
	QCOMPARE(sourcePtr->Read(part, 0), Source::RR_ERROR);
	QCOMPARE(sourcePtr->Read(part, -5), Source::RR_ERROR);

	// The source is still usable
	QCOMPARE(sourcePtr->Read(part, 10), Source::RR_DATA);
}


void CFileRangeBodySourceTest::AbortReleasesDeviceTest()
{
	bool isDestroyed = false;

	std::unique_ptr<CTrackedBuffer> bufferPtr(new CTrackedBuffer(isDestroyed));
	bufferPtr->setData(CreatePattern(0, 100));
	bufferPtr->open(QIODevice::ReadOnly);

	imtrest::IResponseBodySourcePtr sourcePtr = imtrest::CFileRangeBodySource::Create(std::move(bufferPtr), 0, 100);
	QVERIFY(sourcePtr);

	QByteArray part;
	QCOMPARE(sourcePtr->Read(part, 10), Source::RR_DATA);
	QVERIFY(!isDestroyed);

	// The file is closed at once, not when the source is destroyed
	sourcePtr->Abort();
	QVERIFY(isDestroyed);

	QCOMPARE(sourcePtr->Read(part, 10), Source::RR_ERROR);
	QVERIFY(part.isEmpty());

	// The second abort is harmless
	sourcePtr->Abort();
}


void CFileRangeBodySourceTest::EndReleasesDeviceTest()
{
	bool isDestroyed = false;

	std::unique_ptr<CTrackedBuffer> bufferPtr(new CTrackedBuffer(isDestroyed));
	bufferPtr->setData(CreatePattern(0, 100));
	bufferPtr->open(QIODevice::ReadOnly);

	imtrest::IResponseBodySourcePtr sourcePtr = imtrest::CFileRangeBodySource::Create(std::move(bufferPtr), 0, 100);
	QVERIFY(sourcePtr);

	QByteArray body;
	QCOMPARE(ReadAll(*sourcePtr, 100, body), Source::RR_END);

	// The file is not held after the last byte
	QVERIFY(isDestroyed);
}


void CFileRangeBodySourceTest::DestructorReleasesDeviceTest()
{
	bool isDestroyed = false;

	std::unique_ptr<CTrackedBuffer> bufferPtr(new CTrackedBuffer(isDestroyed));
	bufferPtr->setData(CreatePattern(0, 100));
	bufferPtr->open(QIODevice::ReadOnly);

	imtrest::IResponseBodySourcePtr sourcePtr = imtrest::CFileRangeBodySource::Create(std::move(bufferPtr), 0, 100);
	QVERIFY(sourcePtr);
	QVERIFY(!isDestroyed);

	sourcePtr.reset();
	QVERIFY(isDestroyed);
}


I_ADD_TEST(CFileRangeBodySourceTest);
