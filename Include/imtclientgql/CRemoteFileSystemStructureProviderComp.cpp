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
#include <imtclientgql/TClientRequestManagerCompWrap.h>


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


// reimplemented (imtfile::IFileDownloadLinkProvider)

bool CRemoteFileSystemStructureProviderComp::GetDownloadLink(
			const QString& path,
			DownloadLink& link,
			QString& error) const
{
	link = DownloadLink();
	error.clear();

	sdl::V1_0::imtbase::GetDownloadLinkRequestArguments arguments;
	arguments.input.emplace();
	arguments.input->path = path;

	imtgql::CGqlRequest gqlRequest;
	if (!sdl::V1_0::imtbase::CGetDownloadLinkGqlRequest::SetupGqlRequest(gqlRequest, arguments)){
		error = QStringLiteral("Unable to build GetDownloadLink request");
		SendErrorMessage(0, error, "CRemoteFileSystemStructureProviderComp");

		return false;
	}

	// Every field of the link; the names come from the SDL types
	/// \todo Inspect this problem and use the SDL types, instead of manually specifying the fields(GQL).
	using PayloadFields = sdl::V1_0::imtbase::CGetDownloadLinkPayload::GetDownloadLinkPayloadFields;
	using DownloadLinkFields = sdl::V1_0::imtbase::CDownloadLink::DownloadLinkFields;
	imtgql::CGqlFieldObject linkFields;
	linkFields.InsertField(DownloadLinkFields::Url.toUtf8());
	linkFields.InsertField(DownloadLinkFields::AlternativeUrls.toUtf8());
	linkFields.InsertField(DownloadLinkFields::ExpiresAt.toUtf8());
	linkFields.InsertField(DownloadLinkFields::Size.toUtf8());
	linkFields.InsertField(DownloadLinkFields::Sha256.toUtf8());
	linkFields.InsertField(DownloadLinkFields::Etag.toUtf8());
	gqlRequest.AddField(PayloadFields::Link.toUtf8(), linkFields);

	QJsonObject commandData;
	if (!SendRequest(gqlRequest, commandData, error)){
		return false;
	}

	sdl::V1_0::imtbase::CGetDownloadLinkPayload payload;
	if (!payload.ReadFromJsonObject(commandData) || !payload.link.has_value()){
		error = QStringLiteral("Unable to parse remote GetDownloadLink payload");

		return false;
	}

	const sdl::V1_0::imtbase::CDownloadLink& sdlLink = *payload.link;
	if (sdlLink.url.has_value()){
		link.url = *sdlLink.url;
	}
	if (sdlLink.alternativeUrls.has_value()){
		link.alternativeUrls = sdlLink.alternativeUrls->ToList();
	}
	if (sdlLink.expiresAt.has_value()){
		link.expiresAt = QDateTime::fromString(*sdlLink.expiresAt, Qt::ISODate);
	}
	if (sdlLink.size.has_value()){
		link.size = *sdlLink.size;
	}
	if (sdlLink.sha256.has_value()){
		link.sha256 = *sdlLink.sha256;
	}
	if (sdlLink.etag.has_value()){
		link.etag = *sdlLink.etag;
	}

	return true;
}


// private methods

bool CRemoteFileSystemStructureProviderComp::SendGetEntries(
			const FileSystemQuery& query,
			FileSystemListing& result,
			QString& error) const
{
	result = FileSystemListing();
	error.clear();

	imtgql::CGqlRequest gqlRequest;

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

	// Ensure selection set includes all payload fields; the names come from the SDL types.
	using PayloadFields = sdl::V1_0::imtbase::CGetFileSystemEntriesPayload::GetFileSystemEntriesPayloadFields;
	using EntryFields = sdl::V1_0::imtbase::CFileSystemEntry::FileSystemEntryFields;
	gqlRequest.AddSimpleField(PayloadFields::Path.toUtf8());
	gqlRequest.AddSimpleField(PayloadFields::ParentPath.toUtf8());
	gqlRequest.AddSimpleField(PayloadFields::TotalCount.toUtf8());
	gqlRequest.AddSimpleField(PayloadFields::HasMore.toUtf8());

	imtgql::CGqlFieldObject entriesFields;
	entriesFields.InsertField(EntryFields::Name.toUtf8());
	entriesFields.InsertField(EntryFields::Path.toUtf8());
	entriesFields.InsertField(EntryFields::EntryType.toUtf8());
	entriesFields.InsertField(EntryFields::Size.toUtf8());
	entriesFields.InsertField(EntryFields::LastModified.toUtf8());
	entriesFields.InsertField(EntryFields::TotalBytes.toUtf8());
	entriesFields.InsertField(EntryFields::FreeBytes.toUtf8());
	gqlRequest.AddField(PayloadFields::Entries.toUtf8(), entriesFields);

	QJsonObject commandData;
	if (!SendRequest(gqlRequest, commandData, error)){
		return false;
	}

	sdl::V1_0::imtbase::CGetFileSystemEntriesPayload payload;
	if (!payload.ReadFromJsonObject(commandData)){
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
				entry.totalBytes = *sdlEntry.totalBytes;
			}
			if (sdlEntry.freeBytes.has_value()){
				entry.freeBytes = *sdlEntry.freeBytes;
			}
			result.entries.append(entry);
		}

		if (!payload.totalCount.has_value()){
			result.totalCount = result.entries.size();
		}
	}

	return true;
}


bool CRemoteFileSystemStructureProviderComp::SendRequest(
			const imtgql::CGqlRequest& gqlRequest,
			QJsonObject& commandData,
			QString& error) const
{
	const QString commandId = QString::fromUtf8(gqlRequest.GetCommandId());

	if (!m_apiClientCompPtr.IsValid()){
		error = QStringLiteral("Unable to send %1 request. Error: Attribute 'ApiClient' was not set").arg(commandId);
		SendErrorMessage(0, error, "CRemoteFileSystemStructureProviderComp");

		return false;
	}

	IGqlClient::GqlRequestPtr requestPtr;
	requestPtr.MoveCastedPtr(gqlRequest.CloneMe());
	if (!requestPtr.IsValid()){
		error = QStringLiteral("Unable to clone %1 request").arg(commandId);
		SendErrorMessage(0, error, "CRemoteFileSystemStructureProviderComp");

		return false;
	}

	// Attach current request context when available (auth headers, clientid, etc.).
	CClientRequestModelHelpers::AttachMissingContext(requestPtr);

	imtbase::IUrlParam* urlParamPtr = nullptr;
	if (m_urlParamCompPtr.IsValid()){
		urlParamPtr = m_urlParamCompPtr.GetPtr();
	}

	const IGqlClient::GqlResult requestResult = m_apiClientCompPtr->SendRequest(requestPtr, urlParamPtr);
	if (!requestResult){
		error = requestResult.errorMessage;
		SendErrorMessage(0, error, "CRemoteFileSystemStructureProviderComp");

		return false;
	}

	const QJsonDocument document = QJsonDocument::fromJson(requestResult.responsePtr->GetResponseData());
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
		const QJsonValue commandValue = object.value(commandId);
		if (commandValue.isObject()){
			object = commandValue.toObject();
		}
	}

	commandData = object;

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
