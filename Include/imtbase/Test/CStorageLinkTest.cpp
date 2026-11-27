// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include "CStorageLinkTest.h"


// STL includes
#include <algorithm>
#include <atomic>
#include <memory>
#include <optional>
#include <thread>
#include <vector>

// Qt includes
#include <QtCore/QList>
#include <QtTest/QtTest>

// ACF includes
#include <istd/TSingleFactory.h>
#include <icomp/TMakeComponentWrap.h>
#include <icomp/TSimComponentWrap.h>
#include <iser/CArchiveTag.h>
#include <iser/CMemoryReadArchive.h>
#include <iser/CMemoryWriteArchive.h>
#include <itest/CStandardTestExecutor.h>

// ImtCore includes
#include <imtbase/CObjectCollection.h>
#include <imtbase/CCollectionStorageResolver.h>
#include <imtbase/CStorageResolverRegistry.h>
#include <imtbase/CStorageResolverRegistryComp.h>
#include <imtbase/CCollectionStorageResolverComp.h>
#include <imtbase/TStorageLink.h>


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
public:
	// reimplemented (istd::IChangeable)
	virtual int GetSupportedOperations() const override { return SO_COPY; }

	virtual bool CopyFrom(const IChangeable& object, CompatibilityMode /*mode*/ = CM_WITHOUT_REFS) override
	{
		return dynamic_cast<const COtherTestData*>(&object) != nullptr;
	}
};


using DataFactory		= istd::TSingleFactory<istd::IChangeable, CLinkTestData>;
using OtherDataFactory	= istd::TSingleFactory<istd::IChangeable, COtherTestData>;

const QByteArray s_typeId		= QByteArrayLiteral("LinkTestData");
const QByteArray s_otherTypeId	= QByteArrayLiteral("OtherTestData");


void SetupCollection(imtbase::CObjectCollection& collection)
{
	collection.SetOperationFlags(imtbase::IObjectCollection::OF_ALL);
	collection.RegisterFactory<DataFactory>(s_typeId);
	collection.RegisterFactory<OtherDataFactory>(s_otherTypeId);
}


/**
	Object collection of the test data with the storage resolver over it.
	The objects are inserted into the collection, the links only load and save them.
*/
class CSimStorage: public imtbase::CCollectionStorageResolver
{
public:
	CSimStorage()
	{
		SetupCollection(m_collection);
		SetCollection(&m_collection);
	}

	QByteArray InsertData(int value)
	{
		const CLinkTestData data(value);

		return m_collection.InsertNewObject(s_typeId, QString(), QString(), &data);
	}

	imtbase::CObjectCollection m_collection;
};


/**
	Storage that counts the loads, to see how often a link asks the storage.
*/
class CSimCountingStorage: public CSimStorage
{
public:
	CSimCountingStorage(): m_loadCount(0) {}

	mutable std::atomic<int> m_loadCount;

	// reimplemented (imtbase::IStorageResolver)
	virtual istd::IChangeableSharedPtr LoadObject(const Id& objectId) const override
	{
		++m_loadCount;

		return CSimStorage::LoadObject(objectId);
	}
};


using DataLink					= imtbase::TStorageLink<CLinkTestData>;
using SimStorageComp			= icomp::TMakeComponentWrap<CSimStorage, imtbase::IStorageResolver>;
using RegistryComp				= icomp::TSimComponentWrap<imtbase::CStorageResolverRegistryComp>;
using CollectionComp			= icomp::TMakeComponentWrap<imtbase::CObjectCollection, imtbase::IObjectCollection>;
using CollectionResolverComp	= icomp::TSimComponentWrap<imtbase::CCollectionStorageResolverComp>;


DataLink CreateLink(CSimStorage& storage, int value)
{
	DataLink link(storage.InsertData(value));
	link.SetResolver(&storage);

	return link;
}


} // namespace


void CStorageLinkTest::initTestCase()
{
	imtbase::CStorageLinkBase::SetDefaultResolverProvider(nullptr);
}


void CStorageLinkTest::cleanupTestCase()
{
	imtbase::CStorageLinkBase::SetDefaultResolverProvider(nullptr);
}


