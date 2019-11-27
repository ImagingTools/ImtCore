#include <imtbase/CCollectionInfo.h>


// ACF includes
#include <istd/TDelPtr.h>
#include <istd/CChangeNotifier.h>


namespace imtbase
{


// public methods

QByteArray CCollectionInfo::InsertItem(const QByteArray& id, const QString & name, const QString & description)
{
	Ids existingIds = GetElementIds();
	if (existingIds.contains(id)){
		return QByteArray();
	}

	istd::CChangeNotifier changeNotifier(this);

	Item newItem(id);
	newItem.description = description;
	newItem.name = name;

	m_items.push_back(newItem);

	return newItem.id;
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

} // namespace imtbase


