// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtrest/CHttpTemporaryFileBodySink.h>


// Qt includes
#include <QtCore/QDir>


namespace imtrest
{


namespace
{


// QTemporaryFile replaces "XXXXXX" with random characters and creates the file only if there is no file with such a name, so every
// receiver gets its own file and cannot overwrite the file of another one, whatever number of them work at the same time
const QLatin1String s_fileNameTemplate("temp-file-body-XXXXXX");


} // namespace


// public methods

CHttpTemporaryFileBodySink::CHttpTemporaryFileBodySink(const QString& directory)
	:m_fileTemplate(QDir(directory.isEmpty() ? QDir::tempPath() : directory).absoluteFilePath(s_fileNameTemplate))
{
}


bool CHttpTemporaryFileBodySink::Open()
{
	if (m_isAborted){
		m_errorString = QStringLiteral("The receiver is aborted");

		return false;
	}

	if (m_filePtr != nullptr){
		return true;
	}

	// The receiver that has received the body and has no file has given the file away
	if (m_isFinished){
		m_errorString = QStringLiteral("The file is taken");

		return false;
	}

	std::unique_ptr<QTemporaryFile> filePtr = std::make_unique<QTemporaryFile>(m_fileTemplate);
	if (!filePtr->open()){
		m_errorString = filePtr->errorString();

		return false;
	}

	// On Linux Qt creates the file without a name and names it at the first request of the name. Do it here, so the file is in the directory
	// on every system from the moment it is created and a failure is seen by the policy, and not by the handler that asks for the path
	if (filePtr->fileName().isEmpty()){
		m_errorString = QStringLiteral("The file cannot be named");

		return false;
	}

	m_filePtr = std::move(filePtr);

	return true;
}


QString CHttpTemporaryFileBodySink::GetFilePath() const
{
	return (m_filePtr != nullptr) ? m_filePtr->fileName() : QString();
}


qint64 CHttpTemporaryFileBodySink::GetReceivedSize() const
{
	return m_receivedSize;
}


bool CHttpTemporaryFileBodySink::IsFinished() const
{
	return m_isFinished;
}


QString CHttpTemporaryFileBodySink::GetErrorString() const
{
	return m_errorString;
}


std::unique_ptr<QTemporaryFile> CHttpTemporaryFileBodySink::TakeFile()
{
	if (!m_isFinished){
		return nullptr;
	}

	// It is empty if the file is deleted by Abort() or is taken already
	return std::move(m_filePtr);
}


// reimplemented (IHttpRequestBodySink)

bool CHttpTemporaryFileBodySink::Write(const char* data, qint64 size)
{
	if (m_filePtr == nullptr || !m_filePtr->isOpen()){
		m_errorString = QStringLiteral("The file is not open");

		return false;
	}

	if (m_filePtr->write(data, size) != size){
		m_errorString = m_filePtr->errorString();

		return false;
	}

	m_receivedSize += size;

	return true;
}


bool CHttpTemporaryFileBodySink::Finish()
{
	if (m_filePtr == nullptr || !m_filePtr->isOpen()){
		m_errorString = QStringLiteral("The file is not open");

		return false;
	}

	// The buffered data are written by the closing, so the failure of the writing is seen here and not after the body is reported as received
	m_filePtr->close();
	if (m_filePtr->error() != QFileDevice::NoError){
		m_errorString = m_filePtr->errorString();

		return false;
	}

	m_isFinished = true;

	return true;
}


void CHttpTemporaryFileBodySink::Abort()
{
	m_isAborted = true;

	// The file that is not taken is removed by its destructor
	m_filePtr.reset();
}


} // namespace imtrest
