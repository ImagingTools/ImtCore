#include <imtbase/CMultiSelection.h>


// ACF includes
#include <istd/TDelPtr.h>
#include <istd/CChangeNotifier.h>
#include <iprm/IOptionsList.h>
#include <iser/IArchive.h>
#include <iser/CArchiveTag.h>


namespace imtbase
{


// public methods

CMultiSelection::CMultiSelection()
	:CMultiSelection(SelectionMode::SM_MULTI_SELECTION)
{
}


CMultiSelection::CMultiSelection(SelectionMode selectionMode)
	:m_selectionMode(selectionMode),
	m_constraintsObserver(*this)
{
}


void CMultiSelection::SetSelectionConstraints(ICollectionInfo* selectionConstraintsPtr)
{
	m_constraintsObserver.AttachOrSetObject(selectionConstraintsPtr);
}


// reimplemented (IMultiSelection)

const ICollectionInfo* CMultiSelection::GetSelectionConstraints() const
{
	return m_constraintsObserver.GetObjectPtr();
}


IMultiSelection::SelectionMode CMultiSelection::GetSelectionMode() const
{
	return m_selectionMode;
}


IMultiSelection::Ids CMultiSelection::GetSelectedIds() const
{
	return m_selectedIds;
}


bool CMultiSelection::SetSelectedIds(const Ids& selectedIds)
{
	if ((m_selectionMode == SelectionMode::SM_SINGLE_SELECTION) && (selectedIds.count() > 1)){
		return false;
	}

	Ids filteredIds = FilterIds(selectedIds, false);
	if (filteredIds.isEmpty()){
		ApplyNewIds(selectedIds);

		return true;
	}

	return false;
}


// reimplement (iser::ISerializable)

bool CMultiSelection::Serialize(iser::IArchive& archive)
{
	static iser::CArchiveTag selectedItemsTag("SelectedItems", "List of selected items");
	static iser::CArchiveTag selectedItemTag("Item", "Single selected item");

	int itemCount = m_selectedIds.count();

	bool retVal = true;

	retVal = retVal && archive.BeginMultiTag(selectedItemsTag, selectedItemTag, itemCount);

	Ids selectedIds = archive.IsStoring() ? m_selectedIds : Ids();

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

	if (retVal && !archive.IsStoring()){
		Ids filteredIds = FilterIds(selectedIds, true);
		if ((m_selectionMode == SelectionMode::SM_SINGLE_SELECTION) && filteredIds.count() > 1){
			filteredIds = {filteredIds[0]};
		}

		if (m_selectedIds != filteredIds){
			ApplyNewIds(filteredIds);
		}
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
	if (sourcePtr != nullptr){
		if (m_selectedIds != sourcePtr->m_selectedIds){
			Ids filteredIds = FilterIds(sourcePtr->m_selectedIds, true);
			if ((m_selectionMode == SelectionMode::SM_SINGLE_SELECTION) && filteredIds.count() > 1){
				filteredIds = {filteredIds[0]};
			}

			ApplyNewIds(filteredIds);
		}

		return true;
	}

	return false;
}


bool CMultiSelection::IsEqual(const IChangeable & object) const
{
	const CMultiSelection* sourcePtr = dynamic_cast<const CMultiSelection*>(&object);
	if (sourcePtr != nullptr){
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
	ApplyNewIds(Ids());

	return true;
}


// private methods

IMultiSelection::Ids CMultiSelection::FilterIds(Ids ids, bool containedInConstraints) const
{
	if (m_constraintsObserver.GetObservedObject() != nullptr){
		Ids filteredIds;

		for (Id id : ids){
			bool isContained = m_constraintsCache.contains(id);

			if (isContained && containedInConstraints){
				filteredIds.append(id);
			}

			if (!isContained && !containedInConstraints){
				filteredIds.append(id);
			}
		}

		return filteredIds;
	}

	return ids;
}


void CMultiSelection::ApplyNewIds(Ids selectedIds)
{
	if (m_selectedIds != selectedIds){
		istd::IChangeable::ChangeSet changeSet(CF_SELECTION_CHANGED);
		changeSet.SetChangeInfo(IMultiSelection::CN_SELECTION_CHANGED, QVariant());
		istd::CChangeNotifier notifier(this, &changeSet);

		m_selectedIds = selectedIds;
	}
}


// public methods of the embedded class ConstraintsObserver

CMultiSelection::ConstraintsObserver::ConstraintsObserver(CMultiSelection& parent)
	:m_parent(parent)
{
}


// protected methods of the embedded class ConstraintsObserver

void CMultiSelection::ConstraintsObserver::OnUpdate(const istd::IChangeable::ChangeSet& changeset)
{
	Ids constraints = GetObservedObject()->GetElementIds();
	m_parent.m_constraintsCache = QSet<Id>(constraints.constBegin(), constraints.constEnd());

	Ids filteredIds = m_parent.FilterIds(m_parent.m_selectedIds, true);
	m_parent.ApplyNewIds(filteredIds);
}


} // namespace imtbase


