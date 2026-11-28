// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include "CFileSystemQueryToolsTest.h"


// ImtCore includes
#include <imtfile/CFileSystemQueryTools.h>


namespace
{


typedef imtfile::IFileSystemStructureProvider::FileSystemEntry FileSystemEntry;
typedef imtfile::IFileSystemStructureProvider::FileSystemQuery FileSystemQuery;
typedef imtfile::IFileSystemStructureProvider::FileSystemListing FileSystemListing;
typedef imtfile::IFileSystemStructureProvider::SortBy SortBy;


FileSystemEntry MakeEntry(const QString& name, FileSystemEntry::Type type, qint64 size = 0, int dayOfMonth = 0)
{
	FileSystemEntry entry;
	entry.name = name;
	entry.path = QStringLiteral("/folder/") + name;
	entry.type = type;
	entry.size = size;
	if (dayOfMonth > 0){
		entry.lastModified = QDateTime(QDate(2026, 1, dayOfMonth), QTime(12, 0));
	}

	return entry;
}


FileSystemEntry MakeDrive(const QString& name, qint64 totalBytes, qint64 freeBytes)
{
	FileSystemEntry entry = MakeEntry(name, FileSystemEntry::Type::Drive);
	entry.totalBytes = totalBytes;
	entry.freeBytes = freeBytes;

	return entry;
}


// Two folders and four files of different names, sizes and dates
QList<FileSystemEntry> MakeFolderContent()
{
	return QList<FileSystemEntry>{
		MakeEntry(QStringLiteral("file10.txt"), FileSystemEntry::Type::File, 300, 3),
		MakeEntry(QStringLiteral("Docs"), FileSystemEntry::Type::Dir),
		MakeEntry(QStringLiteral("Setup.EXE"), FileSystemEntry::Type::File, 200, 2),
		MakeEntry(QStringLiteral("file2.txt"), FileSystemEntry::Type::File, 100, 1),
		MakeEntry(QStringLiteral("bin"), FileSystemEntry::Type::Dir),
		MakeEntry(QStringLiteral("service"), FileSystemEntry::Type::File, 50, 4)};
}


QStringList GetNames(const FileSystemListing& listing)
{
	QStringList retVal;
	for (const FileSystemEntry& entry : listing.entries){
		retVal.append(entry.name);
	}

	return retVal;
}


QStringList ApplyToFolderContent(const FileSystemQuery& query)
{
	FileSystemListing listing;
	imtfile::CFileSystemQueryTools::ApplyQuery(MakeFolderContent(), query, listing);

	return GetNames(listing);
}


} // namespace


// Containers stay above the files whatever the direction; names compare without case and by numbers
void CFileSystemQueryToolsTest::ContainersComeFirstInEveryOrderTest()
{
	FileSystemQuery query;
	QCOMPARE(ApplyToFolderContent(query), (QStringList{"bin", "Docs", "file2.txt", "file10.txt", "service", "Setup.EXE"}));

	query.sortAscending = false;
	QCOMPARE(ApplyToFolderContent(query), (QStringList{"Docs", "bin", "Setup.EXE", "service", "file10.txt", "file2.txt"}));
}


void CFileSystemQueryToolsTest::SortBySizeAndDateTest()
{
	FileSystemQuery query;
	query.sortBy = SortBy::Size;
	QCOMPARE(ApplyToFolderContent(query), (QStringList{"bin", "Docs", "service", "file2.txt", "Setup.EXE", "file10.txt"}));

	query.sortBy = SortBy::Date;
	QCOMPARE(ApplyToFolderContent(query), (QStringList{"bin", "Docs", "file2.txt", "Setup.EXE", "file10.txt", "service"}));
}


