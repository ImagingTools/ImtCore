// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtfile/CPrivateAutoPersistenceComp.h>


// Qt includes
#include <QtCore/QCryptographicHash>
#include <QtCore/QFile>
#include <QtCore/QFileInfo>
#include <QtCore/QThread>


namespace
{


/**
	The hash of the contents of the file, empty if it cannot be read.
*/
QByteArray GetFileHash(const QString& filePath)
{
	QFile file(filePath);
	if (!file.open(QIODevice::ReadOnly)){
		return {};
	}

	QCryptographicHash hash(QCryptographicHash::Sha256);
	if (!hash.addData(&file)){
		return {};
	}

	return hash.result();
}


} // namespace


namespace imtfile
{


// public methods

CPrivateAutoPersistenceComp::CPrivateAutoPersistenceComp()
	:m_loadingThreadId(nullptr)
{
}


// protected methods

// reimplemented (icomp::CComponentBase)

void CPrivateAutoPersistenceComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	const QString filePath = GetFilePath();
	if (*m_restoreOnBeginAttrPtr && QFileInfo::exists(filePath)){
		LoadObject();
	}

	if (*m_storeOnBeginAttrPtr && !filePath.isEmpty() && !QFileInfo::exists(filePath)){
		StoreObject();
	}

	if (m_objectModelCompPtr.IsValid()){
		BaseClass2::RegisterModel(m_objectModelCompPtr.GetPtr(), MI_OBJECT);
	}

	if (m_filePathModelCompPtr.IsValid()){
		BaseClass2::RegisterModel(m_filePathModelCompPtr.GetPtr(), MI_FILE_PATH);
	}

	if (*m_reloadOnFileChangeAttrPtr){
		QObject::connect(&m_fileWatcher, &QFileSystemWatcher::fileChanged, &m_fileWatcher, [this](){ OnFileSystemChanged(); });
		QObject::connect(&m_fileWatcher, &QFileSystemWatcher::directoryChanged, &m_fileWatcher, [this](){ OnFileSystemChanged(); });

		WatchFile();
	}
}


void CPrivateAutoPersistenceComp::OnComponentDestroyed()
{
	BaseClass2::UnregisterAllModels();

	m_fileWatcher.disconnect();

	BaseClass::OnComponentDestroyed();
}


// reimplemented (imod::CMultiModelDispatcherBase)

void CPrivateAutoPersistenceComp::OnModelChanged(int modelId, const istd::IChangeable::ChangeSet& changeSet)
{
	if (modelId == MI_FILE_PATH){
		if (*m_restoreOnBeginAttrPtr && QFileInfo::exists(GetFilePath())){
			LoadObject();
		}

		if (*m_reloadOnFileChangeAttrPtr){
			WatchFile();
		}

		return;
	}

	// The object that is being loaded from the file is not stored back into it
	if (changeSet.Contains(istd::IChangeable::CF_NO_UNDO) || (m_loadingThreadId == QThread::currentThreadId())){
		return;
	}

	if (*m_storeOnChangeAttrPtr){
		StoreObject();
	}
}


// private methods

QString CPrivateAutoPersistenceComp::GetFilePath() const
{
	return m_filePathCompPtr.IsValid() ? m_filePathCompPtr->GetPath() : QString();
}


bool CPrivateAutoPersistenceComp::LoadObject()
{
	if (!m_objectCompPtr.IsValid() || !m_fileLoaderCompPtr.IsValid()){
		return false;
	}

	QMutexLocker locker(&m_fileMutex);

	const QString filePath = GetFilePath();
	const QByteArray fileHash = GetFileHash(filePath);

	m_loadingThreadId = QThread::currentThreadId();
	const int operationState = m_fileLoaderCompPtr->LoadFromFile(*m_objectCompPtr, filePath);
	m_loadingThreadId = nullptr;

	if (operationState != ifile::IFilePersistence::OS_OK){
		SendErrorMessage(0, QStringLiteral("The data are not loaded from the file '%1'").arg(filePath));

		return false;
	}

	m_fileHash = fileHash;

	return true;
}


bool CPrivateAutoPersistenceComp::StoreObject()
{
	if (!m_objectCompPtr.IsValid() || !m_fileLoaderCompPtr.IsValid()){
		return false;
	}

	QMutexLocker locker(&m_fileMutex);

	const QString filePath = GetFilePath();
	if (m_fileLoaderCompPtr->SaveToFile(*m_objectCompPtr, filePath) != ifile::IFilePersistence::OS_OK){
		SendErrorMessage(0, QStringLiteral("The data are not saved to the file '%1'").arg(filePath));

		return false;
	}

	m_fileHash = GetFileHash(filePath);

	return true;
}


void CPrivateAutoPersistenceComp::WatchFile()
{
	const QStringList watchedPaths = m_fileWatcher.files() + m_fileWatcher.directories();
	if (!watchedPaths.isEmpty()){
		m_fileWatcher.removePaths(watchedPaths);
	}

	const QString filePath = GetFilePath();
	if (filePath.isEmpty()){
		return;
	}

	const QFileInfo fileInfo(filePath);
	if (fileInfo.exists()){
		m_fileWatcher.addPath(fileInfo.absoluteFilePath());
	}

	if (QFileInfo::exists(fileInfo.absolutePath())){
		m_fileWatcher.addPath(fileInfo.absolutePath());
	}
}


void CPrivateAutoPersistenceComp::OnFileSystemChanged()
{
	WatchFile();

	const QString filePath = GetFilePath();
	if (!QFileInfo::exists(filePath)){
		return;
	}

	// The own saves and the other files of the folder change nothing
	{
		QMutexLocker locker(&m_fileMutex);

		if (GetFileHash(filePath) == m_fileHash){
			return;
		}
	}

	LoadObject();
}


} // namespace imtfile


