#include <imtbase/CCollectionInfo.h>


// ACF includes
#include <iser/CArchiveTag.h>
#include <istd/TDelPtr.h>
#include <istd/CChangeNotifier.h>
#include <istd/CChangeGroup.h>


namespace imtbase
{


// public methods

int CCollectionInfo::GetItemIndex(const QByteArray& id) const
{
	for (int i = 0; i < m_items.count(); ++i){
		if (m_items[i].id == id){
			return i;
		}
	}

	return -1;
}


QByteArray CCollectionInfo::InsertItem(const QByteArray& id, const QString & name, const QString & description, int position)
{
	Ids existingIds = GetElementIds();
	if (existingIds.contains(id)){
		return QByteArray();
	}

	Item newItem(id);
	newItem.description = description;
	newItem.name = name;

	ElementInsertInfo info;
	info.elementId = newItem.id;
	istd::IChangeable::ChangeSet changeSet(CF_ADDED);
	changeSet.SetChangeInfo(CN_ELEMENT_INSERTED, QVariant::fromValue<ElementInsertInfo>(info));
	istd::CChangeNotifier changeNotifier(this, &changeSet);

	if (position < 0 || (position > m_items.count() - 1)){
		m_items.push_back(newItem);
	}
	else{
		m_items.insert(position, newItem);
	}

	return newItem.id;
}


void CCollectionInfo::RemoveItem(const QByteArray& id)
{
	for (const Item& item : m_items){
		if (item.id == id){
			ElementRemoveInfo info;
			info.elementId = id;
			istd::IChangeable::ChangeSet changeSet(CF_REMOVED);
			changeSet.SetChangeInfo(CN_ELEMENT_REMOVED, QVariant::fromValue<ElementRemoveInfo>(info));
			istd::CChangeNotifier changeNotifier(this, &changeSet);

			m_items.removeOne(item);

			break;
		}
	}
}


void CCollectionInfo::UpdateItem(const QByteArray& id, const QString& name, const QString& description)
{
	for (Item& item : m_items){
		if (item.id == id){
			if (item.name != name || item.description != description){
				ElementUpdateInfo info;
				info.elementId = id;
				istd::IChangeable::ChangeSet changeSet(CF_UPDATED);
				changeSet.SetChangeInfo(CN_ELEMENT_UPDATED, QVariant::fromValue<ElementUpdateInfo>(info));
				istd::CChangeNotifier changeNotifier(this, &changeSet);

				item.name = name;
				item.description = description;
			}

			break;
		}
	}
}


// reimplemented (ICollectionInfo)

int CCollectionInfo::GetElementsCount(
			const iprm::IParamsSet* /*selectionParamPtr*/,
			const Id& /*parentId*/,
			int /*iterationFlags*/) const
{
	return m_items.size();
}


ICollectionInfo::Ids CCollectionInfo::GetElementIds(
			int offset,
			int count,
			const iprm::IParamsSet* /*selectionParamsPtr*/,
			const Id& /*parentId*/,
			int /*iterationFlags*/) const
{
	Ids retVal;

	Q_ASSERT(offset >= 0);

	int elementsCount = count >= 0 ? qMin(count, m_items.count()) : m_items.count();

	for (int i = offset; i < elementsCount; ++i){
		retVal.push_back(m_items[i].id);
	}

	return retVal;
}


bool CCollectionInfo::GetSubsetInfo(
			imtbase::ICollectionInfo& subsetInfo,
			int offset,
			int count,
			const iprm::IParamsSet* selectionParamsPtr,
			const Id& parentId,
			int iterationFlags) const
{
	return false;
}


ICollectionInfo::Id CCollectionInfo::GetParentId(const Id& /*elementId*/) const
{
	return Id();
}


ICollectionInfo::Ids CCollectionInfo::GetElementPath(const Id& /*elementId*/) const
{
	return Ids();
}


bool CCollectionInfo::IsBranch(const Id& /*elementId*/) const
{
	return false;
}


QVariant CCollectionInfo::GetElementInfo(const QByteArray& elementId, int infoType) const
{
	int objectIndex = -1;
	for (int i = 0; i < m_items.count(); ++i){
		if (m_items[i].id == elementId){
			 objectIndex = i;
			 break;
		}
	}

	if (objectIndex >= 0){
		switch (infoType){
		case EIT_DESCRIPTION:
			return m_items[objectIndex].description;

		case EIT_NAME:
			return m_items[objectIndex].name;

		case EIT_ENABLED:
			return m_items[objectIndex].isEnabled;
		default:
			return QVariant();
		}
	}

	return QVariant();
}

idoc::MetaInfoPtr CCollectionInfo::GetElementMetaInfo(const Id& /*elementId*/) const
{
	return idoc::MetaInfoPtr();
}


bool CCollectionInfo::SetElementName(const Id& /*elementId*/, const QString& /*name*/)
{
	return false;
}


bool CCollectionInfo::SetElementDescription(const Id& /*elementId*/, const QString& /*description*/)
{
	return false;
}


bool CCollectionInfo::SetElementEnabled(const Id& /*elementId*/, bool /*isEnabled*/)
{
	return false;
}


// reimplemented (istd::IChangeable)

int CCollectionInfo::GetSupportedOperations() const
{
	return SO_CLONE | SO_COPY | SO_RESET | SO_COMPARE;
}


bool CCollectionInfo::CopyFrom(const IChangeable& object, CompatibilityMode /*mode*/)
{
	const CCollectionInfo* sourcePtr = dynamic_cast<const CCollectionInfo*>(&object);
	if (sourcePtr != nullptr){
		istd::IChangeable::ChangeSet changeSet(CF_ANY);
		changeSet.SetChangeInfo(CN_ALL_CHANGED, QVariant());
		istd::CChangeNotifier changeNotifier(this, &changeSet);

		m_items = sourcePtr->m_items;

		return true;
	}
	
	return false;
}


bool CCollectionInfo::IsEqual(const IChangeable & object) const
{
	const CCollectionInfo* sourcePtr = dynamic_cast<const CCollectionInfo*>(&object);
	if (sourcePtr != NULL){
		return (m_items == sourcePtr->m_items);
	}

	return false;
}

istd::IChangeable* CCollectionInfo::CloneMe(CompatibilityMode mode) const
{
	istd::TDelPtr<CCollectionInfo> clonePtr(new CCollectionInfo());
	if (clonePtr->CopyFrom(*this, mode)){
		return clonePtr.PopPtr();
	}

	return nullptr;
}


bool CCollectionInfo::ResetData(CompatibilityMode /*mode*/)
{
	istd::IChangeable::ChangeSet changeSet(CF_ANY);
	changeSet.SetChangeInfo(CN_ALL_CHANGED, QVariant());
	istd::CChangeNotifier changeNotifier(this, &changeSet);

	m_items.clear();

	return true;
}


// reimplemented (iser::ISerializable)

bool CCollectionInfo::Serialize(iser::IArchive& archive)
{
	istd::IChangeable::ChangeSet changeSet(CF_ANY);
	changeSet.SetChangeInfo(CN_ALL_CHANGED, QVariant());
	istd::CChangeNotifier changeNotifier(archive.IsStoring() ? nullptr : this, &changeSet);

	int itemCount = m_items.count();

	if (!archive.IsStoring()){
		m_items.clear();
		itemCount = 0;
	}

	bool retVal = true;

	static iser::CArchiveTag itemsTag("Items", "List of collection items", iser::CArchiveTag::TT_MULTIPLE);
	static iser::CArchiveTag itemTag("Item", "Item", iser::CArchiveTag::TT_GROUP, &itemsTag);
	retVal = retVal && archive.BeginMultiTag(itemsTag, itemTag, itemCount);

	for (int i = 0; i < itemCount; i++){
		retVal = retVal && archive.BeginTag(itemTag);

		Item item;
		if (archive.IsStoring()){
			item = m_items[i];
		}

		static iser::CArchiveTag idTag("Id", "Id", iser::CArchiveTag::TT_LEAF, &itemTag);
		retVal = retVal && archive.BeginTag(idTag);
		retVal = retVal && archive.Process(item.id);
		retVal = retVal && archive.EndTag(idTag);

		static iser::CArchiveTag nameTag("Name", "Name", iser::CArchiveTag::TT_LEAF, &itemTag);
		retVal = retVal && archive.BeginTag(nameTag);
		retVal = retVal && archive.Process(item.name);
		retVal = retVal && archive.EndTag(nameTag);

		static iser::CArchiveTag descriptionTag("Description", "Description", iser::CArchiveTag::TT_LEAF, &itemTag);
		retVal = retVal && archive.BeginTag(descriptionTag);
		retVal = retVal && archive.Process(item.description);
		retVal = retVal && archive.EndTag(descriptionTag);

		static iser::CArchiveTag isEnabledTag("IsEnabled", "IsEnabled", iser::CArchiveTag::TT_LEAF, &itemTag);
		retVal = retVal && archive.BeginTag(isEnabledTag);
		retVal = retVal && archive.Process(item.isEnabled);
		retVal = retVal && archive.EndTag(isEnabledTag);

		if (!archive.IsStoring()){
			InsertItem(item.id, item.name, item.description);
			m_items.back().isEnabled = item.isEnabled;
		}

		retVal = retVal && archive.EndTag(itemTag);
	}

	retVal = retVal && archive.EndTag(itemsTag);

	return retVal;
}


} // namespace imtbase


