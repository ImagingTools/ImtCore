// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QObject>
#include <QtTest/QtTest>

// ACF includes
#include <itest/CStandardTestExecutor.h>


/**
	Tests of CPrivateFilePersistenceComp (the file and the folder of secrets are accessible to their owner only, the data never leave
	the folder), of CPrivateAutoPersistenceComp and of the path of CSystemConfigLocationComp.
*/
class CPrivateFilePersistenceTest: public QObject
{
	Q_OBJECT
private slots:
	void initTestCase();
	void SaveCreatesPrivateFolderAndFileTest();
	void SaveReplacesSharedFileTest();
	void SaveWaitsForBrieflyOpenFileTest();
	void SaveFailsWhileFileIsKeptOpenTest();
	void SaveKeepsFileOnFailureTest();
	void OtherFolderIsRefusedTest();
	void ReplaceableParentFolderIsRefusedTest();
	void LinkInReplaceableFolderIsRefusedTest();
	void InheritedAccessIsRefusedTest();
	void LinkChainThroughReplaceableFolderIsRefusedTest();
	void JunctionIsRefusedTest();
	void SavedFileDigestTest();
	void LoadRefusesSharedFileTest();
	void LoadRefusesSharedFolderTest();
	void LoadOfMissingFileDoesNotReachSlaveTest();
	void LoadRefusesDanglingLinkTest();
	void OperationSupportChecksFileToLoadTest();
	void FolderPathIsRefusedTest();
	void AdministratorsTrustedSaveTest();
	void ParentFoldersIgnoreUmaskTest();
	void NewFolderDropsInheritedAccessTest();
	void AutoPersistenceStoresInPlaceTest();
	void AutoPersistenceReloadTest();
	void AutoPersistenceStoresWholeObjectChangeTest();
	void AutoPersistenceWatchesCreatedFolderTest();
	void AutoPersistenceWatchesFolderCreatedByOtherProgramTest();
	void AutoPersistenceReloadsChangeAfterOwnSaveTest();
	void AutoPersistenceIgnoresSpecialFileTest();
	void AutoPersistenceLoadsFileWithoutDigestTest();
	void AutoPersistenceStoresWithoutDigestProviderTest();
	void AutoPersistenceWarnsWithoutDigestProviderOnlyForReloadTest();
	void AutoPersistenceWithoutLoaderIgnoresChangesTest();
	void SystemConfigPathTest();
};