void CStorageLinkTest::testEmptyLink()
{
	DataLink link;

	QVERIFY(link.IsNull());
	QVERIFY(!link.IsLoaded());
	QVERIFY(link.GetResolver() == nullptr);
	QVERIFY(!link.GetObject().IsValid());
	QVERIFY(!link.Save());
}


void CStorageLinkTest::testCollectionResolverSnapshot()
{
	CSimStorage storage;

	DataLink link = CreateLink(storage, 5);
	QVERIFY(!link.IsNull());
	QCOMPARE(storage.m_collection.GetElementsCount(), 1);

	DataLink other(link.GetObjectId());
	other.SetResolver(&storage);
	QCOMPARE(other, link);

	QCOMPARE(other->m_value, 5);

	// Changes are visible in the collection only after Save
	other->m_value = 6;

	DataLink checkBeforeSave(link.GetObjectId());
	checkBeforeSave.SetResolver(&storage);
	QCOMPARE(checkBeforeSave->m_value, 5);

	QVERIFY(other.Save());

	DataLink checkAfterSave(link.GetObjectId());
	checkAfterSave.SetResolver(&storage);
	QCOMPARE(checkAfterSave->m_value, 6);

	// Unload forces the loading from the collection again
	link.Unload();
	QVERIFY(!link.IsLoaded());
	QCOMPARE(link->m_value, 6);
}


void CStorageLinkTest::testRemovedObject()
{
	CSimStorage storage;

	DataLink link = CreateLink(storage, 1);
	QVERIFY(link.has_value());

	DataLink copy = link;

	QVERIFY(storage.m_collection.RemoveElements(imtbase::IObjectCollection::Ids() << link.GetObjectId()));
	QCOMPARE(storage.m_collection.GetElementsCount(), 0);

	// The links still point to the removed object: after unloading they must not be resolved anymore.
	link.Unload();
	QVERIFY(!link.IsNull());
	QVERIFY(!link.GetObject().IsValid());

	copy.Unload();
	QVERIFY(!copy.GetObject().IsValid());
}


void CStorageLinkTest::testRegistryLateBinding()
{
	CSimStorage desktopStorage;
	CSimStorage serverStorage;

	// The same data model, two "application profiles"
	imtbase::CStorageResolverRegistry desktopProfile;
	desktopProfile.RegisterResolver(&desktopStorage, QByteArrayLiteral("images"));

	imtbase::CStorageResolverRegistry serverProfile;
	serverProfile.RegisterResolver(&serverStorage, QByteArrayLiteral("images"));

	const QByteArray desktopId = desktopStorage.InsertData(1);
	const QByteArray serverId = serverStorage.InsertData(2);

	DataLink desktopLink(desktopId, QByteArrayLiteral("images"));
	desktopLink.SetResolverProvider(&desktopProfile);
	QCOMPARE(desktopLink->m_value, 1);
	QVERIFY(desktopLink.GetResolver() == &desktopStorage);

	DataLink serverLink(serverId, QByteArrayLiteral("images"));
	serverLink.SetResolverProvider(&serverProfile);
	QCOMPARE(serverLink->m_value, 2);
	QVERIFY(serverLink.GetResolver() == &serverStorage);

	// Re-binding to another provider drops the loaded object
	desktopLink.SetLink(serverId, QByteArrayLiteral("images"));
	desktopLink.SetResolverProvider(&serverProfile);
	QVERIFY(!desktopLink.IsLoaded());
	QCOMPARE(desktopLink->m_value, 2);
}


void CStorageLinkTest::testResolverPriority()
{
	CSimStorage directStorage;
	CSimStorage providerStorage;
	CSimStorage defaultStorage;

	imtbase::CStorageResolverRegistry providerRegistry;
	providerRegistry.RegisterResolver(&providerStorage);

	imtbase::CStorageResolverRegistry defaultRegistry;
	defaultRegistry.RegisterResolver(&defaultStorage);
	imtbase::CStorageLinkBase::SetDefaultResolverProvider(&defaultRegistry);

	DataLink link(QByteArrayLiteral("someId"));
	QVERIFY(link.GetResolver() == &defaultStorage);

	link.SetResolverProvider(&providerRegistry);
	QVERIFY(link.GetResolver() == &providerStorage);

	link.SetResolver(&directStorage);
	QVERIFY(link.GetResolver() == &directStorage);

	link.SetResolver(nullptr);
	QVERIFY(link.GetResolver() == &providerStorage);

	imtbase::CStorageLinkBase::SetDefaultResolverProvider(nullptr);
}


