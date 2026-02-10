// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QVector>
#include <QtCore/QSet>

// ACF includes
#include <istd/TOptInterfacePtr.h>
#include <imod/CModelUpdateBridge.h>

// ImtCore includes
#include <imtbase/IOrderedObjectCollection.h>


namespace imtbase
{


/**
	Implementation of an ordered object collection facade.
	This class wraps an existing IObjectCollection and maintains a custom ordering for its items.
	Uses composition (aggregation) to delegate operations to the wrapped collection.
	
	\note Change Notification: The proxy uses imod::CModelUpdateBridge to forward change notifications
	from the aggregated collection to observers of the proxy. When the underlying collection changes,
	observers of the proxy are automatically notified.
	
	\note The proxy automatically synchronizes the custom order with the aggregated collection's current
	state. If items are added or removed from the aggregated collection, the custom order will be
	updated when GetOrderedItemIds() or GetElementIds() is called.
	
	\note Ownership: The proxy uses istd::TOptInterfacePtr which supports both managed (owned) 
	and unmanaged (non-owned) pointers. Use the takeOwnership parameter to control ownership.
	
	\ingroup Collection
*/
class COrderedObjectCollectionProxy: virtual public IOrderedObjectCollection
{
public:
	/**
		Constructor.
		\param collectionPtr    Pointer to the collection to wrap (must not be null).
		\param takeOwnership    If true, proxy takes ownership; if false, caller manages lifetime (default: false).
	*/
	explicit COrderedObjectCollectionProxy(IObjectCollection* collectionPtr, bool takeOwnership = false);
	
	COrderedObjectCollectionProxy(IObjectCollectionUniquePtr&& collectionPtr);
	COrderedObjectCollectionProxy(const istd::TOptInterfacePtr<IObjectCollection>& collectionPtr);

	virtual ~COrderedObjectCollectionProxy();

	// reimplemented (IOrderedObjectCollection)
	virtual bool SetItemOrder(const Id& itemId, int position) override;
	virtual int GetItemOrder(const Id& itemId) const override;
	virtual bool SetItemsOrder(const Ids& orderedIds) override;
	virtual Ids GetOrderedItemIds() const override;
	virtual bool ResetItemOrder() override;

	// reimplemented (imtbase::IObjectCollection)
	virtual IHierarchicalStructure* GetCollectionStructure() const override;
	virtual const IRevisionController* GetRevisionController() const override;
	virtual const ICollectionDataController* GetDataController() const override;
	virtual int GetOperationFlags(const QByteArray& objectId = QByteArray()) const override;
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
	virtual bool SetObjectData(
				const Id& objectId,
				const istd::IChangeable& object,
				CompatibilityMode mode = CM_WITHOUT_REFS,
				const IOperationContext* operationContextPtr = nullptr) override;
	virtual IObjectCollectionUniquePtr CreateSubCollection(
				int offset = 0,
				int count = -1,
				const iprm::IParamsSet* selectionParamsPtr = nullptr) const override;
	virtual IObjectCollectionIterator* CreateObjectCollectionIterator(
				const QByteArray& objectId = QByteArray(),
				int offset = 0,
				int count = -1,
				const iprm::IParamsSet* selectionParamsPtr = nullptr) const override;

	// reimplemented (IObjectCollectionInfo)
	virtual const iprm::IOptionsList* GetObjectTypesInfo() const override;
	virtual Id GetObjectTypeId(const Id& objectId) const override;
	virtual idoc::MetaInfoPtr GetDataMetaInfo(const Id& objectId) const override;

	// reimplemented (ICollectionInfo)
	virtual int GetElementsCount(
				const iprm::IParamsSet* selectionParamPtr = nullptr,
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

	// reimplemented (iser::ISerializable)
	virtual bool Serialize(iser::IArchive& archive) override;

	// reimplemented (istd::IChangeable)
	virtual int GetSupportedOperations() const override;
	virtual bool CopyFrom(const IChangeable& object, CompatibilityMode mode = CM_WITHOUT_REFS) override;
	virtual bool IsEqual(const IChangeable& object) const override;
	virtual istd::IChangeableUniquePtr CloneMe(CompatibilityMode mode = CM_WITHOUT_REFS) const override;
	virtual bool ResetData(CompatibilityMode mode = CM_WITHOUT_REFS) override;

private:
	/**
		Get the ordered list of IDs from the aggregated collection.
	*/
	Ids GetCollectionElementIds() const;

	/**
		Synchronize the custom order with the aggregated collection's current state.
		This adds any new items and removes deleted items from the custom order.
	*/
	void SynchronizeOrder() const;

	/**
		Apply the custom order to a list of IDs.
	*/
	Ids ApplyCustomOrder(const Ids& ids) const;
	
	/**
		Copy the ordering state from another proxy (used internally).
	*/
	void CopyOrderingState(const COrderedObjectCollectionProxy& other);
	
	/**
		Attach the aggregated collection to the update bridge (helper for constructors).
	*/
	void AttachCollectionObserver();

private:
	istd::TOptInterfacePtr<IObjectCollection> m_collectionPtr;	// Managed or unmanaged pointer to the aggregated collection
	imod::CModelUpdateBridge m_updateBridge;					// Bridge for forwarding change notifications from aggregated collection to proxy observers
	mutable QVector<QByteArray> m_customOrder;					// Stores the custom order of item IDs when m_hasCustomOrder is true
	mutable bool m_hasCustomOrder;								// Flag indicating whether a custom order is currently active
};


} // namespace imtbase
