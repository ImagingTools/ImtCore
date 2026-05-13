// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QObject>
#include <QtTest/QtTest>

// ACF includes
#include <itest/CStandardTestExecutor.h>

// ImtCore includes
#include <imt3d/CPointCloud3d.h>


/**
	Unit test for CPointCloud3d class.
*/
class CPointCloud3dTest: public QObject
{
	Q_OBJECT

private slots:
	void initTestCase();

	void testCreateCloudXyz32();
	void testCreateCloudXyz64();
	void testCreateCloudWithData();
	void testInsertPoints();
	void testGetPointsCount();
	void testGetPointFormat();
	void testGetPointData();
	void testGetBoundingCuboid();
	void testGridOperations();
	void testSerialization();
	void testCopyFrom();
	void testIsEqual();
	void testCloneMe();
	void testResetData();

	void cleanupTestCase();
};


