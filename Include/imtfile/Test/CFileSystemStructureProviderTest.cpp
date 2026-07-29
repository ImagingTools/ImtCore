// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include "CFileSystemStructureProviderTest.h"

// Qt includes
#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QFileInfo>


void CFileSystemStructureProviderTest::initTestCase()
{
	QVERIFY2(m_tempDir.isValid(), "Temporary directory could not be created");

	CreateSubDir(QStringLiteral("sub"));
	CreateSubDir(QStringLiteral("sub/nested"));
	CreateFile(QStringLiteral("sub/file.txt"), QByteArrayLiteral("hello"));
	CreateFile(QStringLiteral("other.txt"), QByteArrayLiteral("other"));

	ConfigureProvider(m_tempDir.path(), true, false);
}


void CFileSystemStructureProviderTest::cleanupTestCase()
{
}


void CFileSystemStructureProviderTest::EmptyPathResolvesToRootTest()
{
	imtfile::IFileSystemStructureProvider::FileSystemQuery query;
	imtfile::IFileSystemStructureProvider::FileSystemListing listing;
	QString error;

	QVERIFY2(m_provider.GetEntries(query, listing, error), qPrintable(error));
	QCOMPARE(QDir(listing.resolvedPath).canonicalPath(), QDir(m_tempDir.path()).canonicalPath());
	QVERIFY(listing.parentPath.isEmpty());
	QVERIFY(listing.totalCount >= 2);
}


void CFileSystemStructureProviderTest::PathTraversalOutsideRootDeniedTest()
{
	imtfile::IFileSystemStructureProvider::FileSystemQuery query;
	query.path = m_tempDir.path() + QStringLiteral("/../");

	const QString parentOfRoot = QFileInfo(m_tempDir.path()).absolutePath();
	imtfile::IFileSystemStructureProvider::FileSystemListing listing;
	QString error;

	const bool ok = m_provider.GetEntries(query, listing, error);
	if (QDir(parentOfRoot).canonicalPath() != QDir(m_tempDir.path()).canonicalPath()){
		QVERIFY2(!ok, "Path traversal outside root must be denied");
		QVERIFY(!error.isEmpty());
	}
}


void CFileSystemStructureProviderTest::AbsolutePathOutsideRootDeniedTest()
{
	const QString tempCanonical = QDir(QDir::tempPath()).canonicalPath();
	const QString rootCanonical = QDir(m_tempDir.path()).canonicalPath();
	if (tempCanonical == rootCanonical || tempCanonical.startsWith(rootCanonical + QLatin1Char('/'))){
		QSKIP("System temp path is inside the test root; cannot assert outside-root denial");
	}

	imtfile::IFileSystemStructureProvider::FileSystemQuery query;
	query.path = QDir::tempPath();

	imtfile::IFileSystemStructureProvider::FileSystemListing listing;
	QString error;
	QVERIFY2(!m_provider.GetEntries(query, listing, error), "Absolute path outside root must be denied");
	QVERIFY(!error.isEmpty());
}


void CFileSystemStructureProviderTest::ExistingSubfolderListingTest()
{
	imtfile::IFileSystemStructureProvider::FileSystemQuery query;
	query.path = m_tempDir.path() + QStringLiteral("/sub");

	imtfile::IFileSystemStructureProvider::FileSystemListing listing;
	QString error;
	QVERIFY2(m_provider.GetEntries(query, listing, error), qPrintable(error));

	QVERIFY(listing.totalCount >= 2);

	bool foundFile = false;
	bool foundNested = false;
	for (const auto& entry : listing.entries){
		if (entry.name == QStringLiteral("file.txt")){
			foundFile = true;
			QCOMPARE(entry.type, imtfile::IFileSystemStructureProvider::FileSystemEntry::Type::File);
			QVERIFY(entry.size > 0);
		}
		if (entry.name == QStringLiteral("nested")){
			foundNested = true;
			QCOMPARE(entry.type, imtfile::IFileSystemStructureProvider::FileSystemEntry::Type::Dir);
		}
	}

	QVERIFY(foundFile);
	QVERIFY(foundNested);
	QCOMPARE(QDir(listing.parentPath).canonicalPath(), QDir(m_tempDir.path()).canonicalPath());
}


