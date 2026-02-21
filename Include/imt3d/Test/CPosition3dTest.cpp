// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include "CPosition3dTest.h"


// Qt includes
#include <QtTest/QtTest>

// ACF includes
#include <itest/CStandardTestExecutor.h>
#include <iser/CMemoryReadArchive.h>
#include <iser/CMemoryWriteArchive.h>

// ImtCore includes
#include <imt3d/CPosition3d.h>


void CPosition3dTest::initTestCase()
{
}


void CPosition3dTest::testDefaultConstructor()
{
	imt3d::CPosition3d position;
	
	QCOMPARE(position.GetPosition().GetX(), 0.0);
	QCOMPARE(position.GetPosition().GetY(), 0.0);
	QCOMPARE(position.GetPosition().GetZ(), 0.0);
}


void CPosition3dTest::testConstructorWithPosition()
{
	i3d::CVector3d vec(1.0, 2.0, 3.0);
	imt3d::CPosition3d position(vec);
	
	QVERIFY(!position.IsEmpty());
	QCOMPARE(position.GetPosition().GetX(), 1.0);
	QCOMPARE(position.GetPosition().GetY(), 2.0);
	QCOMPARE(position.GetPosition().GetZ(), 3.0);
}


void CPosition3dTest::testGetPosition()
{
	i3d::CVector3d vec(5.5, 10.5, 15.5);
	imt3d::CPosition3d position(vec);
	
	const i3d::CVector3d& retrievedPos = position.GetPosition();
	QCOMPARE(retrievedPos.GetX(), 5.5);
	QCOMPARE(retrievedPos.GetY(), 10.5);
	QCOMPARE(retrievedPos.GetZ(), 15.5);
}


void CPosition3dTest::testSetPosition()
{
	imt3d::CPosition3d position;
	i3d::CVector3d newPos(7.0, 8.0, 9.0);
	
	position.SetPosition(newPos);
	
	QCOMPARE(position.GetPosition().GetX(), 7.0);
	QCOMPARE(position.GetPosition().GetY(), 8.0);
	QCOMPARE(position.GetPosition().GetZ(), 9.0);
}


void CPosition3dTest::testTranslate()
{
	i3d::CVector3d initialPos(1.0, 2.0, 3.0);
	imt3d::CPosition3d position(initialPos);
	
	i3d::CVector3d translation(5.0, 10.0, 15.0);
	position.Translate(translation);
	
	QCOMPARE(position.GetPosition().GetX(), 6.0);
	QCOMPARE(position.GetPosition().GetY(), 12.0);
	QCOMPARE(position.GetPosition().GetZ(), 18.0);
}


void CPosition3dTest::testGetTranslated()
{
	i3d::CVector3d initialPos(1.0, 2.0, 3.0);
	imt3d::CPosition3d position(initialPos);
	
	i3d::CVector3d translation(5.0, 10.0, 15.0);
	imt3d::CPosition3d translated = position.GetTranslated(translation);
	
	// Original should be unchanged
	QCOMPARE(position.GetPosition().GetX(), 1.0);
	QCOMPARE(position.GetPosition().GetY(), 2.0);
	QCOMPARE(position.GetPosition().GetZ(), 3.0);
	
	// Translated should have new values
	QCOMPARE(translated.GetPosition().GetX(), 6.0);
	QCOMPARE(translated.GetPosition().GetY(), 12.0);
	QCOMPARE(translated.GetPosition().GetZ(), 18.0);
}


void CPosition3dTest::testIsEmpty()
{
	i3d::CVector3d vec(1.0, 2.0, 3.0);
	imt3d::CPosition3d nonEmptyPosition(vec);
	QVERIFY(!nonEmptyPosition.IsEmpty());
}


void CPosition3dTest::testGetCenter()
{
	i3d::CVector3d vec(10.0, 20.0, 30.0);
	imt3d::CPosition3d position(vec);
	
	i3d::CVector3d center = position.GetCenter();
	QCOMPARE(center.GetX(), 10.0);
	QCOMPARE(center.GetY(), 20.0);
	QCOMPARE(center.GetZ(), 30.0);
}


void CPosition3dTest::testMoveCenterTo()
{
	i3d::CVector3d initialPos(5.0, 10.0, 15.0);
	imt3d::CPosition3d position(initialPos);
	
	i3d::CVector3d newCenter(20.0, 25.0, 30.0);
	position.MoveCenterTo(newCenter);
	
	QCOMPARE(position.GetPosition().GetX(), 20.0);
	QCOMPARE(position.GetPosition().GetY(), 25.0);
	QCOMPARE(position.GetPosition().GetZ(), 30.0);
}


