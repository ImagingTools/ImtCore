// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtclientgql/CRemoteFileSystemStructureProviderComp.h>
#include <GeneratedFiles/imtbasesdl/SDL/1.0/CPP/FileSystem.h>

// Qt includes
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QJsonArray>
#include <QtCore/QDateTime>
#include <QtCore/QFileInfo>

// ImtCore includes
#include <imtgql/CGqlRequest.h>
#include <imtgql/CGqlFieldObject.h>
#include <imtgql/CGqlContext.h>
#include <imtgql/CGqlRequestContextManager.h>


namespace imtclientgql
{


// public methods

// reimplemented (imtfile::IFileSystemStructureProvider)

bool CRemoteFileSystemStructureProviderComp::GetEntries(
			const FileSystemQuery& query,
			FileSystemListing& result,
			QString& error) const
{
	return SendGetEntries(query, result, error);
}


bool CRemoteFileSystemStructureProviderComp::Stat(
			const QString& path,
			FileSystemEntry& entry,
			QString& error) const
{
	entry = FileSystemEntry();

	// Remote Stat via listing of the parent and match, or GetEntries on the path if it is a dir.
	// Prefer a dedicated lightweight approach: request entries for parent and find the name,
	// or if path is empty - not applicable.
	// For remote FS we re-use GetFileSystemEntries on the parent folder and match by path.
	const QFileInfo pathInfo(path);
	const QString parentPath = pathInfo.absolutePath();
	const QString fileName = pathInfo.fileName();

	FileSystemQuery query;
	query.path = parentPath;
	query.offset = 0;
	query.limit = 0;

	FileSystemListing listing;
	if (!SendGetEntries(query, listing, error)){
		return false;
	}

	for (const FileSystemEntry& candidate : listing.entries){
		if (candidate.path == path || candidate.name == fileName){
			// Prefer exact path match; name match as fallback when path formatting differs.
			if (candidate.path == path || QFileInfo(candidate.path).fileName() == fileName){
				entry = candidate;

				return true;
			}
		}
	}

	// Also allow Stat of the resolved folder itself (path equals listing root).
	if (listing.resolvedPath == path || (path.isEmpty() && !listing.resolvedPath.isEmpty())){
		entry.name = QFileInfo(listing.resolvedPath).fileName();
		entry.path = listing.resolvedPath;
		entry.type = FileSystemEntry::Type::Dir;
		entry.size = 0;

		return true;
	}

	error = QStringLiteral("Path '%1' was not found on the remote file service").arg(path);

	return false;
}


// private methods

bool CRemoteFileSystemStructureProviderComp::SendGetEntries(
			const FileSystemQuery& query,
			FileSystemListing& result,
			QString& error) const
{
	result = FileSystemListing();
	error.clear();

	if (!m_apiClientCompPtr.IsValid()){
		error = QStringLiteral("Unable to get remote file system entries. Error: Attribute 'ApiClient' was not set");
		SendErrorMessage(0, error, "CRemoteFileSystemStructureProviderComp");

		return false;
	}

	imtgql::CGqlRequest gqlRequest;

	// Attach current request context when available (auth headers, clientid, etc.).
	imtgql::IGqlContext* currentContextPtr = imtgql::CGqlRequestContextManager::GetContext();
	if (currentContextPtr != nullptr){
		istd::IChangeableUniquePtr clonedContextPtr = currentContextPtr->CloneMe();
		imtgql::IGqlContextUniquePtr castedContextPtr;
		castedContextPtr.MoveCastedPtr(std::move(clonedContextPtr));
		gqlRequest.SetGqlContext(imtgql::IGqlContextSharedPtr::CreateFromUnique(std::move(castedContextPtr)));
	}

	sdl::V1_0::imtbase::GetFileSystemEntriesRequestArguments arguments;
	arguments.input.emplace();
	arguments.input->path = query.path;
	if (query.offset > 0){
		arguments.input->offset = query.offset;
	}
	if (query.limit > 0){
		arguments.input->limit = query.limit;
	}
	if (!query.nameFilter.isEmpty()){
		arguments.input->nameFilter = query.nameFilter;
	}
	if (!query.extensions.isEmpty()){
		// Forward as the same ';'-separated wire string the controller expects.
		arguments.input->extensionFilter = query.extensions.join(QLatin1Char(';'));
	}
	// Always send sort so the remote side does not fall back to its default while
	// the caller asked for Date/Size descending, etc.
	switch (query.sortBy){
	case imtfile::IFileSystemStructureProvider::SortBy::Date:
		arguments.input->sortBy = sdl::V1_0::imtbase::FileSystemSortBy::Date;
		break;
	case imtfile::IFileSystemStructureProvider::SortBy::Size:
		arguments.input->sortBy = sdl::V1_0::imtbase::FileSystemSortBy::Size;
		break;
	case imtfile::IFileSystemStructureProvider::SortBy::Name:
	default:
		arguments.input->sortBy = sdl::V1_0::imtbase::FileSystemSortBy::Name;
		break;
	}
	arguments.input->sortAscending = query.sortAscending;

	if (!sdl::V1_0::imtbase::CGetFileSystemEntriesGqlRequest::SetupGqlRequest(gqlRequest, arguments)){
		error = QStringLiteral("Unable to build GetFileSystemEntries request");
		SendErrorMessage(0, error, "CRemoteFileSystemStructureProviderComp");

		return false;
	}

	// Ensure selection set includes all payload fields.
	gqlRequest.AddSimpleField(QByteArrayLiteral("path"));
	gqlRequest.AddSimpleField(QByteArrayLiteral("parentPath"));
	gqlRequest.AddSimpleField(QByteArrayLiteral("totalCount"));
	gqlRequest.AddSimpleField(QByteArrayLiteral("hasMore"));

	imtgql::CGqlFieldObject entriesFields;
	entriesFields.InsertField(QByteArrayLiteral("name"));
	entriesFields.InsertField(QByteArrayLiteral("path"));
	entriesFields.InsertField(QByteArrayLiteral("entryType"));
	entriesFields.InsertField(QByteArrayLiteral("size"));
	entriesFields.InsertField(QByteArrayLiteral("lastModified"));
	entriesFields.InsertField(QByteArrayLiteral("totalBytes"));
	entriesFields.InsertField(QByteArrayLiteral("freeBytes"));
	gqlRequest.AddField(QByteArrayLiteral("entries"), entriesFields);

	IGqlClient::GqlRequestPtr requestPtr;
	requestPtr.MoveCastedPtr(gqlRequest.CloneMe());
	if (!requestPtr.IsValid()){
		error = QStringLiteral("Unable to clone GetFileSystemEntries request");
		SendErrorMessage(0, error, "CRemoteFileSystemStructureProviderComp");

		return false;
	}

	imtbase::IUrlParam* urlParamPtr = nullptr;
	if (m_urlParamCompPtr.IsValid()){
		urlParamPtr = m_urlParamCompPtr.GetPtr();
	}

	IGqlClient::GqlResponsePtr responsePtr = m_apiClientCompPtr->SendRequest(requestPtr, urlParamPtr);
	if (!responsePtr.IsValid()){
		error = QStringLiteral("Remote file service is not reachable");
		SendErrorMessage(0, error, "CRemoteFileSystemStructureProviderComp");

		return false;
	}

	const QStringList errorMessages = responsePtr->GetErrorMessageList();
	if (!errorMessages.isEmpty()){
		error = errorMessages.first();

		return false;
	}

	const QJsonDocument document = QJsonDocument::fromJson(responsePtr->GetResponseData());
	if (!document.isObject()){
		error = QStringLiteral("Remote file service returned an invalid response");

		return false;
	}

	QJsonObject object = document.object();

	const QJsonValue payloadValue = object.value(QStringLiteral("payload"));
	if (payloadValue.isObject()){
		object = payloadValue.toObject();
	}

	const QJsonValue errorsValue = object.value(QStringLiteral("errors"));
	if (errorsValue.isArray() && !errorsValue.toArray().isEmpty()){
		const QJsonObject firstError = errorsValue.toArray().at(0).toObject();
		error = firstError.value(QStringLiteral("message")).toString();
		if (error.isEmpty()){
			error = QStringLiteral("Remote file service returned an error");
		}

		return false;
	}

	const QJsonValue dataValue = object.value(QStringLiteral("data"));
	if (dataValue.isObject()){
		object = dataValue.toObject();
		const QJsonValue commandValue = object.value(
					QString::fromUtf8(sdl::V1_0::imtbase::CGetFileSystemEntriesGqlRequest::GetCommandId()));
		if (commandValue.isObject()){
			object = commandValue.toObject();
		}
	}

	sdl::V1_0::imtbase::CGetFileSystemEntriesPayload payload;
	if (!payload.ReadFromJsonObject(object)){
		error = QStringLiteral("Unable to parse remote GetFileSystemEntries payload");

		return false;
	}

	if (payload.path.has_value()){
		result.resolvedPath = *payload.path;
	}
	// Presence, not emptiness, means "a parent exists" - an empty parentPath
	// addresses the virtual drives root that sits above "C:/".
	result.hasParent = payload.parentPath.has_value();
	if (payload.parentPath.has_value()){
		result.parentPath = *payload.parentPath;
	}
	if (payload.totalCount.has_value()){
		result.totalCount = *payload.totalCount;
	}
	if (payload.hasMore.has_value()){
		result.hasMore = *payload.hasMore;
	}

	if (payload.entries.has_value()){
		for (const sdl::V1_0::imtbase::CFileSystemEntry& sdlEntry : payload.entries->ToList()){
			FileSystemEntry entry;
			if (sdlEntry.name.has_value()){
				entry.name = *sdlEntry.name;
			}
			if (sdlEntry.path.has_value()){
				entry.path = *sdlEntry.path;
			}
			if (sdlEntry.entryType.has_value()){
				entry.type = FromSdlEntryType(static_cast<int>(*sdlEntry.entryType));
			}
			if (sdlEntry.size.has_value()){
				entry.size = *sdlEntry.size;
			}
			if (sdlEntry.lastModified.has_value()){
				entry.lastModified = QDateTime::fromString(*sdlEntry.lastModified, Qt::ISODate);
			}
			if (sdlEntry.totalBytes.has_value()){
				entry.totalBytes = static_cast<qint64>(*sdlEntry.totalBytes);
			}
			if (sdlEntry.freeBytes.has_value()){
				entry.freeBytes = static_cast<qint64>(*sdlEntry.freeBytes);
			}
			result.entries.append(entry);
		}

		if (!payload.totalCount.has_value()){
			result.totalCount = result.entries.size();
		}
	}

	return true;
}


imtfile::IFileSystemStructureProvider::FileSystemEntry::Type
CRemoteFileSystemStructureProviderComp::FromSdlEntryType(int sdlEntryType)
{
	using SdlType = sdl::V1_0::imtbase::FileSystemEntryType;
	using DomainType = imtfile::IFileSystemStructureProvider::FileSystemEntry::Type;

	switch (static_cast<SdlType>(sdlEntryType)){
	case SdlType::Dir:
		return DomainType::Dir;
	case SdlType::Symlink:
		return DomainType::Symlink;
	case SdlType::Drive:
		return DomainType::Drive;
	case SdlType::File:
	default:
		return DomainType::File;
	}
}


} // namespace imtclientgql
