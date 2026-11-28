// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtclientgql/CRemoteFileSystemStructureProviderComp.h>
#include <GeneratedFiles/imtbasesdl/SDL/1.0/CPP/FileSystem.h>

// Qt includes
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QJsonArray>
#include <QtCore/QDateTime>

// ImtCore includes
#include <imtgql/CGqlRequest.h>
#include <imtgql/CGqlFieldObject.h>
#include <imtclientgql/TClientRequestManagerCompWrap.h>


namespace imtclientgql
{
namespace
{


// One more attempt after a refused token: the session signs in again for it
constexpr int s_sessionAttemptCount = 2;


// The root of the remote file service has no parent to look it up in
bool IsRootPath(const QString& path)
{
	return path.isEmpty() || path == QStringLiteral("/");
}


QString TrimTrailingSeparators(const QString& path)
{
	QString retVal = path;
	while (retVal.endsWith(QLatin1Char('/'))){
		retVal.chop(1);
	}

	return retVal;
}


/**
	Parent of a path of the remote file service, found by '/' whatever the OS of this side:
	"/A/B" -> "/A", "/A" -> "/", "C:/A" -> "C:/", "C:/" -> "" (the drives root).
*/
QString GetParentPath(const QString& path)
{
	const QString trimmedPath = TrimTrailingSeparators(path);
	const qsizetype separatorIndex = trimmedPath.lastIndexOf(QLatin1Char('/'));
	if (separatorIndex < 0){
		return QString();
	}

	// A root keeps its separator: "/" and a drive root like "C:/"
	QString parentPath = trimmedPath.left(separatorIndex);
	if (parentPath.isEmpty() || parentPath.endsWith(QLatin1Char(':'))){
		return trimmedPath.left(separatorIndex + 1);
	}

	return parentPath;
}


// Last segment of a path: "/A/B" -> "B", "C:/" -> "C:", "/" -> ""
QString GetPathName(const QString& path)
{
	const QString trimmedPath = TrimTrailingSeparators(path);

	return trimmedPath.mid(trimmedPath.lastIndexOf(QLatin1Char('/')) + 1);
}


} // namespace


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

	// The root is a folder when it can be listed
	if (IsRootPath(path)){
		FileSystemQuery query;
		query.path = path;

		FileSystemListing listing;
		if (!SendGetEntries(query, listing, error)){
			return false;
		}

		entry.name = GetPathName(listing.resolvedPath);
		entry.path = listing.resolvedPath;
		entry.type = FileSystemEntry::Type::Dir;

		return true;
	}

	// Any other path is looked up in the listing of its parent. The parent is found by '/',
	// not by the rules of the local OS, and the entry only by its whole path as the listing gives it.
	FileSystemQuery query;
	query.path = GetParentPath(path);

	FileSystemListing listing;
	if (!SendGetEntries(query, listing, error)){
		return false;
	}

	for (const FileSystemEntry& candidate : listing.entries){
		if (candidate.path == path){
			entry = candidate;

			return true;
		}
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

	IGqlClient::GqlResult requestResult;
	if (m_sessionCompPtr.IsValid()){
		// After a refused token the session signs in again, and the request goes once more
		for (int attemptIndex = 0; attemptIndex < s_sessionAttemptCount; ++attemptIndex){
			const imtgql::IGqlContextSharedPtr contextPtr = m_sessionCompPtr->CreateRequestContext(error);
			if (!contextPtr.IsValid()){
				return false;
			}

			requestResult = SendWithContext(gqlRequest, contextPtr);
			if (requestResult.errorCategory != IGqlClient::EC_ACCESS_DENIED){
				break;
			}

			m_sessionCompPtr->OnTokenRejected(contextPtr->GetToken());
		}
	}
	else{
		requestResult = SendWithContext(gqlRequest, imtgql::IGqlContextSharedPtr());
	}

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


IGqlClient::GqlResult CRemoteFileSystemStructureProviderComp::SendWithContext(
			const imtgql::CGqlRequest& gqlRequest,
			const imtgql::IGqlContextSharedPtr& contextPtr) const
{
	IGqlClient::GqlRequestPtr requestPtr;
	requestPtr.MoveCastedPtr(gqlRequest.CloneMe());
	auto* sentRequestPtr = dynamic_cast<imtgql::CGqlRequest*>(requestPtr.GetPtr());
	if (sentRequestPtr == nullptr){
		return {{}, IGqlClient::EC_INTERNAL, QStringLiteral("Unable to clone %1 request").arg(QString::fromUtf8(gqlRequest.GetCommandId()))};
	}

	if (contextPtr.IsValid()){
		sentRequestPtr->SetGqlContext(contextPtr);
	}
	else{
		// Attach current request context when available (auth headers, clientid, etc.).
		CClientRequestModelHelpers::AttachMissingContext(requestPtr);
	}

	imtbase::IUrlParam* urlParamPtr = nullptr;
	if (m_urlParamCompPtr.IsValid()){
		urlParamPtr = m_urlParamCompPtr.GetPtr();
	}

	return m_apiClientCompPtr->SendRequest(requestPtr, urlParamPtr);
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
