#pragma once


// Qt includes
#include <QtCore/QObject>


/**
	Unit test for the ordered object collection facade.
*/
class COrderedObjectCollectionTest: public QObject
{
	Q_OBJECT

private slots:
	void initTestCase();
	void testSetItemOrder();
	void testGetItemOrder();
	void testSetItemsOrder();
	void testResetItemOrder();
	void testInsertNewItem();
	void testRemoveItem();
	void testGetOrderedItemIds();
	void testDelegatedOperations();
	void testSerialization();
};


