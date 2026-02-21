// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include "CCuboidTest.h"


// Qt includes
#include <QtTest/QtTest>

// ACF includes
#include <itest/CStandardTestExecutor.h>

// ImtCore includes
#include <imt3d/CCuboid.h>


void CCuboidTest::initTestCase()
{
}


void CCuboidTest::testDefaultConstructor()
{
	imt3d::CCuboid cuboid;
	
	QCOMPARE(cuboid.GetLeft(), 0.0);
	QCOMPARE(cuboid.GetRight(), 0.0);
	QCOMPARE(cuboid.GetBottom(), 0.0);
	QCOMPARE(cuboid.GetTop(), 0.0);
	QCOMPARE(cuboid.GetNear(), 0.0);
	QCOMPARE(cuboid.GetFar(), 0.0);
}


void CCuboidTest::testConstructorWithValues()
{
	imt3d::CCuboid cuboid(1.0, 10.0, 2.0, 20.0, 3.0, 30.0);
	
	QCOMPARE(cuboid.GetLeft(), 1.0);
	QCOMPARE(cuboid.GetRight(), 10.0);
	QCOMPARE(cuboid.GetBottom(), 2.0);
	QCOMPARE(cuboid.GetTop(), 20.0);
	QCOMPARE(cuboid.GetNear(), 3.0);
	QCOMPARE(cuboid.GetFar(), 30.0);
}


void CCuboidTest::testGetSetLeft()
{
	imt3d::CCuboid cuboid;
	cuboid.SetLeft(5.5);
	QCOMPARE(cuboid.GetLeft(), 5.5);
}


void CCuboidTest::testGetSetRight()
{
	imt3d::CCuboid cuboid;
	cuboid.SetRight(15.5);
	QCOMPARE(cuboid.GetRight(), 15.5);
}


void CCuboidTest::testGetSetTop()
{
	imt3d::CCuboid cuboid;
	cuboid.SetTop(25.5);
	QCOMPARE(cuboid.GetTop(), 25.5);
}


void CCuboidTest::testGetSetBottom()
{
	imt3d::CCuboid cuboid;
	cuboid.SetBottom(35.5);
	QCOMPARE(cuboid.GetBottom(), 35.5);
}


void CCuboidTest::testGetSetNear()
{
	imt3d::CCuboid cuboid;
	cuboid.SetNear(45.5);
	QCOMPARE(cuboid.GetNear(), 45.5);
}


void CCuboidTest::testGetSetFar()
{
	imt3d::CCuboid cuboid;
	cuboid.SetFar(55.5);
	QCOMPARE(cuboid.GetFar(), 55.5);
}


void CCuboidTest::testGetWidth()
{
	imt3d::CCuboid cuboid(0.0, 10.0, 0.0, 5.0, 0.0, 3.0);
	QCOMPARE(cuboid.GetWidth(), 10.0);
}


void CCuboidTest::testGetHeight()
{
	imt3d::CCuboid cuboid(0.0, 10.0, 0.0, 5.0, 0.0, 3.0);
	QCOMPARE(cuboid.GetHeight(), 5.0);
}


void CCuboidTest::testGetDepth()
{
	imt3d::CCuboid cuboid(0.0, 10.0, 0.0, 5.0, 0.0, 3.0);
	QVERIFY(qAbs(cuboid.GetDepth() - 3.0) <= I_BIG_EPSILON);
}


void CCuboidTest::testGetCenterPoint()
{
	imt3d::CCuboid cuboid(0.0, 10.0, 0.0, 20.0, 0.0, 30.0);
	i3d::CVector3d center = cuboid.GetCenterPoint();
	
	QCOMPARE(center.GetX(), 5.0);
	QCOMPARE(center.GetY(), 10.0);
	QCOMPARE(center.GetZ(), 15.0);
}


void CCuboidTest::testExpandToPoint()
{
	imt3d::CCuboid cuboid(0.0, 10.0, 0.0, 10.0, 0.0, 10.0);
	
	// Expand with point outside cuboid
	i3d::CVector3d pointOutside(15.0, 20.0, 25.0);
	cuboid.ExpandToPoint(pointOutside);
	
	QCOMPARE(cuboid.GetLeft(), 0.0);
	QCOMPARE(cuboid.GetRight(), 15.0);
	QCOMPARE(cuboid.GetBottom(), 0.0);
	QCOMPARE(cuboid.GetTop(), 20.0);
	QCOMPARE(cuboid.GetNear(), 0.0);
	QCOMPARE(cuboid.GetFar(), 25.0);
	
	// Expand with point inside cuboid (should not change)
	i3d::CVector3d pointInside(5.0, 10.0, 15.0);
	cuboid.ExpandToPoint(pointInside);
	
	QCOMPARE(cuboid.GetRight(), 15.0);
	QCOMPARE(cuboid.GetTop(), 20.0);
	QCOMPARE(cuboid.GetFar(), 25.0);
}


