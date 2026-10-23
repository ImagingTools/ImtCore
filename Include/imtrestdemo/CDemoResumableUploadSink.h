// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QFile>
#include <QtCore/QString>

// ImtCore includes
#include <imtrest/IHttpRequestBodySink.h>


namespace imtrestdemo
{


/**
	Receiver of a part of an upload that can be resumed. The client sends the file with \c Content-Range: bytes first-last/total (RFC 9110, section 14.4),
	the first part or the rest of a file that was not received to the end. The data are appended to the file \c name.uploading of the directory of the
	uploads, the total size is in the file \c name.uploading.json. Unlike CHttpFileBodySink the received data are not deleted when the connection breaks:
	they stay, so the client asks how much the server has (see ReadState()) and sends the rest. The part must start where the data end (\c first is
	the size of the file), otherwise the part is refused. When the last byte is received, the file is put in the place of the target file with the
	same name and the temporary files are deleted.
	Only one request can write the same file at the same time.
*/
class CDemoResumableUploadSink: virtual public imtrest::IHttpRequestBodySink
{
public:
	static const char* const s_dataSuffix;
	static const char* const s_metaSuffix;

	/**
		The state of an upload that is not finished.
	*/
	struct SState
	{
		bool exists = false;
		qint64 received = 0;
		qint64 total = -1;

		/**
			A request writes the file now.
		*/
		bool isInProgress = false;
	};

	enum OpenResult
	{
		OR_OK,
		OR_OFFSET_MISMATCH,
		OR_TOTAL_MISMATCH,
		OR_IN_PROGRESS,
		OR_FAILED
	};

	/**
		\param offset	Position of the first byte of the part in the file.
		\param total	Size of the whole file.
	*/
	CDemoResumableUploadSink(const QString& uploadDirectory, const QString& name, qint64 offset, qint64 total);
	~CDemoResumableUploadSink();

	/**
		Get the state of the upload that is not finished (the files with the suffixes are looked at).
	*/
	static SState ReadState(const QString& uploadDirectory, const QString& name);

	/**
		Remove the temporary files of the upload. \return \c true if there were files.
	*/
	static bool RemoveUpload(const QString& uploadDirectory, const QString& name);

	/**
		Check that the part can be written and open the file. It is called by the policy before the body: the part that cannot be
		accepted is refused and the client does not send it. \c state is the state before the part.
	*/
	OpenResult Open(SState& state);

	QString GetFilePath() const;
	QString GetErrorString() const;

	/**
		Size of the file after the part: all bytes of the file when the upload is complete.
	*/
	qint64 GetFileSize() const;
	qint64 GetTotal() const;

	/**
		The last byte of the file is received and the file is in its place.
	*/
	bool IsComplete() const;

	/**
		SHA-256 of the whole file in the hexadecimal form, valid when the upload is complete.
	*/
	QByteArray GetSha256() const;

	// reimplemented (imtrest::IHttpRequestBodySink)
	virtual bool Write(const char* data, qint64 size) override;
	virtual bool Finish() override;
	virtual void Abort() override;

private:
	void Release();

private:
	QString m_uploadDirectory;
	QString m_name;
	qint64 m_offset;
	qint64 m_total;
	qint64 m_receivedSize = 0;
	QFile m_file;
	bool m_isLocked = false;
	bool m_isComplete = false;
	QByteArray m_sha256;
	QString m_errorString;
};


} // namespace imtrestdemo

