// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include "CStorageLinkTest.h"


// STL includes
#include <algorithm>
#include <optional>

// Qt includes
#include <QtCore/QList>
#include <QtTest/QtTest>

// ACF includes
#include <istd/TSingleFactory.h>
#include <iser/IArchive.h>
#include <iser/ISerializable.h>
#include <iser/CArchiveTag.h>
#include <iser/CMinimalVersionInfo.h>
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


/**
	Serializable data without comparing: its changes can be detected only by serialization.
*/
class CSerializableTestData: virtual public iser::ISerializable
{
public:
	CSerializableTestData(int value = 0): m_value(value) {}

	int m_value;

	// reimplemented (iser::ISerializable)
	virtual bool Serialize(iser::IArchive& archive) override
	{
		iser::CArchiveTag valueTag("Value", "Test value", iser::CArchiveTag::TT_LEAF);
		bool retVal = archive.BeginTag(valueTag);
		retVal = retVal && archive.Process(m_value);
		retVal = retVal && archive.EndTag(valueTag);

		return retVal;
	}

	// reimplemented (istd::IChangeable)
	virtual int GetSupportedOperations() const override { return SO_CLONE | SO_COPY; }

	virtual bool CopyFrom(const IChangeable& object, CompatibilityMode /*mode*/ = CM_WITHOUT_REFS) override
	{
		const CSerializableTestData* sourcePtr = dynamic_cast<const CSerializableTestData*>(&object);
		if (sourcePtr != nullptr){
			m_value = sourcePtr->m_value;

			return true;
		}

		return false;
	}

	virtual istd::IChangeableUniquePtr CloneMe(CompatibilityMode /*mode*/ = CM_WITHOUT_REFS) const override
	{
		return istd::IChangeableUniquePtr(new CSerializableTestData(m_value));
	}
};


typedef imtbase::StorageLink<CLinkTestData> DataLink;
typedef istd::TSingleFactory<istd::IChangeable, CLinkTestData> DataFactory;

const QByteArray s_typeId = "LinkTestData";


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


/**
	Collection resolver counting the writes.
*/
class CCountingCollectionResolver: public imtbase::CCollectionStorageResolver
{
public:
	typedef imtbase::CCollectionStorageResolver BaseClass;

	CCountingCollectionResolver(
				imtbase::IObjectCollection& collection,
				const QByteArray& typeId = s_typeId,
				const iser::IVersionInfo* versionInfoPtr = nullptr)
		:BaseClass(collection, typeId, versionInfoPtr),
		m_saveCount(0)
	{
	}

	virtual bool SaveObject(const Id& objectId, const istd::IChangeable& object) override
	{
		++m_saveCount;

		return BaseClass::SaveObject(objectId, object);
	}

	int m_saveCount;
};


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

	DataLink link = CreateLink(resolver, 5, "Item");
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

	DataLink collectionLink = CreateLink(collectionResolver, 1, "Named item");

	QVariantMap params = collectionLink.GetStorageParams();
	QCOMPARE(params["storage"].toString(), QString("collection"));
	QCOMPARE(params["objectId"].toByteArray(), collectionLink.GetObjectId());
	QCOMPARE(params["typeId"].toByteArray(), s_typeId);
	QCOMPARE(params["name"].toString(), QString("Named item"));

	DataLink memoryLink = CreateLink(memoryResolver, 1);
	QCOMPARE(memoryLink.GetStorageParams()["storage"].toString(), QString("memory"));
}


