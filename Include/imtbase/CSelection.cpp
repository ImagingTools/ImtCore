#include <imtbase/CSelection.h>


// Std includes
#include <algorithm>

// ACF includes
#include <istd/TDelPtr.h>
#include <istd/CChangeGroup.h>
#include <istd/CChangeNotifier.h>
#include <iser/CArchiveTag.h>
#include <iser/CPrimitiveTypesSerializer.h>
#include <iser/IArchive.h>


namespace imtbase
{


// public methods

CSelection::CSelection()
	:CSelection(SelectionMode::SM_SINGLE)
{
}


CSelection::CSelection(SelectionMode selectionMode)
	:m_selectionMode(selectionMode)
{
}


void CSelection::SetSelectionConstraints(ICollectionInfo* selectionConstraintsPtr)
{
	istd::IChangeable::ChangeSet changeSet(CF_CONSTRAINTS_CHANGED);
	changeSet.SetChangeInfo(ISelection::CN_CONSTRAINTS_CHANGED, QVariant());
	istd::TDelPtr<istd::CChangeNotifier> notifierPtr;

	if (GetModelPtr() == nullptr){
		notifierPtr.SetPtr(new istd::CChangeNotifier(this, &changeSet));
	}

	AttachOrSetObject(selectionConstraintsPtr);
}


// reimplemented (ISelection)

const ICollectionInfo* CSelection::GetSelectionConstraints() const
{
	return GetObjectPtr();
}


ISelection::SelectionMode CSelection::GetSelectionMode() const
{
	return m_selectionMode;
}


void CSelection::SetSelectionMode(SelectionMode mode)
{
	return ApplySelectionMode(mode);
}


ISelection::Ids CSelection::GetSelectedIds() const
{
	return m_selectedIds;
}


bool CSelection::SetSelectedIds(const Ids& selectedIds)
{
	return ApplySelection(selectedIds);
}


// reimplement (iser::ISerializable)

bool CSelection::Serialize(iser::IArchive& archive)
{
	bool retVal = true;

	SelectionMode selectionMode = m_selectionMode;
	QByteArrayList selectedIds = m_selectedIds.values();

	std::sort(selectedIds.begin(), selectedIds.end());

	static iser::CArchiveTag selectionModeTag("SelectionMode", "Selection mode", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(selectionModeTag);
	retVal = retVal && I_SERIALIZE_ENUM(SelectionMode, archive, selectionMode);
	retVal = retVal && archive.EndTag(selectionModeTag);

	retVal = retVal && iser::CPrimitiveTypesSerializer::SerializeContainer(archive, selectedIds, "SelectedItems", "Item");

	if (retVal && !archive.IsStoring()){
		istd::CChangeGroup group(this);

		ApplySelectionMode(selectionMode);
		retVal = ApplySelection(Ids(selectedIds.cbegin(), selectedIds.cend()));

		if (!retVal){
			ApplySelection(Ids());
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
		istd::CChangeGroup group(this);

		ApplySelectionMode(sourcePtr->m_selectionMode);
		ApplySelection(sourcePtr->m_selectedIds);

		return true;
	}

	return false;
}


bool CSelection::IsEqual(const IChangeable & object) const
{
	const CSelection* sourcePtr = dynamic_cast<const CSelection*>(&object);
	if (sourcePtr != nullptr){
		return (m_selectionMode == sourcePtr->m_selectionMode) && (m_selectedIds == sourcePtr->m_selectedIds);
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
	ApplySelection(Ids());

	return true;
}


// reimplemented (imod::CSingleModelObserverBase)

void CSelection::OnUpdate(const istd::IChangeable::ChangeSet& /*changeSet*/)
{
	istd::IChangeable::ChangeSet changeSet(CF_CONSTRAINTS_CHANGED);
	changeSet.SetChangeInfo(ISelection::CN_CONSTRAINTS_CHANGED, QVariant());
	istd::CChangeNotifier notifier(this, &changeSet);
}


// private methods

void CSelection::ApplySelectionMode(SelectionMode selectionMode)
{
	if (m_selectionMode != selectionMode){
		istd::CChangeGroup group(this);

		istd::IChangeable::ChangeSet changeSet(CF_SELECTION_MODE_CHANGED);
		changeSet.SetChangeInfo(ISelection::CN_SELECTION_MODE_CHANGED, QVariant());
		istd::CChangeNotifier notifier(this, &changeSet);

		m_selectionMode = selectionMode;

		if (m_selectionMode == SM_SINGLE && m_selectedIds.count() > 1){
			ApplySelection(Ids());
		}
	}
}


bool CSelection::ApplySelection(const Ids& selectionIds)
{
	for (const Id& id : selectionIds){
		if (id.isEmpty()){
			return false;
		}
	}

	if (m_selectionMode == SM_SINGLE && selectionIds.count() > 1){
		return false;
	}

	if (m_selectedIds != selectionIds){
		istd::IChangeable::ChangeSet changeSet(CF_SELECTION_CHANGED);
		changeSet.SetChangeInfo(ISelection::CN_SELECTION_CHANGED, QVariant());
		istd::CChangeNotifier notifier(this, &changeSet);

		m_selectedIds = selectionIds;
	}

	return true;
}


} // namespace imtbase