void CFileSystemStructureProviderTest::NonExistingPathFailsTest()
{
	imtfile::IFileSystemStructureProvider::FileSystemQuery query;
	query.path = m_tempDir.path() + QStringLiteral("/does-not-exist");

	imtfile::IFileSystemStructureProvider::FileSystemListing listing;
	QString error;
	QVERIFY2(!m_provider.GetEntries(query, listing, error), "Non-existing path must fail");
	QVERIFY(!error.isEmpty());
}


void CFileSystemStructureProviderTest::PaginationTest()
{
	for (int i = 0; i < 10; ++i){
		CreateFile(QStringLiteral("page_%1.txt").arg(i), QByteArrayLiteral("p"));
	}

	imtfile::IFileSystemStructureProvider::FileSystemQuery query;
	query.offset = 0;
	query.limit = 3;

	imtfile::IFileSystemStructureProvider::FileSystemListing listing;
	QString error;
	QVERIFY2(m_provider.GetEntries(query, listing, error), qPrintable(error));
	QCOMPARE(listing.entries.size(), 3);
	QVERIFY(listing.totalCount >= 10);
	QVERIFY(listing.hasMore);

	query.offset = listing.totalCount - 1;
	query.limit = 5;
	imtfile::IFileSystemStructureProvider::FileSystemListing lastPage;
	QVERIFY2(m_provider.GetEntries(query, lastPage, error), qPrintable(error));
	QCOMPARE(lastPage.entries.size(), 1);
	QVERIFY(!lastPage.hasMore);
}


void CFileSystemStructureProviderTest::StatExistingFileTest()
{
	imtfile::IFileSystemStructureProvider::FileSystemEntry entry;
	QString error;
	const QString filePath = m_tempDir.path() + QStringLiteral("/sub/file.txt");
	QVERIFY2(m_provider.Stat(filePath, entry, error), qPrintable(error));
	QCOMPARE(entry.name, QStringLiteral("file.txt"));
	QCOMPARE(entry.type, imtfile::IFileSystemStructureProvider::FileSystemEntry::Type::File);
	QVERIFY(entry.size > 0);
}


void CFileSystemStructureProviderTest::StatOutsideRootDeniedTest()
{
	const QString outside = QDir::rootPath();
	const QString rootCanonical = QDir(m_tempDir.path()).canonicalPath();
	if (QDir(outside).canonicalPath() == rootCanonical){
		QSKIP("Root path equals test sandbox");
	}

	imtfile::IFileSystemStructureProvider::FileSystemEntry entry;
	QString error;
	QVERIFY2(!m_provider.Stat(outside, entry, error), "Stat outside root must be denied");
	QVERIFY(!error.isEmpty());
}


void CFileSystemStructureProviderTest::EmptyRootFailsTest()
{
	QTemporaryDir goneDir;
	QVERIFY(goneDir.isValid());
	const QString gonePath = goneDir.path();
	goneDir.remove();

	ConfigureProvider(gonePath, true, false);

	imtfile::IFileSystemStructureProvider::FileSystemQuery query;
	imtfile::IFileSystemStructureProvider::FileSystemListing listing;
	QString error;
	QVERIFY2(!m_provider.GetEntries(query, listing, error), "Empty/missing root must fail");
	QVERIFY(!error.isEmpty());

	ConfigureProvider(m_tempDir.path(), true, false);
}


// Whole-file-system mode

void CFileSystemStructureProviderTest::WholeFsSandboxRootHasNoParentTest()
{
	// Guards the sandboxed meaning of hasParent: the configured root must report
	// no parent, so the new flag cannot silently open a way out of the sandbox.
	imtfile::IFileSystemStructureProvider::FileSystemQuery query;
	imtfile::IFileSystemStructureProvider::FileSystemListing listing;
	QString error;

	QVERIFY2(m_provider.GetEntries(query, listing, error), qPrintable(error));
	QVERIFY(!listing.hasParent);
	QVERIFY(listing.parentPath.isEmpty());
}


