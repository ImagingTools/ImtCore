// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtfile/CFileSystemStructureProviderComp.h>

// Qt includes
#include <QtCore/QCoreApplication>
#include <QtCore/QCollator>
#include <QtCore/QFileInfo>
#include <QtCore/QRegularExpression>
#include <QtCore/QStorageInfo>
#include <algorithm>


namespace imtfile
{
namespace
{


// Shell-style glob → regex: * = any run of chars, ? = one char; all other
// regex metacharacters are escaped so "file.txt" and "*.exe" behave as users
// expect in a file browser (not as raw PCRE, where leading * is invalid).
QString GlobToRegexPattern(const QString& glob)
{
	QString pattern;
	pattern.reserve(glob.size() * 2);
	for (const QChar ch : glob){
		switch (ch.unicode()){
		case L'*':
			pattern += QStringLiteral(".*");
			break;
		case L'?':
			pattern += QLatin1Char('.');
			break;
		case L'.':
		case L'(':
		case L')':
		case L'[':
		case L']':
		case L'{':
		case L'}':
		case L'+':
		case L'|':
		case L'^':
		case L'$':
		case L'\\':
			pattern += QLatin1Char('\\');
			pattern += ch;
			break;
		default:
			pattern += ch;
			break;
		}
	}
	return pattern;
}


// Explicit full regex: /pattern/ (optional trailing junk after the closing slash
// is ignored). Returns false when the filter is not delimited that way or the
// inner pattern does not compile.
bool TryParseDelimitedRegex(const QString& nameFilter, QRegularExpression& outRegex)
{
	if (nameFilter.size() < 2 || !nameFilter.startsWith(QLatin1Char('/'))){
		return false;
	}

	const int lastSlash = nameFilter.lastIndexOf(QLatin1Char('/'));
	if (lastSlash <= 0){
		return false;
	}

	outRegex.setPattern(nameFilter.mid(1, lastSlash - 1));
	outRegex.setPatternOptions(
				QRegularExpression::CaseInsensitiveOption
				| QRegularExpression::UseUnicodePropertiesOption);
	return outRegex.isValid();
}


// Lexical containment check that is correct for drive roots. For a normal root
// like "C:/App" the boundary is "C:/App/"; for a drive root like "C:/" the
// canonical path already ends with '/', so appending another '/' would yield
// "C://" and reject every child. Normalizing the trailing separator fixes both.
bool IsWithinRoot(const QString& path, const QString& canonicalRootPath)
{
	if (path == canonicalRootPath){
		return true;
	}

	QString rootPrefix = canonicalRootPath;
	if (!rootPrefix.endsWith(QLatin1Char('/'))){
		rootPrefix += QLatin1Char('/');
	}

	return path.startsWith(rootPrefix);
}


// Containers (dir / symlink / drive) sort above plain files, matching QDir::DirsFirst
// and the QML dialog's "folders first" rule.
bool IsContainerType(IFileSystemStructureProvider::FileSystemEntry::Type type)
{
	return type != IFileSystemStructureProvider::FileSystemEntry::Type::File;
}


} // namespace


// public methods

// reimplemented (IFileSystemStructureProvider)

bool CFileSystemStructureProviderComp::GetEntries(
			const FileSystemQuery& query,
			FileSystemListing& result,
			QString& error) const
{
	result = FileSystemListing();

	// Whole-file-system mode: an empty path addresses the virtual drives root,
	// which is not a real directory and therefore bypasses the QDir path below.
	if (IsWholeFileSystemAllowed() && query.path.isEmpty()){
		return GetDrivesListing(query, result);
	}

	QString canonicalPath;
	if (!NormalizeAndAuthorize(query.path, canonicalPath, error)){
		return false;
	}

	QDir dir(canonicalPath);
	if (!dir.exists()){
		error = QStringLiteral("Folder '%1' does not exist").arg(query.path.isEmpty() ? canonicalPath : query.path);

		return false;
	}

	result.resolvedPath = canonicalPath;
	FillParent(canonicalPath, result);

	// QDir::System keeps junctions/reparse points visible as entries (Windows);
	// they are still constrained by NormalizeAndAuthorize via canonical paths.
	QDir::Filters filters = QDir::Dirs | QDir::NoDotAndDotDot | QDir::System;
	if (IsShowFiles()){
		filters |= QDir::Files;
	}
	if (IsShowHidden()){
		filters |= QDir::Hidden;
	}

	AppendPage(dir.entryInfoList(filters, QDir::DirsFirst | QDir::Name | QDir::IgnoreCase), query, false, result);

	return true;
}


// Lists the storage volumes as the virtual root of a whole-file-system provider.
bool CFileSystemStructureProviderComp::GetDrivesListing(
			const FileSystemQuery& query,
			FileSystemListing& result) const
{
	// The virtual root has no real path and nothing above it.
	result.resolvedPath.clear();
	result.hasParent = false;

	AppendPage(QDir::drives(), query, true, result);

	return true;
}


// Fills the parent of an already-authorized canonical directory.
void CFileSystemStructureProviderComp::FillParent(
			const QString& canonicalPath,
			FileSystemListing& result) const
{
	QDir parentDir(canonicalPath);
	if (parentDir.cdUp()){
		const QString parentCanonical = parentDir.canonicalPath();
		if (IsWholeFileSystemAllowed()){
			result.hasParent = true;
			result.parentPath = parentCanonical;

			return;
		}

		// Sandboxed: a parent outside the root is not reachable, so report none.
		const QString canonicalRootPath = QDir(GetRootPath()).canonicalPath();
		if (canonicalPath != canonicalRootPath && IsWithinRoot(parentCanonical, canonicalRootPath)){
			result.hasParent = true;
			result.parentPath = parentCanonical;
		}

		return;
	}

	// cdUp() failed: we are at a volume root ("C:/", "/"). Whole-file-system mode
	// puts the virtual drives root above it - addressed by an empty path, which is
	// why hasParent must carry that fact rather than parentPath's emptiness.
	if (IsWholeFileSystemAllowed()){
		result.hasParent = true;
		result.parentPath.clear();
	}
}


// Builds the full entry list, applies nameFilter + sort, then offset/limit.
// totalCount/hasMore always describe the filtered set (not the raw directory).
void CFileSystemStructureProviderComp::AppendPage(
			const QFileInfoList& infoList,
			const FileSystemQuery& query,
			bool drivesRoot,
			FileSystemListing& result) const
{
	QList<FileSystemEntry> entries;
	entries.reserve(infoList.size());
	for (const QFileInfo& info : infoList){
		entries.append(drivesRoot ? MakeDriveEntry(info) : MakeEntry(info));
	}

	ApplyNameFilter(entries, query.nameFilter);
	ApplyExtensionFilter(entries, query.extensions);
	SortEntries(entries, query);

	result.totalCount = entries.size();

	int offset = query.offset;
	if (offset < 0){
		offset = 0;
	}

	int endIndex = result.totalCount;
	if (query.limit > 0){
		endIndex = qMin(result.totalCount, offset + query.limit);
	}

	for (int i = offset; i < endIndex; ++i){
		result.entries.append(entries.at(i));
	}

	result.hasMore = endIndex < result.totalCount;
}


void CFileSystemStructureProviderComp::ApplyNameFilter(
			QList<FileSystemEntry>& entries,
			const QString& nameFilter) const
{
	if (nameFilter.isEmpty()){
		return;
	}

	// Matching rules (case-insensitive):
	// 1) /pattern/  → full regular expression (PCRE via QRegularExpression)
	// 2) otherwise  → shell-style glob: * and ? wildcards; other metacharacters
	//    are literal (so "*.exe" and "file.txt" work as in Explorer).
	// 3) if neither compiles, fall back to plain substring contains.
	//
	// QString::contains(QRegularExpression) searches anywhere in the name, so
	// plain text without wildcards still behaves as a substring filter.
	QRegularExpression regex;
	bool useRegex = TryParseDelimitedRegex(nameFilter, regex);
	if (!useRegex){
		regex.setPattern(GlobToRegexPattern(nameFilter));
		regex.setPatternOptions(
					QRegularExpression::CaseInsensitiveOption
					| QRegularExpression::UseUnicodePropertiesOption);
		useRegex = regex.isValid();
	}

	QList<FileSystemEntry> filtered;
	filtered.reserve(entries.size());
	for (const FileSystemEntry& entry : entries){
		const bool matches = useRegex
				? entry.name.contains(regex)
				: entry.name.contains(nameFilter, Qt::CaseInsensitive);
		if (matches){
			filtered.append(entry);
		}
	}
	entries = filtered;
}


void CFileSystemStructureProviderComp::ApplyExtensionFilter(
			QList<FileSystemEntry>& entries,
			const QStringList& extensions) const
{
	if (extensions.isEmpty()){
		return;
	}

	// Normalize the whitelist once: trim, strip a leading dot, lowercase.
	// Special token "*" (or "none") allows extensionless file names — needed for
	// Linux/Unix executables that have no suffix (e.g. "myservice").
	QStringList allowed;
	allowed.reserve(extensions.size());
	bool allowExtensionless = false;
	for (const QString& ext : extensions){
		QString normalized = ext.trimmed().toLower();
		if (normalized.startsWith(QLatin1Char('.'))){
			normalized = normalized.mid(1);
		}
		if (normalized.isEmpty()){
			continue;
		}
		if (normalized == QLatin1String("*") || normalized == QLatin1String("none")){
			allowExtensionless = true;
			continue;
		}
		allowed.append(normalized);
	}
	if (allowed.isEmpty() && !allowExtensionless){
		return;
	}

	QList<FileSystemEntry> filtered;
	filtered.reserve(entries.size());
	for (const FileSystemEntry& entry : entries){
		// Containers stay listed so folders remain navigable whatever the filter.
		if (IsContainerType(entry.type)){
			filtered.append(entry);

			continue;
		}
		const QString suffix = QFileInfo(entry.name).suffix().toLower();
		if (suffix.isEmpty()){
			if (allowExtensionless){
				filtered.append(entry);
			}
			continue;
		}
		if (allowed.contains(suffix)){
			filtered.append(entry);
		}
	}
	entries = filtered;
}


qint64 CFileSystemStructureProviderComp::SortKeySize(const FileSystemEntry& entry)
{
	if (entry.type == FileSystemEntry::Type::Drive){
		return entry.totalBytes >= 0 ? entry.totalBytes : 0;
	}
	if (IsContainerType(entry.type)){
		return 0;
	}

	return entry.size;
}


qint64 CFileSystemStructureProviderComp::SortKeyDate(const FileSystemEntry& entry)
{
	// Drives have no mtime; the UI column shows free space, so sort by that.
	if (entry.type == FileSystemEntry::Type::Drive){
		return entry.freeBytes >= 0 ? entry.freeBytes : 0;
	}
	if (!entry.lastModified.isValid()){
		return 0;
	}

	return entry.lastModified.toMSecsSinceEpoch();
}


void CFileSystemStructureProviderComp::SortEntries(
			QList<FileSystemEntry>& entries,
			const FileSystemQuery& query) const
{
	QCollator collator;
	collator.setCaseSensitivity(Qt::CaseInsensitive);
	collator.setNumericMode(true);

	const SortBy sortBy = query.sortBy;
	const bool ascending = query.sortAscending;

	std::sort(entries.begin(), entries.end(),
			[&](const FileSystemEntry& left, const FileSystemEntry& right) -> bool {
		const bool leftContainer = IsContainerType(left.type);
		const bool rightContainer = IsContainerType(right.type);
		if (leftContainer != rightContainer){
			// Containers always above files, independent of sort direction.
			return leftContainer;
		}

		int cmp = 0;
		switch (sortBy){
		case SortBy::Size:{
			const qint64 leftKey = SortKeySize(left);
			const qint64 rightKey = SortKeySize(right);
			if (leftKey < rightKey){
				cmp = -1;
			}
			else if (leftKey > rightKey){
				cmp = 1;
			}
			break;
		}
		case SortBy::Date:{
			const qint64 leftKey = SortKeyDate(left);
			const qint64 rightKey = SortKeyDate(right);
			if (leftKey < rightKey){
				cmp = -1;
			}
			else if (leftKey > rightKey){
				cmp = 1;
			}
			break;
		}
		case SortBy::Name:
		default:
			cmp = collator.compare(left.name, right.name);
			break;
		}

		if (cmp == 0){
			cmp = collator.compare(left.name, right.name);
		}

		return ascending ? (cmp < 0) : (cmp > 0);
	});
}


bool CFileSystemStructureProviderComp::Stat(
			const QString& path,
			FileSystemEntry& entry,
			QString& error) const
{
	entry = FileSystemEntry();

	QString canonicalPath;
	if (!NormalizeAndAuthorize(path, canonicalPath, error)){
		return false;
	}

	const QFileInfo fileInfo(canonicalPath);
	if (!fileInfo.exists()){
		error = QStringLiteral("Path '%1' does not exist").arg(path.isEmpty() ? canonicalPath : path);

		return false;
	}

	entry = MakeEntry(fileInfo);

	return true;
}


// protected methods

QString CFileSystemStructureProviderComp::GetRootPath() const
{
	// Default: OS drive root (e.g. C:/) so service executables outside the agent
	// install directory remain browsable. Still a single hard root (no multi-drive list).
	if (!m_rootPathAttrPtr.IsValid()){
		return QDir::rootPath();
	}

	const QString configured = *m_rootPathAttrPtr;
	if (configured.isEmpty() || configured == QLatin1String("${DriveRoot}")){
		return QDir(QDir::rootPath()).absolutePath();
	}
	if (configured == QLatin1String("${ApplicationDir}")){
		return QDir(QCoreApplication::applicationDirPath()).absolutePath();
	}

	return QDir(configured).absolutePath();
}


bool CFileSystemStructureProviderComp::IsShowFiles() const
{
	return m_showFilesAttrPtr.IsValid() ? *m_showFilesAttrPtr : true;
}


bool CFileSystemStructureProviderComp::IsShowHidden() const
{
	return m_showHiddenAttrPtr.IsValid() ? *m_showHiddenAttrPtr : false;
}


bool CFileSystemStructureProviderComp::IsWholeFileSystemAllowed() const
{
	return m_allowWholeFileSystemAttrPtr.IsValid() ? *m_allowWholeFileSystemAttrPtr : false;
}


bool CFileSystemStructureProviderComp::NormalizeAndAuthorize(
			const QString& path,
			QString& canonicalPath,
			QString& error) const
{
	canonicalPath.clear();

	// Whole-file-system mode: no root, so nothing to contain the path within -
	// resolve it and let the caller's existence check reject what is not there.
	if (IsWholeFileSystemAllowed()){
		const QFileInfo requestedInfo(path);

		QString resolvedPath = requestedInfo.canonicalFilePath();
		if (resolvedPath.isEmpty()){
			resolvedPath = QDir::cleanPath(requestedInfo.absoluteFilePath());
		}

		canonicalPath = resolvedPath;

		return true;
	}

	const QString rootPath = GetRootPath();
	const QDir rootDir(rootPath);
	if (rootPath.isEmpty() || !rootDir.exists()){
		error = QStringLiteral("File system root folder does not exist");
		SendErrorMessage(0, error, "CFileSystemStructureProviderComp");

		return false;
	}

	const QString canonicalRootPath = rootDir.canonicalPath();
	if (canonicalRootPath.isEmpty()){
		error = QStringLiteral("File system root folder cannot be resolved");
		SendErrorMessage(0, error, "CFileSystemStructureProviderComp");

		return false;
	}

	// Empty path resolves to the configured hard root.
	const QString requestedPath = path.isEmpty() ? canonicalRootPath : path;
	const QFileInfo requestedInfo(requestedPath);

	// Prefer canonical path (resolves symlinks/junctions/".."). Fall back to absolute
	// cleaned path only when the target does not exist yet for clearer error messages
	// after the root check - non-existing targets still must not escape the root via "..".
	QString resolvedPath = requestedInfo.canonicalFilePath();
	if (resolvedPath.isEmpty()){
		resolvedPath = QDir::cleanPath(requestedInfo.absoluteFilePath());
	}

	if (!IsWithinRoot(resolvedPath, canonicalRootPath)){
		// Also reject Windows path mismatch (case / separator variants via cleanPath above).
		error = QStringLiteral("Access to path '%1' is denied").arg(requestedPath);
		SendWarningMessage(0, error, "CFileSystemStructureProviderComp");

		return false;
	}

	// Symlink policy: if the path itself is a symlink, authorize by the symlink path only
	// when its canonical target remains inside the root (checked above via canonicalFilePath).
	// Junction/".." escapes outside root fail the startsWith check.
	canonicalPath = resolvedPath;

	return true;
}


IFileSystemStructureProvider::FileSystemEntry CFileSystemStructureProviderComp::MakeEntry(
			const QFileInfo& fileInfo) const
{
	FileSystemEntry entry;
	entry.name = fileInfo.fileName();
	entry.path = fileInfo.absoluteFilePath();
	entry.lastModified = fileInfo.lastModified();

	if (fileInfo.isSymLink()){
		entry.type = FileSystemEntry::Type::Symlink;
		entry.size = 0;
	}
	else if (fileInfo.isDir()){
		entry.type = FileSystemEntry::Type::Dir;
		entry.size = 0;
	}
	else{
		entry.type = FileSystemEntry::Type::File;
		entry.size = fileInfo.size();
	}

	return entry;
}


IFileSystemStructureProvider::FileSystemEntry CFileSystemStructureProviderComp::MakeDriveEntry(
			const QFileInfo& driveInfo) const
{
	FileSystemEntry entry;
	entry.type = FileSystemEntry::Type::Drive;
	entry.path = driveInfo.absoluteFilePath();
	entry.size = 0;

	// QFileInfo::fileName() is empty for a volume root ("C:/"), so a drive needs
	// its own naming. Prefer the volume label and always surface the root key
	// (Windows: "C:") so two volumes with the same label stay distinguishable -
	// Explorer-style "Local Disk (C:)" / "Data (D:)".
	const QStorageInfo storage(entry.path);

	QString rootKey;
	if (entry.path.size() >= 2 && entry.path.at(1) == QLatin1Char(':')){
		rootKey = entry.path.left(2).toUpper();
	}
	else{
		// Unix / other: QDir::drives() is typically a single "/".
		rootKey = QDir::toNativeSeparators(QDir::cleanPath(entry.path));
		if (rootKey.isEmpty()){
			rootKey = entry.path;
		}
	}

	const QString displayName = storage.displayName().trimmed();
	if (displayName.isEmpty() || displayName.compare(rootKey, Qt::CaseInsensitive) == 0
				|| displayName.compare(entry.path, Qt::CaseInsensitive) == 0){
		entry.name = rootKey;
	}
	else if (!rootKey.isEmpty()
				&& displayName.contains(rootKey, Qt::CaseInsensitive)){
		// Already Explorer-like ("Local Disk (C:)") — keep as reported.
		entry.name = displayName;
	}
	else if (!rootKey.isEmpty()){
		entry.name = QStringLiteral("%1 (%2)").arg(displayName, rootKey);
	}
	else{
		entry.name = displayName;
	}

	if (storage.isValid() && storage.isReady()){
		entry.totalBytes = storage.bytesTotal();
		entry.freeBytes = storage.bytesAvailable();
	}

	// lastModified stays invalid: a volume has no meaningful modification time.

	return entry;
}


} // namespace imtfile
