// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtservergql/CFileSystemControllerComp.h>
#include <GeneratedFiles/imtbasesdl/SDL/1.0/CPP/FileSystem.h>

// std includes
#include <limits>


namespace imtservergql
{
namespace
{


sdl::V1_0::imtbase::FileSystemEntryType ToSdlEntryType(imtfile::IFileSystemStructureProvider::FileSystemEntry::Type type)
{
	using DomainType = imtfile::IFileSystemStructureProvider::FileSystemEntry::Type;
	using SdlType = sdl::V1_0::imtbase::FileSystemEntryType;

	switch (type){
	case DomainType::Dir:
		return SdlType::Dir;
	case DomainType::Symlink:
		return SdlType::Symlink;
	case DomainType::Drive:
		return SdlType::Drive;
	case DomainType::File:
	default:
		return SdlType::File;
	}
}


} // namespace


// protected methods

// reimplemented (sdl::V1_0::imtbase::CFileSystemGqlHandlerCompBase)

imtfile::IFileSystemStructureProvider* CFileSystemControllerComp::GetFileSystemProvider() const
{
	return m_fileSystemProviderCompPtr.IsValid() ? m_fileSystemProviderCompPtr.GetPtr() : nullptr;
}


sdl::V1_0::imtbase::CGetFileSystemEntriesPayload CFileSystemControllerComp::OnGetFileSystemEntries(
			const sdl::V1_0::imtbase::CGetFileSystemEntriesGqlRequest& getFileSystemEntriesRequest,
			const ::imtgql::CGqlRequest& /*gqlRequest*/,
			QString& errorMessage) const
{
	sdl::V1_0::imtbase::CGetFileSystemEntriesPayload response;

	imtfile::IFileSystemStructureProvider* providerPtr = GetFileSystemProvider();
	if (providerPtr == nullptr){
		errorMessage = QStringLiteral("Unable to get file system entries. Error: Attribute 'FileSystemProvider' was not set");
		SendErrorMessage(0, errorMessage, "CFileSystemControllerComp");

		return response;
	}

	imtfile::IFileSystemStructureProvider::FileSystemQuery query;

	const sdl::V1_0::imtbase::GetFileSystemEntriesRequestArguments& arguments =
			getFileSystemEntriesRequest.GetRequestedArguments();
	if (arguments.input.has_value()){
		if (arguments.input->path.has_value()){
			query.path = *arguments.input->path;
		}
		if (arguments.input->offset.has_value()){
			query.offset = *arguments.input->offset;
		}
		if (arguments.input->limit.has_value()){
			query.limit = *arguments.input->limit;
		}
		if (arguments.input->nameFilter.has_value()){
			query.nameFilter = *arguments.input->nameFilter;
		}
		if (arguments.input->extensionFilter.has_value()){
			// Wire format is a single ';'-separated string (list inputs are awkward
			// to build from the web client); split into the domain extension list.
			const QStringList parts = arguments.input->extensionFilter->split(
						QLatin1Char(';'), Qt::SkipEmptyParts);
			for (const QString& part : parts){
				const QString trimmed = part.trimmed();
				if (!trimmed.isEmpty()){
					query.extensions.append(trimmed);
				}
			}
		}
		if (arguments.input->sortBy.has_value()){
			switch (*arguments.input->sortBy){
			case sdl::V1_0::imtbase::FileSystemSortBy::Date:
				query.sortBy = imtfile::IFileSystemStructureProvider::SortBy::Date;
				break;
			case sdl::V1_0::imtbase::FileSystemSortBy::Size:
				query.sortBy = imtfile::IFileSystemStructureProvider::SortBy::Size;
				break;
			case sdl::V1_0::imtbase::FileSystemSortBy::Name:
			default:
				query.sortBy = imtfile::IFileSystemStructureProvider::SortBy::Name;
				break;
			}
		}
		if (arguments.input->sortAscending.has_value()){
			query.sortAscending = *arguments.input->sortAscending;
		}
	}

	imtfile::IFileSystemStructureProvider::FileSystemListing listing;
	if (!providerPtr->GetEntries(query, listing, errorMessage)){
		return response;
	}

	response.path = listing.resolvedPath;
	// Presence (not emptiness) carries "a parent exists": an empty parentPath is
	// itself a valid address - the virtual drives root above "C:/".
	if (listing.hasParent){
		response.parentPath = listing.parentPath;
	}
	response.totalCount = listing.totalCount;
	response.hasMore = listing.hasMore;

	// Leave `entries` unset (rather than an empty TElementList) for an empty folder:
	// the generated GQL (de)serializer treats a present-but-empty JSON array for a
	// list field as a parse error on the read side, even though the field is
	// nullable. Omitting the key entirely (unset optional) sidesteps that path.
	if (!listing.entries.isEmpty()){
		QList<sdl::V1_0::imtbase::CFileSystemEntry> entryList;
		for (const imtfile::IFileSystemStructureProvider::FileSystemEntry& domainEntry : listing.entries){
			sdl::V1_0::imtbase::CFileSystemEntry entry;
			entry.name = domainEntry.name;
			entry.path = domainEntry.path;
			entry.entryType = ToSdlEntryType(domainEntry.type);
			// SDL `size: Int` is 32-bit; files larger than INT_MAX are reported as INT_MAX.
			entry.size = static_cast<int>(qMin(domainEntry.size, static_cast<qint64>(std::numeric_limits<int>::max())));
			if (domainEntry.lastModified.isValid()){
				entry.lastModified = domainEntry.lastModified.toString(Qt::ISODate);
			}
			// Drive-only capacity. Double, not the 32-bit `size`, which saturates
			// at 2 GB and cannot express a volume.
			if (domainEntry.totalBytes >= 0){
				entry.totalBytes = static_cast<double>(domainEntry.totalBytes);
			}
			if (domainEntry.freeBytes >= 0){
				entry.freeBytes = static_cast<double>(domainEntry.freeBytes);
			}
			entryList.append(entry);
		}
		imtsdl::TElementList<sdl::V1_0::imtbase::CFileSystemEntry> entries;
		entries.FromList(entryList);
		response.entries = entries;
	}

	return response;
}


} // namespace imtservergql
