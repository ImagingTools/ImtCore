// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtbase/CSelection.h>


// STL includes
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
	:m_selectionMode(SelectionMode::SM_SINGLE),
	m_constraintsObserver(*this)
{
}


CSelection::CSelection(SelectionMode selectionMode)
	:m_selectionMode(selectionMode),
	m_constraintsObserver(*this)
{
}


void CSelection::SetSelectionConstraints(ICollectionInfo* selectionConstraintsPtr)
{
	m_constraintsObserver.UnregisterAllObjects();

	if (selectionConstraintsPtr != nullptr){
		m_constraintsObserver.RegisterObject(selectionConstraintsPtr, &CSelection::OnConstraintsUpdated);
	}
}


// reimplemented (ISelection)

const ICollectionInfo* CSelection::GetSelectionConstraints() const
{
	return m_constraintsObserver.GetObjectAt(0);
}


ISelection::SelectionMode CSelection::GetSelectionMode() const
{
	return m_selectionMode;
}


bool CSelection::SetSelectionMode(SelectionMode mode)
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

	istd::CChangeNotifier changeNotifier(archive.IsStoring() ? nullptr : this);

	SelectionMode selectionMode = m_selectionMode;
	QByteArrayList selectedIds = m_selectedIds.values();

	std::sort(selectedIds.begin(), selectedIds.end());

	static iser::CArchiveTag selectionModeTag("SelectionMode", "Selection mode", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(selectionModeTag);
	retVal = retVal && I_SERIALIZE_ENUM(SelectionMode, archive, selectionMode);
	retVal = retVal && archive.EndTag(selectionModeTag);

	retVal = retVal && iser::CPrimitiveTypesSerializer::SerializeContainer(archive, selectedIds, "SelectedItems", "Item");

	if (retVal && !archive.IsStoring()){
		ApplySelectionMode(selectionMode);

#if QT_VERSION < 0x060000
		retVal = ApplySelection(selectedIds.toSet());
#else
		retVal = ApplySelection(Ids(selectedIds.cbegin(), selectedIds.cend()));
#endif
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


istd::IChangeableUniquePtr CSelection::CloneMe(CompatibilityMode mode) const
{
	istd::IChangeableUniquePtr clonePtr(new CSelection());

	if (clonePtr->CopyFrom(*this, mode)){
		return clonePtr;
	}

	return nullptr;
}


bool CSelection::ResetData(CompatibilityMode /*mode*/)
{
	istd::CChangeGroup group(this);

	ApplySelectionMode(SM_SINGLE);
	ApplySelection(Ids());

	return true;
}


// private methods

void CSelection::OnConstraintsUpdated(const istd::IChangeable::ChangeSet& /*changeset*/, const ICollectionInfo* /*modelPtr*/)
{
}


bool CSelection::ApplySelectionMode(SelectionMode selectionMode)
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

	return true;
}


bool CSelection::ApplySelection(const Ids& selectionIds)
{
	if (m_selectionMode == SM_SINGLE && selectionIds.count() > 1){
		return false;
	}

	if (selectionIds.contains(QByteArray())){
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


