// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtfile/IFileSystemStructureProvider.h>
#include <GeneratedFiles/imtbasesdl/SDL/1.0/CPP/FileSystem_fwd.h>


namespace imtservergql
{


/**
	GraphQL controller providing the folder hierarchy of a file system.
	Handles 'GetFileSystemEntries' via generated SDL types and permission checks.
	File system access is delegated to IFileSystemStructureProvider (local or remote).
 */
class CFileSystemControllerComp: public sdl::V1_0::imtbase::CFileSystemGqlHandlerCompBase
{
public:
	typedef sdl::V1_0::imtbase::CFileSystemGqlHandlerCompBase BaseClass;

	I_BEGIN_COMPONENT(CFileSystemControllerComp);
		I_ASSIGN(m_fileSystemProviderCompPtr, "FileSystemProvider", "Provider of the file system structure", true, "FileSystemProvider");
	I_END_COMPONENT;

protected:
	// reimplemented (sdl::V1_0::imtbase::CFileSystemGqlHandlerCompBase)
	virtual sdl::V1_0::imtbase::CGetFileSystemEntriesPayload OnGetFileSystemEntries(
				const sdl::V1_0::imtbase::CGetFileSystemEntriesGqlRequest& getFileSystemEntriesRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;

	// Hook for unit tests (default: component reference).
	virtual imtfile::IFileSystemStructureProvider* GetFileSystemProvider() const;

protected:
	I_REF(imtfile::IFileSystemStructureProvider, m_fileSystemProviderCompPtr);
};


} // namespace imtservergql
