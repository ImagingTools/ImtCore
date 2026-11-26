// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QObject>
#include <QtTest/QtTest>

// ACF includes
#include <itest/CStandardTestExecutor.h>


/**
	Tests of CFileSystemQueryTools: the filters, the sorting and the page of a file system query.
*/
class CFileSystemQueryToolsTest: public QObject
{
	Q_OBJECT
private slots:
	void ContainersComeFirstInEveryOrderTest();
	void SortBySizeAndDateTest();
	void DrivesSortByCapacityTest();
	void NameFilterTest_data();
	void NameFilterTest();
	void ExtensionFilterKeepsContainersTest_data();
	void ExtensionFilterKeepsContainersTest();
	void PageCountsTheFilteredEntriesTest();
};
