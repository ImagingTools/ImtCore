// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QDateTime>
#include <QtCore/QList>

// ACF includes
#include <istd/IPolymorphic.h>


namespace imtfile
{


/**
	Interface for providing the folder hierarchy of a file system (read-only).
 */
class IFileSystemStructureProvider: virtual public istd::IPolymorphic
{
public:
	struct FileSystemEntry
	{
		enum class Type
		{
			File,
			Dir,
			Symlink,
			// A storage volume ("C:/", "/"). Listed only at the virtual drives
			// root of a whole-file-system provider. Navigable like a directory -
			// every Type-based branch that accepts Dir must accept Drive too.
			Drive
		};

		QString name;
		QString path;
		Type type = Type::File;
		qint64 size = 0;
		QDateTime lastModified;
		// Volume capacity, set for Type::Drive only; -1 when not applicable or
		// unavailable. 64-bit on purpose: drives overflow a 32-bit byte count.
		qint64 totalBytes = -1;
		qint64 freeBytes = -1;
	};

	enum class SortBy
	{
		Name,
		Date,
		Size
	};

	struct FileSystemQuery
	{
		QString path;
		int offset = 0;
		int limit = 0; // 0 = no limit
		// Case-insensitive substring on entry name; empty = no name filter.
		// Applied before sort and pagination (totalCount/hasMore reflect it).
		QString nameFilter;
		// Case-insensitive whitelist of file extensions (without the dot, e.g.
		// "exe"); empty = all files. Applies to files only - containers (dir /
		// symlink / drive) always pass so navigation is never blocked. Applied
		// before sort and pagination (totalCount/hasMore reflect it).
		QStringList extensions;
		SortBy sortBy = SortBy::Name;
		bool sortAscending = true;
	};

	struct FileSystemListing
	{
		// Empty for the virtual drives root of a whole-file-system provider.
		QString resolvedPath;
		// Whether a parent exists at all. Kept separate from parentPath because
		// an empty parentPath is itself meaningful: it addresses the virtual
		// drives root (the parent of "C:/"), which must stay distinguishable
		// from "there is no parent" (the drives root itself, or a sandbox root).
		bool hasParent = false;
		QString parentPath;
		QList<FileSystemEntry> entries;
		int totalCount = 0;
		bool hasMore = false;
	};

	/**
		\brief Lists file system entries for the given folder.
		\param query [INPUT] - folder path and optional pagination
		\param result [OUTPUT] - listing result
		\param error [OUTPUT] - error description if the operation fails
		\return \c true if the listing could be provided
	*/
	virtual bool GetEntries(
				const FileSystemQuery& query,
				FileSystemListing& result,
				QString& error) const = 0;

	/**
		\brief Returns metadata for a single path inside the authorized root.
		\param path [INPUT] - path to inspect
		\param entry [OUTPUT] - entry metadata
		\param error [OUTPUT] - error description if the operation fails
		\return \c true if the path exists and is accessible
	*/
	virtual bool Stat(
				const QString& path,
				FileSystemEntry& entry,
				QString& error) const = 0;
};


} // namespace imtfile
