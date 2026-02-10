// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtbase/COrderedObjectCollectionProxy.h>


// C++ includes
#include <utility>

// ACF includes
#include <istd/TDelPtr.h>
#include <istd/CChangeNotifier.h>
#include <iser/CArchiveTag.h>
#include <iser/IArchive.h>

// ImtCore includes
#include <imtbase/CObjectCollection.h>


namespace imtbase
{


// public methods

COrderedObjectCollectionProxy::COrderedObjectCollectionProxy(IObjectCollection* collectionPtr, bool takeOwnership)
	:m_updateBridge(this, imod::CModelUpdateBridge::UF_SOURCE),
	 m_hasCustomOrder(false)
{
	Q_ASSERT(collectionPtr != nullptr);
	
	if (takeOwnership) {
		m_collectionPtr.AdoptRawPtr(collectionPtr);
	}
	else {
		m_collectionPtr.SetUnmanagedPtr(collectionPtr);
	}
	
	AttachCollectionObserver();
}


COrderedObjectCollectionProxy::COrderedObjectCollectionProxy(IObjectCollectionUniquePtr&& collectionPtr)
	:m_updateBridge(this, imod::CModelUpdateBridge::UF_SOURCE),
	m_hasCustomOrder(false)
{
	m_collectionPtr.TakeOver(collectionPtr);
}


COrderedObjectCollectionProxy::COrderedObjectCollectionProxy(const istd::TOptInterfacePtr<IObjectCollection>& collectionPtr)
	:m_updateBridge(this, imod::CModelUpdateBridge::UF_SOURCE),
	m_hasCustomOrder(false)
{
	m_collectionPtr = collectionPtr;
}


COrderedObjectCollectionProxy::~COrderedObjectCollectionProxy()
{
	// Ensure all models are properly detached before destruction
	m_updateBridge.EnsureModelsDetached();
}


// reimplemented (IOrderedObjectCollection)

bool COrderedObjectCollectionProxy::SetItemOrder(const Id& itemId, int position)
{
	if (itemId.isEmpty() || position < 0){
		return false;
	}

	// Initialize custom order if not yet done
	if (!m_hasCustomOrder){
		m_customOrder = GetCollectionElementIds();
		m_hasCustomOrder = true;
	}

	// Find the item in the current order
	int currentIndex = m_customOrder.indexOf(itemId);
	if (currentIndex < 0){
		// Item not found in collection
		return false;
	}

	// Notify change before modifying data
	istd::CChangeNotifier changeNotifier(this);

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

	// No custom order, get position from collection
	const Ids collectionIds = GetCollectionElementIds();
	return collectionIds.indexOf(itemId);
}


bool COrderedObjectCollectionProxy::SetItemsOrder(const Ids& orderedIds)
{
	if (orderedIds.isEmpty()){
		return false;
	}

	// Verify all IDs exist in collection
	const Ids collectionIds = GetCollectionElementIds();
	
	// Check that all provided IDs are valid
	for (const Id& id : orderedIds){
		if (!collectionIds.contains(id)){
			return false;
		}
	}
	
	// Note: This method allows setting a partial order (subset of items).
	// Any items not in orderedIds will be appended at the end when retrieved.
	istd::CChangeNotifier changeNotifier(this);

	// Set the new order
	m_customOrder = orderedIds;
	m_hasCustomOrder = true;

	return true;
}


imtbase::ICollectionInfo::Ids COrderedObjectCollectionProxy::GetOrderedItemIds() const
{
	if (m_hasCustomOrder){
		// Synchronize to ensure we have current items
		SynchronizeOrder();
		return m_customOrder;
	}

	return GetCollectionElementIds();
}


bool COrderedObjectCollectionProxy::ResetItemOrder()
{
	if (!m_hasCustomOrder){
		return true;
	}

	istd::CChangeNotifier changeNotifier(this);

	m_customOrder.clear();
	m_hasCustomOrder = false;

	return true;
}


// reimplemented (imtbase::IObjectCollection)

IHierarchicalStructure* COrderedObjectCollectionProxy::GetCollectionStructure() const
{
	if (!m_collectionPtr.IsValid()){
		return nullptr;
	}
	return m_collectionPtr->GetCollectionStructure();
}


const IRevisionController* COrderedObjectCollectionProxy::GetRevisionController() const
{
	if (!m_collectionPtr.IsValid()){
		return nullptr;
	}
	return m_collectionPtr->GetRevisionController();
}


const ICollectionDataController* COrderedObjectCollectionProxy::GetDataController() const
{
	if (!m_collectionPtr.IsValid()){
		return nullptr;
	}
	return m_collectionPtr->GetDataController();
}


int COrderedObjectCollectionProxy::GetOperationFlags(const QByteArray& objectId) const
{
	if (!m_collectionPtr.IsValid()){
		return 0;
	}
	return m_collectionPtr->GetOperationFlags(objectId);
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
	if (!m_collectionPtr.IsValid()){
		return Id();
	}
	// Insert into collection
	const Id newId = m_collectionPtr->InsertNewObject(
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


bool COrderedObjectCollectionProxy::RemoveElements(const Ids& elementIds, const IOperationContext* operationContextPtr)
{
	if (!m_collectionPtr.IsValid()){
		return false;
	}
	const bool result = m_collectionPtr->RemoveElements(elementIds, operationContextPtr);

	if (result && m_hasCustomOrder){
		// Remove deleted items from custom order
		for (const Id& id : elementIds){
			m_customOrder.removeAll(id);
		}
	}

	return result;
}


bool COrderedObjectCollectionProxy::RemoveElementSet(
			const iprm::IParamsSet* selectionParamsPtr,
			const IOperationContext* operationContextPtr)
{
	if (!m_collectionPtr.IsValid()){
		return false;
	}
	return m_collectionPtr->RemoveElementSet(selectionParamsPtr, operationContextPtr);
}


bool COrderedObjectCollectionProxy::RestoreObjects(
			const Ids& objectIds,
			const IOperationContext* operationContextPtr)
{
	if (!m_collectionPtr.IsValid()){
		return false;
	}
	return m_collectionPtr->RestoreObjects(objectIds, operationContextPtr);
}


bool COrderedObjectCollectionProxy::RestoreObjectSet(
			const iprm::IParamsSet* selectionParamsPtr,
			const IOperationContext* operationContextPtr)
{
	if (!m_collectionPtr.IsValid()){
		return false;
	}
	return m_collectionPtr->RestoreObjectSet(selectionParamsPtr, operationContextPtr);
}


const istd::IChangeable* COrderedObjectCollectionProxy::GetObjectPtr(const Id& objectId) const
{
	if (!m_collectionPtr.IsValid()){
		return nullptr;
	}
	return m_collectionPtr->GetObjectPtr(objectId);
}


bool COrderedObjectCollectionProxy::GetObjectData(const Id& objectId, DataPtr& dataPtr, const iprm::IParamsSet* dataConfigurationPtr) const
{
	if (!m_collectionPtr.IsValid()){
		return false;
	}
	return m_collectionPtr->GetObjectData(objectId, dataPtr, dataConfigurationPtr);
}


bool COrderedObjectCollectionProxy::SetObjectData(
			const Id& objectId,
			const istd::IChangeable& object,
			CompatibilityMode mode,
			const IOperationContext* operationContextPtr)
{
	if (!m_collectionPtr.IsValid()){
		return false;
	}
	return m_collectionPtr->SetObjectData(objectId, object, mode, operationContextPtr);
}


IObjectCollectionUniquePtr COrderedObjectCollectionProxy::CreateSubCollection(
			int offset,
			int count,
			const iprm::IParamsSet* selectionParamsPtr) const
{
	if (!m_collectionPtr.IsValid()){
		return IObjectCollectionUniquePtr();
	}
	// Create a proxy around a subcollection
	IObjectCollectionUniquePtr subCollection = m_collectionPtr->CreateSubCollection(offset, count, selectionParamsPtr);
	if (subCollection.IsValid()){
		// Release ownership from the smart pointer and pass to the proxy's owning constructor
		COrderedObjectCollectionProxy* proxyPtr = new COrderedObjectCollectionProxy(std::move(subCollection));

		// Copy the ordering state using the dedicated method
		proxyPtr->CopyOrderingState(*this);

		IObjectCollectionUniquePtr retVal;
		retVal.SetPtr(proxyPtr);
		return proxyPtr;
	}
	return IObjectCollectionUniquePtr();
}


IObjectCollectionIterator* COrderedObjectCollectionProxy::CreateObjectCollectionIterator(
			const QByteArray& objectId,
			int offset,
			int count,
			const iprm::IParamsSet* selectionParamsPtr) const
{
	if (!m_collectionPtr.IsValid()){
		return nullptr;
	}
	return m_collectionPtr->CreateObjectCollectionIterator(objectId, offset, count, selectionParamsPtr);
}


// reimplemented (IObjectCollectionInfo)

const iprm::IOptionsList* COrderedObjectCollectionProxy::GetObjectTypesInfo() const
{
	if (!m_collectionPtr.IsValid()){
		return nullptr;
	}
	return m_collectionPtr->GetObjectTypesInfo();
}


ICollectionInfo::Id COrderedObjectCollectionProxy::GetObjectTypeId(const Id& objectId) const
{
	if (!m_collectionPtr.IsValid()){
		return Id();
	}
	return m_collectionPtr->GetObjectTypeId(objectId);
}


idoc::MetaInfoPtr COrderedObjectCollectionProxy::GetDataMetaInfo(const Id& objectId) const
{
	if (!m_collectionPtr.IsValid()){
		return idoc::MetaInfoPtr();
	}
	return m_collectionPtr->GetDataMetaInfo(objectId);
}


// reimplemented (ICollectionInfo)

int COrderedObjectCollectionProxy::GetElementsCount(
			const iprm::IParamsSet* selectionParamPtr,
			ilog::IMessageConsumer* logPtr) const
{
	if (!m_collectionPtr.IsValid()){
		return 0;
	}
	return m_collectionPtr->GetElementsCount(selectionParamPtr, logPtr);
}


imtbase::ICollectionInfo::Ids COrderedObjectCollectionProxy::GetElementIds(
			int offset,
			int count,
			const iprm::IParamsSet* selectionParamsPtr,
			ilog::IMessageConsumer* logPtr) const
{
	if (!m_collectionPtr.IsValid()){
		return Ids();
	}
	// Get IDs from collection
	Ids collectionIds = m_collectionPtr->GetElementIds(0, -1, selectionParamsPtr, logPtr);

	// Apply custom ordering if active
	if (m_hasCustomOrder){
		collectionIds = ApplyCustomOrder(collectionIds);
	}

	// Apply offset and count
	if (offset > 0 || count >= 0){
		const int startIndex = qMax(0, offset);
		const int endIndex = (count >= 0) ? qMin(startIndex + count, collectionIds.size()) : collectionIds.size();
		
		if (startIndex < collectionIds.size()){
			return collectionIds.mid(startIndex, endIndex - startIndex);
		}
		
		return Ids();
	}

	return collectionIds;
}


bool COrderedObjectCollectionProxy::GetSubsetInfo(
			ICollectionInfo& subsetInfo,
			int offset,
			int count,
			const iprm::IParamsSet* selectionParamsPtr,
			ilog::IMessageConsumer* logPtr) const
{
	if (!m_collectionPtr.IsValid()){
		return false;
	}
	return m_collectionPtr->GetSubsetInfo(subsetInfo, offset, count, selectionParamsPtr, logPtr);
}


QVariant COrderedObjectCollectionProxy::GetElementInfo(const Id& elementId, int infoType, ilog::IMessageConsumer* logPtr) const
{
	if (!m_collectionPtr.IsValid()){
		return QVariant();
	}
	return m_collectionPtr->GetElementInfo(elementId, infoType, logPtr);
}


idoc::MetaInfoPtr COrderedObjectCollectionProxy::GetElementMetaInfo(const Id& elementId, ilog::IMessageConsumer* logPtr) const
{
	if (!m_collectionPtr.IsValid()){
		return idoc::MetaInfoPtr();
	}
	return m_collectionPtr->GetElementMetaInfo(elementId, logPtr);
}


bool COrderedObjectCollectionProxy::SetElementName(const Id& elementId, const QString& name, ilog::IMessageConsumer* logPtr)
{
	if (!m_collectionPtr.IsValid()){
		return false;
	}
	return m_collectionPtr->SetElementName(elementId, name, logPtr);
}


bool COrderedObjectCollectionProxy::SetElementDescription(const Id& elementId, const QString& description, ilog::IMessageConsumer* logPtr)
{
	if (!m_collectionPtr.IsValid()){
		return false;
	}
	return m_collectionPtr->SetElementDescription(elementId, description, logPtr);
}


bool COrderedObjectCollectionProxy::SetElementEnabled(const Id& elementId, bool isEnabled, ilog::IMessageConsumer* logPtr)
{
	if (!m_collectionPtr.IsValid()){
		return false;
	}
	return m_collectionPtr->SetElementEnabled(elementId, isEnabled, logPtr);
}


// reimplemented (iser::ISerializable)

bool COrderedObjectCollectionProxy::Serialize(iser::IArchive& archive)
{
	istd::CChangeNotifier changeNotifier(archive.IsStoring() ? nullptr : this);

	bool retVal = true;

	// Serialize custom order flag
	static iser::CArchiveTag hasCustomOrderTag("HasCustomOrder", "Flag indicating if custom order is active");
	retVal = retVal && archive.BeginTag(hasCustomOrderTag);
	retVal = retVal && archive.Process(m_hasCustomOrder);
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
			retVal = retVal && archive.Process(m_customOrder[i]);
			retVal = retVal && archive.EndTag(itemIdTag);
		}

		retVal = retVal && archive.EndTag(customOrderTag);
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
	return SO_COPY | SO_COMPARE | SO_CLONE;
}


bool COrderedObjectCollectionProxy::CopyFrom(const IChangeable& object, CompatibilityMode /*mode*/)
{
	const COrderedObjectCollectionProxy* otherPtr = dynamic_cast<const COrderedObjectCollectionProxy*>(&object);
	if (otherPtr != nullptr){
		m_customOrder = otherPtr->m_customOrder;
		m_hasCustomOrder = otherPtr->m_hasCustomOrder;
		return true;
	}
	return false;
}


bool COrderedObjectCollectionProxy::IsEqual(const IChangeable& object) const
{
	const COrderedObjectCollectionProxy* otherPtr = dynamic_cast<const COrderedObjectCollectionProxy*>(&object);
	if (otherPtr != nullptr){
		return (m_hasCustomOrder == otherPtr->m_hasCustomOrder) && (m_customOrder == otherPtr->m_customOrder);
	}
	return false;
}


istd::IChangeableUniquePtr COrderedObjectCollectionProxy::CloneMe(CompatibilityMode /*mode*/) const
{
	if (!m_collectionPtr.IsValid()){
		return istd::IChangeableUniquePtr();
	}
	
	// Note: The clone always creates a non-owning proxy that shares the same collection pointer.
	// This is intentional for the proxy pattern, where clones manage ordering independently
	// but delegate data operations to the same collection.
	COrderedObjectCollectionProxy* clonePtr = new COrderedObjectCollectionProxy(m_collectionPtr);
	clonePtr->m_customOrder = m_customOrder;
	clonePtr->m_hasCustomOrder = m_hasCustomOrder;

	return istd::IChangeableUniquePtr(clonePtr);
}


bool COrderedObjectCollectionProxy::ResetData(CompatibilityMode /*mode*/)
{
	istd::CChangeNotifier changeNotifier(this);

	m_customOrder.clear();
	m_hasCustomOrder = false;

	return true;
}


// private methods

imtbase::ICollectionInfo::Ids COrderedObjectCollectionProxy::GetCollectionElementIds() const
{
	if (!m_collectionPtr.IsValid()){
		return Ids();
	}
	return m_collectionPtr->GetElementIds();
}


void COrderedObjectCollectionProxy::SynchronizeOrder() const
{
	if (!m_hasCustomOrder){
		return;
	}

	// Get current IDs from collection
	const Ids collectionIds = GetCollectionElementIds();
	
	// Use QSet for O(1) lookups instead of O(n) contains()
	const QSet<QByteArray> collectionIdsSet(collectionIds.begin(), collectionIds.end());

	// Remove items that no longer exist in collection
	QVector<QByteArray> newOrder;
	newOrder.reserve(m_customOrder.size());
	for (const QByteArray& id : m_customOrder){
		if (collectionIdsSet.contains(id)){
			newOrder.append(id);
		}
	}

	// Use QSet for O(1) lookups of items already in new order
	const QSet<QByteArray> newOrderSet(newOrder.begin(), newOrder.end());
	
	// Add any new items from collection that aren't in custom order
	for (const QByteArray& id : collectionIds){
		if (!newOrderSet.contains(id)){
			newOrder.append(id);
		}
	}

	m_customOrder = newOrder;
}


imtbase::ICollectionInfo::Ids COrderedObjectCollectionProxy::ApplyCustomOrder(const Ids& ids) const
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


void COrderedObjectCollectionProxy::CopyOrderingState(const COrderedObjectCollectionProxy& other)
{
	m_customOrder = other.m_customOrder;
	m_hasCustomOrder = other.m_hasCustomOrder;
}


void COrderedObjectCollectionProxy::AttachCollectionObserver()
{
	// Attach the aggregated collection to the update bridge so changes are forwarded to proxy observers
	// Note: Q_ASSERT in constructor ensures m_collectionPtr is valid
	imod::IModel* modelPtr = dynamic_cast<imod::IModel*>(m_collectionPtr.GetPtr());
	if (modelPtr != nullptr) {
		modelPtr->AttachObserver(&m_updateBridge);
	}
}


} // namespace imtbase