// A drive has no size and no date of its own: it sorts by its total and by its free space, as its columns show them
void CFileSystemQueryToolsTest::DrivesSortByCapacityTest()
{
	const QList<FileSystemEntry> drives{
		MakeDrive(QStringLiteral("C:"), 100, 10),
		MakeDrive(QStringLiteral("D:"), 50, 40)};

	FileSystemQuery query;
	FileSystemListing listing;
	query.sortBy = SortBy::Size;
	imtfile::CFileSystemQueryTools::ApplyQuery(drives, query, listing);
	QCOMPARE(GetNames(listing), (QStringList{"D:", "C:"}));

	query.sortBy = SortBy::Date;
	imtfile::CFileSystemQueryTools::ApplyQuery(drives, query, listing);
	QCOMPARE(GetNames(listing), (QStringList{"C:", "D:"}));
}


void CFileSystemQueryToolsTest::NameFilterTest_data()
{
	QTest::addColumn<QString>("nameFilter");
	QTest::addColumn<QStringList>("names");

	QTest::newRow("substring") << QStringLiteral("FILE") << QStringList{"file2.txt", "file10.txt"};
	QTest::newRow("glob") << QStringLiteral("*.exe") << QStringList{"Setup.EXE"};
	QTest::newRow("one character") << QStringLiteral("fi?e2") << QStringList{"file2.txt"};
	QTest::newRow("regular expression") << QStringLiteral("/^file\\d+\\.txt$/") << QStringList{"file2.txt", "file10.txt"};
	QTest::newRow("folders as well") << QStringLiteral("o") << QStringList{"Docs"};
}


// The name filter applies to every entry, containers included
void CFileSystemQueryToolsTest::NameFilterTest()
{
	QFETCH(QString, nameFilter);
	QFETCH(QStringList, names);

	FileSystemQuery query;
	query.nameFilter = nameFilter;
	QCOMPARE(ApplyToFolderContent(query), names);
}


void CFileSystemQueryToolsTest::ExtensionFilterKeepsContainersTest_data()
{
	QTest::addColumn<QStringList>("extensions");
	QTest::addColumn<QStringList>("names");

	QTest::newRow("other case") << QStringList{"exe"} << QStringList{"bin", "Docs", "Setup.EXE"};
	QTest::newRow("leading dot") << QStringList{".TXT"} << QStringList{"bin", "Docs", "file2.txt", "file10.txt"};
	QTest::newRow("no extension") << QStringList{"none"} << QStringList{"bin", "Docs", "service"};
	QTest::newRow("no extension or exe") << QStringList{"*", "exe"} << QStringList{"bin", "Docs", "service", "Setup.EXE"};
}


// The extension filter drops files only: the folders stay navigable
void CFileSystemQueryToolsTest::ExtensionFilterKeepsContainersTest()
{
	QFETCH(QStringList, extensions);
	QFETCH(QStringList, names);

	FileSystemQuery query;
	query.extensions = extensions;
	QCOMPARE(ApplyToFolderContent(query), names);
}


// The page is cut from the filtered and sorted entries; totalCount and hasMore describe them
void CFileSystemQueryToolsTest::PageCountsTheFilteredEntriesTest()
{
	FileSystemQuery query;
	query.extensions = QStringList{"txt"};
	query.offset = 1;
	query.limit = 2;

	FileSystemListing listing;
	imtfile::CFileSystemQueryTools::ApplyQuery(MakeFolderContent(), query, listing);
	QCOMPARE(GetNames(listing), (QStringList{"Docs", "file2.txt"}));
	QCOMPARE(listing.totalCount, 4);
	QVERIFY(listing.hasMore);

	query.offset = 3;
	imtfile::CFileSystemQueryTools::ApplyQuery(MakeFolderContent(), query, listing);
	QCOMPARE(GetNames(listing), QStringList{"file10.txt"});
	QCOMPARE(listing.totalCount, 4);
	QVERIFY(!listing.hasMore);

	query.offset = -1;
	query.limit = 0;
	imtfile::CFileSystemQueryTools::ApplyQuery(MakeFolderContent(), query, listing);
	QCOMPARE(GetNames(listing), (QStringList{"bin", "Docs", "file2.txt", "file10.txt"}));
	QVERIFY(!listing.hasMore);
}


I_ADD_TEST(CFileSystemQueryToolsTest);
