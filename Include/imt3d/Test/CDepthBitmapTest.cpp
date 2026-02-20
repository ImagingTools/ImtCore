// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include "CDepthBitmapTest.h"


// Qt includes
#include <QtTest/QtTest>

// ACF includes
#include <iser/CMemoryReadArchive.h>
#include <iser/CMemoryWriteArchive.h>
#include <iimg/CGeneralBitmap.h>

// ImtCore includes
#include <imt3d/CDepthBitmap.h>
#include <imt3d/CImage3dCalibration.h>


void CDepthBitmapTest::initTestCase()
{
}


void CDepthBitmapTest::testDefaultConstructor()
{
	imt3d::CDepthBitmap bitmap;
	
	QVERIFY(bitmap.IsEmpty());
	QCOMPARE(bitmap.GetImageSize().GetX(), 0);
	QCOMPARE(bitmap.GetImageSize().GetY(), 0);
}


void CDepthBitmapTest::testCreateDepthBitmap()
{
	imt3d::CDepthBitmap bitmap;
	
	istd::CRange depthRange(0.0, 100.0);
	istd::CIndex2d size(640, 480);
	
	bool result = bitmap.CreateDepthBitmap(depthRange, size);
	
	QVERIFY(result);
	QVERIFY(!bitmap.IsEmpty());
	QCOMPARE(bitmap.GetImageSize().GetX(), 640);
	QCOMPARE(bitmap.GetImageSize().GetY(), 480);
	QCOMPARE(bitmap.GetDepthRange().GetMinValue(), 0.0);
	QCOMPARE(bitmap.GetDepthRange().GetMaxValue(), 100.0);
}


void CDepthBitmapTest::testGetDepthRange()
{
	imt3d::CDepthBitmap bitmap;
	
	istd::CRange depthRange(10.0, 200.0);
	istd::CIndex2d size(320, 240);
	bitmap.CreateDepthBitmap(depthRange, size);
	
	istd::CRange retrievedRange = bitmap.GetDepthRange();
	QCOMPARE(retrievedRange.GetMinValue(), 10.0);
	QCOMPARE(retrievedRange.GetMaxValue(), 200.0);
}


void CDepthBitmapTest::testSetDepthRange()
{
	imt3d::CDepthBitmap bitmap;
	
	istd::CRange initialRange(0.0, 100.0);
	istd::CIndex2d size(320, 240);
	bitmap.CreateDepthBitmap(initialRange, size);
	
	istd::CRange newRange(50.0, 150.0);
	bitmap.SetDepthRange(newRange);
	
	istd::CRange retrievedRange = bitmap.GetDepthRange();
	QCOMPARE(retrievedRange.GetMinValue(), 50.0);
	QCOMPARE(retrievedRange.GetMaxValue(), 150.0);
}


void CDepthBitmapTest::testGetSize()
{
	imt3d::CDepthBitmap bitmap;
	
	istd::CRange depthRange(0.0, 100.0);
	istd::CIndex2d size(800, 600);
	bitmap.CreateDepthBitmap(depthRange, size);
	
	istd::CIndex2d retrievedSize = bitmap.GetImageSize();
	QCOMPARE(retrievedSize.GetX(), 800);
	QCOMPARE(retrievedSize.GetY(), 600);
}


void CDepthBitmapTest::testBitmapProperties()
{
	imt3d::CDepthBitmap bitmap;
	
	istd::CRange depthRange(0.0, 100.0);
	istd::CIndex2d size(10, 10);
	bitmap.CreateDepthBitmap(depthRange, size);
	
	// Verify bitmap properties after creation
	QVERIFY(!bitmap.IsEmpty());
	QCOMPARE(bitmap.GetImageSize().GetX(), 10);
	QCOMPARE(bitmap.GetImageSize().GetY(), 10);
	QCOMPARE(bitmap.GetPixelFormat(), iimg::IBitmap::PF_FLOAT32);
}


void CDepthBitmapTest::testSetCalibration3d()
{
	imt3d::CDepthBitmap bitmap;
	
	istd::CRange depthRange(0.0, 100.0);
	istd::CIndex2d size(640, 480);
	bitmap.CreateDepthBitmap(depthRange, size);
	
	imt3d::CImage3dCalibration calibration;
	bitmap.SetCalibration3d(calibration);
	
	// Verify calibration is set
	const imt3d::IImage3dCalibration* retrievedCalibration = bitmap.GetCalibration3d();
	QVERIFY(retrievedCalibration != nullptr);
}


void CDepthBitmapTest::testGetCalibration3d()
{
	imt3d::CDepthBitmap bitmap;
	
	// Initially, calibration object exists but may be empty
	const imt3d::IImage3dCalibration* calibration = bitmap.GetCalibration3d();
	QVERIFY(calibration != nullptr);
	
	// After setting calibration
	istd::CRange depthRange(0.0, 100.0);
	istd::CIndex2d size(320, 240);
	bitmap.CreateDepthBitmap(depthRange, size);
	
	imt3d::CImage3dCalibration cal3d;
	bitmap.SetCalibration3d(cal3d);
	
	calibration = bitmap.GetCalibration3d();
	QVERIFY(calibration != nullptr);
}


void CDepthBitmapTest::testSetReferenceBitmap()
{
	imt3d::CDepthBitmap depthBitmap;
	
	istd::CRange depthRange(0.0, 100.0);
	istd::CIndex2d size(320, 240);
	depthBitmap.CreateDepthBitmap(depthRange, size);
	
	// Create a reference bitmap
	iimg::CGeneralBitmap referenceBitmap;
	referenceBitmap.CreateBitmap(istd::CIndex2d(320, 240), iimg::IBitmap::PF_GRAY);
	
	depthBitmap.SetReferenceBitmap(referenceBitmap);
	
	const iimg::IBitmap* retrievedBitmap = depthBitmap.GetReferenceBitmap();
	QVERIFY(retrievedBitmap != nullptr);
}


