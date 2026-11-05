// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QObject>


/**
	Unit test for the storage links, storage resolvers and the resolver registry.
*/
class CStorageLinkTest: public QObject
{
	Q_OBJECT

private slots:
	void initTestCase();
	void testEmptyLink();
	void testMemoryResolverLiveObject();
	void testCollectionResolverSnapshot();
	void testCollectionResolverInsertAndRemove();
	void testRegistryLateBinding();
	void testResolverPriority();
	void testDefaultProvider();
	void testUnknownStorageId();
	void testTypeMismatch();
	void testStorageParams();
	void testSerialization();
	void testLinksInContainer();
	void testOptionalInterface();
	void cleanupTestCase();
};
