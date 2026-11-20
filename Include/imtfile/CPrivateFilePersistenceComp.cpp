// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtfile/CPrivateFilePersistenceComp.h>


// Qt includes
#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QFileInfo>


namespace
{


const QFileDevice::Permissions s_ownerFilePermissions = QFileDevice::ReadOwner | QFileDevice::WriteOwner;


#ifndef Q_OS_WIN
const QFileDevice::Permissions s_sharedPermissions =
			QFileDevice::ReadGroup | QFileDevice::WriteGroup | QFileDevice::ExeGroup |
			QFileDevice::ReadOther | QFileDevice::WriteOther | QFileDevice::ExeOther;
#endif


/**
	Create a new empty file that only its owner can read and write. On Windows the permissions become the access list of the file.
*/
bool CreatePrivateFile(const QString& filePath)
{
	QFile file(filePath);
#if QT_VERSION >= QT_VERSION_CHECK(6, 3, 0)
	if (!file.open(QIODevice::WriteOnly | QIODevice::NewOnly, s_ownerFilePermissions)){
		return false;
	}
#else
	if (!file.open(QIODevice::WriteOnly | QIODevice::NewOnly) || !file.setPermissions(s_ownerFilePermissions)){
		return false;
	}
#endif

	file.close();

	return true;
}


} // namespace


namespace imtfile
{


// public static methods

bool CPrivateFilePersistenceComp::EnsurePrivateFolder(const QString& folderPath, QString& errorMessage)
{
	const QFileInfo folderInfo(folderPath);
	if (!folderInfo.exists()){
		const QString parentPath = folderInfo.absolutePath();
		if (!QDir().mkpath(parentPath)){
			errorMessage = QStringLiteral("The folder '%1' cannot be created").arg(parentPath);

			return false;
		}

		const QFileDevice::Permissions ownerFolderPermissions = QFileDevice::ReadOwner | QFileDevice::WriteOwner | QFileDevice::ExeOwner;
#if QT_VERSION >= QT_VERSION_CHECK(6, 3, 0)
		// On Windows the permissions become the access list of the folder
		const bool isCreated = QDir().mkdir(folderInfo.absoluteFilePath(), ownerFolderPermissions);
#else
		const bool isCreated = QDir().mkdir(folderInfo.absoluteFilePath()) && QFile::setPermissions(folderInfo.absoluteFilePath(), ownerFolderPermissions);
#endif
		if (!isCreated){
			errorMessage = QStringLiteral("The folder '%1' cannot be created for its owner only").arg(folderInfo.absoluteFilePath());

			return false;
		}

		return true;
	}

	if (!folderInfo.isDir()){
		errorMessage = QStringLiteral("'%1' is not a folder").arg(folderInfo.absoluteFilePath());

		return false;
	}

#ifndef Q_OS_WIN
	if ((folderInfo.permissions() & s_sharedPermissions) != 0){
		errorMessage = QStringLiteral("Other users can open the folder '%1': make it accessible to its owner only (chmod 700)")
					.arg(folderInfo.absoluteFilePath());

		return false;
	}
#endif

	return true;
}


bool CPrivateFilePersistenceComp::IsPrivateFile(const QString& filePath, QString& errorMessage)
{
#ifndef Q_OS_WIN
	if ((QFile::permissions(filePath) & s_sharedPermissions) != 0){
		errorMessage = QStringLiteral("Other users can read the file '%1': make it readable by its owner only (chmod 600)").arg(filePath);

		return false;
	}
#else
	Q_UNUSED(filePath);
	Q_UNUSED(errorMessage);
#endif

	return true;
}


// reimplemented (ifile::IFilePersistence)

bool CPrivateFilePersistenceComp::IsOperationSupported(
			const istd::IChangeable* dataObjectPtr,
			const QString* filePathPtr,
			int flags,
			bool beQuiet) const
{
	return m_slavePersistenceCompPtr.IsValid() && m_slavePersistenceCompPtr->IsOperationSupported(dataObjectPtr, filePathPtr, flags, beQuiet);
}


ifile::IFilePersistence::OperationState CPrivateFilePersistenceComp::LoadFromFile(
			istd::IChangeable& data,
			const QString& filePath,
			ibase::IProgressManager* progressManagerPtr) const
{
	if (!m_slavePersistenceCompPtr.IsValid() || !m_filePathCompPtr.IsValid()){
		return OS_FAILED;
	}

	// The data can be read from a copy of the file (a reload): the file itself and the copy are checked, the file is named
	const QString targetPath = m_filePathCompPtr->GetPath();
	const QString path = filePath.isEmpty() ? targetPath : filePath;
	if (QFileInfo::exists(path)){
		QString errorMessage;
		const bool isPrivate =
					EnsurePrivateFolder(QFileInfo(targetPath).absolutePath(), errorMessage) &&
					(!QFileInfo::exists(targetPath) || IsPrivateFile(targetPath, errorMessage)) &&
					IsPrivateFile(path, errorMessage);
		if (!isPrivate){
			SendErrorMessage(0, QStringLiteral("The file '%1' is not loaded: %2").arg(targetPath, errorMessage));

			return OS_FAILED;
		}
	}

	return m_slavePersistenceCompPtr->LoadFromFile(data, path, progressManagerPtr);
}


ifile::IFilePersistence::OperationState CPrivateFilePersistenceComp::SaveToFile(
			const istd::IChangeable& data,
			const QString& filePath,
			ibase::IProgressManager* progressManagerPtr) const
{
	if (!m_slavePersistenceCompPtr.IsValid() || !m_filePathCompPtr.IsValid()){
		return OS_FAILED;
	}

	QString errorMessage;
	if (!EnsurePrivateFolder(QFileInfo(m_filePathCompPtr->GetPath()).absolutePath(), errorMessage)){
		SendErrorMessage(0, QStringLiteral("The data are not saved: %1").arg(errorMessage));

		return OS_FAILED;
	}

	// The slave writes into a new file of the owner: it truncates the file and keeps its permissions
	const QString path = filePath.isEmpty() ? m_filePathCompPtr->GetPath() : filePath;
	if ((QFileInfo::exists(path) && !QFile::remove(path)) || !CreatePrivateFile(path)){
		SendErrorMessage(0, QStringLiteral("The file '%1' cannot be created for its owner only").arg(path));

		return OS_FAILED;
	}

	const OperationState retVal = m_slavePersistenceCompPtr->SaveToFile(data, path, progressManagerPtr);
	if ((retVal == OS_OK) && !IsPrivateFile(path, errorMessage)){
		QFile::remove(path);

		SendErrorMessage(0, QStringLiteral("The data are not saved: %1").arg(errorMessage));

		return OS_FAILED;
	}

	return retVal;
}


// reimplemented (ifile::IFileTypeInfo)

bool CPrivateFilePersistenceComp::GetFileExtensions(QStringList& result, const istd::IChangeable* dataObjectPtr, int flags, bool doAppend) const
{
	return m_slavePersistenceCompPtr.IsValid() && m_slavePersistenceCompPtr->GetFileExtensions(result, dataObjectPtr, flags, doAppend);
}


QString CPrivateFilePersistenceComp::GetTypeDescription(const QString* extensionPtr) const
{
	return m_slavePersistenceCompPtr.IsValid() ? m_slavePersistenceCompPtr->GetTypeDescription(extensionPtr) : QString();
}


} // namespace imtfile


