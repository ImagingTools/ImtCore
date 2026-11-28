// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtfile/IFileSystemStructureProvider.h>


namespace imtfile
{


/**
	The rules of IFileSystemStructureProvider::FileSystemQuery, common to all the sources of file system listings.
*/
class CFileSystemQueryTools
{
public:
	/**
		Applies a query to the whole content of a folder: the name filter, the extension filter, the sorting
		(containers always above files), then the page (offset and limit).
		\param entries [INPUT] - all the entries of the folder
		\param query [INPUT] - the query; its path is not used
		\param listing [OUTPUT] - gets the entries of the page; totalCount and hasMore describe the filtered entries
	*/
	static void ApplyQuery(
				QList<IFileSystemStructureProvider::FileSystemEntry> entries,
				const IFileSystemStructureProvider::FileSystemQuery& query,
				IFileSystemStructureProvider::FileSystemListing& listing);
};


} // namespace imtfile
