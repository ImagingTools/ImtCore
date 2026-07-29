// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <ilog/TLoggerCompWrap.h>

// ImtCore includes
#include <imtfile/IFileSystemStructureProvider.h>
#include <imtclientgql/IGqlClient.h>
#include <imtbase/IUrlParam.h>


namespace imtclientgql
{


/**
	Remote implementation of IFileSystemStructureProvider.
	Forwards GetEntries/Stat through IGqlClient to a remote FileSystem controller.
 */
class CRemoteFileSystemStructureProviderComp:
			virtual public imtfile::IFileSystemStructureProvider,
			public ilog::CLoggerComponentBase
{
public:
	typedef ilog::CLoggerComponentBase BaseClass;

	I_BEGIN_COMPONENT(CRemoteFileSystemStructureProviderComp);
		I_REGISTER_INTERFACE(imtfile::IFileSystemStructureProvider);
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

private:
	bool SendGetEntries(
				const FileSystemQuery& query,
				FileSystemListing& result,
				QString& error) const;
	static imtfile::IFileSystemStructureProvider::FileSystemEntry::Type FromSdlEntryType(int sdlEntryType);

protected:
	I_REF(imtclientgql::IGqlClient, m_apiClientCompPtr);
	I_REF(imtbase::IUrlParam, m_urlParamCompPtr);
};


} // namespace imtclientgql
