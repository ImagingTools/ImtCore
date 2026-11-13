// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtrest/CFileRangeBodySource.h>


// Qt includes
#include <QtCore/QFile>


namespace imtrest
{


// public static methods

IResponseBodySourcePtr CFileRangeBodySource::Create(std::unique_ptr<QIODevice> devicePtr, qint64 offset, qint64 length)
{
	if (!devicePtr || !devicePtr->isOpen() || !devicePtr->isReadable() || devicePtr->isSequential()){
		return nullptr;
	}

	if ((offset < 0) || (length < 0)){
		return nullptr;
	}

	// The size is checked here, so a file that became shorter does not break the response in the middle: the head
	// has the length already
	const qint64 deviceSize = devicePtr->size();
	if ((offset > deviceSize) || (length > deviceSize - offset)){
		return nullptr;
	}

	if (!devicePtr->seek(offset)){
		return nullptr;
	}

	return IResponseBodySourcePtr(new CFileRangeBodySource(std::move(devicePtr), length));
}


IResponseBodySourcePtr CFileRangeBodySource::CreateForFile(const QString& filePath, qint64 offset, qint64 length)
{
	std::unique_ptr<QFile> filePtr(new QFile(filePath));
	if (!filePtr->open(QIODevice::ReadOnly)){
		return nullptr;
	}

	return Create(std::move(filePtr), offset, length);
}


// reimplemented (imtrest::IResponseBodySource)

IResponseBodySource::ReadResult CFileRangeBodySource::Read(QByteArray& data, qint64 maxSize)
{
	data.clear();

	if (!m_devicePtr || (maxSize <= 0)){
		return RR_ERROR;
	}

	if (m_remainingLength == 0){
		m_devicePtr.reset();

		return RR_END;
	}

	data.resize(qsizetype(qMin(maxSize, m_remainingLength)));

	const qint64 readCount = m_devicePtr->read(data.data(), data.size());
	if (readCount <= 0){
		// The file became shorter or cannot be read: the length of the response is already announced
		data.clear();

		return RR_ERROR;
	}

	data.resize(qsizetype(readCount));
	m_remainingLength -= readCount;

	return RR_DATA;
}


void CFileRangeBodySource::SetDataAvailableHandler(const DataAvailableHandler& /*handler*/)
{
	// A file is always ready: the source never waits
}


void CFileRangeBodySource::Abort()
{
	m_devicePtr.reset();
}


// private methods

CFileRangeBodySource::CFileRangeBodySource(std::unique_ptr<QIODevice> devicePtr, qint64 length)
	:m_devicePtr(std::move(devicePtr)),
	m_remainingLength(length)
{
}


} // namespace imtrest


