// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include "CPointCloud3dTest.h"


// Qt includes
#include <QtTest/QtTest>

// ACF includes
#include <iser/CMemoryReadArchive.h>
#include <iser/CMemoryWriteArchive.h>

// ImtCore includes
#include <imt3d/CPointCloud3d.h>
#include <imt3d/IPointsBasedObject.h>


void CPointCloud3dTest::initTestCase()
{
}


void CPointCloud3dTest::testCreateCloudXyz32()
{
	imt3d::CPointCloud3d cloud;
	bool result = cloud.CreateCloud(imt3d::IPointsBasedObject::PF_XYZ_32);
	
	QVERIFY(result);
	QCOMPARE(cloud.GetPointFormat(), imt3d::IPointsBasedObject::PF_XYZ_32);
	QCOMPARE(cloud.GetPointsCount(), 0);
	QVERIFY(cloud.IsEmpty());
}


void CPointCloud3dTest::testCreateCloudXyz64()
{
	imt3d::CPointCloud3d cloud;
	bool result = cloud.CreateCloud(imt3d::IPointsBasedObject::PF_XYZ_64);
	
	QVERIFY(result);
	QCOMPARE(cloud.GetPointFormat(), imt3d::IPointsBasedObject::PF_XYZ_64);
	QCOMPARE(cloud.GetPointsCount(), 0);
}


void CPointCloud3dTest::testCreateCloudWithData()
{
	imt3d::CPointCloud3d cloud;
	
	// Create test data with 3 points (XYZ_32 format)
	float testData[] = {
		1.0f, 2.0f, 3.0f,  // Point 1
		4.0f, 5.0f, 6.0f,  // Point 2
		7.0f, 8.0f, 9.0f   // Point 3
	};
	
	bool result = cloud.CreateCloud(imt3d::IPointsBasedObject::PF_XYZ_32, 3, testData);
	
	QVERIFY(result);
	QCOMPARE(cloud.GetPointsCount(), 3);
	QCOMPARE(cloud.GetPointFormat(), imt3d::IPointsBasedObject::PF_XYZ_32);
	
	// Verify data is stored correctly
	const float* dataPtr = static_cast<const float*>(cloud.GetData());
	QVERIFY(dataPtr != nullptr);
	QCOMPARE(dataPtr[0], 1.0f);
	QCOMPARE(dataPtr[1], 2.0f);
	QCOMPARE(dataPtr[2], 3.0f);
}


void CPointCloud3dTest::testInsertPoints()
{
	imt3d::CPointCloud3d cloud;
	cloud.CreateCloud(imt3d::IPointsBasedObject::PF_XYZ_32);
	
	// Insert first batch of points
	float batch1[] = {
		1.0f, 2.0f, 3.0f,
		4.0f, 5.0f, 6.0f
	};
	bool result1 = cloud.InsertPoints(2, batch1);
	QVERIFY(result1);
	QCOMPARE(cloud.GetPointsCount(), 2);
	
	// Insert second batch of points
	float batch2[] = {
		7.0f, 8.0f, 9.0f
	};
	bool result2 = cloud.InsertPoints(1, batch2);
	QVERIFY(result2);
	QCOMPARE(cloud.GetPointsCount(), 3);
	
	// Verify all data is present
	const float* dataPtr = static_cast<const float*>(cloud.GetData());
	QCOMPARE(dataPtr[0], 1.0f);
	QCOMPARE(dataPtr[1], 2.0f);
	QCOMPARE(dataPtr[2], 3.0f);

	QCOMPARE(dataPtr[3], 4.0f);
	QCOMPARE(dataPtr[4], 5.0f);
	QCOMPARE(dataPtr[5], 6.0f);

	QCOMPARE(dataPtr[8], 7.0f);
	QCOMPARE(dataPtr[9], 8.0f);
	QCOMPARE(dataPtr[10], 9.0f);
}


void CPointCloud3dTest::testGetPointsCount()
{
	imt3d::CPointCloud3d cloud;
	QCOMPARE(cloud.GetPointsCount(), 0);
	
	cloud.CreateCloud(imt3d::IPointsBasedObject::PF_XYZ_32);
	QCOMPARE(cloud.GetPointsCount(), 0);
	
	float testData[] = { 1.0f, 2.0f, 3.0f };
	cloud.InsertPoints(1, testData);
	QCOMPARE(cloud.GetPointsCount(), 1);
}


