// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include "CStorageLinkTest.h"


// std includes
#include <algorithm>
#include <optional>

// Qt includes
#include <QtCore/QList>
#include <QtTest/QtTest>

// ACF includes
#include <istd/TSingleFactory.h>
#include <iser/CMemoryReadArchive.h>
#include <iser/CMemoryWriteArchive.h>
#include <itest/CStandardTestExecutor.h>

// ImtCore includes
#include <imtbase/CObjectCollection.h>
#include <imtbase/CMemoryStorageResolver.h>
#include <imtbase/CCollectionStorageResolver.h>
#include <imtbase/CStorageResolverRegistry.h>
#include <imtbase/StorageLink.h>


namespace
{


class CLinkTestData: public istd::IChangeable
{
public:
	CLinkTestData(int value = 0): m_value(value) {}

	int m_value;

	// reimplemented (istd::IChangeable)
	virtual int GetSupportedOperations() const override { return SO_CLONE | SO_COPY | SO_COMPARE; }

	virtual bool CopyFrom(const IChangeable& object, CompatibilityMode /*mode*/ = CM_WITHOUT_REFS) override
	{
		const CLinkTestData* sourcePtr = dynamic_cast<const CLinkTestData*>(&object);
		if (sourcePtr != nullptr){
			m_value = sourcePtr->m_value;

			return true;
		}

		return false;
	}

	virtual bool IsEqual(const IChangeable& object) const override
	{
		const CLinkTestData* sourcePtr = dynamic_cast<const CLinkTestData*>(&object);

		return (sourcePtr != nullptr) && (sourcePtr->m_value == m_value);
	}

	virtual istd::IChangeableUniquePtr CloneMe(CompatibilityMode /*mode*/ = CM_WITHOUT_REFS) const override
	{
		return istd::IChangeableUniquePtr(new CLinkTestData(m_value));
	}
};


class COtherTestData: public istd::IChangeable
{
};


typedef imtbase::StorageLink<CLinkTestData> DataLink;
typedef istd::TSingleFactory<istd::IChangeable, CLinkTestData> DataFactory;

const QByteArray s_typeId = "LinkTestData";


istd::TSharedInterfacePtr<CLinkTestData> CreateData(int value)
{
	return istd::TSharedInterfacePtr<CLinkTestData>(new CLinkTestData(value));
}


void SetupCollection(imtbase::CObjectCollection& collection)
{
	collection.SetOperationFlags(imtbase::IObjectCollection::OF_ALL);
	collection.RegisterFactory<DataFactory>(s_typeId);
}


} // namespace


void CStorageLinkTest::initTestCase()
{
	imtbase::StorageLinkBase::SetDefaultResolverProvider(nullptr);
}


void CStorageLinkTest::cleanupTestCase()
{
	imtbase::StorageLinkBase::SetDefaultResolverProvider(nullptr);
}


void CStorageLinkTest::testEmptyLink()
{
	DataLink link;

	QVERIFY(link.IsNull());
	QVERIFY(!link.IsLoaded());
	QVERIFY(link.GetResolver() == nullptr);
	QVERIFY(!link.GetObject().IsValid());
	QVERIFY(!link.Save());
	QVERIFY(!link.Remove());
	QVERIFY(link.GetStorageParams().isEmpty());
}


void CStorageLinkTest::testMemoryResolverLiveObject()
{
	imtbase::CMemoryStorageResolver resolver;

	DataLink first;
	first.SetResolver(&resolver);
	QVERIFY(first.Insert(CreateData(10)));
	QVERIFY(!first.IsNull());
	QCOMPARE(resolver.GetObjectsCount(), 1);

	DataLink second(first.GetObjectId());
	second.SetResolver(&resolver);

	QCOMPARE(second->m_value, 10);

	// Memory storage has the live semantic: no Save is needed
	first->m_value = 20;
	QCOMPARE(second->m_value, 20);
	QVERIFY(first.Save());
	QCOMPARE(second->m_value, 20);

	QCOMPARE(first, second);
}


void CStorageLinkTest::testCollectionResolverSnapshot()
{
	imtbase::CObjectCollection collection;
	SetupCollection(collection);

	imtbase::CCollectionStorageResolver resolver(collection, s_typeId);

	DataLink link;
	link.SetResolver(&resolver);
	QVERIFY(link.Insert(CreateData(5), "Item"));
	QCOMPARE(collection.GetElementsCount(), 1);

	DataLink other(link.GetObjectId());
	other.SetResolver(&resolver);

	QCOMPARE(other->m_value, 5);

	// Changes are visible in the collection only after Save
	other->m_value = 6;

	DataLink checkBeforeSave(link.GetObjectId());
	checkBeforeSave.SetResolver(&resolver);
	QCOMPARE(checkBeforeSave->m_value, 5);

	QVERIFY(other.Save());

	DataLink checkAfterSave(link.GetObjectId());
	checkAfterSave.SetResolver(&resolver);
	QCOMPARE(checkAfterSave->m_value, 6);

	// Unload forces the loading from the collection again
	link.Unload();
	QVERIFY(!link.IsLoaded());
	QCOMPARE(link->m_value, 6);
}


