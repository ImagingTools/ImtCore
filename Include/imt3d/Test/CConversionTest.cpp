// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include "CConversionTest.h"


// Qt includes
#include <QtTest/QtTest>

// ACF includes
#include <itest/CStandardTestExecutor.h>

// ImtCore includes
#include <imt3d/imt3d.h>
#include <imt3d/CDepthBitmap.h>
#include <imt3d/CPointCloud3d.h>


void CConversionTest::initTestCase()
{
}


void CConversionTest::testConvertDepthImageToCloud()
{
	// Create a depth bitmap
	imt3d::CDepthBitmap depthBitmap;
	istd::CRange depthRange(0.0, 100.0);
	istd::CIndex2d size(10, 10);
	depthBitmap.CreateDepthBitmap(depthRange, size);
	
	// Create point cloud
	imt3d::CPointCloud3d pointCloud;
	
	// Perform conversion
	bool result = imt3d::ConvertDepthImageToCloud(depthBitmap, pointCloud);
	
	// Verify conversion result
	QVERIFY(result);
	QVERIFY(!pointCloud.IsEmpty());
	// The point cloud should have points based on the depth image
	QVERIFY(pointCloud.GetPointsCount() >= 0);
}


void CConversionTest::testConvertPointCloudToDepthBitmap()
{
	// Create a point cloud with some test data
	imt3d::CPointCloud3d pointCloud;
	
	float testData[] = {
		0.0f, 0.0f, 10.0f,
		1.0f, 0.0f, 15.0f,
		0.0f, 1.0f, 20.0f,
		1.0f, 1.0f, 25.0f
	};
	
	pointCloud.CreateCloud(imt3d::IPointsBasedObject::PF_XYZ_32, 4, testData);
	pointCloud.SetGridSize(istd::CIndex2d(2, 2));
	
	// Create depth bitmap
	imt3d::CDepthBitmap depthBitmap;
	
	// Perform conversion
	bool result = imt3d::ConvertPointCloudToDepthBitmap(pointCloud, depthBitmap);
	
	// Verify conversion result
	QVERIFY(result);
	QVERIFY(!depthBitmap.IsEmpty());
	// The depth bitmap should have the same grid size as the point cloud
	QCOMPARE(depthBitmap.GetSize().GetX(), 2);
	QCOMPARE(depthBitmap.GetSize().GetY(), 2);
}


void CConversionTest::cleanupTestCase()
{
}


I_ADD_TEST(CConversionTest);


