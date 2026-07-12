// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QString>
#include <QtCore/QDateTime>
#include <QtCore/QList>

// ACF includes
#include <istd/IPolymorphic.h>


namespace imtfile
{


/**
	Interface for providing the folder hierarchy of a file system.
 */
class IFileSystemStructureProvider: virtual public istd::IPolymorphic
{
public:
	struct FileSystemEntry
	{
		QString name;
		QString path;
		bool isDir = false;
		qint64 size = 0;
		QDateTime lastModified;
	};

	typedef QList<FileSystemEntry> FileSystemEntryList;

	/**
		\brief provides the list of file system entries for the given folder
		\param path [INPUT] - path of the folder, empty for the root folder
		\param entries [OUTPUT] - list of the file system entries in the folder
		\param resolvedPath [OUTPUT] - normalized path of the browsed folder
		\param parentPath [OUTPUT] - path of the parent folder, empty if the folder is the root
		\param errorMessage [OUTPUT] - error description if the operation fails
		\return \c true if the entries could be provided
	*/
	virtual bool GetFileSystemEntries(
				const QString& path,
				FileSystemEntryList& entries,
				QString& resolvedPath,
				QString& parentPath,
				QString& errorMessage) const = 0;
};


} // namespace imtfile
