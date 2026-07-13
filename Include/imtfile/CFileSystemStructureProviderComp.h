// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QString>
#include <QtCore/QDir>

// ACF includes
#include <ilog/TLoggerCompWrap.h>

// ImtCore includes
#include <imtfile/IFileSystemStructureProvider.h>


namespace imtfile
{


/**
	This component provides the folder hierarchy of the local file system.
	Browsing is restricted to the configured root folder.
 */
class CFileSystemStructureProviderComp:
			virtual public IFileSystemStructureProvider,
			public ilog::CLoggerComponentBase
{
public:
	typedef ilog::CLoggerComponentBase BaseClass;

	I_BEGIN_COMPONENT(CFileSystemStructureProviderComp);
		I_REGISTER_INTERFACE(IFileSystemStructureProvider);
		I_ASSIGN(m_rootPathAttrPtr, "RootPath", "Root folder of the browsable file system", true, QDir::rootPath());
		I_ASSIGN(m_showFilesAttrPtr, "ShowFiles", "If 'true' files will be listed in addition to folders", true, true);
		I_ASSIGN(m_showHiddenAttrPtr, "ShowHidden", "If 'true' hidden entries will be listed", true, false);
	I_END_COMPONENT;

	// reimplemented (IFileSystemStructureProvider)
	virtual bool GetFileSystemEntries(
				const QString& path,
				FileSystemEntryList& entries,
				QString& resolvedPath,
				QString& parentPath,
				QString& errorMessage) const override;

private:
	QString GetRootPath() const;

private:
	I_ATTR(QString, m_rootPathAttrPtr);
	I_ATTR(bool, m_showFilesAttrPtr);
	I_ATTR(bool, m_showHiddenAttrPtr);
};


} // namespace imtfile
