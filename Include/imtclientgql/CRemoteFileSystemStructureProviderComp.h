// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QJsonObject>

// ACF includes
#include <ilog/TLoggerCompWrap.h>

// ImtCore includes
#include <imtfile/IFileSystemStructureProvider.h>
#include <imtfile/IFileDownloadLinkProvider.h>
#include <imtclientgql/IGqlClient.h>
#include <imtbase/IUrlParam.h>
#include <imtgql/CGqlRequest.h>


namespace imtclientgql
{


/**
	Remote implementation of IFileSystemStructureProvider and IFileDownloadLinkProvider.
	Forwards GetEntries/Stat/GetDownloadLink through IGqlClient to a remote FileSystem controller.
 */
class CRemoteFileSystemStructureProviderComp:
			virtual public imtfile::IFileSystemStructureProvider,
			virtual public imtfile::IFileDownloadLinkProvider,
			public ilog::CLoggerComponentBase
{
public:
	typedef ilog::CLoggerComponentBase BaseClass;

	I_BEGIN_COMPONENT(CRemoteFileSystemStructureProviderComp);
		I_REGISTER_INTERFACE(imtfile::IFileSystemStructureProvider);
		I_REGISTER_INTERFACE(imtfile::IFileDownloadLinkProvider);
		I_ASSIGN(m_apiClientCompPtr, "ApiClient", "API client connected to the remote file service", true, "ApiClient");
		I_ASSIGN(m_urlParamCompPtr, "UrlParam", "Optional URL override for the remote file service", false, "UrlParam");
	I_END_COMPONENT;

	// reimplemented (imtfile::IFileSystemStructureProvider)
	virtual bool GetEntries(
				const FileSystemQuery& query,
				FileSystemListing& result,
				QString& error) const override;
	virtual bool Stat(
				const QString& path,
				FileSystemEntry& entry,
				QString& error) const override;

	// reimplemented (imtfile::IFileDownloadLinkProvider)
	virtual bool GetDownloadLink(
				const QString& path,
				DownloadLink& link,
				QString& error) const override;

private:
	bool SendGetEntries(
				const FileSystemQuery& query,
				FileSystemListing& result,
				QString& error) const;
	/**
		Sends the request to the remote file service.
		\param commandData [OUTPUT] - the answer to the command of the request
	*/
	bool SendRequest(
				const imtgql::CGqlRequest& gqlRequest,
				QJsonObject& commandData,
				QString& error) const;
	static imtfile::IFileSystemStructureProvider::FileSystemEntry::Type FromSdlEntryType(int sdlEntryType);

protected:
	I_REF(imtclientgql::IGqlClient, m_apiClientCompPtr);
	I_REF(imtbase::IUrlParam, m_urlParamCompPtr);
};


} // namespace imtclientgql