void CDepthBitmapTest::testGetReferenceBitmap()
{
	imt3d::CDepthBitmap bitmap;
	
	// Initially, reference bitmap object exists but is empty
	const iimg::IBitmap* referenceBitmap = bitmap.GetReferenceBitmap();
	QVERIFY(referenceBitmap != nullptr);
	QVERIFY(referenceBitmap->IsEmpty());
}


void CDepthBitmapTest::testResetReferenceBitmap()
{
	imt3d::CDepthBitmap depthBitmap;
	
	istd::CRange depthRange(0.0, 100.0);
	istd::CIndex2d size(320, 240);
	depthBitmap.CreateDepthBitmap(depthRange, size);
	
	// Set a reference bitmap
	iimg::CGeneralBitmap referenceBitmap;
	referenceBitmap.CreateBitmap(istd::CIndex2d(320, 240), iimg::IBitmap::PF_GRAY);
	depthBitmap.SetReferenceBitmap(referenceBitmap);
	
	QVERIFY(depthBitmap.GetReferenceBitmap() != nullptr);
	
	// Reset the reference bitmap
	depthBitmap.ResetReferenceBitmap();
	
	QVERIFY(depthBitmap.GetReferenceBitmap() != nullptr);
	QVERIFY(depthBitmap.GetReferenceBitmap()->IsEmpty());
}


void CDepthBitmapTest::testSerialization()
{
	imt3d::CDepthBitmap original;
	
	istd::CRange depthRange(0.0, 255.0);
	istd::CIndex2d size(100, 100);
	original.CreateDepthBitmap(depthRange, size);
	
	// Serialize
	iser::CMemoryWriteArchive writeArchive;
	bool serializeResult = original.Serialize(writeArchive);
	QVERIFY(serializeResult);
	QVERIFY(writeArchive.GetBufferSize() > 0);
	
	// Deserialize
	iser::CMemoryReadArchive readArchive(writeArchive.GetBuffer(), writeArchive.GetBufferSize());
	imt3d::CDepthBitmap deserialized;
	bool deserializeResult = deserialized.Serialize(readArchive);
	QVERIFY(deserializeResult);
	
	// Verify
	QCOMPARE(deserialized.GetImageSize().GetX(), 100);
	QCOMPARE(deserialized.GetImageSize().GetY(), 100);
	QCOMPARE(deserialized.GetDepthRange().GetMinValue(), 0.0);
	QCOMPARE(deserialized.GetDepthRange().GetMaxValue(), 255.0);
}


void CDepthBitmapTest::testCopyFrom()
{
	imt3d::CDepthBitmap source;
	istd::CRange depthRange(0.0, 100.0);
	istd::CIndex2d size(50, 50);
	source.CreateDepthBitmap(depthRange, size);
	
	imt3d::CDepthBitmap destination;
	bool copyResult = destination.CopyFrom(source);
	QVERIFY(copyResult);
	
	QCOMPARE(destination.GetImageSize().GetX(), 50);
	QCOMPARE(destination.GetImageSize().GetY(), 50);
	QCOMPARE(destination.GetDepthRange().GetMinValue(), 0.0);
	QCOMPARE(destination.GetDepthRange().GetMaxValue(), 100.0);
}


void CDepthBitmapTest::testIsEqual()
{
	imt3d::CDepthBitmap bitmap1;
	imt3d::CDepthBitmap bitmap2;
	
	istd::CRange depthRange(0.0, 100.0);
	istd::CIndex2d size(50, 50);
	
	bitmap1.CreateDepthBitmap(depthRange, size);
	bitmap2.CreateDepthBitmap(depthRange, size);
	
	QVERIFY(bitmap1.IsEqual(bitmap2));
	
	// Modify bitmap2
	istd::CRange differentRange(0.0, 200.0);
	bitmap2.SetDepthRange(differentRange);
	
	QVERIFY(!bitmap1.IsEqual(bitmap2));
}


void CDepthBitmapTest::testCloneMe()
{
	imt3d::CDepthBitmap original;
	istd::CRange depthRange(0.0, 150.0);
	istd::CIndex2d size(80, 60);
	original.CreateDepthBitmap(depthRange, size);
	
	istd::IChangeableUniquePtr clonedPtr = original.CloneMe();
	QVERIFY(clonedPtr);
	
	imt3d::CDepthBitmap* clonedBitmap = dynamic_cast<imt3d::CDepthBitmap*>(clonedPtr.get());
	QVERIFY(clonedBitmap != nullptr);
	QCOMPARE(clonedBitmap->GetImageSize().GetX(), 80);
	QCOMPARE(clonedBitmap->GetImageSize().GetY(), 60);
}


void CDepthBitmapTest::testResetData()
{
	imt3d::CDepthBitmap bitmap;
	istd::CRange depthRange(0.0, 100.0);
	istd::CIndex2d size(50, 50);
	bitmap.CreateDepthBitmap(depthRange, size);
	
	QVERIFY(!bitmap.IsEmpty());
	
	bool resetResult = bitmap.ResetData();
	QVERIFY(resetResult);
	
	QVERIFY(bitmap.IsEmpty());
	QCOMPARE(bitmap.GetImageSize().GetX(), 0);
	QCOMPARE(bitmap.GetImageSize().GetY(), 0);
}


void CDepthBitmapTest::cleanupTestCase()
{
}


I_ADD_TEST(CDepthBitmapTest);
