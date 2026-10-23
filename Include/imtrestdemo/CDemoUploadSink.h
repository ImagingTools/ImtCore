// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QByteArray>
#include <QtCore/QCryptographicHash>

// ImtCore includes
#include <imtrest/CHttpFileBodySink.h>


namespace imtrestdemo
{


/**
	Receiver of an uploaded file: writes the body to the file and calculates SHA-256 of it while the body is received,
	so the servlet can answer with the size and the hash of the file that never was in memory.
*/
class CDemoUploadSink: public imtrest::CHttpFileBodySink
{
public:
	typedef imtrest::CHttpFileBodySink BaseClass;

	explicit CDemoUploadSink(const QString& filePath);

	/**
		Get SHA-256 of the received body in the hexadecimal form. It is valid after the body is received completely.
	*/
	QByteArray GetSha256() const;

	// reimplemented (imtrest::IHttpRequestBodySink)
	virtual bool Write(const char* data, qint64 size) override;
	virtual bool Finish() override;

private:
	QCryptographicHash m_hash;
	QByteArray m_result;
};


} // namespace imtrestdemo


