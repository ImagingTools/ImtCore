// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtfile/CFileSystemStructureProviderComp.h>

// Qt includes
#include <QtCore/QFileInfo>


namespace imtfile
{


// public methods

// reimplemented (IFileSystemStructureProvider)

bool CFileSystemStructureProviderComp::GetFileSystemEntries(
			const QString& path,
			FileSystemEntryList& entries,
			QString& resolvedPath,
			QString& parentPath,
			QString& errorMessage) const
{
	entries.clear();
	resolvedPath.clear();
	parentPath.clear();

	QString rootPath = GetRootPath();

	QDir rootDir(rootPath);
	if (rootPath.isEmpty() || !rootDir.exists()){
		errorMessage = QStringLiteral("File system root folder does not exist");
		SendErrorMessage(0, errorMessage, "CFileSystemStructureProviderComp");

		return false;
	}

	QString canonicalRootPath = rootDir.canonicalPath();

	QString requestedPath = path.isEmpty() ? canonicalRootPath : path;

	QDir requestedDir(requestedPath);
	if (!requestedDir.exists()){
		errorMessage = QStringLiteral("Folder '%1' does not exist").arg(requestedPath);

		return false;
	}

	QString canonicalPath = requestedDir.canonicalPath();

	// Restrict browsing to the configured root folder (no path traversal outside of it):
	if (	(canonicalPath != canonicalRootPath) &&
			!canonicalPath.startsWith(canonicalRootPath + QLatin1Char('/'))){
		errorMessage = QStringLiteral("Access to folder '%1' is denied").arg(requestedPath);
		SendWarningMessage(0, errorMessage, "CFileSystemStructureProviderComp");

		return false;
	}

	resolvedPath = canonicalPath;

	if (canonicalPath != canonicalRootPath){
		QDir parentDir(canonicalPath);
		if (parentDir.cdUp()){
			parentPath = parentDir.canonicalPath();
		}
	}

	QDir::Filters filters = QDir::Dirs | QDir::NoDotAndDotDot;
	if (*m_showFilesAttrPtr){
		filters |= QDir::Files;
	}
	if (*m_showHiddenAttrPtr){
		filters |= QDir::Hidden;
	}

	QDir dir(canonicalPath);
	QFileInfoList infoList = dir.entryInfoList(filters, QDir::DirsFirst | QDir::Name | QDir::IgnoreCase);

	for (const QFileInfo& fileInfo : infoList){
		FileSystemEntry entry;
		entry.name = fileInfo.fileName();
		entry.path = fileInfo.absoluteFilePath();
		entry.isDir = fileInfo.isDir();
		entry.size = fileInfo.isDir() ? 0 : fileInfo.size();
		entry.lastModified = fileInfo.lastModified();

		entries.append(entry);
	}

	return true;
}


// private methods

QString CFileSystemStructureProviderComp::GetRootPath() const
{
	return QDir(*m_rootPathAttrPtr).absolutePath();
}


} // namespace imtfile