void CStorageLinkTest::testCollectionResolverInsertAndRemove()
{
	imtbase::CObjectCollection collection;
	SetupCollection(collection);

	imtbase::CCollectionStorageResolver resolver(collection, s_typeId);

	DataLink link;
	link.SetResolver(&resolver);
	QVERIFY(link.Insert(CreateData(1)));

	DataLink copy = link;

	QVERIFY(link.Remove());
	QVERIFY(link.IsNull());
	QCOMPARE(collection.GetElementsCount(), 0);

	// The copy still points to the removed object: it must not be resolved anymore
	copy.Unload();
	QVERIFY(!copy.GetObject().IsValid());
}


void CStorageLinkTest::testRegistryLateBinding()
{
	imtbase::CMemoryStorageResolver desktopStorage;
	imtbase::CMemoryStorageResolver serverStorage;

	// The same data model, two "application profiles"
	imtbase::CStorageResolverRegistry desktopProfile;
	desktopProfile.RegisterResolver(&desktopStorage, "images");

	imtbase::CStorageResolverRegistry serverProfile;
	serverProfile.RegisterResolver(&serverStorage, "images");

	const QByteArray desktopId = desktopStorage.InsertObject(CreateData(1));
	const QByteArray serverId = serverStorage.InsertObject(CreateData(2));

	DataLink desktopLink(desktopId, "images");
	desktopLink.SetResolverProvider(&desktopProfile);
	QCOMPARE(desktopLink->m_value, 1);
	QVERIFY(desktopLink.GetResolver() == &desktopStorage);

	DataLink serverLink(serverId, "images");
	serverLink.SetResolverProvider(&serverProfile);
	QCOMPARE(serverLink->m_value, 2);
	QVERIFY(serverLink.GetResolver() == &serverStorage);

	// Re-binding to another provider drops the loaded object
	desktopLink.SetLink(serverId, "images");
	desktopLink.SetResolverProvider(&serverProfile);
	QVERIFY(!desktopLink.IsLoaded());
	QCOMPARE(desktopLink->m_value, 2);
}


void CStorageLinkTest::testResolverPriority()
{
	imtbase::CMemoryStorageResolver directStorage;
	imtbase::CMemoryStorageResolver providerStorage;
	imtbase::CMemoryStorageResolver defaultStorage;

	imtbase::CStorageResolverRegistry providerRegistry;
	providerRegistry.RegisterResolver(&providerStorage);

	imtbase::CStorageResolverRegistry defaultRegistry;
	defaultRegistry.RegisterResolver(&defaultStorage);
	imtbase::StorageLinkBase::SetDefaultResolverProvider(&defaultRegistry);

	DataLink link("someId");
	QVERIFY(link.GetResolver() == &defaultStorage);

	link.SetResolverProvider(&providerRegistry);
	QVERIFY(link.GetResolver() == &providerStorage);

	link.SetResolver(&directStorage);
	QVERIFY(link.GetResolver() == &directStorage);

	link.SetResolver(nullptr);
	QVERIFY(link.GetResolver() == &providerStorage);

	imtbase::StorageLinkBase::SetDefaultResolverProvider(nullptr);
}


void CStorageLinkTest::testDefaultProvider()
{
	imtbase::CMemoryStorageResolver storage;
	imtbase::CStorageResolverRegistry registry;
	registry.RegisterResolver(&storage);

	const QByteArray objectId = storage.InsertObject(CreateData(42));

	DataLink link(objectId);
	QVERIFY(link.GetObject().GetPtr() == nullptr);

	imtbase::StorageLinkBase::SetDefaultResolverProvider(&registry);
	QVERIFY(imtbase::StorageLinkBase::GetDefaultResolverProvider() == &registry);

	QCOMPARE(link->m_value, 42);

	imtbase::StorageLinkBase::SetDefaultResolverProvider(nullptr);
}


void CStorageLinkTest::testUnknownStorageId()
{
	imtbase::CMemoryStorageResolver storage;
	imtbase::CStorageResolverRegistry registry;
	registry.RegisterResolver(&storage);

	const QByteArray objectId = storage.InsertObject(CreateData(1));

	// An unknown non-empty storage ID must not fall back to the default resolver
	DataLink link(objectId, "unknown");
	link.SetResolverProvider(&registry);

	QVERIFY(link.GetResolver() == nullptr);
	QVERIFY(!link.GetObject().IsValid());
	QVERIFY(!link.Save());

	QVERIFY(registry.UnregisterResolver());
	QVERIFY(registry.GetStorageResolver() == nullptr);
}


void CStorageLinkTest::testTypeMismatch()
{
	imtbase::CMemoryStorageResolver storage;

	const QByteArray objectId = storage.InsertObject(istd::IChangeableSharedPtr(new COtherTestData));

	DataLink link(objectId);
	link.SetResolver(&storage);

	QVERIFY(!link.GetObject().IsValid());

	imtbase::StorageLink<COtherTestData> otherLink(objectId);
	otherLink.SetResolver(&storage);

	QVERIFY(otherLink.GetObject().IsValid());
}


