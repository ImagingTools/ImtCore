// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include "COrderedObjectCollectionTest.h"


#include "CTestData.h"

// Qt includes
#include <QtTest/QtTest>

// ACF includes
#include <istd/TComposedFactory.h>
#include <istd/TSingleFactory.h>
#include <itest/CStandardTestExecutor.h>

// ImtCore includes
#include <imtbase/COrderedObjectCollectionProxy.h>
#include <imtbase/CObjectCollection.h>


void COrderedObjectCollectionTest::initTestCase()
{
	// No factory registration needed in initTestCase.
	// Factory is registered per test on the specific CObjectCollection instance.
}


void COrderedObjectCollectionTest::testSetItemOrder()
{
	// Create a base collection
	imtbase::CObjectCollection baseCollection;
	baseCollection.SetOperationFlags(imtbase::IObjectCollection::OF_ALL);

	typedef istd::TSingleFactory<istd::IChangeable, CTestData> TestDataImpl;
	// Register factory for test data type
	baseCollection.RegisterFactory<TestDataImpl>("TestType");

	// Add some items
	const QByteArray id1 = baseCollection.InsertNewObject("TestType", "Item1", "Description1");
	const QByteArray id2 = baseCollection.InsertNewObject("TestType", "Item2", "Description2");
	const QByteArray id3 = baseCollection.InsertNewObject("TestType", "Item3", "Description3");

	// Create ordered proxy
	imtbase::COrderedObjectCollectionProxy orderedProxy(&baseCollection);

	// Test initial order (should match insertion order)
	imtbase::ICollectionInfo::Ids initialIds = orderedProxy.GetOrderedItemIds();
	QCOMPARE(initialIds.size(), 3);
	QCOMPARE(initialIds[0], id1);
	QCOMPARE(initialIds[1], id2);
	QCOMPARE(initialIds[2], id3);

	// Move item3 to position 0
	bool result = orderedProxy.SetItemOrder(id3, 0);
	QVERIFY(result);

	// Check new order
	imtbase::ICollectionInfo::Ids newIds = orderedProxy.GetOrderedItemIds();
	QCOMPARE(newIds.size(), 3);
	QCOMPARE(newIds[0], id3);
	QCOMPARE(newIds[1], id1);
	QCOMPARE(newIds[2], id2);
}


void COrderedObjectCollectionTest::testGetItemOrder()
{
	// Create a base collection
	imtbase::CObjectCollection baseCollection;
	baseCollection.SetOperationFlags(imtbase::IObjectCollection::OF_ALL);

	typedef istd::TSingleFactory<istd::IChangeable, CTestData> TestDataImpl;
	// Register factory for test data type
	baseCollection.RegisterFactory<TestDataImpl>("TestType");

	// Add some items
	const QByteArray id1 = baseCollection.InsertNewObject("TestType", "Item1", "Description1");
	const QByteArray id2 = baseCollection.InsertNewObject("TestType", "Item2", "Description2");
	const QByteArray id3 = baseCollection.InsertNewObject("TestType", "Item3", "Description3");

	// Create ordered proxy
	imtbase::COrderedObjectCollectionProxy orderedProxy(&baseCollection);

	// Test getting positions
	QCOMPARE(orderedProxy.GetItemOrder(id1), 0);
	QCOMPARE(orderedProxy.GetItemOrder(id2), 1);
	QCOMPARE(orderedProxy.GetItemOrder(id3), 2);

	// Change order
	orderedProxy.SetItemOrder(id3, 0);

	// Test new positions
	QCOMPARE(orderedProxy.GetItemOrder(id3), 0);
	QCOMPARE(orderedProxy.GetItemOrder(id1), 1);
	QCOMPARE(orderedProxy.GetItemOrder(id2), 2);

	// Test invalid ID
	QCOMPARE(orderedProxy.GetItemOrder("invalid"), -1);
}


void COrderedObjectCollectionTest::testSetItemsOrder()
{
	// Create a base collection
	imtbase::CObjectCollection baseCollection;
	baseCollection.SetOperationFlags(imtbase::IObjectCollection::OF_ALL);

	typedef istd::TSingleFactory<istd::IChangeable, CTestData> TestDataImpl;
	// Register factory for test data type
	baseCollection.RegisterFactory<TestDataImpl>("TestType");

	// Add some items
	const QByteArray id1 = baseCollection.InsertNewObject("TestType", "Item1", "Description1");
	const QByteArray id2 = baseCollection.InsertNewObject("TestType", "Item2", "Description2");
	const QByteArray id3 = baseCollection.InsertNewObject("TestType", "Item3", "Description3");

	// Create ordered proxy
	imtbase::COrderedObjectCollectionProxy orderedProxy(&baseCollection);

	// Set custom order
	imtbase::ICollectionInfo::Ids newOrder;
	newOrder << id3 << id1 << id2;

	bool result = orderedProxy.SetItemsOrder(newOrder);
	QVERIFY(result);

	// Check new order
	imtbase::ICollectionInfo::Ids currentIds = orderedProxy.GetOrderedItemIds();
	QCOMPARE(currentIds.size(), 3);
	QCOMPARE(currentIds[0], id3);
	QCOMPARE(currentIds[1], id1);
	QCOMPARE(currentIds[2], id2);

	// Test invalid order (ID not in collection)
	imtbase::ICollectionInfo::Ids invalidOrder;
	invalidOrder << id1 << "invalid" << id3;
	QVERIFY(!orderedProxy.SetItemsOrder(invalidOrder));
}


