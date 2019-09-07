#include <imtbase/CMultiSelection.h>


// ACF includes
#include <istd/CChangeNotifier.h>
#include <iprm/IOptionsList.h>
#include <iser/IArchive.h>
#include <iser/CArchiveTag.h>


namespace imtbase
{


// reimplemented (IMultiSelection)

const iprm::IOptionsList* CMultiSelection::GetSelectionConstraints() const
{
	return NULL;
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


} // namespace imtbase


