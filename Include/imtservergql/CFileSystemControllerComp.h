// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QJsonObject>

// ImtCore includes
#include <imtfile/IFileSystemStructureProvider.h>
#include <imtservergql/CGqlRequestHandlerCompBase.h>


namespace imtservergql
{


/**
	GraphQL controller providing the folder hierarchy of a file system.
	It handles the 'GetFileSystemEntries' query and delegates
	the file system access to a IFileSystemStructureProvider component.
 */
class CFileSystemControllerComp: public CGqlRequestHandlerCompBase
{
public:
	typedef CGqlRequestHandlerCompBase BaseClass;

	I_BEGIN_COMPONENT(CFileSystemControllerComp);
		I_ASSIGN(m_fileSystemProviderCompPtr, "FileSystemProvider", "Provider of the file system structure", true, "FileSystemProvider");
	I_END_COMPONENT;

protected:
	// reimplemented (CGqlRequestHandlerCompBase)
	virtual QJsonObject CreateInternalResponse(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const override;

protected:
	I_REF(imtfile::IFileSystemStructureProvider, m_fileSystemProviderCompPtr);
};


} // namespace imtservergql