void CStorageLinkTest::testStorageParams()
{
	imtbase::CObjectCollection collection;
	SetupCollection(collection);

	imtbase::CCollectionStorageResolver collectionResolver(collection, s_typeId);
	imtbase::CMemoryStorageResolver memoryResolver;

	DataLink collectionLink;
	collectionLink.SetResolver(&collectionResolver);
	QVERIFY(collectionLink.Insert(CreateData(1), "Named item"));

	QVariantMap params = collectionLink.GetStorageParams();
	QCOMPARE(params["storage"].toString(), QString("collection"));
	QCOMPARE(params["objectId"].toByteArray(), collectionLink.GetObjectId());
	QCOMPARE(params["typeId"].toByteArray(), s_typeId);
	QCOMPARE(params["name"].toString(), QString("Named item"));

	DataLink memoryLink;
	memoryLink.SetResolver(&memoryResolver);
	QVERIFY(memoryLink.Insert(CreateData(1)));
	QCOMPARE(memoryLink.GetStorageParams()["storage"].toString(), QString("memory"));
}


void CStorageLinkTest::testSerialization()
{
	imtbase::CMemoryStorageResolver storage;

	DataLink source;
	source.SetResolver(&storage);
	QVERIFY(source.Insert(CreateData(7)));
	source.SetLink(source.GetObjectId(), "images");

	iser::CMemoryWriteArchive writeArchive;
	QVERIFY(source.Serialize(writeArchive));

	DataLink restored;
	iser::CMemoryReadArchive readArchive(writeArchive);
	QVERIFY(restored.Serialize(readArchive));

	QCOMPARE(restored.GetObjectId(), source.GetObjectId());
	QCOMPARE(restored.GetStorageId(), QByteArray("images"));
	QVERIFY(restored == source);

	// Resolver binding is runtime state and is not serialized
	QVERIFY(restored.GetResolver() == nullptr);

	imtbase::CStorageResolverRegistry registry;
	registry.RegisterResolver(&storage, "images");
	restored.SetResolverProvider(&registry);
	QCOMPARE(restored->m_value, 7);
}


void CStorageLinkTest::testLinksInContainer()
{
	imtbase::CMemoryStorageResolver storage;

	QList<DataLink> links;
	for (int i = 0; i < 3; ++i){
		DataLink link;
		link.SetResolver(&storage);
		QVERIFY(link.Insert(CreateData(i)));
		links.append(link);
	}

	QCOMPARE(storage.GetObjectsCount(), 3);

	// Copies of the links share the loaded object
	QList<DataLink> copies = links;
	copies[1]->m_value = 100;
	QCOMPARE(links[1]->m_value, 100);

	// Links keep IDs, so the order of the container doesn't matter
	std::reverse(links.begin(), links.end());
	QCOMPARE(links[0]->m_value, 2);
	QCOMPARE(links[1]->m_value, 100);
	QCOMPARE(links[2]->m_value, 0);
}


void CStorageLinkTest::testOptionalInterface()
{
	imtbase::CMemoryStorageResolver storage;

	// Empty link behaves like an empty optional
	DataLink link;
	QVERIFY(!link.has_value());
	QVERIFY(!link);
	QVERIFY(link.IsNull());
	QCOMPARE(link.value_or(CLinkTestData(99)).m_value, 99);

	bool exceptionThrown = false;
	try{
		link.value();
	}
	catch (const std::bad_optional_access&){
		exceptionThrown = true;
	}
	QVERIFY(exceptionThrown);

	DataLink nullLink(std::nullopt);
	QVERIFY(!nullLink);

	// Engaged link
	link.SetResolver(&storage);
	QVERIFY(link.Insert(CreateData(5)));
	QVERIFY(link.has_value());
	QVERIFY(link);
	QCOMPARE(link.value().m_value, 5);
	QCOMPARE((*link).m_value, 5);
	QCOMPARE(link->m_value, 5);
	QCOMPARE(link.value_or(CLinkTestData(99)).m_value, 5);

	// A link with an ID of a missing object is not engaged, although it is not null
	DataLink dangling("missing-id");
	dangling.SetResolver(&storage);
	QVERIFY(!dangling.IsNull());
	QVERIFY(!dangling.has_value());
	QVERIFY(!dangling);

	// reset only clears the link: the object remains in the storage
	DataLink second(link.GetObjectId());
	second.SetResolver(&storage);

	link.reset();
	QVERIFY(link.IsNull());
	QVERIFY(!link.has_value());
	QVERIFY(link.GetResolver() == &storage);
	QCOMPARE(storage.GetObjectsCount(), 1);
	QVERIFY(second.has_value());

	second = std::nullopt;
	QVERIFY(second.IsNull());
	QVERIFY(!second);
	QCOMPARE(storage.GetObjectsCount(), 1);
}


I_ADD_TEST(CStorageLinkTest);