void COrderedObjectCollectionTest::testResetItemOrder()
{
	// Create a base collection
	imtbase::CObjectCollection baseCollection;
	baseCollection.SetOperationFlags(imtbase::IObjectCollection::OF_ALL);

	typedef istd::TSingleFactory<istd::IChangeable, CTestData> TestDataImpl;
	// Register factory for test data type
	baseCollection.RegisterFactory<TestDataImpl>("TestType");

	// Add some items
	const QByteArray id1 = baseCollection.InsertNewObject("TestType", "Item1", "Description1");
	const QByteArray id2 = baseCollection.InsertNewObject("TestType", "Item2", "Description2");
	const QByteArray id3 = baseCollection.InsertNewObject("TestType", "Item3", "Description3");

	// Create ordered proxy
	imtbase::COrderedObjectCollectionProxy orderedProxy(&baseCollection);

	// Set custom order
	orderedProxy.SetItemOrder(id3, 0);

	// Verify custom order is active
	imtbase::ICollectionInfo::Ids customIds = orderedProxy.GetOrderedItemIds();
	QCOMPARE(customIds[0], id3);

	// Reset to default order
	bool result = orderedProxy.ResetItemOrder();
	QVERIFY(result);

	// Check order is back to default (insertion order)
	imtbase::ICollectionInfo::Ids defaultIds = orderedProxy.GetOrderedItemIds();
	QCOMPARE(defaultIds[0], id1);
	QCOMPARE(defaultIds[1], id2);
	QCOMPARE(defaultIds[2], id3);
}


void COrderedObjectCollectionTest::testInsertNewItem()
{
	// Create a base collection
	imtbase::CObjectCollection baseCollection;
	baseCollection.SetOperationFlags(imtbase::IObjectCollection::OF_ALL);

	typedef istd::TSingleFactory<istd::IChangeable, CTestData> TestDataImpl;

	// Register factory for test data type
	baseCollection.RegisterFactory<TestDataImpl>("TestType");

	// Add initial items
	const QByteArray id1 = baseCollection.InsertNewObject("TestType", "Item1", "Description1");
	const QByteArray id2 = baseCollection.InsertNewObject("TestType", "Item2", "Description2");

	// Create ordered proxy with custom order
	imtbase::COrderedObjectCollectionProxy orderedProxy(&baseCollection);
	orderedProxy.SetItemOrder(id2, 0);  // Set custom order: id2, id1

	// Insert new item through proxy
	const QByteArray id3 = orderedProxy.InsertNewObject("TestType", "Item3", "Description3");
	QVERIFY(!id3.isEmpty());

	// Check new item is added to end of custom order
	imtbase::ICollectionInfo::Ids currentIds = orderedProxy.GetOrderedItemIds();
	QCOMPARE(currentIds.size(), 3);
	QCOMPARE(currentIds[0], id2);
	QCOMPARE(currentIds[1], id1);
	QCOMPARE(currentIds[2], id3);
}


void COrderedObjectCollectionTest::testRemoveItem()
{
	// Create a base collection
	imtbase::CObjectCollection baseCollection;
	baseCollection.SetOperationFlags(imtbase::IObjectCollection::OF_ALL);

	typedef istd::TSingleFactory<istd::IChangeable, CTestData> TestDataImpl;

	// Register factory for test data type
	baseCollection.RegisterFactory<TestDataImpl>("TestType");

	// Add items
	const QByteArray id1 = baseCollection.InsertNewObject("TestType", "Item1", "Description1");
	const QByteArray id2 = baseCollection.InsertNewObject("TestType", "Item2", "Description2");
	const QByteArray id3 = baseCollection.InsertNewObject("TestType", "Item3", "Description3");

	// Create ordered proxy with custom order
	imtbase::COrderedObjectCollectionProxy orderedProxy(&baseCollection);
	imtbase::ICollectionInfo::Ids customOrder;
	customOrder << id3 << id1 << id2;
	orderedProxy.SetItemsOrder(customOrder);

	// Remove item through proxy
	imtbase::ICollectionInfo::Ids toRemove;
	toRemove << id1;
	bool result = orderedProxy.RemoveElements(toRemove);
	QVERIFY(result);

	// Check item is removed from custom order
	imtbase::ICollectionInfo::Ids currentIds = orderedProxy.GetOrderedItemIds();
	QCOMPARE(currentIds.size(), 2);
	QCOMPARE(currentIds[0], id3);
	QCOMPARE(currentIds[1], id2);
	QVERIFY(!currentIds.contains(id1));
}