void CStorageLinkTest::testDefaultProvider()
{
	CSimStorage storage;
	imtbase::CStorageResolverRegistry registry;
	registry.RegisterResolver(&storage);

	const QByteArray objectId = storage.InsertData(42);

	DataLink link(objectId);
	QVERIFY(link.GetObject().GetPtr() == nullptr);

	imtbase::CStorageLinkBase::SetDefaultResolverProvider(&registry);
	QVERIFY(imtbase::CStorageLinkBase::GetDefaultResolverProvider() == &registry);

	QCOMPARE(link->m_value, 42);

	imtbase::CStorageLinkBase::SetDefaultResolverProvider(nullptr);
}


void CStorageLinkTest::testUnknownStorageId()
{
	CSimStorage storage;
	imtbase::CStorageResolverRegistry registry;
	registry.RegisterResolver(&storage);

	const QByteArray objectId = storage.InsertData(1);

	// An unknown non-empty storage ID must not fall back to the default resolver
	DataLink link(objectId, QByteArrayLiteral("unknown"));
	link.SetResolverProvider(&registry);

	QVERIFY(link.GetResolver() == nullptr);
	QVERIFY(!link.GetObject().IsValid());
	QVERIFY(!link.Save());

	QVERIFY(registry.UnregisterResolver());
	QVERIFY(registry.GetStorageResolver() == nullptr);
}


void CStorageLinkTest::testTypeMismatch()
{
	CSimStorage storage;

	const QByteArray objectId = storage.m_collection.InsertNewObject(s_otherTypeId, QString(), QString());
	QVERIFY(!objectId.isEmpty());

	DataLink link(objectId);
	link.SetResolver(&storage);

	QVERIFY(!link.GetObject().IsValid());

	imtbase::TStorageLink<COtherTestData> otherLink(objectId);
	otherLink.SetResolver(&storage);

	QVERIFY(otherLink.GetObject().IsValid());
}


void CStorageLinkTest::testLinksInContainer()
{
	CSimStorage storage;

	QList<DataLink> links;
	for (int i = 0; i < 3; ++i){
		links.append(CreateLink(storage, i));
	}

	QCOMPARE(storage.m_collection.GetElementsCount(), 3);

	// Copies of the loaded links share the loaded object
	QCOMPARE(links[1]->m_value, 1);

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
	CSimStorage storage;

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
	link = CreateLink(storage, 5);
	QVERIFY(link.has_value());
	QVERIFY(link);
	QCOMPARE(link.value().m_value, 5);
	QCOMPARE((*link).m_value, 5);
	QCOMPARE(link->m_value, 5);
	QCOMPARE(link.value_or(CLinkTestData(99)).m_value, 5);

	// A link with an ID of a missing object is not engaged, although it is not null
	DataLink dangling(QByteArrayLiteral("missing-id"));
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
	QCOMPARE(storage.m_collection.GetElementsCount(), 1);
	QVERIFY(second.has_value());

	second = std::nullopt;
	QVERIFY(second.IsNull());
	QVERIFY(!second);
	QCOMPARE(storage.m_collection.GetElementsCount(), 1);
}