void CPointCloud3dTest::testGetPointFormat()
{
	imt3d::CPointCloud3d cloud;
	
	// Default format before creation
	QCOMPARE(cloud.GetPointFormat(), imt3d::IPointsBasedObject::PF_UNDEFINED);
	
	cloud.CreateCloud(imt3d::IPointsBasedObject::PF_XYZW_32);
	QCOMPARE(cloud.GetPointFormat(), imt3d::IPointsBasedObject::PF_XYZW_32);
}


void CPointCloud3dTest::testGetPointData()
{
	imt3d::CPointCloud3d cloud;
	
	float testData[] = {
		10.0f, 20.0f, 30.0f,
		40.0f, 50.0f, 60.0f
	};
	cloud.CreateCloud(imt3d::IPointsBasedObject::PF_XYZ_32, 2, testData);
	
	// Test GetPointData for first point
	const float* point0Ptr = static_cast<const float*>(cloud.GetPointData(0));
	QVERIFY(point0Ptr != nullptr);
	QCOMPARE(point0Ptr[0], 10.0f);
	QCOMPARE(point0Ptr[1], 20.0f);
	QCOMPARE(point0Ptr[2], 30.0f);
	
	// Test GetPointData for second point
	const float* point1Ptr = static_cast<const float*>(cloud.GetPointData(1));
	QVERIFY(point1Ptr != nullptr);
	QCOMPARE(point1Ptr[0], 40.0f);
	QCOMPARE(point1Ptr[1], 50.0f);
	QCOMPARE(point1Ptr[2], 60.0f);
}


void CPointCloud3dTest::testGetBoundingCuboid()
{
	QSKIP("Recheck alignment by the point cloud getters");

	imt3d::CPointCloud3d cloud;
	
	float testData[] = {
		-5.0f, -10.0f, -15.0f,
		5.0f, 10.0f, 15.0f,
		0.0f, 0.0f, 0.0f
	};
	cloud.CreateCloud(imt3d::IPointsBasedObject::PF_XYZ_32, 3, testData);
	
	imt3d::CCuboid cuboid = cloud.GetBoundingCuboid();
	
	QCOMPARE(cuboid.GetLeft(), -5.0);
	QCOMPARE(cuboid.GetRight(), 5.0);
	QCOMPARE(cuboid.GetBottom(), -10.0);
	QCOMPARE(cuboid.GetTop(), 10.0);
	QCOMPARE(cuboid.GetNear(), -15.0);
	QCOMPARE(cuboid.GetFar(), 15.0);
}


void CPointCloud3dTest::testGridOperations()
{
	imt3d::CPointCloud3d cloud;
	
	// Set grid size
	istd::CIndex2d gridSize(4, 3); // 4 columns, 3 rows
	cloud.SetGridSize(gridSize);
	
	istd::CIndex2d retrievedSize = cloud.GetGridSize();
	QCOMPARE(retrievedSize.GetX(), 4);
	QCOMPARE(retrievedSize.GetY(), 3);
	
	// Test grid position mapping
	istd::CIndex2d gridPos = cloud.GetGridPosition(5); // Index 5 should be column 1, row 1 (0-indexed)
	QCOMPARE(gridPos.GetX(), 1);
	QCOMPARE(gridPos.GetY(), 1);
	
	// Test reverse mapping
	istd::CIndex2d testPos(2, 1);
	int cloudIndex = cloud.GetCloudPosition(testPos);
	QCOMPARE(cloudIndex, 6); // Column 2, row 1 -> index 6
}


