// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QObject>
#include <QtTest/QtTest>

// ACF includes
#include <itest/CStandardTestExecutor.h>

// ImtCore includes
#include <imt3d/CMesh3d.h>


/**
	Unit test for CMesh3d class.
*/
class CMesh3dTest: public QObject
{
	Q_OBJECT

private slots:
	void initTestCase();

	void testCreateMesh();
	void testCreateMeshWithData();
	void testInsertData();
	void testGetIndices();
	void testGetPointsCount();
	void testSerialization();
	void testCopyFrom();
	void testIsEqual();
	void testCloneMe();
	void testResetData();

	void cleanupTestCase();
};


