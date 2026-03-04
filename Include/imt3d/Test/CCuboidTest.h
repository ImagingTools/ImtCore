// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QObject>
#include <QtTest/QtTest>

// ACF includes
#include <itest/CStandardTestExecutor.h>

// ImtCore includes
#include <imt3d/CCuboid.h>


/**
	Unit test for CCuboid class.
*/
class CCuboidTest: public QObject
{
	Q_OBJECT

private slots:
	void initTestCase();

	void testDefaultConstructor();
	void testConstructorWithValues();
	void testGetSetLeft();
	void testGetSetRight();
	void testGetSetTop();
	void testGetSetBottom();
	void testGetSetNear();
	void testGetSetFar();
	void testGetWidth();
	void testGetHeight();
	void testGetDepth();
	void testGetCenterPoint();
	void testExpandToPoint();
	void testGetSetHorizontalRange();
	void testGetSetVerticalRange();
	void testGetSetDepthRange();
	void testCornerPoints();
	void testIsEmpty();
	void testIntersects();
	void testContains();

	void cleanupTestCase();
};


