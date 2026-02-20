// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QObject>
#include <QtTest/QtTest>

// ACF includes
#include <itest/CStandardTestExecutor.h>

// ImtCore includes
#include <imt3d/CDepthBitmap.h>


/**
	Unit test for CDepthBitmap class.
*/
class CDepthBitmapTest: public QObject
{
	Q_OBJECT

private slots:
	void initTestCase();

	void testDefaultConstructor();
	void testCreateDepthBitmap();
	void testGetDepthRange();
	void testSetDepthRange();
	void testGetSize();
	void testGetPixelValue();
	void testSetCalibration3d();
	void testGetCalibration3d();
	void testSetReferenceBitmap();
	void testGetReferenceBitmap();
	void testResetReferenceBitmap();
	void testSerialization();
	void testCopyFrom();
	void testIsEqual();
	void testCloneMe();
	void testResetData();

	void cleanupTestCase();
};