void CStorageLinkTest::testStorageIdKept()
{
	CSimStorage storage;
	imtbase::CStorageResolverRegistry registry;
	registry.RegisterResolver(&storage, QByteArrayLiteral("images"));

	const QByteArray firstId = storage.InsertData(1);
	const QByteArray secondId = storage.InsertData(2);

	DataLink link(firstId, QByteArrayLiteral("images"));
	link.SetResolverProvider(&registry);
	QCOMPARE(link->m_value, 1);

	// A new object ID keeps the storage ID and drops the loaded object
	link.SetObjectId(secondId);
	QVERIFY(!link.IsLoaded());
	QCOMPARE(link.GetStorageId(), QByteArrayLiteral("images"));
	QCOMPARE(link->m_value, 2);

	// Clearing the link keeps the storage ID, so the link does not fall back to the default storage
	link.reset();
	QVERIFY(link.IsNull());
	QCOMPARE(link.GetStorageId(), QByteArrayLiteral("images"));

	link.SetObjectId(firstId);
	link = std::nullopt;
	QCOMPARE(link.GetStorageId(), QByteArrayLiteral("images"));
}


void CStorageLinkTest::testSerializeObjectId()
{
	CSimStorage storage;
	const QByteArray objectId = storage.InsertData(7);

	const QByteArray tagId = QByteArrayLiteral("AddressId");

	DataLink source(objectId, QByteArrayLiteral("images"));
	iser::CMemoryWriteArchive linkArchive;
	QVERIFY(source.SerializeObjectId(linkArchive, tagId));

	// A plain ID field of an existing data model, with its own tag description.
	const iser::CArchiveTag plainTag(tagId, QByteArrayLiteral("Address Id"), iser::CArchiveTag::TT_LEAF);
	QByteArray plainId = objectId;
	iser::CMemoryWriteArchive plainArchive;
	QVERIFY(plainArchive.BeginTag(plainTag));
	QVERIFY(plainArchive.Process(plainId));
	QVERIFY(plainArchive.EndTag(plainTag));

	// The link writes exactly what a plain ID field writes under the same tag name
	const QByteArray linkData(static_cast<const char*>(linkArchive.GetBuffer()), linkArchive.GetBufferSize());
	const QByteArray plainData(static_cast<const char*>(plainArchive.GetBuffer()), plainArchive.GetBufferSize());
	QCOMPARE(linkData, plainData);

	// Data of a plain ID field is read into a link, which keeps its own storage ID
	DataLink restored(QByteArray(), QByteArrayLiteral("images"));
	iser::CMemoryReadArchive readArchive(plainArchive);
	QVERIFY(restored.SerializeObjectId(readArchive, tagId));
	QCOMPARE(restored.GetObjectId(), objectId);
	QCOMPARE(restored.GetStorageId(), QByteArrayLiteral("images"));

	// Resolver binding is runtime state and is not serialized
	QVERIFY(restored.GetResolver() == nullptr);

	imtbase::CStorageResolverRegistry registry;
	registry.RegisterResolver(&storage, QByteArrayLiteral("images"));
	restored.SetResolverProvider(&registry);
	QCOMPARE(restored->m_value, 7);
}


void CStorageLinkTest::testRegistryComponent()
{
	icomp::TSimSharedComponentPtr<SimStorageComp> imagesStoragePtr;
	imagesStoragePtr->InitComponent();

	icomp::TSimSharedComponentPtr<SimStorageComp> defaultStoragePtr;
	defaultStoragePtr->InitComponent();

	const QByteArray objectId = imagesStoragePtr->InsertData(3);

	std::unique_ptr<RegistryComp> registryPtr(new RegistryComp);
	registryPtr->InsertMultiAttr(QByteArrayLiteral("StorageIds"), QByteArrayLiteral("images"));
	registryPtr->InsertMultiRef(QByteArrayLiteral("StorageResolvers"), imagesStoragePtr);
	registryPtr->InsertMultiAttr(QByteArrayLiteral("StorageIds"), QByteArray());
	registryPtr->InsertMultiRef(QByteArrayLiteral("StorageResolvers"), defaultStoragePtr);
	registryPtr->InitComponent();

	// The created registry is the application-wide provider of the links
	QVERIFY(imtbase::CStorageLinkBase::GetDefaultResolverProvider() == registryPtr.get());

	DataLink link(objectId, QByteArrayLiteral("images"));
	QCOMPARE(link->m_value, 3);

	DataLink defaultLink(objectId);
	QVERIFY(defaultLink.GetResolver() == &*defaultStoragePtr);
	QVERIFY(!defaultLink.GetObject().IsValid());

	// The destroyed registry is not the provider anymore
	registryPtr.reset();
	QVERIFY(imtbase::CStorageLinkBase::GetDefaultResolverProvider() == nullptr);
	QVERIFY(link.GetResolver() == nullptr);
}


