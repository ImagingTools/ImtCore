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
	void SaveCreatesPrivateFolderAndFileTest();
	void SaveReplacesSharedFileTest();
	void SaveKeepsFileOnFailureTest();
	void OtherFolderIsRefusedTest();
	void ReplaceableParentFolderIsRefusedTest();
	void LoadRefusesSharedFileTest();
	void LoadRefusesSharedFolderTest();
	void AutoPersistenceStoresInPlaceTest();
	void AutoPersistenceReloadTest();
	void SystemConfigPathTest();
};