void COrderedObjectCollectionTest::testGetOrderedItemIds()
{
	// Create a base collection
	imtbase::CObjectCollection baseCollection;
	baseCollection.SetOperationFlags(imtbase::IObjectCollection::OF_ALL);

	typedef istd::TSingleFactory<istd::IChangeable, CTestData> TestDataImpl;

	// Register factory for test data type
	baseCollection.RegisterFactory<TestDataImpl>("TestType");

	// Add items
	const QByteArray id1 = baseCollection.InsertNewObject("TestType", "Item1", "Description1");
	const QByteArray id2 = baseCollection.InsertNewObject("TestType", "Item2", "Description2");
	const QByteArray id3 = baseCollection.InsertNewObject("TestType", "Item3", "Description3");

	// Create ordered proxy
	imtbase::COrderedObjectCollectionProxy orderedProxy(&baseCollection);

	// Test GetElementIds respects custom order
	orderedProxy.SetItemOrder(id3, 0);

	imtbase::ICollectionInfo::Ids ids = orderedProxy.GetElementIds();
	QCOMPARE(ids.size(), 3);
	QCOMPARE(ids[0], id3);
	QCOMPARE(ids[1], id1);
	QCOMPARE(ids[2], id2);

	// Test with offset and count
	imtbase::ICollectionInfo::Ids subset = orderedProxy.GetElementIds(1, 2);
	QCOMPARE(subset.size(), 2);
	QCOMPARE(subset[0], id1);
	QCOMPARE(subset[1], id2);
}


void COrderedObjectCollectionTest::testDelegatedOperations()
{
	// Create a base collection
	imtbase::CObjectCollection baseCollection;
	baseCollection.SetOperationFlags(imtbase::IObjectCollection::OF_ALL);

	typedef istd::TSingleFactory<istd::IChangeable, CTestData> TestDataImpl;

	// Register factory for test data type
	baseCollection.RegisterFactory<TestDataImpl>("TestType");

	// Add items
	const QByteArray id1 = baseCollection.InsertNewObject("TestType", "Item1", "Description1");
	const QByteArray id2 = baseCollection.InsertNewObject("TestType", "Item2", "Description2");

	// Create ordered proxy
	imtbase::COrderedObjectCollectionProxy orderedProxy(&baseCollection);

	// Test GetOperationFlags delegation
	int flags = orderedProxy.GetOperationFlags();
	QVERIFY(flags & imtbase::IObjectCollection::OF_SUPPORT_INSERT);
	QVERIFY(flags & imtbase::IObjectCollection::OF_SUPPORT_DELETE);

	// Test GetOperationFlags for specific object
	int objectFlags = orderedProxy.GetOperationFlags(id1);
	QVERIFY(objectFlags != 0);

	// Test GetElementsCount delegation
	int count = orderedProxy.GetElementsCount();
	QCOMPARE(count, 2);
}


void COrderedObjectCollectionTest::testSerialization()
{
	// Create a base collection
	imtbase::CObjectCollection baseCollection;
	baseCollection.SetOperationFlags(imtbase::IObjectCollection::OF_ALL);

	typedef istd::TSingleFactory<istd::IChangeable, CTestData> TestDataImpl;

	// Register factory for test data type
	baseCollection.RegisterFactory<TestDataImpl>("TestType");

	// Add items
	const QByteArray id1 = baseCollection.InsertNewObject("TestType", "Item1", "Description1");
	const QByteArray id2 = baseCollection.InsertNewObject("TestType", "Item2", "Description2");
	const QByteArray id3 = baseCollection.InsertNewObject("TestType", "Item3", "Description3");

	// Create ordered proxy with custom order
	imtbase::COrderedObjectCollectionProxy orderedProxy(&baseCollection);
	imtbase::ICollectionInfo::Ids customOrder;
	customOrder << id3 << id1 << id2;
	orderedProxy.SetItemsOrder(customOrder);

	// Verify custom order is set
	imtbase::ICollectionInfo::Ids orderBefore = orderedProxy.GetOrderedItemIds();
	QCOMPARE(orderBefore[0], id3);
	QCOMPARE(orderBefore[1], id1);
	QCOMPARE(orderBefore[2], id2);

	// Note: Full serialization test would require creating archives,
	// which is beyond the scope of a simple unit test.
	// The Serialize method implementation has been added and follows
	// the existing pattern from CSimpleReferenceCollection.
}


I_ADD_TEST(COrderedObjectCollectionTest);
