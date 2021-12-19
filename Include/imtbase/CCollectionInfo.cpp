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

	istd::CChangeNotifier changeNotifier(this);

	Item newItem(id);
	newItem.description = description;
	newItem.name = name;

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
			istd::CChangeNotifier changeNotifier(this);
		
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
				istd::CChangeNotifier changeNotifier(this);

				item.name = name;
				item.description = description;
			}

			break;
		}
	}
}


// reimplemented (ICollectionInfo)

ICollectionInfo::Ids CCollectionInfo::GetElementIds() const
{
	Ids retVal;

	for (const Item& item : m_items){
		retVal.push_back(item.id);
	}

	return retVal;
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


// reimplemented (istd::IChangeable)

int CCollectionInfo::GetSupportedOperations() const
{
	return SO_CLONE | SO_COPY | SO_RESET | SO_COMPARE;
}


bool CCollectionInfo::CopyFrom(const IChangeable& object, CompatibilityMode /*mode*/)
{
	const CCollectionInfo* sourcePtr = dynamic_cast<const CCollectionInfo*>(&object);
	if (sourcePtr != nullptr){
		istd::CChangeNotifier changeNotifier(this);

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
	istd::CChangeNotifier changeNotifier(this);

	m_items.clear();

	return true;
}


// reimplemented (iser::ISerializable)

bool CCollectionInfo::Serialize(iser::IArchive& archive)
{
	istd::CChangeNotifier changeNotifier(archive.IsStoring() ? nullptr : this);

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