void CPosition3dTest::testGetBoundingCuboid()
{
	i3d::CVector3d vec(5.0, 10.0, 15.0);
	imt3d::CPosition3d position(vec);
	
	imt3d::CCuboid cuboid = position.GetBoundingCuboid();
	
	// For a point, the bounding cuboid should have the point at all corners
	QVERIFY(qAbs(cuboid.GetLeft() - 5.0) <= I_BIG_EPSILON);
	QVERIFY(qAbs(cuboid.GetRight() - 5.0) <= I_BIG_EPSILON);
	QVERIFY(qAbs(cuboid.GetBottom() - 10.0) <= I_BIG_EPSILON);
	QVERIFY(qAbs(cuboid.GetTop() - 10.0) <= I_BIG_EPSILON);
	QVERIFY(qAbs(cuboid.GetNear() - 15.0) <= I_BIG_EPSILON);
	QVERIFY(qAbs(cuboid.GetFar() - 15.0) <= I_BIG_EPSILON);
}


void CPosition3dTest::testSerialization()
{
	i3d::CVector3d vec(11.5, 22.5, 33.5);
	imt3d::CPosition3d original(vec);
	
	// Serialize
	iser::CMemoryWriteArchive writeArchive;
	bool serializeResult = original.Serialize(writeArchive);
	QVERIFY(serializeResult);
	QVERIFY(writeArchive.GetBufferSize() > 0);
	
	// Deserialize
	iser::CMemoryReadArchive readArchive(writeArchive.GetBuffer(), writeArchive.GetBufferSize());
	imt3d::CPosition3d deserialized;
	bool deserializeResult = deserialized.Serialize(readArchive);
	QVERIFY(deserializeResult);
	
	// Verify
	QCOMPARE(deserialized.GetPosition().GetX(), 11.5);
	QCOMPARE(deserialized.GetPosition().GetY(), 22.5);
	QCOMPARE(deserialized.GetPosition().GetZ(), 33.5);
}


void CPosition3dTest::testCopyFrom()
{
	i3d::CVector3d vec1(1.0, 2.0, 3.0);
	imt3d::CPosition3d source(vec1);
	
	imt3d::CPosition3d destination;
	bool copyResult = destination.CopyFrom(source);
	QVERIFY(copyResult);
	
	QCOMPARE(destination.GetPosition().GetX(), 1.0);
	QCOMPARE(destination.GetPosition().GetY(), 2.0);
	QCOMPARE(destination.GetPosition().GetZ(), 3.0);
}


void CPosition3dTest::testIsEqual()
{
	i3d::CVector3d vec(4.0, 5.0, 6.0);
	imt3d::CPosition3d pos1(vec);
	imt3d::CPosition3d pos2(vec);
	
	QVERIFY(pos1.IsEqual(pos2));
	
	i3d::CVector3d differentVec(7.0, 8.0, 9.0);
	imt3d::CPosition3d pos3(differentVec);
	
	QVERIFY(!pos1.IsEqual(pos3));
}


void CPosition3dTest::testCloneMe()
{
	i3d::CVector3d vec(12.0, 13.0, 14.0);
	imt3d::CPosition3d original(vec);
	
	istd::IChangeableUniquePtr clonedPtr = original.CloneMe();
	QVERIFY(clonedPtr);
	
	imt3d::CPosition3d* clonedPosition = dynamic_cast<imt3d::CPosition3d*>(&(*clonedPtr));
	QVERIFY(clonedPosition != nullptr);
	
	QCOMPARE(clonedPosition->GetPosition().GetX(), 12.0);
	QCOMPARE(clonedPosition->GetPosition().GetY(), 13.0);
	QCOMPARE(clonedPosition->GetPosition().GetZ(), 14.0);
}


void CPosition3dTest::testResetData()
{
	i3d::CVector3d vec(20.0, 30.0, 40.0);
	imt3d::CPosition3d position(vec);
	
	QVERIFY(!position.IsEmpty());
	
	bool resetResult = position.ResetData();
	QVERIFY(resetResult);
	
	QCOMPARE(position.GetPosition().GetX(), 0.0);
	QCOMPARE(position.GetPosition().GetY(), 0.0);
	QCOMPARE(position.GetPosition().GetZ(), 0.0);
}


void CPosition3dTest::cleanupTestCase()
{
}


I_ADD_TEST(CPosition3dTest);
