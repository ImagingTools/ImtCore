// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QReadWriteLock>

// ACF includes
#include <istd/TComposedFactory.h>
#include <icomp/CComponentBase.h>
#include <istd/TDelPtr.h>
#include <istd/TPointerVector.h>
#include <imod/TSingleModelObserverBase.h>

// ImtCore includes
#include <imtbase/IObjectCollection.h>


namespace imtbase
{


/**
	Implementation of a cached data object collection.
	\ingroup Collection
*/
class CCachedObjectCollectionComp:
			public icomp::CComponentBase,
			public imod::TSingleModelObserverBase<istd::IChangeable>,
			virtual public imtbase::IObjectCollection
{
public:
	typedef icomp::CComponentBase BaseClass;
	typedef imod::TSingleModelObserverBase<istd::IChangeable> BaseClass2;

	I_BEGIN_COMPONENT(CCachedObjectCollectionComp);
		I_REGISTER_INTERFACE(IObjectCollection);
		I_REGISTER_INTERFACE(IObjectCollectionInfo);
		I_REGISTER_INTERFACE(ICollectionInfo);
		I_ASSIGN(m_objectCollectionCompPtr, "ObjectCollection", "Base collection containing real data", true, "ObjectCollection");
		I_ASSIGN_TO(m_objectCollectionModelCompPtr, m_objectCollectionCompPtr, true);
		I_ASSIGN(m_metaInfoCacheLimitAttrPtr, "MetaInfoCacheLimit", "Maximal count of filter combinations stored in the ring buffer (meta info cache)", true, 1000);
		I_ASSIGN(m_objectCacheLimitAttrPtr, "ObjectCacheLimit", "Maximal count of the data objects in the ring buffer (cache)", true, 100);
	I_END_COMPONENT;

	CCachedObjectCollectionComp();

	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;
	virtual void OnComponentDestroyed() override;

	// reimplemented (imod::CSingleModelObserverBase)
	virtual void OnUpdate(const istd::IChangeable::ChangeSet& changeSet) override;

	// reimplemented (IObjectCollection)
	virtual const IRevisionController* GetRevisionController() const override;
	virtual const ICollectionDataController* GetDataController() const override;
	virtual int GetOperationFlags(const Id& objectId = Id()) const override;
	virtual Id InsertNewObject(
				const QByteArray& typeId,
				const QString& name,
				const QString& description,
				const istd::IChangeable* defaultValuePtr = nullptr,
				const Id& proposedElementId = Id(),
				const idoc::IDocumentMetaInfo* dataMetaInfoPtr = nullptr,
				const idoc::IDocumentMetaInfo* elementMetaInfoPtr = nullptr,
				const IOperationContext* operationContextPtr = nullptr) override;
	virtual bool RemoveElements(const Ids& elementIds, const IOperationContext* operationContextPtr = nullptr) override;
	virtual bool RemoveElementSet(
				const iprm::IParamsSet* selectionParamsPtr = nullptr,
				const IOperationContext* operationContextPtr = nullptr) override;
	virtual bool RestoreObjects(
				const Ids& objectIds,
				const IOperationContext* operationContextPtr = nullptr) override;
	virtual bool RestoreObjectSet(
				const iprm::IParamsSet* selectionParamsPtr = nullptr,
				const IOperationContext* operationContextPtr = nullptr) override;
	virtual const istd::IChangeable* GetObjectPtr(const Id& objectId) const override;
	virtual bool GetObjectData(const Id& objectId, DataPtr& dataPtr, const iprm::IParamsSet* dataConfigurationPtr = nullptr) const override;
	virtual bool SetObjectData(const Id& objectId, const istd::IChangeable& object, CompatibilityMode mode = CM_WITHOUT_REFS, const IOperationContext* operationContextPtr = nullptr) override;
	virtual IObjectCollectionUniquePtr CreateSubCollection(int offset, int count, const iprm::IParamsSet *selectionParamsPtr) const override;
	virtual imtbase::IObjectCollectionIterator* CreateObjectCollectionIterator(
				const QByteArray& objectId = QByteArray(),
				int offset = 0,
				int count = -1,
				const iprm::IParamsSet* selectionParamsPtr = nullptr) const override;

	// reimplemented (IObjectCollectionInfo)
	virtual const iprm::IOptionsList* GetObjectTypesInfo() const override;
	virtual QByteArray GetObjectTypeId(const Id& objectId) const override;
	virtual idoc::MetaInfoPtr GetDataMetaInfo(const Id& objectId) const override;

	// reimplemented (ICollectionInfo)
	virtual int GetElementsCount(
				const iprm::IParamsSet* selectionParamsPtr = nullptr,
				ilog::IMessageConsumer* logPtr = nullptr) const override;
	virtual Ids GetElementIds(
				int offset = 0,
				int count = -1,
				const iprm::IParamsSet* selectionParamsPtr = nullptr,
				ilog::IMessageConsumer* logPtr = nullptr) const override;
	virtual bool GetSubsetInfo(
				ICollectionInfo& subsetInfo,
				int offset = 0,
				int count = -1,
				const iprm::IParamsSet* selectionParamsPtr = nullptr,
				ilog::IMessageConsumer* logPtr = nullptr) const override;
	virtual QVariant GetElementInfo(const Id& elementId, int infoType, ilog::IMessageConsumer* logPtr = nullptr) const override;
	virtual idoc::MetaInfoPtr GetElementMetaInfo(const Id& elementId, ilog::IMessageConsumer* logPtr = nullptr) const override;
	virtual bool SetElementName(const Id& elementId, const QString& name, ilog::IMessageConsumer* logPtr = nullptr) override;
	virtual bool SetElementDescription(const Id& elementId, const QString& description, ilog::IMessageConsumer* logPtr = nullptr) override;
	virtual bool SetElementEnabled(const Id& elementId, bool isEnabled = true, ilog::IMessageConsumer* logPtr = nullptr) override;

protected:
	struct FilteredCollection
	{
		FilteredCollection(int aOffset, int aCount, const QByteArray& aSelectionParamsData, IObjectCollectionUniquePtr&& aCachePtr)
			:offset(aOffset),
			count(aCount),
			selectionParamsData(aSelectionParamsData),
			cachePtr(std::move(aCachePtr))
		{
		}
		int offset;
		int count;
		QByteArray selectionParamsData;
		imtbase::IObjectCollectionUniquePtr cachePtr;
	};

	FilteredCollection* GetFilteredCollection(
				int offset = 0,
				int count = -1,
				const iprm::IParamsSet* selectionParamsPtr = nullptr) const;

	void ClearCache();
	void RemoveOldestObjectFromCache() const;

private:
	I_REF(imtbase::IObjectCollection, m_objectCollectionCompPtr);
	I_REF(imod::IModel, m_objectCollectionModelCompPtr);
	I_ATTR(int, m_metaInfoCacheLimitAttrPtr);
	I_ATTR(int, m_objectCacheLimitAttrPtr);

	mutable istd::TPointerVector<FilteredCollection> m_cachedCollections;

	struct CacheItem
	{
		istd::IChangeableSharedPtr dataPtr;
		qint64 timestamp = 0;
	};

	typedef QMap<QByteArray, CacheItem> CacheItemMap;
	mutable CacheItemMap m_cacheItems;

	int m_operationFlags;
	mutable QReadWriteLock m_lock;
};


} // namespace imtbase


