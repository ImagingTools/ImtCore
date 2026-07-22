// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QObject>
#include <QtCore/QTemporaryDir>
#include <QtTest/QtTest>

// ImtCore includes
#include <imtfile/CFileSystemStructureProviderComp.h>


/**
	Testable local provider: overrides attribute-backed configuration
	so unit tests do not require a full ACF component host.
 */
class CTestableFileSystemStructureProvider: public imtfile::CFileSystemStructureProviderComp
{
public:
	void SetRootPath(const QString& rootPath)
	{
		m_rootPath = rootPath;
	}

	void SetShowFiles(bool showFiles)
	{
		m_showFiles = showFiles;
	}

	void SetShowHidden(bool showHidden)
	{
		m_showHidden = showHidden;
	}

	void SetAllowWholeFileSystem(bool allowWholeFileSystem)
	{
		m_allowWholeFileSystem = allowWholeFileSystem;
	}

protected:
	virtual QString GetRootPath() const override
	{
		return QDir(m_rootPath).absolutePath();
	}

	virtual bool IsShowFiles() const override
	{
		return m_showFiles;
	}

	virtual bool IsShowHidden() const override
	{
		return m_showHidden;
	}

	virtual bool IsWholeFileSystemAllowed() const override
	{
		return m_allowWholeFileSystem;
	}

private:
	QString m_rootPath;
	bool m_showFiles = true;
	bool m_showHidden = false;
	bool m_allowWholeFileSystem = false;
};


class CFileSystemStructureProviderTest: public QObject
{
	Q_OBJECT

private slots:
	void initTestCase();
	void cleanupTestCase();

	void EmptyPathResolvesToRootTest();
	void PathTraversalOutsideRootDeniedTest();
	void AbsolutePathOutsideRootDeniedTest();
	void ExistingSubfolderListingTest();
	void NonExistingPathFailsTest();
	void PaginationTest();
	void StatExistingFileTest();
	void StatOutsideRootDeniedTest();
	void EmptyRootFailsTest();

	// Whole-file-system mode (AllowWholeFileSystem).
	void WholeFsSandboxRootHasNoParentTest();
	void WholeFsEmptyPathListsDrivesTest();
	void WholeFsDrivesHaveNamesTest();
	void WholeFsDriveRootParentIsDrivesRootTest();
	void WholeFsOutsideRootIsAllowedTest();
	void WholeFsNonExistingPathStillFailsTest();
	void WholeFsDrivesPaginationTest();

	// Server-side nameFilter / sortBy (applied before pagination).
	void NameFilterMatchesSubstringTest();
	void NameFilterAffectsTotalCountTest();
	void NameFilterMatchesGlobStarTest();
	void NameFilterMatchesDelimitedRegexTest();
	void NameFilterGlobTreatsMetacharactersLiterallyTest();
	void ExtensionFilterMatchesSuffixTest();
	void ExtensionFilterStarAllowsExtensionlessTest();
	void SortByNameDescendingTest();
	void SortBySizeFilesAfterDirsTest();

private:
	void ConfigureProvider(const QString& rootPath, bool showFiles = true, bool showHidden = false);
	QString CreateSubDir(const QString& relativePath);
	QString CreateFile(const QString& relativePath, const QByteArray& content = "x");

private:
	QTemporaryDir m_tempDir;
	CTestableFileSystemStructureProvider m_provider;
};
