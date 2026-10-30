// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// STL includes
#include <memory>

// Qt includes
#include <QtCore/QIODevice>
#include <QtCore/QString>

// ImtCore includes
#include <imtrest/IResponseBodySource.h>


namespace imtrest
{


/**
	Source of the body of a response that gives a range of bytes of a file (or of any other seekable device).
	The range is read by parts, so the file is never completely in memory.
	The source owns the device and closes it when the range is read, when the source is aborted and when it is destroyed.
*/
class CFileRangeBodySource: virtual public IResponseBodySource
{
public:
	/**
		Create the source of a range of the device.
		\param	devicePtr	Open readable seekable device. The source takes it.
		\param	offset		Position of the first byte of the range.
		\param	length		Number of bytes of the range, it can be 0.
		\return \c nullptr if the device cannot be used: it is not open, is sequential, is smaller than \c offset + \c length
				(for example, the file was truncated after its size was read) or cannot seek to the position.
	*/
	static IResponseBodySourcePtr Create(std::unique_ptr<QIODevice> devicePtr, qint64 offset, qint64 length);

	/**
		Create the source of a range of a file. The file is opened for reading.
		\return \c nullptr if the file cannot be opened or the range is not inside of it, see Create().
	*/
	static IResponseBodySourcePtr CreateForFile(const QString& filePath, qint64 offset, qint64 length);

	// reimplemented (imtrest::IResponseBodySource)
	virtual ReadResult Read(QByteArray& data, qint64 maxSize) override;
	virtual void SetDataAvailableHandler(const DataAvailableHandler& handler) override;
	virtual void Abort() override;

private:
	CFileRangeBodySource(std::unique_ptr<QIODevice> devicePtr, qint64 length);

private:
	std::unique_ptr<QIODevice> m_devicePtr;
	qint64 m_remainingLength;
};


} // namespace imtrest


