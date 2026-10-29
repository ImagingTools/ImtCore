// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QSaveFile>
#include <QtCore/QString>

// ImtCore includes
#include <imtrest/IHttpRequestBodySink.h>


namespace imtrest
{


/**
	Receiver of the body of an HTTP request that writes it to a file, so the body is not kept in the memory.
	The body is written to a temporary file in the directory of the target file (QSaveFile). The target file appears only when
	the body is received completely (Finish()): it is replaced atomically, the old file stays untouched while the new
	body is being received and if the receiving fails. If the body is not received to the end (Abort() or the destruction of
	the receiver), the temporary file is deleted.
	The receiver is used by one connection and is not thread-safe, the result is read after the request is handled.
*/
class CHttpFileBodySink: virtual public IHttpRequestBodySink
{
public:
	/**
		\param	filePath	Path of the target file. The directory of the file should exist.
	*/
	explicit CHttpFileBodySink(const QString& filePath);

	/**
		Create the temporary file. It should be called before the first Write(): the policy of the body that creates the
		receiver calls it and refuses the request if the file cannot be created (for example the directory does not exist
		or there are no rights), so the client does not send the body that cannot be saved. The repeated call does nothing.
		\return \c false if the file cannot be created, see GetErrorString().
	*/
	bool Open();

	QString GetFilePath() const;

	/**
		Get the number of the bytes that are written to the file.
	*/
	qint64 GetReceivedSize() const;

	/**
		Returns \c true if the body is received completely and the target file is written.
	*/
	bool IsFinished() const;

	/**
		Get the description of the last failure.
	*/
	QString GetErrorString() const;

	// reimplemented (IHttpRequestBodySink)
	virtual bool Write(const char* data, qint64 size) override;
	virtual bool Finish() override;
	virtual void Abort() override;

private:
	QSaveFile m_file;
	qint64 m_receivedSize = 0;
	bool m_isFinished = false;
	QString m_errorString;
};


} // namespace imtrest


