// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtrestdemo/CDemoGeneratedBodySource.h>


// Qt includes
#include <QtCore/QTimer>

// ImtCore includes
#include <imtrestdemo/CDemoProtocol.h>


namespace imtrestdemo
{


CDemoGeneratedBodySource::CDemoGeneratedBodySource(qint64 totalSize, qint64 partSize, int delayMs)
	:m_totalSize(totalSize),
	m_partSize(partSize),
	m_delayMs(delayMs),
	m_isAliveFlag(std::make_shared<bool>(true))
{
}


CDemoGeneratedBodySource::~CDemoGeneratedBodySource()
{
	*m_isAliveFlag = false;
}


// reimplemented (imtrest::IResponseBodySource)

imtrest::IResponseBodySource::ReadResult CDemoGeneratedBodySource::Read(QByteArray& data, qint64 maxSize)
{
	data.clear();

	if (m_isAborted){
		return RR_ERROR;
	}

	if (m_position >= m_totalSize){
		return RR_END;
	}

	if (m_isPaused){
		return RR_WAIT;
	}

	qint64 length = qMin(maxSize, m_totalSize - m_position);
	if (m_partSize > 0){
		length = qMin(length, m_partSize);
	}

	data = DemoProtocol::CreatePatternData(m_position, length);
	m_position += length;

	if (m_delayMs > 0 && m_position < m_totalSize){
		m_isPaused = true;

		std::shared_ptr<bool> isAliveFlag = m_isAliveFlag;
		QTimer::singleShot(m_delayMs, [this, isAliveFlag](){
			if (!*isAliveFlag){
				return;
			}

			m_isPaused = false;

			if (m_handler){
				m_handler();
			}
		});
	}

	return RR_DATA;
}


void CDemoGeneratedBodySource::SetDataAvailableHandler(const DataAvailableHandler& handler)
{
	m_handler = handler;
}


void CDemoGeneratedBodySource::Abort()
{
	m_isAborted = true;
}


} // namespace imtrestdemo
