// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QObject>
#include <QtTest/QtTest>

// ACF includes
#include <itest/CStandardTestExecutor.h>


/**
	Tests of CFileTools: only regular files are read, nothing is waited for.
*/
class CFileToolsTest: public QObject
{
	Q_OBJECT
private slots:
	void DigestOfRegularFileTest();
	void FolderAndMissingFileHaveNoDigestTest();
	void LinkIsNotFollowedTest();
	void SpecialFileIsNotWaitedForTest();
};


