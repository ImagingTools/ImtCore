// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include "CMesh3dTest.h"


// Qt includes
#include <QtTest/QtTest>

// ACF includes
#include <iser/CMemoryReadArchive.h>
#include <iser/CMemoryWriteArchive.h>

// ImtCore includes
#include <imt3d/CMesh3d.h>
#include <imt3d/IPointsBasedObject.h>


void CMesh3dTest::initTestCase()
{
}


void CMesh3dTest::testCreateMesh()
{
	imt3d::CMesh3d mesh;
	bool result = mesh.CreateMesh(imt3d::IPointsBasedObject::PF_XYZ_32);
	
	QVERIFY(result);
	QCOMPARE(mesh.GetPointFormat(), imt3d::IPointsBasedObject::PF_XYZ_32);
	QCOMPARE(mesh.GetPointsCount(), 0);
	QVERIFY(mesh.GetIndices().empty());
}


void CMesh3dTest::testCreateMeshWithData()
{
	imt3d::CMesh3d mesh;
	
	// Create a simple triangle mesh
	float vertices[] = {
		0.0f, 0.0f, 0.0f,  // Vertex 0
		1.0f, 0.0f, 0.0f,  // Vertex 1
		0.0f, 1.0f, 0.0f   // Vertex 2
	};
	
	// Define one triangular face
	imt3d::IMesh3d::Indices indices;
	imt3d::IMesh3d::Face face;
	face.push_back(0);
	face.push_back(1);
	face.push_back(2);
	indices.push_back(face);
	
	bool result = mesh.CreateMesh(imt3d::IPointsBasedObject::PF_XYZ_32, 3, vertices, indices);
	
	QVERIFY(result);
	QCOMPARE(mesh.GetPointsCount(), 3);
	QCOMPARE(mesh.GetIndices().size(), 1);
	QCOMPARE(mesh.GetIndices()[0].size(), 3);
	QCOMPARE(mesh.GetIndices()[0][0], 0u);
	QCOMPARE(mesh.GetIndices()[0][1], 1u);
	QCOMPARE(mesh.GetIndices()[0][2], 2u);
}


void CMesh3dTest::testInsertData()
{
	imt3d::CMesh3d mesh;
	mesh.CreateMesh(imt3d::IPointsBasedObject::PF_XYZ_32);
	
	// Insert first triangle
	float vertices1[] = {
		0.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f
	};
	
	imt3d::IMesh3d::Indices indices1;
	imt3d::IMesh3d::Face face1;
	face1.push_back(0);
	face1.push_back(1);
	face1.push_back(2);
	indices1.push_back(face1);
	
	bool result1 = mesh.InsertData(3, vertices1, indices1);
	QVERIFY(result1);
	QCOMPARE(mesh.GetPointsCount(), 3);
	QCOMPARE(mesh.GetIndices().size(), 1);
	
	// Insert second triangle
	float vertices2[] = {
		1.0f, 1.0f, 0.0f
	};
	
	imt3d::IMesh3d::Indices indices2;
	imt3d::IMesh3d::Face face2;
	face2.push_back(1);
	face2.push_back(3);
	face2.push_back(2);
	indices2.push_back(face2);
	
	bool result2 = mesh.InsertData(1, vertices2, indices2);
	QVERIFY(result2);
	QCOMPARE(mesh.GetPointsCount(), 4);
	QCOMPARE(mesh.GetIndices().size(), 2);
}


void CMesh3dTest::testGetIndices()
{
	imt3d::CMesh3d mesh;
	
	float vertices[] = {
		0.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f
	};
	
	imt3d::IMesh3d::Indices indices;
	imt3d::IMesh3d::Face face;
	face.push_back(0);
	face.push_back(1);
	face.push_back(2);
	indices.push_back(face);
	
	mesh.CreateMesh(imt3d::IPointsBasedObject::PF_XYZ_32, 3, vertices, indices);
	
	const imt3d::IMesh3d::Indices& retrievedIndices = mesh.GetIndices();
	QCOMPARE(retrievedIndices.size(), 1);
	QCOMPARE(retrievedIndices[0].size(), 3);
	QCOMPARE(retrievedIndices[0][0], 0u);
	QCOMPARE(retrievedIndices[0][1], 1u);
	QCOMPARE(retrievedIndices[0][2], 2u);
}


void CMesh3dTest::testGetPointsCount()
{
	imt3d::CMesh3d mesh;
	QCOMPARE(mesh.GetPointsCount(), 0);
	
	mesh.CreateMesh(imt3d::IPointsBasedObject::PF_XYZ_32);
	QCOMPARE(mesh.GetPointsCount(), 0);
	
	float vertices[] = { 1.0f, 2.0f, 3.0f };
	imt3d::IMesh3d::Indices indices;
	mesh.InsertData(1, vertices, indices);
	QCOMPARE(mesh.GetPointsCount(), 1);
}


