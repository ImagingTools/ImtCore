// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QObject>
#include <QtTest/QtTest>

// ACF includes
#include <itest/CStandardTestExecutor.h>

// ImtCore includes
#include <imt3d/CPosition3d.h>


/**
	Unit test for CPosition3d class.
*/
class CPosition3dTest: public QObject
{
	Q_OBJECT

private slots:
	void initTestCase();

	void testDefaultConstructor();
	void testConstructorWithPosition();
	void testGetPosition();
	void testSetPosition();
	void testTranslate();
	void testGetTranslated();
	void testIsEmpty();
	void testGetCenter();
	void testMoveCenterTo();
	void testGetBoundingCuboid();
	void testSerialization();
	void testCopyFrom();
	void testIsEqual();
	void testCloneMe();
	void testResetData();

	void cleanupTestCase();
};


