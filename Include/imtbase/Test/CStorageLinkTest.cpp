// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include "CStorageLinkTest.h"


// STL includes
#include <algorithm>
#include <memory>
#include <optional>

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
#include <imtbase/CMemoryStorageResolver.h>
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
};


using DataLink					= imtbase::TStorageLink<CLinkTestData>;
using DataFactory				= istd::TSingleFactory<istd::IChangeable, CLinkTestData>;
using MemoryResolverComp		= icomp::TMakeComponentWrap<imtbase::CMemoryStorageResolver, imtbase::IStorageResolver>;
using RegistryComp				= icomp::TSimComponentWrap<imtbase::CStorageResolverRegistryComp>;
using CollectionComp			= icomp::TMakeComponentWrap<imtbase::CObjectCollection, imtbase::IObjectCollection>;
using CollectionResolverComp	= icomp::TSimComponentWrap<imtbase::CCollectionStorageResolverComp>;

const QByteArray s_typeId = QByteArrayLiteral("LinkTestData");


istd::TSharedInterfacePtr<CLinkTestData> CreateData(int value)
{
	return istd::TSharedInterfacePtr<CLinkTestData>(new CLinkTestData(value));
}


DataLink CreateLink(imtbase::IStorageResolver& resolver, int value, const QString& name = QString())
{
	DataLink link(resolver.InsertObject(CreateData(value), name));
	link.SetResolver(&resolver);

	return link;
}


void SetupCollection(imtbase::CObjectCollection& collection)
{
	collection.SetOperationFlags(imtbase::IObjectCollection::OF_ALL);
	collection.RegisterFactory<DataFactory>(s_typeId);
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
	QVERIFY(!link.Remove());
}


void CStorageLinkTest::testMemoryResolverLiveObject()
{
	imtbase::CMemoryStorageResolver resolver;

	DataLink first = CreateLink(resolver, 10);
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

	DataLink link = CreateLink(resolver, 5, QStringLiteral("Item"));
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

	DataLink link = CreateLink(resolver, 1);

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
	desktopProfile.RegisterResolver(&desktopStorage, QByteArrayLiteral("images"));

	imtbase::CStorageResolverRegistry serverProfile;
	serverProfile.RegisterResolver(&serverStorage, QByteArrayLiteral("images"));

	const QByteArray desktopId = desktopStorage.InsertObject(CreateData(1));
	const QByteArray serverId = serverStorage.InsertObject(CreateData(2));

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
	imtbase::CMemoryStorageResolver directStorage;
	imtbase::CMemoryStorageResolver providerStorage;
	imtbase::CMemoryStorageResolver defaultStorage;

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
	imtbase::CMemoryStorageResolver storage;
	imtbase::CStorageResolverRegistry registry;
	registry.RegisterResolver(&storage);

	const QByteArray objectId = storage.InsertObject(CreateData(42));

	DataLink link(objectId);
	QVERIFY(link.GetObject().GetPtr() == nullptr);

	imtbase::CStorageLinkBase::SetDefaultResolverProvider(&registry);
	QVERIFY(imtbase::CStorageLinkBase::GetDefaultResolverProvider() == &registry);

	QCOMPARE(link->m_value, 42);

	imtbase::CStorageLinkBase::SetDefaultResolverProvider(nullptr);
}


void CStorageLinkTest::testUnknownStorageId()
{
	imtbase::CMemoryStorageResolver storage;
	imtbase::CStorageResolverRegistry registry;
	registry.RegisterResolver(&storage);

	const QByteArray objectId = storage.InsertObject(CreateData(1));

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
	imtbase::CMemoryStorageResolver storage;

	const QByteArray objectId = storage.InsertObject(istd::IChangeableSharedPtr(new COtherTestData));

	DataLink link(objectId);
	link.SetResolver(&storage);

	QVERIFY(!link.GetObject().IsValid());

	imtbase::TStorageLink<COtherTestData> otherLink(objectId);
	otherLink.SetResolver(&storage);

	QVERIFY(otherLink.GetObject().IsValid());
}


