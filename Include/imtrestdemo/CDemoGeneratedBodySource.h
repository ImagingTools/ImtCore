// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// STL includes
#include <memory>

// ImtCore includes
#include <imtrest/IResponseBodySource.h>


namespace imtrestdemo
{


/**
	Source of the body that is generated, not read from a file: the bytes are given by DemoProtocol::GetPatternByte().
	With a delay between the parts the source works as a slow producer: it answers \c RR_WAIT while it waits and
	then tells the streamer that the data are available, so it shows the waiting for the data of the response.
*/
class CDemoGeneratedBodySource: virtual public imtrest::IResponseBodySource
{
public:
	/**
		\param	totalSize	Number of bytes of the body.
		\param	partSize	Maximal number of bytes that are given by one call, 0 - not limited.
		\param	delayMs		Pause after every part in milliseconds, 0 - no pause.
	*/
	CDemoGeneratedBodySource(qint64 totalSize, qint64 partSize, int delayMs);
	~CDemoGeneratedBodySource();

	// reimplemented (imtrest::IResponseBodySource)
	virtual ReadResult Read(QByteArray& data, qint64 maxSize) override;
	virtual void SetDataAvailableHandler(const DataAvailableHandler& handler) override;
	virtual void Abort() override;

private:
	qint64 m_totalSize;
	qint64 m_partSize;
	int m_delayMs;
	qint64 m_position = 0;
	bool m_isPaused = false;
	bool m_isAborted = false;
	DataAvailableHandler m_handler;

	// The timer can fire after the source is destroyed, the flag tells it
	std::shared_ptr<bool> m_isAliveFlag;
};


} // namespace imtrestdemo