void CFileSystemStructureProviderTest::WholeFsEmptyPathListsDrivesTest()
{
	m_provider.SetAllowWholeFileSystem(true);

	imtfile::IFileSystemStructureProvider::FileSystemQuery query;
	imtfile::IFileSystemStructureProvider::FileSystemListing listing;
	QString error;

	QVERIFY2(m_provider.GetEntries(query, listing, error), qPrintable(error));

	// The virtual drives root: no real path, nothing above it, drives only.
	QVERIFY(listing.resolvedPath.isEmpty());
	QVERIFY(!listing.hasParent);
	QVERIFY(!listing.entries.isEmpty());
	for (const auto& entry : listing.entries){
		QCOMPARE(entry.type, imtfile::IFileSystemStructureProvider::FileSystemEntry::Type::Drive);
	}

	ConfigureProvider(m_tempDir.path(), true, false);
}


void CFileSystemStructureProviderTest::WholeFsDrivesHaveNamesTest()
{
	m_provider.SetAllowWholeFileSystem(true);

	imtfile::IFileSystemStructureProvider::FileSystemQuery query;
	imtfile::IFileSystemStructureProvider::FileSystemListing listing;
	QString error;

	QVERIFY2(m_provider.GetEntries(query, listing, error), qPrintable(error));
	QVERIFY(!listing.entries.isEmpty());

	// QFileInfo::fileName() is empty for a volume root, so drives must be named
	// explicitly - an unnamed row would be unusable in the picker. On Windows the
	// name must also carry the drive key (e.g. "Local Disk (C:)") so volumes with
	// the same label stay distinguishable.
	for (const auto& entry : listing.entries){
		QVERIFY(!entry.name.isEmpty());
		QVERIFY(!entry.path.isEmpty());
		if (entry.path.size() >= 2 && entry.path.at(1) == QLatin1Char(':')){
			const QString rootKey = entry.path.left(2).toUpper();
			QVERIFY2(entry.name.contains(rootKey, Qt::CaseInsensitive),
					 qPrintable(QStringLiteral("Drive name '%1' lacks root key '%2'")
								.arg(entry.name, rootKey)));
		}
	}

	ConfigureProvider(m_tempDir.path(), true, false);
}


void CFileSystemStructureProviderTest::WholeFsDriveRootParentIsDrivesRootTest()
{
	m_provider.SetAllowWholeFileSystem(true);

	const QFileInfoList drives = QDir::drives();
	QVERIFY(!drives.isEmpty());

	imtfile::IFileSystemStructureProvider::FileSystemQuery query;
	query.path = drives.first().absoluteFilePath();

	imtfile::IFileSystemStructureProvider::FileSystemListing listing;
	QString error;
	QVERIFY2(m_provider.GetEntries(query, listing, error), qPrintable(error));

	// A volume root has a parent - the virtual drives root - and it is addressed
	// by an EMPTY path. That pairing is exactly why hasParent exists.
	QVERIFY(listing.hasParent);
	QVERIFY(listing.parentPath.isEmpty());

	ConfigureProvider(m_tempDir.path(), true, false);
}


void CFileSystemStructureProviderTest::WholeFsOutsideRootIsAllowedTest()
{
	const QString outside = QDir::tempPath();
	const QString rootCanonical = QDir(m_tempDir.path()).canonicalPath();
	if (QDir(outside).canonicalPath().startsWith(rootCanonical)){
		QSKIP("System temp path is inside the test root; cannot assert the sandbox is lifted");
	}

	// Same path the sandboxed provider rejects in AbsolutePathOutsideRootDeniedTest.
	m_provider.SetAllowWholeFileSystem(true);

	imtfile::IFileSystemStructureProvider::FileSystemQuery query;
	query.path = outside;

	imtfile::IFileSystemStructureProvider::FileSystemListing listing;
	QString error;
	QVERIFY2(m_provider.GetEntries(query, listing, error), qPrintable(error));
	QCOMPARE(QDir(listing.resolvedPath).canonicalPath(), QDir(outside).canonicalPath());

	ConfigureProvider(m_tempDir.path(), true, false);
}


