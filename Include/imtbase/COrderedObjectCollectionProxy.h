#pragma once


// Qt includes
#include <QtCore/QVector>
#include <QtCore/QMap>

// ACF includes
#include <istd/TComposedFactory.h>

// ImtCore includes
#include <imtbase/CObjectCollectionBase.h>
#include <imtbase/IOrderedObjectCollection.h>


namespace imtbase
{


/**
	Implementation of an ordered object collection facade.
	This class wraps an existing IObjectCollection and maintains a custom ordering for its items.
	\ingroup Collection
*/
class COrderedObjectCollectionProxy:
			public CObjectCollectionBase,
			virtual public IOrderedObjectCollection,
			public istd::TComposedFactory<istd::IChangeable>
{
public:
	typedef CObjectCollectionBase BaseClass;
	typedef istd::TComposedFactory<istd::IChangeable> BaseClass2;

	/**
		Constructor.
		\param parent		The parent collection to wrap
	*/
	COrderedObjectCollectionProxy(IObjectCollection& parent);

	// reimplemented (IOrderedObjectCollection)
	virtual bool SetItemOrder(const Id& itemId, int position) override;
	virtual int GetItemOrder(const Id& itemId) const override;
	virtual bool SetItemsOrder(const Ids& orderedIds) override;
	virtual Ids GetOrderedItemIds() const override;
	virtual bool ResetItemOrder() override;

	// reimplemented (imtbase::IObjectCollection)
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
	virtual bool GetObjectData(const Id& objectId, DataPtr& dataPtr, const iprm::IParamsSet* dataConfigurationPtr = nullptr) const override;
	virtual bool SetObjectData(
				const Id& objectId,
				const istd::IChangeable& object,
				CompatibilityMode mode = CM_WITHOUT_REFS,
				const IOperationContext* operationContextPtr = nullptr) override;
	virtual bool RemoveElements(const Ids& elementIds, const IOperationContext* operationContextPtr = nullptr) override;

	// reimplemented (ICollectionInfo)
	virtual Ids GetElementIds(
				int offset = 0,
				int count = -1,
				const iprm::IParamsSet* selectionParamsPtr = nullptr,
				ilog::IMessageConsumer* logPtr = nullptr) const override;

	// reimplemented (istd::IChangeable)
	virtual int GetSupportedOperations() const override;
	virtual istd::IChangeableUniquePtr CloneMe(CompatibilityMode mode = CM_WITHOUT_REFS) const override;

protected:
	// reimplemented (CObjectCollectionBase)
	virtual istd::IChangeableUniquePtr CreateObjectInstance(const QByteArray& typeId) const override;
	virtual IObjectCollection* CreateSubCollectionInstance() const override;
	virtual bool InsertObjectIntoCollection(ObjectInfo info) override;

private:
	/**
		Get the ordered list of IDs from the parent collection.
	*/
	Ids GetParentElementIds() const;

	/**
		Synchronize the custom order with the parent collection's current state.
		This adds any new items and removes deleted items from the custom order.
	*/
	void SynchronizeOrder();

	/**
		Apply the custom order to a list of IDs.
	*/
	Ids ApplyCustomOrder(const Ids& ids) const;

private:
	IObjectCollection& m_parent;
	QVector<QByteArray> m_customOrder;		// Stores the custom order of item IDs
	bool m_hasCustomOrder;					// Flag indicating whether custom order is active
};


} // namespace imtbase
