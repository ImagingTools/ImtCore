// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QIODevice>


namespace imtrestdemo
{


/**
	Device that gives the bytes of DemoProtocol::GetPatternByte() without any storage: a body of a gigabyte is sent with a few
	kilobytes of memory. It is used by the demo client as the body of an upload.
	A device that is not sequential has the size and the position, so a client library announces \c Content-Length;
	a sequential device has no size known to the library, so the body is sent with \c Transfer-Encoding: chunked.
*/
class CPatternDevice: public QIODevice
{
	Q_OBJECT
public:
	/**
		\param	size			Number of the bytes of the body.
		\param	startOffset		Position in the pattern of the first byte (for a part of a big body).
		\param	isSequential	Do not give the size and the position to the library.
	*/
	CPatternDevice(qint64 size, qint64 startOffset, bool isSequential, QObject* parentPtr = nullptr);

	// reimplemented (QIODevice)
	virtual bool isSequential() const override;
	virtual qint64 size() const override;
	virtual bool seek(qint64 position) override;
	virtual qint64 bytesAvailable() const override;

protected:
	// reimplemented (QIODevice)
	virtual qint64 readData(char* data, qint64 maxSize) override;
	virtual qint64 writeData(const char* data, qint64 size) override;

private:
	qint64 m_size;
	qint64 m_startOffset;
	bool m_isSequential;
	qint64 m_position = 0;
};


} // namespace imtrestdemo