void CFileSystemStructureProviderTest::WholeFsNonExistingPathStillFailsTest()
{
	m_provider.SetAllowWholeFileSystem(true);

	imtfile::IFileSystemStructureProvider::FileSystemQuery query;
	query.path = m_tempDir.path() + QStringLiteral("/does-not-exist");

	imtfile::IFileSystemStructureProvider::FileSystemListing listing;
	QString error;
	QVERIFY2(!m_provider.GetEntries(query, listing, error), "Non-existing path must fail even without a sandbox");
	QVERIFY(!error.isEmpty());

	ConfigureProvider(m_tempDir.path(), true, false);
}


void CFileSystemStructureProviderTest::WholeFsDrivesPaginationTest()
{
	m_provider.SetAllowWholeFileSystem(true);

	const int driveCount = QDir::drives().size();
	QVERIFY(driveCount >= 1);

	imtfile::IFileSystemStructureProvider::FileSystemQuery query;
	query.offset = 0;
	query.limit = 1;

	imtfile::IFileSystemStructureProvider::FileSystemListing listing;
	QString error;
	QVERIFY2(m_provider.GetEntries(query, listing, error), qPrintable(error));

	QCOMPARE(listing.entries.size(), 1);
	QCOMPARE(listing.totalCount, driveCount);
	QCOMPARE(listing.hasMore, driveCount > 1);

	ConfigureProvider(m_tempDir.path(), true, false);
}


void CFileSystemStructureProviderTest::NameFilterMatchesSubstringTest()
{
	imtfile::IFileSystemStructureProvider::FileSystemQuery query;
	query.nameFilter = QStringLiteral("other");

	imtfile::IFileSystemStructureProvider::FileSystemListing listing;
	QString error;
	QVERIFY2(m_provider.GetEntries(query, listing, error), qPrintable(error));

	QCOMPARE(listing.totalCount, 1);
	QCOMPARE(listing.entries.size(), 1);
	QCOMPARE(listing.entries.first().name, QStringLiteral("other.txt"));
	QVERIFY(!listing.hasMore);
}


void CFileSystemStructureProviderTest::NameFilterAffectsTotalCountTest()
{
	// Without filter the root has at least sub/ + other.txt.
	imtfile::IFileSystemStructureProvider::FileSystemQuery allQuery;
	imtfile::IFileSystemStructureProvider::FileSystemListing allListing;
	QString error;
	QVERIFY2(m_provider.GetEntries(allQuery, allListing, error), qPrintable(error));
	QVERIFY(allListing.totalCount >= 2);

	imtfile::IFileSystemStructureProvider::FileSystemQuery filteredQuery;
	filteredQuery.nameFilter = QStringLiteral("no-such-entry-xyz");
	imtfile::IFileSystemStructureProvider::FileSystemListing filteredListing;
	QVERIFY2(m_provider.GetEntries(filteredQuery, filteredListing, error), qPrintable(error));
	QCOMPARE(filteredListing.totalCount, 0);
	QVERIFY(filteredListing.entries.isEmpty());
	QVERIFY(!filteredListing.hasMore);
}


