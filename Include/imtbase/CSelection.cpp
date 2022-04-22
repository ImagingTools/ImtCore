#include <imtbase/CSelection.h>


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
	if ((m_selectionMode == SM_SINGLE) && (selectedIds.count() > 1)){
		return false;
	}

	for (const QByteArray& id : selectedIds){
		if (id.isEmpty()){
			return false;
		}
	}

	if (m_selectedIds != selectedIds){
		ApplySelection(selectedIds);
	}

	return true;
}


// reimplement (iser::ISerializable)

bool CSelection::Serialize(iser::IArchive& archive)
{
	bool retVal = true;

	SelectionMode selectionMode = m_selectionMode;

	static iser::CArchiveTag selectionModeTag("SelectionMode", "Selection mode", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(selectionModeTag);
	retVal = retVal && I_SERIALIZE_ENUM(SelectionMode, archive, selectionMode);
	retVal = retVal && archive.EndTag(selectionModeTag);

	static iser::CArchiveTag selectedItemsTag("SelectedItems", "List of selected items", iser::CArchiveTag::TT_MULTIPLE);
	static iser::CArchiveTag selectedItemTag("Item", "Single selected item", iser::CArchiveTag::TT_GROUP);

	Ids selectedIds;
	int itemCount = m_selectedIds.count();
	retVal = archive.BeginMultiTag(selectedItemsTag, selectedItemTag, itemCount);
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
		istd::CChangeGroup group(this);

		if (m_selectionMode != selectionMode){
			ApplySelectionMode(selectionMode);
		}

		if (m_selectedIds != selectedIds){
			ApplySelection(selectedIds);
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

		if (m_selectionMode != sourcePtr->m_selectionMode){
			ApplySelectionMode(sourcePtr->m_selectionMode);
		}

		if (m_selectedIds != sourcePtr->m_selectedIds){
			ApplySelection(sourcePtr->m_selectedIds);
		}

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


// private methods

void CSelection::ApplySelection(const Ids& selectionIds)
{
	istd::IChangeable::ChangeSet changeSet(CF_SELECTION_CHANGED);
	changeSet.SetChangeInfo(ISelection::CN_SELECTION_CHANGED, QVariant());
	istd::CChangeNotifier notifier(this, &changeSet);

	m_selectedIds = selectionIds;
}


void CSelection::ApplySelectionMode(SelectionMode selectionMode)
{
	istd::IChangeable::ChangeSet changeSet(CF_SELECTION_MODE_CHANGED);
	changeSet.SetChangeInfo(ISelection::CN_SELECTION_MODE_CHANGED, QVariant());
	istd::CChangeNotifier notifier(this, &changeSet);

	m_selectionMode = selectionMode;
}


// public methods of the embedded class ConstraintsObserver

CSelection::ConstraintsObserver::ConstraintsObserver(CSelection& parent)
	:m_parent(parent)
{
}


// protected methods of the embedded class ConstraintsObserver

// reimplemented (imod::CSingleModelObserverBase)

void CSelection::ConstraintsObserver::OnUpdate(const istd::IChangeable::ChangeSet& changeset)
{
	istd::IChangeable::ChangeSet changeSet(CF_CONSTRAINTS_CHANGED);
	changeSet.SetChangeInfo(ISelection::CN_CONSTRAINTS_CHANGED, QVariant());
	istd::CChangeNotifier notifier(&m_parent, &changeSet);
}

// reimplemented (imod::IObserver)

bool CSelection::ConstraintsObserver::OnModelDetached(imod::IModel* modelPtr)
{
	istd::IChangeable::ChangeSet changeSet(CF_CONSTRAINTS_CHANGED);
	changeSet.SetChangeInfo(ISelection::CN_CONSTRAINTS_CHANGED, QVariant());
	istd::CChangeNotifier notifier(&m_parent, &changeSet);

	return BaseClass::OnModelDetached(modelPtr);
}


} // namespace imtbase