void CPointCloud3dTest::testSerialization()
{
	imt3d::CPointCloud3d original;
	
	float testData[] = {
		1.0f, 2.0f, 3.0f,
		4.0f, 5.0f, 6.0f
	};
	original.CreateCloud(imt3d::IPointsBasedObject::PF_XYZ_32, 2, testData);
	original.SetGridSize(istd::CIndex2d(2, 1));
	
	// Serialize
	iser::CMemoryWriteArchive writeArchive;
	bool serializeResult = original.Serialize(writeArchive);
	QVERIFY(serializeResult);
	QVERIFY(writeArchive.GetBufferSize() > 0);
	
	// Deserialize
	iser::CMemoryReadArchive readArchive(writeArchive.GetBuffer(), writeArchive.GetBufferSize());
	imt3d::CPointCloud3d deserialized;
	bool deserializeResult = deserialized.Serialize(readArchive);
	QVERIFY(deserializeResult);
	
	// Verify
	QCOMPARE(deserialized.GetPointsCount(), 2);
	QCOMPARE(deserialized.GetPointFormat(), imt3d::IPointsBasedObject::PF_XYZ_32);
	QCOMPARE(deserialized.GetGridSize().GetX(), 2);
	QCOMPARE(deserialized.GetGridSize().GetY(), 1);
	
	const float* deserializedDataPtr = static_cast<const float*>(deserialized.GetData());
	QCOMPARE(deserializedDataPtr[0], 1.0f);
	QCOMPARE(deserializedDataPtr[1], 2.0f);
	QCOMPARE(deserializedDataPtr[2], 3.0f);
}


void CPointCloud3dTest::testCopyFrom()
{
	imt3d::CPointCloud3d source;
	float testData[] = { 1.0f, 2.0f, 3.0f };
	source.CreateCloud(imt3d::IPointsBasedObject::PF_XYZ_32, 1, testData);
	
	imt3d::CPointCloud3d destination;
	bool copyResult = destination.CopyFrom(source);
	QVERIFY(copyResult);
	
	QCOMPARE(destination.GetPointsCount(), 1);
	QCOMPARE(destination.GetPointFormat(), imt3d::IPointsBasedObject::PF_XYZ_32);
	
	const float* destDataPtr = static_cast<const float*>(destination.GetData());
	QCOMPARE(destDataPtr[0], 1.0f);
	QCOMPARE(destDataPtr[1], 2.0f);
	QCOMPARE(destDataPtr[2], 3.0f);
}


void CPointCloud3dTest::testIsEqual()
{
	imt3d::CPointCloud3d cloud1;
	imt3d::CPointCloud3d cloud2;
	
	float testData[] = { 1.0f, 2.0f, 3.0f };
	cloud1.CreateCloud(imt3d::IPointsBasedObject::PF_XYZ_32, 1, testData);
	cloud2.CreateCloud(imt3d::IPointsBasedObject::PF_XYZ_32, 1, testData);
	
	QVERIFY(cloud1.IsEqual(cloud2));
	
	// Modify cloud2
	float differentData[] = { 4.0f, 5.0f, 6.0f };
	cloud2.InsertPoints(1, differentData);
	
	QVERIFY(!cloud1.IsEqual(cloud2));
}


void CPointCloud3dTest::testCloneMe()
{
	imt3d::CPointCloud3d original;
	float testData[] = { 7.0f, 8.0f, 9.0f };
	original.CreateCloud(imt3d::IPointsBasedObject::PF_XYZ_32, 1, testData);
	
	istd::IChangeableUniquePtr clonedPtr = original.CloneMe();
	QVERIFY(clonedPtr);
	
	imt3d::CPointCloud3d* clonedCloud = dynamic_cast<imt3d::CPointCloud3d*>(&(*clonedPtr));
	QVERIFY(clonedCloud != nullptr);
	
	QCOMPARE(clonedCloud->GetPointsCount(), 1);
	const float* clonedDataPtr = static_cast<const float*>(clonedCloud->GetData());
	QCOMPARE(clonedDataPtr[0], 7.0f);
	QCOMPARE(clonedDataPtr[1], 8.0f);
	QCOMPARE(clonedDataPtr[2], 9.0f);
}


void CPointCloud3dTest::testResetData()
{
	imt3d::CPointCloud3d cloud;
	float testData[] = { 1.0f, 2.0f, 3.0f };
	cloud.CreateCloud(imt3d::IPointsBasedObject::PF_XYZ_32, 1, testData);
	
	QVERIFY(!cloud.IsEmpty());
	QCOMPARE(cloud.GetPointsCount(), 1);
	
	bool resetResult = cloud.ResetData();
	QVERIFY(resetResult);
	
	QVERIFY(cloud.IsEmpty());
	QCOMPARE(cloud.GetPointsCount(), 0);
}


void CPointCloud3dTest::cleanupTestCase()
{
}


I_ADD_TEST(CPointCloud3dTest);