void CFileSystemStructureProviderTest::NameFilterMatchesGlobStarTest()
{
	// Explorer-style "*.txt" must match fixture "other.txt" (and not require
	// users to write a PCRE pattern like .*\.txt$).
	imtfile::IFileSystemStructureProvider::FileSystemQuery query;
	query.nameFilter = QStringLiteral("*.txt");

	imtfile::IFileSystemStructureProvider::FileSystemListing listing;
	QString error;
	QVERIFY2(m_provider.GetEntries(query, listing, error), qPrintable(error));

	QVERIFY2(listing.totalCount >= 1, "Expected at least other.txt from the fixture");
	bool foundOther = false;
	for (const auto& entry : listing.entries){
		QVERIFY2(entry.name.endsWith(QStringLiteral(".txt"), Qt::CaseInsensitive),
				qPrintable(entry.name));
		if (entry.name == QStringLiteral("other.txt")){
			foundOther = true;
		}
	}
	QVERIFY(foundOther);
}


void CFileSystemStructureProviderTest::NameFilterMatchesDelimitedRegexTest()
{
	// Full regex is opt-in via /pattern/ so globs stay the default UX.
	// Anchored: only the fixture file "other.txt".
	imtfile::IFileSystemStructureProvider::FileSystemQuery query;
	query.nameFilter = QStringLiteral(R"(/^other\.txt$/)");

	imtfile::IFileSystemStructureProvider::FileSystemListing listing;
	QString error;
	QVERIFY2(m_provider.GetEntries(query, listing, error), qPrintable(error));

	QCOMPARE(listing.totalCount, 1);
	QCOMPARE(listing.entries.size(), 1);
	QCOMPARE(listing.entries.first().name, QStringLiteral("other.txt"));
	QVERIFY(!listing.hasMore);
}


void CFileSystemStructureProviderTest::NameFilterGlobTreatsMetacharactersLiterallyTest()
{
	// In glob mode '[' is literal (escaped), not a character-class opener.
	CreateFile(QStringLiteral("file[a.txt"));

	imtfile::IFileSystemStructureProvider::FileSystemQuery query;
	query.nameFilter = QStringLiteral("file[a");

	imtfile::IFileSystemStructureProvider::FileSystemListing listing;
	QString error;
	QVERIFY2(m_provider.GetEntries(query, listing, error), qPrintable(error));

	QCOMPARE(listing.totalCount, 1);
	QCOMPARE(listing.entries.size(), 1);
	QCOMPARE(listing.entries.first().name, QStringLiteral("file[a.txt"));
}


void CFileSystemStructureProviderTest::ExtensionFilterMatchesSuffixTest()
{
	CreateFile(QStringLiteral("tool.exe"));
	CreateFile(QStringLiteral("readme.md"));

	imtfile::IFileSystemStructureProvider::FileSystemQuery query;
	query.extensions = QStringList{QStringLiteral("exe")};

	imtfile::IFileSystemStructureProvider::FileSystemListing listing;
	QString error;
	QVERIFY2(m_provider.GetEntries(query, listing, error), qPrintable(error));

	bool foundExe = false;
	for (const auto& entry : listing.entries){
		if (entry.type == imtfile::IFileSystemStructureProvider::FileSystemEntry::Type::File){
			QVERIFY2(entry.name.endsWith(QStringLiteral(".exe"), Qt::CaseInsensitive),
					qPrintable(entry.name));
			if (entry.name == QStringLiteral("tool.exe")){
				foundExe = true;
			}
		}
	}
	QVERIFY(foundExe);
}


void CFileSystemStructureProviderTest::ExtensionFilterStarAllowsExtensionlessTest()
{
	CreateFile(QStringLiteral("myservice"));
	CreateFile(QStringLiteral("notes.txt"));

	imtfile::IFileSystemStructureProvider::FileSystemQuery query;
	// "*" = extensionless only for this assertion (no other suffixes).
	query.extensions = QStringList{QStringLiteral("*")};

	imtfile::IFileSystemStructureProvider::FileSystemListing listing;
	QString error;
	QVERIFY2(m_provider.GetEntries(query, listing, error), qPrintable(error));

	bool foundBinary = false;
	for (const auto& entry : listing.entries){
		if (entry.type == imtfile::IFileSystemStructureProvider::FileSystemEntry::Type::File){
			QVERIFY2(QFileInfo(entry.name).suffix().isEmpty(), qPrintable(entry.name));
			if (entry.name == QStringLiteral("myservice")){
				foundBinary = true;
			}
		}
	}
	QVERIFY(foundBinary);
}


