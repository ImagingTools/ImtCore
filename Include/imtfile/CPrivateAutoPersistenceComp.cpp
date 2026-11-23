// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtfile/CPrivateAutoPersistenceComp.h>


// Qt includes
#include <QtCore/QFileInfo>
#include <QtCore/QThread>

// ImtCore includes
#include <imtfile/CFileTools.h>


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

		// The watcher is not thread safe: the path can be changed in another thread
		if (*m_reloadOnFileChangeAttrPtr){
			QMetaObject::invokeMethod(&m_fileWatcher, [this](){ WatchFile(); }, Qt::AutoConnection);
		}

		return;
	}

	// The object that is being loaded from the file is not stored back into it. The change of the whole object (CF_ALL_DATA) contains
	// every flag for Contains: only the explicit flag counts
	if (changeSet.ContainsExplicit(istd::IChangeable::CF_NO_UNDO) || (m_loadingThreadId == QThread::currentThreadId())){
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

	// Taken before the load: if the file is changed meanwhile, the next change of it loads it once more
	const QByteArray fileHash = CFileTools::GetFileDigest(filePath);

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
	if (!m_objectCompPtr.IsValid() || !m_fileLoaderCompPtr.IsValid() || !m_savedFileDigestProviderCompPtr.IsValid()){
		return false;
	}

	QMutexLocker locker(&m_fileMutex);

	const QString filePath = GetFilePath();
	if (m_fileLoaderCompPtr->SaveToFile(*m_objectCompPtr, filePath) != ifile::IFilePersistence::OS_OK){
		SendErrorMessage(0, QStringLiteral("The data are not saved to the file '%1'").arg(filePath));

		return false;
	}

	// The file read back could be of another program already: the digest of what has been written is told
	m_fileHash = m_savedFileDigestProviderCompPtr->GetSavedFileDigest(filePath);

	if (*m_reloadOnFileChangeAttrPtr){
		WatchCreatedFile();
	}

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

	// A special file (a FIFO) is not watched: the watcher could wait for a program that writes into it. Its folder tells the changes
	const QFileInfo fileInfo(filePath);
	if (fileInfo.isFile()){
		m_fileWatcher.addPath(fileInfo.absoluteFilePath());
	}

	// Until the folder exists (it is not created yet or it has been deleted), its nearest existing ancestor tells about its creation
	QString folderPath = fileInfo.absolutePath();
	while (!QFileInfo(folderPath).isDir()){
		const QString parentPath = QFileInfo(folderPath).absolutePath();
		if (parentPath == folderPath){
			return;
		}

		folderPath = parentPath;
	}

	m_fileWatcher.addPath(folderPath);
}


void CPrivateAutoPersistenceComp::WatchCreatedFile()
{
	QMetaObject::invokeMethod(
				&m_fileWatcher,
				[this](){
					// The folder of the file that is watched already tells about the new file itself, an ancestor of the folder does not
					const QString filePath = GetFilePath();
					if (!filePath.isEmpty() && !m_fileWatcher.directories().contains(QFileInfo(filePath).absolutePath())){
						WatchFile();
					}
				},
				Qt::AutoConnection);
}


void CPrivateAutoPersistenceComp::OnFileSystemChanged()
{
	WatchFile();

	const QString filePath = GetFilePath();
	if (!QFileInfo::exists(filePath)){
		return;
	}

	// The own saves and the other files of the folder change nothing. A special file (a FIFO) is not waited for, the loader refuses it
	{
		QMutexLocker locker(&m_fileMutex);

		if (CFileTools::GetFileDigest(filePath) == m_fileHash){
			return;
		}
	}

	LoadObject();
}


} // namespace imtfile