void CMesh3dTest::testSerialization()
{
	QSKIP("CMesh3d::Serialize is not implemented yet");

	imt3d::CMesh3d original;
	
	float vertices[] = {
		0.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f
	};
	
	imt3d::IMesh3d::Indices indices;
	imt3d::IMesh3d::Face face;
	face.push_back(0);
	face.push_back(1);
	face.push_back(2);
	indices.push_back(face);
	
	original.CreateMesh(imt3d::IPointsBasedObject::PF_XYZ_32, 3, vertices, indices);
	
	// Serialize
	iser::CMemoryWriteArchive writeArchive;
	bool serializeResult = original.Serialize(writeArchive);
	QVERIFY(serializeResult);
	QVERIFY(writeArchive.GetBufferSize() > 0);
	
	// Deserialize
	iser::CMemoryReadArchive readArchive(writeArchive.GetBuffer(), writeArchive.GetBufferSize());
	imt3d::CMesh3d deserialized;
	bool deserializeResult = deserialized.Serialize(readArchive);
	QVERIFY(deserializeResult);
	
	// Verify
	QCOMPARE(deserialized.GetPointsCount(), 3);
	QCOMPARE(deserialized.GetPointFormat(), imt3d::IPointsBasedObject::PF_XYZ_32);
	QCOMPARE(deserialized.GetIndices().size(), 1);
	QCOMPARE(deserialized.GetIndices()[0].size(), 3);
}


void CMesh3dTest::testCopyFrom()
{
	imt3d::CMesh3d source;
	float vertices[] = { 1.0f, 2.0f, 3.0f };
	imt3d::IMesh3d::Indices indices;
	source.CreateMesh(imt3d::IPointsBasedObject::PF_XYZ_32, 1, vertices, indices);
	
	imt3d::CMesh3d destination;
	bool copyResult = destination.CopyFrom(source);
	QVERIFY(copyResult);
	
	QCOMPARE(destination.GetPointsCount(), 1);
	QCOMPARE(destination.GetPointFormat(), imt3d::IPointsBasedObject::PF_XYZ_32);
}


void CMesh3dTest::testIsEqual()
{
	imt3d::CMesh3d mesh1;
	imt3d::CMesh3d mesh2;
	
	float vertices[] = { 1.0f, 2.0f, 3.0f };
	imt3d::IMesh3d::Indices indices;
	
	mesh1.CreateMesh(imt3d::IPointsBasedObject::PF_XYZ_32, 1, vertices, indices);
	mesh2.CreateMesh(imt3d::IPointsBasedObject::PF_XYZ_32, 1, vertices, indices);
	
	bool check1 = mesh1.IsEqual(mesh2);
	QVERIFY(check1);
	
	// Modify mesh2
	float moreVertices[] = { 4.0f, 5.0f, 6.0f };
	mesh2.InsertData(1, moreVertices, indices);
	
	bool check2 = mesh1.IsEqual(mesh2);
	QVERIFY(!check2);
}


void CMesh3dTest::testCloneMe()
{
	imt3d::CMesh3d original;
	float vertices[] = { 7.0f, 8.0f, 9.0f };
	imt3d::IMesh3d::Indices indices;
	original.CreateMesh(imt3d::IPointsBasedObject::PF_XYZ_32, 1, vertices, indices);
	
	istd::IChangeableUniquePtr clonedPtr = original.CloneMe();
	QVERIFY(clonedPtr);
	
	imt3d::CMesh3d* clonedMesh = dynamic_cast<imt3d::CMesh3d*>(&(*clonedPtr));
	QVERIFY(clonedMesh != nullptr);
	QCOMPARE(clonedMesh->GetPointsCount(), 1);
}


void CMesh3dTest::testResetData()
{
	imt3d::CMesh3d mesh;
	float vertices[] = { 1.0f, 2.0f, 3.0f };
	imt3d::IMesh3d::Indices indices;
	mesh.CreateMesh(imt3d::IPointsBasedObject::PF_XYZ_32, 1, vertices, indices);
	
	QVERIFY(!mesh.IsEmpty());
	
	bool resetResult = mesh.ResetData();
	QVERIFY(resetResult);
	
	QVERIFY(mesh.IsEmpty());
	QCOMPARE(mesh.GetPointsCount(), 0);
}


void CMesh3dTest::cleanupTestCase()
{
}


I_ADD_TEST(CMesh3dTest);
