// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QString>
#include <QtCore/QDir>
#include <QtCore/QFileInfo>

// ACF includes
#include <ilog/TLoggerCompWrap.h>

// ImtCore includes
#include <imtfile/IFileSystemStructureProvider.h>


namespace imtfile
{


/**
	Local file system structure provider based on QDir.

	By default browsing is restricted to the configured root folder (hard single
	root). Setting 'AllowWholeFileSystem' removes that restriction entirely and
	exposes a virtual drives root - see the attribute description.
 */
class CFileSystemStructureProviderComp:
			virtual public IFileSystemStructureProvider,
			public ilog::CLoggerComponentBase
{
public:
	typedef ilog::CLoggerComponentBase BaseClass;

	I_BEGIN_COMPONENT(CFileSystemStructureProviderComp);
		I_REGISTER_INTERFACE(IFileSystemStructureProvider);
		I_ASSIGN(m_rootPathAttrPtr, "RootPath", "Browsable root. Tokens: '${DriveRoot}' (OS drive root, default), '${ApplicationDir}' (process directory). Absolute paths are used as-is. Ignored when 'AllowWholeFileSystem' is 'true'.", true, QStringLiteral("${DriveRoot}"));
		I_ASSIGN(m_allowWholeFileSystemAttrPtr, "AllowWholeFileSystem", "SECURITY: if 'true' the whole machine becomes browsable - 'RootPath' is ignored, no containment check is applied and an empty path lists the storage volumes. Only entry metadata (names/sizes/dates) is exposed; file contents are never read.", true, false);
		I_ASSIGN(m_showFilesAttrPtr, "ShowFiles", "If 'true' files will be listed in addition to folders", true, true);
		I_ASSIGN(m_showHiddenAttrPtr, "ShowHidden", "If 'true' hidden entries will be listed", true, false);
	I_END_COMPONENT;

	// reimplemented (IFileSystemStructureProvider)
	virtual bool GetEntries(
				const FileSystemQuery& query,
				FileSystemListing& result,
				QString& error) const override;
	virtual bool Stat(
				const QString& path,
				FileSystemEntry& entry,
				QString& error) const override;

protected:
	// Hooks for unit tests (default: component attributes).
	virtual QString GetRootPath() const;
	virtual bool IsShowFiles() const;
	virtual bool IsShowHidden() const;
	virtual bool IsWholeFileSystemAllowed() const;
	bool NormalizeAndAuthorize(
				const QString& path,
				QString& canonicalPath,
				QString& error) const;

private:
	bool GetDrivesListing(const FileSystemQuery& query, FileSystemListing& result) const;
	void FillParent(const QString& canonicalPath, FileSystemListing& result) const;
	// Builds entries from infoList, applies nameFilter + sort, then offset/limit.
	void AppendPage(const QFileInfoList& infoList, const FileSystemQuery& query, bool drivesRoot, FileSystemListing& result) const;
	void ApplyNameFilter(QList<FileSystemEntry>& entries, const QString& nameFilter) const;
	// Drops files whose extension is not in the whitelist; containers always pass.
	void ApplyExtensionFilter(QList<FileSystemEntry>& entries, const QStringList& extensions) const;
	void SortEntries(QList<FileSystemEntry>& entries, const FileSystemQuery& query) const;
	static qint64 SortKeySize(const FileSystemEntry& entry);
	static qint64 SortKeyDate(const FileSystemEntry& entry);
	FileSystemEntry MakeEntry(const QFileInfo& fileInfo) const;
	FileSystemEntry MakeDriveEntry(const QFileInfo& driveInfo) const;

private:
	I_ATTR(QString, m_rootPathAttrPtr);
	I_ATTR(bool, m_allowWholeFileSystemAttrPtr);
	I_ATTR(bool, m_showFilesAttrPtr);
	I_ATTR(bool, m_showHiddenAttrPtr);
};


} // namespace imtfile
