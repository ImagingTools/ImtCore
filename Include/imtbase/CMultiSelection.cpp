#include <imtbase/CMultiSelection.h>


// ACF includes
#include <istd/TDelPtr.h>
#include <istd/CChangeNotifier.h>
#include <iprm/IOptionsList.h>
#include <iser/IArchive.h>
#include <iser/CArchiveTag.h>


namespace imtbase
{


// reimplemented (IMultiSelection)

const iprm::IOptionsList* CMultiSelection::GetSelectionConstraints() const
{
	return nullptr;
}


IMultiSelection::Ids CMultiSelection::GetSelectedIds() const
{
	return m_selectedIds;
}


bool CMultiSelection::SetSelectedIds(const Ids& selectedIds)
{
	if (m_selectedIds != selectedIds){
		istd::CChangeNotifier notifier(this);

		m_selectedIds = selectedIds;
	}

	return true;
}


// reimplement (iser::ISerializable)

bool CMultiSelection::Serialize(iser::IArchive& archive)
{
	static iser::CArchiveTag selectedItemsTag("SelectedItems", "List of selected items");
	static iser::CArchiveTag selectedItemTag("Item", "Single selected item");

	int itemCount = m_selectedIds.count();

	bool retVal = true;

	retVal = retVal && archive.BeginMultiTag(selectedItemsTag, selectedItemTag, itemCount);

	Ids selectedIds = m_selectedIds;

	for (int i = 0; i < itemCount; ++i){
		QByteArray id;

		retVal = retVal && archive.BeginTag(selectedItemTag);
		retVal = retVal && archive.Process(id);
		retVal = retVal && archive.EndTag(selectedItemTag);

		if (retVal && !archive.IsStoring()){
			selectedIds.push_back(id);
		}
	}

	retVal = retVal && archive.EndTag(selectedItemsTag);

	if (retVal && !archive.IsStoring() && (m_selectedIds != selectedIds)){
		istd::CChangeNotifier changeNotifier(this);

		m_selectedIds = selectedIds;
	}

	return retVal;
}


// reimplemented (istd::IChangeable)

int CMultiSelection::GetSupportedOperations() const
{
	return SO_CLONE | SO_COMPARE | SO_COPY | SO_RESET;
}


bool CMultiSelection::CopyFrom(const IChangeable & object, CompatibilityMode /*mode*/)
{
	const CMultiSelection* sourcePtr = dynamic_cast<const CMultiSelection*>(&object);
	if (sourcePtr != NULL){
		if (m_selectedIds != sourcePtr->m_selectedIds){
			istd::CChangeNotifier changeNotifier(this);

			m_selectedIds = sourcePtr->m_selectedIds;
		}

		return true;
	}

	return false;
}


bool CMultiSelection::IsEqual(const IChangeable & object) const
{
	const CMultiSelection* sourcePtr = dynamic_cast<const CMultiSelection*>(&object);
	if (sourcePtr != NULL){
		return (m_selectedIds == sourcePtr->m_selectedIds);
	}

	return false;
}


istd::IChangeable* CMultiSelection::CloneMe(CompatibilityMode mode) const
{
	istd::TDelPtr<CMultiSelection> clonePtr(new CMultiSelection());

	if (clonePtr->CopyFrom(*this, mode)){
		return clonePtr.PopPtr();
	}

	return nullptr;
}


bool CMultiSelection::ResetData(CompatibilityMode /*mode*/)
{
	if (!m_selectedIds.isEmpty()){
		istd::CChangeNotifier changeNotifier(this);

		m_selectedIds.clear();
	}

	return true;
}


} // namespace imtbase


