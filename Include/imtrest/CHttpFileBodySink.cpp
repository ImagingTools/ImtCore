// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtrest/CHttpFileBodySink.h>


namespace imtrest
{


// public methods

CHttpFileBodySink::CHttpFileBodySink(const QString& filePath)
	:m_file(filePath)
{
}


bool CHttpFileBodySink::Open()
{
	if (m_file.isOpen()){
		return true;
	}

	if (!m_file.open(QIODevice::WriteOnly)){
		m_errorString = m_file.errorString();

		return false;
	}

	return true;
}


QString CHttpFileBodySink::GetFilePath() const
{
	return m_file.fileName();
}


qint64 CHttpFileBodySink::GetReceivedSize() const
{
	return m_receivedSize;
}


bool CHttpFileBodySink::IsFinished() const
{
	return m_isFinished;
}


QString CHttpFileBodySink::GetErrorString() const
{
	return m_errorString;
}


// reimplemented (IHttpRequestBodySink)

bool CHttpFileBodySink::Write(const char* data, qint64 size)
{
	if (!m_file.isOpen()){
		m_errorString = QStringLiteral("The file is not open");

		return false;
	}

	if (m_file.write(data, size) != size){
		m_errorString = m_file.errorString();

		return false;
	}

	m_receivedSize += size;

	return true;
}


bool CHttpFileBodySink::Finish()
{
	if (!m_file.isOpen()){
		m_errorString = QStringLiteral("The file is not open");

		return false;
	}

	// Closes the temporary file and puts it in the place of the target file
	if (!m_file.commit()){
		m_errorString = m_file.errorString();

		return false;
	}

	m_isFinished = true;

	return true;
}


void CHttpFileBodySink::Abort()
{
	if (!m_file.isOpen()){
		return;
	}

	// commit() after cancelWriting() deletes the temporary file and leaves the target file as it was
	m_file.cancelWriting();
	m_file.commit();
}


} // namespace imtrest


