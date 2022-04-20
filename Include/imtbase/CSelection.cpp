#include <imtbase/CSelection.h>


// ACF includes
#include <istd/TDelPtr.h>
#include <istd/CChangeNotifier.h>
#include <iprm/IOptionsList.h>
#include <iser/IArchive.h>
#include <iser/CArchiveTag.h>


namespace imtbase
{


// public methods

CSelection::CSelection()
	:CSelection(SelectionMode::SM_SINGLE)
{
}


CSelection::CSelection(SelectionMode selectionMode)
	:m_selectionMode(selectionMode),
	m_constraintsObserver(*this)
{
}


void CSelection::SetSelectionConstraints(ICollectionInfo* selectionConstraintsPtr)
{
	m_constraintsObserver.AttachOrSetObject(selectionConstraintsPtr);
}


// reimplemented (ISelection)

const ICollectionInfo* CSelection::GetSelectionConstraints() const
{
	return m_constraintsObserver.GetObjectPtr();
}


ISelection::SelectionMode CSelection::GetSelectionMode() const
{
	return m_selectionMode;
}


ISelection::Ids CSelection::GetSelectedIds() const
{
	return m_selectedIds;
}


bool CSelection::SetSelectedIds(const Ids& selectedIds)
{
	if ((m_selectionMode == SelectionMode::SM_SINGLE) && (selectedIds.count() > 1)){
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

bool CSelection::Serialize(iser::IArchive& archive)
{
	static iser::CArchiveTag selectedItemsTag("SelectedItems", "List of selected items");
	static iser::CArchiveTag selectedItemTag("Item", "Single selected item");

	Ids selectedIds;
	int itemCount = m_selectedIds.count();
	bool retVal = archive.BeginMultiTag(selectedItemsTag, selectedItemTag, itemCount);
	for (int i = 0; i < itemCount; ++i){
		QByteArray id;
		if (retVal && archive.IsStoring()){
			id = m_selectedIds[i];
		}

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
		if ((m_selectionMode == SelectionMode::SM_SINGLE) && filteredIds.count() > 1){
			filteredIds = {filteredIds[0]};
		}

		if (m_selectedIds != filteredIds){
			ApplyNewIds(filteredIds);
		}
	}

	return retVal;
}


// reimplemented (istd::IChangeable)

int CSelection::GetSupportedOperations() const
{
	return SO_CLONE | SO_COMPARE | SO_COPY | SO_RESET;
}


bool CSelection::CopyFrom(const IChangeable & object, CompatibilityMode /*mode*/)
{
	const CSelection* sourcePtr = dynamic_cast<const CSelection*>(&object);
	if (sourcePtr != nullptr){
		if (m_selectedIds != sourcePtr->m_selectedIds){
			Ids filteredIds = FilterIds(sourcePtr->m_selectedIds, true);
			if ((m_selectionMode == SelectionMode::SM_SINGLE) && filteredIds.count() > 1){
				filteredIds = {filteredIds[0]};
			}

			ApplyNewIds(filteredIds);
		}

		return true;
	}

	return false;
}


bool CSelection::IsEqual(const IChangeable & object) const
{
	const CSelection* sourcePtr = dynamic_cast<const CSelection*>(&object);
	if (sourcePtr != nullptr){
		return (m_selectedIds == sourcePtr->m_selectedIds);
	}

	return false;
}


istd::IChangeable* CSelection::CloneMe(CompatibilityMode mode) const
{
	istd::TDelPtr<CSelection> clonePtr(new CSelection());

	if (clonePtr->CopyFrom(*this, mode)){
		return clonePtr.PopPtr();
	}

	return nullptr;
}


bool CSelection::ResetData(CompatibilityMode /*mode*/)
{
	ApplyNewIds(Ids());

	return true;
}


// private methods

ISelection::Ids CSelection::FilterIds(Ids ids, bool containedInConstraints) const
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


void CSelection::ApplyNewIds(Ids ids)
{
	if (m_selectedIds != ids){
		istd::IChangeable::ChangeSet changeSet(CF_SELECTION_CHANGED);
		changeSet.SetChangeInfo(ISelection::CN_SELECTION_CHANGED, QVariant());
		istd::CChangeNotifier notifier(this, &changeSet);

		m_selectedIds = ids;
	}
}


// public methods of the embedded class ConstraintsObserver

CSelection::ConstraintsObserver::ConstraintsObserver(CSelection& parent)
	:m_parent(parent)
{
}


// protected methods of the embedded class ConstraintsObserver

void CSelection::ConstraintsObserver::OnUpdate(const istd::IChangeable::ChangeSet& changeset)
{
	Ids constraints = GetObservedObject()->GetElementIds();
	m_parent.m_constraintsCache = QSet<Id>(constraints.constBegin(), constraints.constEnd());

	Ids filteredIds = m_parent.FilterIds(m_parent.m_selectedIds, true);
	m_parent.ApplyNewIds(filteredIds);
}


} // namespace imtbase


