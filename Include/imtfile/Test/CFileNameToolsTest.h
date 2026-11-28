// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QObject>
#include <QtTest/QtTest>

// ACF includes
#include <itest/CStandardTestExecutor.h>


/**
	Tests of CFileNameTools: the names every common file system takes.
	tst_FileNameTools.qml checks FileNameTools.js with the same cases.
*/
class CFileNameToolsTest: public QObject
{
	Q_OBJECT
private slots:
	void CheckFileNameTest_data();
	void CheckFileNameTest();
};