void CCuboidTest::testGetSetHorizontalRange()
{
	imt3d::CCuboid cuboid;
	istd::CRange hRange(2.0, 12.0);
	
	cuboid.SetHorizontalRange(hRange);
	const istd::CRange& retrievedRange = cuboid.GetHorizontalRange();
	
	QCOMPARE(retrievedRange.GetMinValue(), 2.0);
	QCOMPARE(retrievedRange.GetMaxValue(), 12.0);
}


void CCuboidTest::testGetSetVerticalRange()
{
	imt3d::CCuboid cuboid;
	istd::CRange vRange(3.0, 13.0);
	
	cuboid.SetVerticalRange(vRange);
	const istd::CRange& retrievedRange = cuboid.GetVerticalRange();
	
	QCOMPARE(retrievedRange.GetMinValue(), 3.0);
	QCOMPARE(retrievedRange.GetMaxValue(), 13.0);
}


void CCuboidTest::testGetSetDepthRange()
{
	imt3d::CCuboid cuboid;
	istd::CRange dRange(4.0, 14.0);
	
	cuboid.SetDeepRange(dRange);
	const istd::CRange& retrievedRange = cuboid.GetDeepRange();
	
	QCOMPARE(retrievedRange.GetMinValue(), 4.0);
	QCOMPARE(retrievedRange.GetMaxValue(), 14.0);
}


void CCuboidTest::testCornerPoints()
{
	imt3d::CCuboid cuboid(0.0, 10.0, 5.0, 15.0, 20.0, 30.0);
	
	// Test left-bottom-near corner
	i3d::CVector3d lbn = cuboid.GetLeftBottomNear();
	QCOMPARE(lbn.GetX(), 0.0);
	QCOMPARE(lbn.GetY(), 5.0);
	QCOMPARE(lbn.GetZ(), 20.0);  // near value
	
	// Test right-top-far corner
	i3d::CVector3d rtf = cuboid.GetRightTopFar();
	QCOMPARE(rtf.GetX(), 10.0);
	QCOMPARE(rtf.GetY(), 15.0);
	QCOMPARE(rtf.GetZ(), 30.0);  // far value
	
	// Test setting corner points
	i3d::CVector3d newCorner(1.0, 6.0, 25.0);
	cuboid.SetLeftBottomNear(newCorner);
	
	QCOMPARE(cuboid.GetLeft(), 1.0);
	QCOMPARE(cuboid.GetBottom(), 6.0);
	QCOMPARE(cuboid.GetNear(), 25.0);  // SetLeftBottomNear sets near value
}


void CCuboidTest::testIsEmpty()
{
	imt3d::CCuboid emptyCuboid = imt3d::CCuboid::GetEmpty();
	QVERIFY(emptyCuboid.GetWidth() == 0.0);
	QVERIFY(emptyCuboid.GetHeight() == 0.0);
	QVERIFY(emptyCuboid.GetDepth() == 0.0);
	
	imt3d::CCuboid nonEmptyCuboid(0.0, 10.0, 0.0, 10.0, 0.0, 10.0);
	QVERIFY(nonEmptyCuboid.GetWidth() > 0.0);
}


void CCuboidTest::testIntersects()
{
	imt3d::CCuboid cuboid1(0.0, 10.0, 0.0, 10.0, 0.0, 10.0);
	imt3d::CCuboid cuboid2(5.0, 15.0, 5.0, 15.0, 5.0, 15.0);
	imt3d::CCuboid cuboid3(20.0, 30.0, 20.0, 30.0, 20.0, 30.0);
	
	// Cuboid1 and Cuboid2 should intersect
	QVERIFY(cuboid1.Intersects(cuboid2));
	QVERIFY(cuboid2.Intersects(cuboid1));
	
	// Cuboid1 and Cuboid3 should not intersect
	QVERIFY(!cuboid1.Intersects(cuboid3));
	QVERIFY(!cuboid3.Intersects(cuboid1));
}


void CCuboidTest::testContains()
{
	imt3d::CCuboid cuboid(0.0, 10.0, 0.0, 10.0, 0.0, 10.0);
	
	// Point inside
	i3d::CVector3d pointInside(5.0, 5.0, 5.0);
	QVERIFY(cuboid.Contains(pointInside));
	
	// Point on boundary
	i3d::CVector3d pointOnBoundary(0.0, 5.0, 5.0);
	QVERIFY(cuboid.Contains(pointOnBoundary));
	
	// Point outside
	i3d::CVector3d pointOutside(15.0, 15.0, 15.0);
	QVERIFY(!cuboid.Contains(pointOutside));
}


void CCuboidTest::cleanupTestCase()
{
}


I_ADD_TEST(CCuboidTest);


