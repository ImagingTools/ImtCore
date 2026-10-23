// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// STL includes
#include <memory>

// Qt includes
#include <QtCore/QString>
#include <QtCore/QTemporaryFile>

// ImtCore includes
#include <imtrest/IHttpRequestBodySink.h>


namespace imtrest
{


/**
	Receiver of the body of an HTTP request that writes it to a new temporary file with a unique name, so the body is not kept
	in the memory. In contrast to CHttpFileBodySink the name of the file is not known in advance: the handler of the request reads
	it from GetFilePath() and either processes the body from the file or takes the file with TakeFile().

	The name of the file is unique, a receiver never writes to a file of another one. It is made from the template "imtrest-body-XXXXXX":
	QTemporaryFile replaces "XXXXXX" with random characters and creates the file only if there is no file with such a name (it never opens an
	existing file), so any number of the receivers that work at the same time get different files: in one directory, in different
	threads and even in different processes.

	The file belongs to the receiver and is deleted together with it, so a file that nobody has taken is never left on the disk:
	the receiver is released when the request is destroyed and the handler does not keep it. If the body is not received to the end
	(Abort()), the file is deleted at once. If the handler wants to keep the file, it takes it (TakeFile()): from then on the file
	belongs to the one who has taken it and the receiver does not touch it.
	On Unix the file is created with the rights of its owner only.

	The receiver is used by one connection and is not thread-safe, the result is read after the request is handled.
*/
class CHttpTemporaryFileBodySink: virtual public IHttpRequestBodySink
{
public:
	/**
		\param	directory	Directory of the file. The directory of the files of the system if it is empty.
							The directory should exist.
	*/
	explicit CHttpTemporaryFileBodySink(const QString& directory);

	/**
		Create the file. It should be called before the first Write(): the policy of the body that creates the receiver calls it and
		refuses the request if the file cannot be created (for example the directory does not exist or there are no rights), so the
		client does not send the body that cannot be saved. The repeated call does nothing.
		\return \c false if the file cannot be created, or the receiver is aborted, or the file is taken, see GetErrorString().
	*/
	bool Open();

	/**
		Get the path of the file. It is empty until the file is created (Open()), after Abort() and after the file is taken (TakeFile()).
	*/
	QString GetFilePath() const;

	/**
		Get the number of the bytes that are written to the file.
	*/
	qint64 GetReceivedSize() const;

	/**
		Returns \c true if the body is received completely and the file is closed. It stays \c true after the file is taken.
	*/
	bool IsFinished() const;

	/**
		Get the description of the last failure.
	*/
	QString GetErrorString() const;

	/**
		Give the file away: the receiver does not own it any more and does not touch it, so it is not deleted with the receiver, and its
		new owner is responsible for it. After the call GetFilePath() is empty and the receiver cannot be used any more.
		\return The file with the received body, or \c nullptr if the body is not received completely (see IsFinished()), if the file is
				deleted by Abort() or if it is taken already: the file of a body that is not received is never given away.
		\warning	The call does not change the state of the returned QTemporaryFile: it is the same object that the receiver has written
					and closed in Finish(), so it is still closed and it still removes the file when it is destroyed (see
					QTemporaryFile::autoRemove()). What the new owner does with the file is its own responsibility.
	*/
	std::unique_ptr<QTemporaryFile> TakeFile();

	// reimplemented (IHttpRequestBodySink)
	virtual bool Write(const char* data, qint64 size) override;
	virtual bool Finish() override;
	virtual void Abort() override;

private:
	QString m_fileTemplate;
	std::unique_ptr<QTemporaryFile> m_filePtr;
	qint64 m_receivedSize = 0;
	bool m_isFinished = false;
	bool m_isAborted = false;
	QString m_errorString;
};


} // namespace imtrest