void CStorageLinkTest::testSerialization()
{
	imtbase::CMemoryStorageResolver storage;

	DataLink source = CreateLink(storage, 7);
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


void CStorageLinkTest::testConstLink()
{
	imtbase::CObjectCollection collection;
	SetupCollection(collection);

	imtbase::CCollectionStorageResolver resolver(collection, s_typeId);

	const QByteArray objectId = resolver.InsertObject(CreateData(3));

	// Read-only links are used to follow the references of a data model
	imtbase::StorageLink<const CLinkTestData> link(objectId);
	link.SetResolver(&resolver);

	QVERIFY(link);
	QCOMPARE(link->m_value, 3);
	QCOMPARE(link.value().m_value, 3);

	istd::TSharedInterfacePtr<const CLinkTestData> objectPtr = link.GetObject();
	QVERIFY(objectPtr.IsValid());
	QCOMPARE(objectPtr->m_value, 3);

	imtbase::StorageLink<const COtherTestData> otherLink(objectId);
	otherLink.SetResolver(&resolver);
	QVERIFY(!otherLink);
}


void CStorageLinkTest::testConstLinkNeverSaves()
{
	imtbase::CObjectCollection collection;
	SetupCollection(collection);

	CCountingCollectionResolver resolver(collection);

	const QByteArray objectId = resolver.InsertObject(CreateData(4));

	imtbase::StorageLink<const CLinkTestData> link(objectId);
	link.SetResolver(&resolver);
	QCOMPARE(link->m_value, 4);

	// A read-only link can't change the object, so saving is a no-op
	QVERIFY(link.Save());
	QCOMPARE(resolver.m_saveCount, 0);
}


void CStorageLinkTest::testSaveRemovedObject()
{
	imtbase::CObjectCollection collection;
	SetupCollection(collection);

	imtbase::CCollectionStorageResolver resolver(collection, s_typeId);

	DataLink link = CreateLink(resolver, 1);
	link->m_value = 2;

	QVERIFY(collection.RemoveElements(imtbase::IObjectCollection::Ids() << link.GetObjectId()));

	// The loaded object of a removed one must not be written back, and must not create a new object
	QVERIFY(!link.Save());
	QCOMPARE(collection.GetElementsCount(), 0);

	// Without a loaded object there is nothing to save
	link.Unload();
	QVERIFY(link.Save());
}


void CStorageLinkTest::testSaveUnchangedObject()
{
	imtbase::CObjectCollection collection;
	SetupCollection(collection);

	CCountingCollectionResolver resolver(collection);

	DataLink link = CreateLink(resolver, 1);
	link.Unload();

	// Loaded and not changed: no write, no new revision in the storages with revisions
	QCOMPARE(link->m_value, 1);
	QVERIFY(link.Save());
	QCOMPARE(resolver.m_saveCount, 0);

	link->m_value = 2;
	QVERIFY(link.Save());
	QCOMPARE(resolver.m_saveCount, 1);

	// The saved state follows the last saving, also for the copies of the link
	DataLink copy = link;
	QVERIFY(link.Save());
	QVERIFY(copy.Save());
	QCOMPARE(resolver.m_saveCount, 1);

	// A changed and reverted object is not written either
	link->m_value = 3;
	link->m_value = 2;
	QVERIFY(link.Save());
	QCOMPARE(resolver.m_saveCount, 1);

	// An object replaced by a different one is written
	link = CreateData(5);
	QVERIFY(link.Save());
	QCOMPARE(resolver.m_saveCount, 2);

	DataLink check(link.GetObjectId());
	check.SetResolver(&resolver);
	QCOMPARE(check->m_value, 5);
}


void CStorageLinkTest::testSerializedChangeDetection()
{
	using SerializableFactory = istd::TSingleFactory<istd::IChangeable, CSerializableTestData>;
	using SerializableLink = imtbase::StorageLink<CSerializableTestData>;

	const QByteArray typeId = QByteArrayLiteral("SerializableTestData");

	imtbase::CObjectCollection collection;
	collection.SetOperationFlags(imtbase::IObjectCollection::OF_ALL);
	collection.RegisterFactory<SerializableFactory>(typeId);

	CSerializableTestData versionSource;
	iser::CMinimalVersionInfo versionInfo(&versionSource);

	// With the version info the object is compared by serialization
	CCountingCollectionResolver resolver(collection, typeId, &versionInfo);

	SerializableLink link(resolver.InsertObject(istd::TSharedInterfacePtr<CSerializableTestData>(new CSerializableTestData(1))));
	link.SetResolver(&resolver);

	QCOMPARE(link->m_value, 1);
	QVERIFY(link.Save());
	QCOMPARE(resolver.m_saveCount, 0);

	link->m_value = 2;
	QVERIFY(link.Save());
	QCOMPARE(resolver.m_saveCount, 1);

	QVERIFY(link.Save());
	QCOMPARE(resolver.m_saveCount, 1);

	// Without the version info and without comparing the changes can't be detected: the object is not written at all
	CCountingCollectionResolver blindResolver(collection, typeId);

	SerializableLink blindLink(link.GetObjectId());
	blindLink.SetResolver(&blindResolver);
	QCOMPARE(blindLink->m_value, 2);

	blindLink->m_value = 3;
	QVERIFY(!blindLink.Save());
	QCOMPARE(blindResolver.m_saveCount, 0);
}


I_ADD_TEST(CStorageLinkTest);