void CStorageLinkTest::testSerialization()
{
	imtbase::CMemoryStorageResolver storage;

	DataLink source = CreateLink(storage, 7);
	source.SetLink(source.GetObjectId(), QByteArrayLiteral("images"));

	iser::CMemoryWriteArchive writeArchive;
	QVERIFY(source.Serialize(writeArchive));

	DataLink restored;
	iser::CMemoryReadArchive readArchive(writeArchive);
	QVERIFY(restored.Serialize(readArchive));

	QCOMPARE(restored.GetObjectId(), source.GetObjectId());
	QCOMPARE(restored.GetStorageId(), QByteArrayLiteral("images"));
	QVERIFY(restored == source);

	// Resolver binding is runtime state and is not serialized
	QVERIFY(restored.GetResolver() == nullptr);

	imtbase::CStorageResolverRegistry registry;
	registry.RegisterResolver(&storage, QByteArrayLiteral("images"));
	restored.SetResolverProvider(&registry);
	QCOMPARE(restored->m_value, 7);
}


void CStorageLinkTest::testLinksInContainer()
{
	imtbase::CMemoryStorageResolver storage;

	QList<DataLink> links;
	for (int i = 0; i < 3; ++i){
		links.append(CreateLink(storage, i));
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
	QCOMPARE(storage.GetObjectsCount(), 1);
	QVERIFY(second.has_value());

	second = std::nullopt;
	QVERIFY(second.IsNull());
	QVERIFY(!second);
	QCOMPARE(storage.GetObjectsCount(), 1);
}


void CStorageLinkTest::testStorageIdKept()
{
	imtbase::CMemoryStorageResolver storage;
	imtbase::CStorageResolverRegistry registry;
	registry.RegisterResolver(&storage, QByteArrayLiteral("images"));

	const QByteArray firstId = storage.InsertObject(CreateData(1));
	const QByteArray secondId = storage.InsertObject(CreateData(2));

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
	imtbase::CMemoryStorageResolver storage;
	const QByteArray objectId = storage.InsertObject(CreateData(7));

	const iser::CArchiveTag tag(QByteArrayLiteral("AddressId"), QByteArrayLiteral("Address Id"), iser::CArchiveTag::TT_LEAF);

	DataLink source(objectId, QByteArrayLiteral("images"));
	iser::CMemoryWriteArchive linkArchive;
	QVERIFY(source.SerializeObjectId(linkArchive, tag));

	QByteArray plainId = objectId;
	iser::CMemoryWriteArchive plainArchive;
	QVERIFY(plainArchive.BeginTag(tag));
	QVERIFY(plainArchive.Process(plainId));
	QVERIFY(plainArchive.EndTag(tag));

	// The link writes exactly what a plain ID field writes under the same tag
	const QByteArray linkData(static_cast<const char*>(linkArchive.GetBuffer()), linkArchive.GetBufferSize());
	const QByteArray plainData(static_cast<const char*>(plainArchive.GetBuffer()), plainArchive.GetBufferSize());
	QCOMPARE(linkData, plainData);

	// Data of a plain ID field is read into a link, which keeps its own storage ID
	DataLink restored(QByteArray(), QByteArrayLiteral("images"));
	iser::CMemoryReadArchive readArchive(plainArchive);
	QVERIFY(restored.SerializeObjectId(readArchive, tag));
	QCOMPARE(restored.GetObjectId(), objectId);
	QCOMPARE(restored.GetStorageId(), QByteArrayLiteral("images"));

	imtbase::CStorageResolverRegistry registry;
	registry.RegisterResolver(&storage, QByteArrayLiteral("images"));
	restored.SetResolverProvider(&registry);
	QCOMPARE(restored->m_value, 7);
}


void CStorageLinkTest::testRegistryComponent()
{
	icomp::TSimSharedComponentPtr<MemoryResolverComp> imagesStoragePtr;
	imagesStoragePtr->InitComponent();

	icomp::TSimSharedComponentPtr<MemoryResolverComp> defaultStoragePtr;
	defaultStoragePtr->InitComponent();

	const QByteArray objectId = imagesStoragePtr->InsertObject(CreateData(3));

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

	// The compositions that only read the linked objects leave the type-ID out.
	std::unique_ptr<CollectionResolverComp> resolverPtr(new CollectionResolverComp);
	resolverPtr->SetRef(QByteArrayLiteral("ObjectCollection"), collectionPtr);
	resolverPtr->InitComponent();

	DataLink link(objectId);
	link.SetResolver(resolverPtr.get());
	QVERIFY(link.has_value());
	QCOMPARE(link->m_value, 8);
}


I_ADD_TEST(CStorageLinkTest);