void CStorageLinkTest::testCollectionResolverComponent()
{
	icomp::TSimSharedComponentPtr<CollectionComp> collectionPtr;
	collectionPtr->InitComponent();
	SetupCollection(*collectionPtr);

	const CLinkTestData data(8);
	const QByteArray objectId = collectionPtr->InsertNewObject(s_typeId, QStringLiteral("Item"), QString(), &data);
	QVERIFY(!objectId.isEmpty());

	std::unique_ptr<CollectionResolverComp> resolverPtr(new CollectionResolverComp);
	resolverPtr->SetRef(QByteArrayLiteral("ObjectCollection"), collectionPtr);
	resolverPtr->InitComponent();

	DataLink link(objectId);
	link.SetResolver(resolverPtr.get());
	QVERIFY(link.has_value());
	QCOMPARE(link->m_value, 8);
}


void CStorageLinkTest::testSaveWithoutStoredObject()
{
	CSimStorage storage;

	// Without a loaded object nothing is written.
	DataLink unloadedLink = CreateLink(storage, 1);
	QVERIFY(!unloadedLink.IsLoaded());
	QVERIFY(!unloadedLink.Save());

	// An object removed from the storage after loading is not created again.
	DataLink link = CreateLink(storage, 2);
	QVERIFY(link.has_value());
	QVERIFY(storage.m_collection.RemoveElements(imtbase::IObjectCollection::Ids() << link.GetObjectId()));
	QVERIFY(!link.Save());
	QCOMPARE(storage.m_collection.GetElementsCount(), 1);
}


void CStorageLinkTest::testMissingObjectLoadedOnce()
{
	CSimCountingStorage resolver;

	DataLink link(QByteArrayLiteral("missing"));
	link.SetResolver(&resolver);

	// A missing object is looked up once, not on every access.
	QVERIFY(!link.has_value());
	QVERIFY(!link.GetObject().IsValid());
	QVERIFY(!link.IsLoaded());
	QCOMPARE(resolver.m_loadCount.load(), 1);

	// A copy knows it too.
	DataLink copy = link;
	QVERIFY(!copy.has_value());
	QCOMPARE(resolver.m_loadCount.load(), 1);

	// Unloading looks again.
	link.Unload();
	QVERIFY(!link.has_value());
	QCOMPARE(resolver.m_loadCount.load(), 2);

	// A new target is looked up.
	const QByteArray objectId = resolver.InsertData(5);
	link.SetObjectId(objectId);
	QVERIFY(link.has_value());
	QCOMPARE(link->m_value, 5);
	QCOMPARE(resolver.m_loadCount.load(), 3);

	// Without a resolver nothing is remembered, so a later binding finds the object.
	DataLink unboundLink(objectId);
	QVERIFY(!unboundLink.has_value());
	unboundLink.SetResolver(&resolver);
	QVERIFY(unboundLink.has_value());
}


void CStorageLinkTest::testConcurrentLoading()
{
	CSimCountingStorage resolver;
	const QByteArray objectId = resolver.InsertData(9);

	DataLink link(objectId);
	link.SetResolver(&resolver);

	const DataLink& constLink = link;
	std::atomic<int> failureCount(0);

	std::vector<std::thread> threads;
	for (int threadIndex = 0; threadIndex < 8; ++threadIndex){
		threads.emplace_back([&constLink, &failureCount](){
			for (int i = 0; i < 1000; ++i){
				if (!constLink.has_value() || (constLink->m_value != 9)){
					++failureCount;
				}
			}
		});
	}

	for (std::thread& thread : threads){
		thread.join();
	}

	QCOMPARE(failureCount.load(), 0);
	QCOMPARE(resolver.m_loadCount.load(), 1);
}


I_ADD_TEST(CStorageLinkTest);

