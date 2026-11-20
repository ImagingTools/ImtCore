// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include "CCachedObjectCollectionTest.h"

#include "CTestData.h"


#include <functional>
#include <icomp/TSimComponentWrap.h>
#include <imod/TModelWrap.h>
#include <istd/TSingleFactory.h>
#include <itest/CStandardTestExecutor.h>
#include <imtbase/CCachedObjectCollectionComp.h>
#include <imtbase/CObjectCollection.h>


namespace
{

class CCacheTestData: public CTestData
{
public:
	bool CopyFrom(const istd::IChangeable& source, CompatibilityMode) override
	{
		return dynamic_cast<const CCacheTestData*>(&source) != nullptr;
	}
};

class CCacheTestCollection: public imtbase::CObjectCollection
{
public:
	CCacheTestCollection()
	{
		using TestDataFactory = istd::TSingleFactory<istd::IChangeable, CCacheTestData>;
		RegisterFactory<TestDataFactory>(QByteArrayLiteral("TestType"));
		SetOperationFlags(OF_ALL);
	}

protected:
	imtbase::IObjectCollection* CreateSubCollectionInstance() const override
	{
		return new CCacheTestCollection;
	}
};


// The data of the remote server as the local server sees it: a source and a cache over it
class CSourceCollectionComp: public icomp::CComponentBase, public imod::TModelWrap<CCacheTestCollection>
{
public:
	typedef icomp::CComponentBase BaseClass;
	I_BEGIN_COMPONENT(CSourceCollectionComp)
		I_REGISTER_INTERFACE(imtbase::IObjectCollection);
		I_REGISTER_INTERFACE(imod::IModel);
	I_END_COMPONENT

	mutable int objectReads = 0;
	mutable int filteredReads = 0;
	std::function<void()> afterSnapshot;

	bool GetObjectData(const Id& id, DataPtr& data, const iprm::IParamsSet* paramsPtr) const override
	{
		++objectReads;
		const bool result = imtbase::CObjectCollection::GetObjectData(id, data, paramsPtr);
		if (afterSnapshot){
			afterSnapshot();
		}
		return result;
	}

	imtbase::IObjectCollectionUniquePtr CreateSubCollection(int offset, int count, const iprm::IParamsSet* paramsPtr) const override
	{
		++filteredReads;
		auto result = imtbase::CObjectCollection::CreateSubCollection(offset, count, paramsPtr);
		if (afterSnapshot){
			afterSnapshot();
		}
		return result;
	}
};


class CObservableCacheComp: public imod::TModelWrap<imtbase::CCachedObjectCollectionComp>
{
public:
	typedef imod::TModelWrap<imtbase::CCachedObjectCollectionComp> BaseClass;
	I_BEGIN_COMPONENT(CObservableCacheComp)
		I_REGISTER_INTERFACE(imod::IModel);
	I_END_COMPONENT
};


} // namespace


void CCachedObjectCollectionTest::InvalidationDuringSourceRead()
{
	icomp::TSimSharedComponentPtr<CSourceCollectionComp> source;
	source->InitComponent();
	const QByteArray id = source->InsertNewObject(QByteArrayLiteral("TestType"), QStringLiteral("Sample"), QString());
	QVERIFY(!id.isEmpty());
	icomp::TSimSharedComponentPtr<CObservableCacheComp> cache;
	cache->SetRef("ObjectCollection", source);
	cache->SetIntAttr("ObjectCacheLimit", 2);
	cache->SetIntAttr("MetaInfoCacheLimit", 2);
	cache->InitComponent();

	source->afterSnapshot = [&](){ cache->InvalidateCache(); };
	imtbase::IObjectCollection::DataPtr object;
	QVERIFY(cache->GetObjectData(id, object));
	QVERIFY(object.IsValid());
	auto subCollection = cache->CreateSubCollection(0, -1, nullptr);
	QVERIFY(subCollection.IsValid());
	QCOMPARE(subCollection->GetElementIds(), imtbase::ICollectionInfo::Ids{id});
	QCOMPARE(cache->GetElementIds(), imtbase::ICollectionInfo::Ids{id});

	source->afterSnapshot = {};
	for (int i = 0; i < 2; ++i){
		QVERIFY(cache->GetObjectData(id, object));
		QCOMPARE(cache->GetElementIds(), imtbase::ICollectionInfo::Ids{id});
	}
	QCOMPARE(source->objectReads, 2);
	QCOMPARE(source->filteredReads, 3);
}


I_ADD_TEST(CCachedObjectCollectionTest);
