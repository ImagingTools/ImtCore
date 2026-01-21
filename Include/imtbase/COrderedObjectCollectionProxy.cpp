#include <imtbase/COrderedObjectCollectionProxy.h>


// ACF includes
#include <istd/TDelPtr.h>
#include <istd/CChangeNotifier.h>

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
	for (const Id& id : orderedIds){
		if (!parentIds.contains(id)){
			return false;
		}
	}

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
		const_cast<COrderedObjectCollectionProxy*>(this)->SynchronizeOrder();
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


// reimplemented (istd::IChangeable)

int COrderedObjectCollectionProxy::GetSupportedOperations() const
{
	return OP_COPY | OP_COMPARE | OP_CLONE;
}


istd::IChangeableUniquePtr COrderedObjectCollectionProxy::CloneMe(CompatibilityMode mode) const
{
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
	// This method is called by base class methods
	// For the proxy, we delegate to parent
	return false;
}


// private methods

Ids COrderedObjectCollectionProxy::GetParentElementIds() const
{
	return m_parent.GetElementIds();
}


void COrderedObjectCollectionProxy::SynchronizeOrder()
{
	if (!m_hasCustomOrder){
		return;
	}

	// Get current IDs from parent
	const Ids parentIds = GetParentElementIds();

	// Remove items that no longer exist in parent
	QVector<QByteArray> newOrder;
	for (const QByteArray& id : m_customOrder){
		if (parentIds.contains(id)){
			newOrder.append(id);
		}
	}

	// Add any new items from parent that aren't in custom order
	for (const QByteArray& id : parentIds){
		if (!newOrder.contains(id)){
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

	// Add items in custom order
	for (const QByteArray& id : m_customOrder){
		if (ids.contains(id)){
			orderedIds.append(id);
		}
	}

	// Add any items not in custom order at the end
	for (const QByteArray& id : ids){
		if (!orderedIds.contains(id)){
			orderedIds.append(id);
		}
	}

	return orderedIds;
}


} // namespace imtbase
