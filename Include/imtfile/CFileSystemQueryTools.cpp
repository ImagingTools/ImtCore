// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtfile/CFileSystemQueryTools.h>


// Qt includes
#include <QtCore/QCollator>
#include <QtCore/QFileInfo>
#include <QtCore/QRegularExpression>

// std includes
#include <algorithm>


namespace imtfile
{
namespace
{


typedef IFileSystemStructureProvider::FileSystemEntry FileSystemEntry;
typedef IFileSystemStructureProvider::FileSystemQuery FileSystemQuery;
typedef IFileSystemStructureProvider::SortBy SortBy;


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


// Containers (dir / symlink / drive) sort above plain files, matching QDir::DirsFirst
// and the QML dialog's "folders first" rule.
bool IsContainerType(FileSystemEntry::Type type)
{
	return type != FileSystemEntry::Type::File;
}


void ApplyNameFilter(QList<FileSystemEntry>& entries, const QString& nameFilter)
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


// Drops files whose extension is not in the whitelist; containers always pass.
void ApplyExtensionFilter(QList<FileSystemEntry>& entries, const QStringList& extensions)
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


qint64 SortKeySize(const FileSystemEntry& entry)
{
	if (entry.type == FileSystemEntry::Type::Drive){
		return entry.totalBytes >= 0 ? entry.totalBytes : 0;
	}
	if (IsContainerType(entry.type)){
		return 0;
	}

	return entry.size;
}


qint64 SortKeyDate(const FileSystemEntry& entry)
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


void SortEntries(QList<FileSystemEntry>& entries, const FileSystemQuery& query)
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


} // namespace


// public static methods

void CFileSystemQueryTools::ApplyQuery(
			QList<IFileSystemStructureProvider::FileSystemEntry> entries,
			const IFileSystemStructureProvider::FileSystemQuery& query,
			IFileSystemStructureProvider::FileSystemListing& listing)
{
	ApplyNameFilter(entries, query.nameFilter);
	ApplyExtensionFilter(entries, query.extensions);
	SortEntries(entries, query);

	// totalCount and hasMore describe the filtered set, not the raw folder
	listing.totalCount = entries.size();

	int offset = query.offset;
	if (offset < 0){
		offset = 0;
	}

	int endIndex = listing.totalCount;
	if (query.limit > 0){
		endIndex = qMin(listing.totalCount, offset + query.limit);
	}

	listing.entries.clear();
	for (int i = offset; i < endIndex; ++i){
		listing.entries.append(entries.at(i));
	}

	listing.hasMore = endIndex < listing.totalCount;
}


} // namespace imtfile
