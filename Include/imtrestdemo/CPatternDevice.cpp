// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtrestdemo/CPatternDevice.h>


// ImtCore includes
#include <imtrestdemo/CDemoProtocol.h>


namespace imtrestdemo
{


CPatternDevice::CPatternDevice(qint64 size, qint64 startOffset, bool isSequential, QObject* parentPtr)
	:QIODevice(parentPtr),
	m_size(qMax<qint64>(0, size)),
	m_startOffset(qMax<qint64>(0, startOffset)),
	m_isSequential(isSequential)
{
}


// reimplemented (QIODevice)

bool CPatternDevice::isSequential() const
{
	return m_isSequential;
}


qint64 CPatternDevice::size() const
{
	return m_size;
}


bool CPatternDevice::seek(qint64 position)
{
	if (m_isSequential || position < 0 || position > m_size){
		return false;
	}

	QIODevice::seek(position);
	m_position = position;

	return true;
}


qint64 CPatternDevice::bytesAvailable() const
{
	return QIODevice::bytesAvailable() + (m_size - m_position);
}


// protected methods

qint64 CPatternDevice::readData(char* data, qint64 maxSize)
{
	const qint64 length = qMin(maxSize, m_size - m_position);
	if (length <= 0){
		return -1;
	}

	for (qint64 i = 0; i < length; ++i){
		data[i] = char(DemoProtocol::GetPatternByte(m_startOffset + m_position + i));
	}

	m_position += length;

	return length;
}


qint64 CPatternDevice::writeData(const char* /*data*/, qint64 /*size*/)
{
	return -1;
}


} // namespace imtrestdemo