void CFileSystemStructureProviderTest::SortByNameDescendingTest()
{
	imtfile::IFileSystemStructureProvider::FileSystemQuery query;
	query.sortBy = imtfile::IFileSystemStructureProvider::SortBy::Name;
	query.sortAscending = false;

	imtfile::IFileSystemStructureProvider::FileSystemListing listing;
	QString error;
	QVERIFY2(m_provider.GetEntries(query, listing, error), qPrintable(error));
	QVERIFY(listing.entries.size() >= 2);

	// Dirs first regardless of direction; file names within the file group Z→A.
	bool sawFile = false;
	QStringList fileNames;
	for (const auto& entry : listing.entries){
		const bool isFile =
				entry.type == imtfile::IFileSystemStructureProvider::FileSystemEntry::Type::File;
		if (isFile){
			sawFile = true;
			fileNames.append(entry.name);
		}
		else{
			QVERIFY2(!sawFile, "Containers must stay above files after sort");
		}
	}

	if (fileNames.size() >= 2){
		for (int i = 1; i < fileNames.size(); ++i){
			QVERIFY(QString::compare(fileNames.at(i - 1), fileNames.at(i), Qt::CaseInsensitive) >= 0);
		}
	}
}


void CFileSystemStructureProviderTest::SortBySizeFilesAfterDirsTest()
{
	CreateFile(QStringLiteral("tiny.bin"), QByteArray(1, 'a'));
	CreateFile(QStringLiteral("big.bin"), QByteArray(64, 'b'));

	imtfile::IFileSystemStructureProvider::FileSystemQuery query;
	query.sortBy = imtfile::IFileSystemStructureProvider::SortBy::Size;
	query.sortAscending = true;

	imtfile::IFileSystemStructureProvider::FileSystemListing listing;
	QString error;
	QVERIFY2(m_provider.GetEntries(query, listing, error), qPrintable(error));

	// First non-container entries should be ordered by size ascending.
	QList<qint64> fileSizes;
	bool sawFile = false;
	for (const auto& entry : listing.entries){
		const bool isFile =
				entry.type == imtfile::IFileSystemStructureProvider::FileSystemEntry::Type::File;
		if (isFile){
			sawFile = true;
			fileSizes.append(entry.size);
		}
		else{
			QVERIFY2(!sawFile, "Dirs/symlinks must stay above files when sorting by size");
		}
	}

	QVERIFY(fileSizes.size() >= 2);
	for (int i = 1; i < fileSizes.size(); ++i){
		QVERIFY(fileSizes.at(i - 1) <= fileSizes.at(i));
	}
}


// private methods

void CFileSystemStructureProviderTest::ConfigureProvider(
			const QString& rootPath,
			bool showFiles,
			bool showHidden)
{
	m_provider.SetRootPath(rootPath);
	m_provider.SetShowFiles(showFiles);
	m_provider.SetShowHidden(showHidden);
	// Sandboxed by default: whole-file-system tests opt in explicitly, so test
	// order can never leak the unrestricted mode into a sandbox assertion.
	m_provider.SetAllowWholeFileSystem(false);
}


QString CFileSystemStructureProviderTest::CreateSubDir(const QString& relativePath)
{
	const QString fullPath = m_tempDir.path() + QLatin1Char('/') + relativePath;
	QDir().mkpath(fullPath);

	return fullPath;
}


QString CFileSystemStructureProviderTest::CreateFile(const QString& relativePath, const QByteArray& content)
{
	const QString fullPath = m_tempDir.path() + QLatin1Char('/') + relativePath;
	QFileInfo info(fullPath);
	QDir().mkpath(info.absolutePath());

	QFile file(fullPath);
	const bool opened = file.open(QIODevice::WriteOnly | QIODevice::Truncate);
	Q_ASSERT(opened);
	file.write(content);
	file.close();

	return fullPath;
}
