#include <imtbase/COrderedObjectCollectionProxy.h>


// ACF includes
#include <istd/TDelPtr.h>
#include <istd/CChangeNotifier.h>
#include <iser/CArchiveTag.h>
#include <iser/CPrimitiveTypesSerializer.h>

// ImtCore includes
#include <imtbase/CObjectCollection.h>


namespace imtbase
{


// public methods

COrderedObjectCollectionProxy::COrderedObjectCollectionProxy(IObjectCollection& parent)
	:m_parent(parent),
	 m_hasCustomOrder(false)
{
}


// reimplemented (IOrderedObjectCollection)

bool COrderedObjectCollectionProxy::SetItemOrder(const Id& itemId, int position)
{
	if (itemId.isEmpty() || position < 0){
		return false;
	}

	// Initialize custom order if not yet done
	if (!m_hasCustomOrder){
		m_customOrder = GetParentElementIds();
		m_hasCustomOrder = true;
	}

	// Find the item in the current order
	int currentIndex = m_customOrder.indexOf(itemId);
	if (currentIndex < 0){
		// Item not found in parent collection
		return false;
	}

	// Notify change before modifying data
	istd::IChangeable::ChangeSet changeSet(CF_CHANGED);
	istd::CChangeNotifier changeNotifier(this, &changeSet);

	// Remove from current position
	m_customOrder.remove(currentIndex);

	// Insert at new position (clamp to valid range)
	int targetPosition = qMin(position, m_customOrder.size());
	m_customOrder.insert(targetPosition, itemId);

	return true;
}


int COrderedObjectCollectionProxy::GetItemOrder(const Id& itemId) const
{
	if (itemId.isEmpty()){
		return -1;
	}

	if (m_hasCustomOrder){
		return m_customOrder.indexOf(itemId);
	}

	// No custom order, get position from parent
	const Ids parentIds = GetParentElementIds();
	return parentIds.indexOf(itemId);
}


bool COrderedObjectCollectionProxy::SetItemsOrder(const Ids& orderedIds)
{
	if (orderedIds.isEmpty()){
		return false;
	}

	// Verify all IDs exist in parent collection
	const Ids parentIds = GetParentElementIds();
	
	// Check that all provided IDs are valid
	for (const Id& id : orderedIds){
		if (!parentIds.contains(id)){
			return false;
		}
	}
	
	// Note: This method allows setting a partial order (subset of items).
	// Any items not in orderedIds will be appended at the end when retrieved.

	// Notify change before modifying data
	istd::IChangeable::ChangeSet changeSet(CF_CHANGED);
	istd::CChangeNotifier changeNotifier(this, &changeSet);

	// Set the new order
	m_customOrder = orderedIds;
	m_hasCustomOrder = true;

	return true;
}


Ids COrderedObjectCollectionProxy::GetOrderedItemIds() const
{
	if (m_hasCustomOrder){
		// Synchronize to ensure we have current items
		SynchronizeOrder();
		return m_customOrder;
	}

	return GetParentElementIds();
}


bool COrderedObjectCollectionProxy::ResetItemOrder()
{
	if (!m_hasCustomOrder){
		return true;
	}

	// Notify change before modifying data
	istd::IChangeable::ChangeSet changeSet(CF_CHANGED);
	istd::CChangeNotifier changeNotifier(this, &changeSet);

	m_customOrder.clear();
	m_hasCustomOrder = false;

	return true;
}


// reimplemented (imtbase::IObjectCollection)

int COrderedObjectCollectionProxy::GetOperationFlags(const QByteArray& objectId) const
{
	return m_parent.GetOperationFlags(objectId);
}


ICollectionInfo::Id COrderedObjectCollectionProxy::InsertNewObject(
			const QByteArray& typeId,
			const QString& name,
			const QString& description,
			const istd::IChangeable* defaultValuePtr,
			const Id& proposedElementId,
			const idoc::IDocumentMetaInfo* dataMetaInfoPtr,
			const idoc::IDocumentMetaInfo* elementMetaInfoPtr,
			const IOperationContext* operationContextPtr)
{
	// Insert into parent collection
	const Id newId = m_parent.InsertNewObject(
				typeId,
				name,
				description,
				defaultValuePtr,
				proposedElementId,
				dataMetaInfoPtr,
				elementMetaInfoPtr,
				operationContextPtr);

	if (!newId.isEmpty() && m_hasCustomOrder){
		// Add new item to end of custom order
		m_customOrder.append(newId);
	}

	return newId;
}


bool COrderedObjectCollectionProxy::GetObjectData(const Id& objectId, DataPtr& dataPtr, const iprm::IParamsSet* dataConfigurationPtr) const
{
	return m_parent.GetObjectData(objectId, dataPtr, dataConfigurationPtr);
}


bool COrderedObjectCollectionProxy::SetObjectData(
			const Id& objectId,
			const istd::IChangeable& object,
			CompatibilityMode mode,
			const IOperationContext* operationContextPtr)
{
	return m_parent.SetObjectData(objectId, object, mode, operationContextPtr);
}


bool COrderedObjectCollectionProxy::RemoveElements(const Ids& elementIds, const IOperationContext* operationContextPtr)
{
	const bool result = m_parent.RemoveElements(elementIds, operationContextPtr);

	if (result && m_hasCustomOrder){
		// Remove deleted items from custom order
		for (const Id& id : elementIds){
			m_customOrder.removeAll(id);
		}
	}

	return result;
}


// reimplemented (ICollectionInfo)

Ids COrderedObjectCollectionProxy::GetElementIds(
			int offset,
			int count,
			const iprm::IParamsSet* selectionParamsPtr,
			ilog::IMessageConsumer* logPtr) const
{
	// Get IDs from parent
	Ids parentIds = m_parent.GetElementIds(0, -1, selectionParamsPtr, logPtr);

	// Apply custom ordering if active
	if (m_hasCustomOrder){
		parentIds = ApplyCustomOrder(parentIds);
	}

	// Apply offset and count
	if (offset > 0 || count >= 0){
		const int startIndex = qMax(0, offset);
		const int endIndex = (count >= 0) ? qMin(startIndex + count, parentIds.size()) : parentIds.size();
		
		if (startIndex < parentIds.size()){
			return parentIds.mid(startIndex, endIndex - startIndex);
		}
		
		return Ids();
	}

	return parentIds;
}


// reimplemented (iser::ISerializable)

bool COrderedObjectCollectionProxy::Serialize(iser::IArchive& archive)
{
	istd::CChangeNotifier changeNotifier(archive.IsStoring() ? nullptr : this);

	bool retVal = true;

	// Serialize custom order flag
	static iser::CArchiveTag hasCustomOrderTag("HasCustomOrder", "Flag indicating if custom order is active");
	retVal = retVal && archive.BeginTag(hasCustomOrderTag);
	retVal = retVal && iser::Serialize(archive, m_hasCustomOrder);
	retVal = retVal && archive.EndTag(hasCustomOrderTag);

	// Serialize custom order list if active
	if (m_hasCustomOrder){
		static iser::CArchiveTag customOrderTag("CustomOrder", "Custom order of item IDs", iser::CArchiveTag::TT_MULTIPLE);
		static iser::CArchiveTag itemIdTag("ItemId", "Item ID", iser::CArchiveTag::TT_LEAF, &customOrderTag);

		int orderCount = m_customOrder.size();
		retVal = retVal && archive.BeginMultiTag(customOrderTag, itemIdTag, orderCount);

		if (!archive.IsStoring()){
			m_customOrder.resize(orderCount);
		}

		for (int i = 0; i < orderCount; ++i){
			retVal = retVal && archive.BeginTag(itemIdTag);
			retVal = retVal && iser::Serialize(archive, m_customOrder[i]);
			retVal = retVal && archive.EndTag(itemIdTag);
		}

		retVal = retVal && archive.EndMultiTag(customOrderTag);
	}
	else if (!archive.IsStoring()){
		// Clear custom order when loading and flag is false
		m_customOrder.clear();
	}

	return retVal;
}


// reimplemented (istd::IChangeable)

int COrderedObjectCollectionProxy::GetSupportedOperations() const
{
	return OP_COPY | OP_COMPARE | OP_CLONE;
}


istd::IChangeableUniquePtr COrderedObjectCollectionProxy::CloneMe(CompatibilityMode mode) const
{
	// Note: The clone shares the same parent collection reference as the original.
	// This is intentional for the proxy pattern, where clones manage ordering independently
	// but delegate data operations to the same parent collection.
	COrderedObjectCollectionProxy* clonePtr = new COrderedObjectCollectionProxy(m_parent);
	clonePtr->m_customOrder = m_customOrder;
	clonePtr->m_hasCustomOrder = m_hasCustomOrder;
	return istd::IChangeableUniquePtr(clonePtr);
}


// reimplemented (CObjectCollectionBase)

istd::IChangeableUniquePtr COrderedObjectCollectionProxy::CreateObjectInstance(const QByteArray& typeId) const
{
	// Delegate to parent's factory
	const IObjectCollection::IDataFactory* factoryPtr = dynamic_cast<const IObjectCollection::IDataFactory*>(&m_parent);
	if (factoryPtr != nullptr){
		return factoryPtr->CreateInstance(typeId);
	}
	return istd::IChangeableUniquePtr();
}


IObjectCollection* COrderedObjectCollectionProxy::CreateSubCollectionInstance() const
{
	return new COrderedObjectCollectionProxy(m_parent);
}


bool COrderedObjectCollectionProxy::InsertObjectIntoCollection(ObjectInfo info)
{
	// This method is called by base class methods.
	// For the proxy, insertion is delegated to the parent through InsertNewObject.
	// This method should not be used directly for the proxy pattern.
	// Return false to indicate insertion should go through the overridden InsertNewObject.
	return false;
}


// private methods

Ids COrderedObjectCollectionProxy::GetParentElementIds() const
{
	return m_parent.GetElementIds();
}


void COrderedObjectCollectionProxy::SynchronizeOrder() const
{
	if (!m_hasCustomOrder){
		return;
	}

	// Get current IDs from parent
	const Ids parentIds = GetParentElementIds();
	
	// Use QSet for O(1) lookups instead of O(n) contains()
	const QSet<QByteArray> parentIdsSet(parentIds.begin(), parentIds.end());

	// Remove items that no longer exist in parent
	QVector<QByteArray> newOrder;
	newOrder.reserve(m_customOrder.size());
	for (const QByteArray& id : m_customOrder){
		if (parentIdsSet.contains(id)){
			newOrder.append(id);
		}
	}

	// Use QSet for O(1) lookups of items already in new order
	const QSet<QByteArray> newOrderSet(newOrder.begin(), newOrder.end());
	
	// Add any new items from parent that aren't in custom order
	for (const QByteArray& id : parentIds){
		if (!newOrderSet.contains(id)){
			newOrder.append(id);
		}
	}

	m_customOrder = newOrder;
}


Ids COrderedObjectCollectionProxy::ApplyCustomOrder(const Ids& ids) const
{
	if (!m_hasCustomOrder || m_customOrder.isEmpty()){
		return ids;
	}

	Ids orderedIds;
	orderedIds.reserve(ids.size());
	
	// Use QSet for O(1) lookups instead of O(n) contains()
	const QSet<QByteArray> idsSet(ids.begin(), ids.end());

	// Add items in custom order
	for (const QByteArray& id : m_customOrder){
		if (idsSet.contains(id)){
			orderedIds.append(id);
		}
	}

	// Use QSet for O(1) lookups of items already in ordered list
	const QSet<QByteArray> orderedIdsSet(orderedIds.begin(), orderedIds.end());
	
	// Add any items not in custom order at the end
	for (const QByteArray& id : ids){
		if (!orderedIdsSet.contains(id)){
			orderedIds.append(id);
		}
	}

	return orderedIds;
}


} // namespace imtbase
