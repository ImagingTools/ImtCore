// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtrestdemo/CDemoResumableUploadSink.h>


// Qt includes
#include <QtCore/QCryptographicHash>
#include <QtCore/QDir>
#include <QtCore/QFileInfo>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QMutex>
#include <QtCore/QMutexLocker>
#include <QtCore/QSet>


namespace imtrestdemo
{


const char* const CDemoResumableUploadSink::s_dataSuffix = ".uploading";
const char* const CDemoResumableUploadSink::s_metaSuffix = ".uploading.json";


namespace
{


// The files that a request writes now, one request for a file
QMutex s_activeMutex;
QSet<QString> s_activePaths;


QString GetDataPath(const QString& uploadDirectory, const QString& name)
{
	return QDir(uploadDirectory).absoluteFilePath(name + CDemoResumableUploadSink::s_dataSuffix);
}


QString GetMetaPath(const QString& uploadDirectory, const QString& name)
{
	return QDir(uploadDirectory).absoluteFilePath(name + CDemoResumableUploadSink::s_metaSuffix);
}


qint64 ReadTotal(const QString& metaPath)
{
	QFile file(metaPath);
	if (!file.open(QIODevice::ReadOnly)){
		return -1;
	}

	const QJsonValue total = QJsonDocument::fromJson(file.readAll()).object().value("total");

	return total.isDouble() ? qint64(total.toDouble()) : -1;
}


} // namespace


CDemoResumableUploadSink::CDemoResumableUploadSink(const QString& uploadDirectory, const QString& name, qint64 offset, qint64 total)
	:m_uploadDirectory(uploadDirectory),
	m_name(name),
	m_offset(offset),
	m_total(total),
	m_file(GetDataPath(uploadDirectory, name))
{
}


CDemoResumableUploadSink::~CDemoResumableUploadSink()
{
	Release();
}


// static public methods

CDemoResumableUploadSink::SState CDemoResumableUploadSink::ReadState(const QString& uploadDirectory, const QString& name)
{
	SState state;

	const QString dataPath = GetDataPath(uploadDirectory, name);
	const QFileInfo dataInfo(dataPath);
	if (!dataInfo.exists()){
		return state;
	}

	state.exists = true;
	state.received = dataInfo.size();
	state.total = ReadTotal(GetMetaPath(uploadDirectory, name));

	QMutexLocker locker(&s_activeMutex);
	state.isInProgress = s_activePaths.contains(dataInfo.absoluteFilePath());

	return state;
}


bool CDemoResumableUploadSink::RemoveUpload(const QString& uploadDirectory, const QString& name)
{
	const bool isDataRemoved = QFile::remove(GetDataPath(uploadDirectory, name));
	const bool isMetaRemoved = QFile::remove(GetMetaPath(uploadDirectory, name));

	return isDataRemoved || isMetaRemoved;
}


// public methods

CDemoResumableUploadSink::OpenResult CDemoResumableUploadSink::Open(SState& state)
{
	const QString dataPath = QFileInfo(m_file.fileName()).absoluteFilePath();

	bool isBusy = false;
	{
		QMutexLocker locker(&s_activeMutex);
		isBusy = s_activePaths.contains(dataPath);
		if (!isBusy){
			s_activePaths.insert(dataPath);
			m_isLocked = true;
		}
	}

	// ReadState() takes the same mutex
	if (isBusy){
		state = ReadState(m_uploadDirectory, m_name);
		state.isInProgress = true;

		return OR_IN_PROGRESS;
	}

	state = ReadState(m_uploadDirectory, m_name);

	if (state.exists && state.total >= 0 && state.total != m_total){
		Release();

		return OR_TOTAL_MISMATCH;
	}

	if (state.received != m_offset){
		Release();

		return OR_OFFSET_MISMATCH;
	}

	// Without the size of the file the first part of the upload cannot be continued after a break
	if (!state.exists){
		QFile meta(GetMetaPath(m_uploadDirectory, m_name));
		QJsonObject json;
		json.insert("total", double(m_total));
		if (!meta.open(QIODevice::WriteOnly | QIODevice::Truncate) || meta.write(QJsonDocument(json).toJson(QJsonDocument::Compact)) < 0){
			m_errorString = meta.errorString();
			Release();

			return OR_FAILED;
		}
	}

	// The data go to the disk at once: what the server has received is what stays after a break
	if (!m_file.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Unbuffered)){
		m_errorString = m_file.errorString();
		Release();

		return OR_FAILED;
	}

	return OR_OK;
}


QString CDemoResumableUploadSink::GetFilePath() const
{
	return QDir(m_uploadDirectory).absoluteFilePath(m_name);
}


QString CDemoResumableUploadSink::GetErrorString() const
{
	return m_errorString;
}


qint64 CDemoResumableUploadSink::GetFileSize() const
{
	return m_offset + m_receivedSize;
}


qint64 CDemoResumableUploadSink::GetTotal() const
{
	return m_total;
}


bool CDemoResumableUploadSink::IsComplete() const
{
	return m_isComplete;
}


QByteArray CDemoResumableUploadSink::GetSha256() const
{
	return m_sha256;
}


// reimplemented (imtrest::IHttpRequestBodySink)

bool CDemoResumableUploadSink::Write(const char* data, qint64 size)
{
	if (!m_file.isOpen() || m_file.write(data, size) != size){
		m_errorString = m_file.errorString();

		return false;
	}

	m_receivedSize += size;

	return true;
}


bool CDemoResumableUploadSink::Finish()
{
	m_file.close();

	if (GetFileSize() != m_total){
		// A part: the next one continues the file
		Release();

		return true;
	}

	// The last byte: the file takes its place, the temporary files go away
	const QString targetPath = GetFilePath();
	QFile::remove(targetPath);
	if (!QFile::rename(m_file.fileName(), targetPath)){
		m_errorString = QStringLiteral("The file '%1' cannot be put in its place").arg(targetPath);
		Release();

		return false;
	}

	QFile::remove(GetMetaPath(m_uploadDirectory, m_name));

	QFile target(targetPath);
	QCryptographicHash hash(QCryptographicHash::Sha256);
	if (!target.open(QIODevice::ReadOnly) || !hash.addData(&target)){
		m_errorString = target.errorString();
		Release();

		return false;
	}

	m_sha256 = hash.result().toHex();
	m_isComplete = true;
	Release();

	return true;
}


void CDemoResumableUploadSink::Abort()
{
	// What is received stays in the file, the client resumes from it
	m_file.close();
	Release();
}


// private methods

void CDemoResumableUploadSink::Release()
{
	if (m_isLocked){
		QMutexLocker locker(&s_activeMutex);
		s_activePaths.remove(QFileInfo(m_file.fileName()).absoluteFilePath());
		m_isLocked = false;
	}
}


} // namespace imtrestdemo

