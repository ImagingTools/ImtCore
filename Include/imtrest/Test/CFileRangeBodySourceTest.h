// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QObject>
#include <QtTest/QtTest>

// ACF includes
#include <itest/CStandardTestExecutor.h>


class CFileRangeBodySourceTest: public QObject
{
	Q_OBJECT
private slots:
	// The range
	void RangeTest_data();
	void RangeTest();
	void PartSizeTest();
	void EmptyRangeTest();
	void LargeFileTest();
	void ReadsFromDevicePositionTest();

	// The device that cannot be used
	void CreateRejectsInvalidDeviceTest();
	void CreateRejectsInvalidRangeTest_data();
	void CreateRejectsInvalidRangeTest();
	void CreateForFileTest();

	// Failures and the end
	void FileBecomesShorterTest();
	void NonPositiveMaxSizeTest();
	void AbortReleasesDeviceTest();
	void EndReleasesDeviceTest();
	void DestructorReleasesDeviceTest();
};


