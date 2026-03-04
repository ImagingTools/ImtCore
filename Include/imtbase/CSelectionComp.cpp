// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtbase/CSelectionComp.h>


namespace imtbase
{


// protected methods

// reimplemented (CSelection)

void CSelectionComp::OnConstraintsUpdated(const istd::IChangeable::ChangeSet& /*changeset*/, const ICollectionInfo* modelPtr)
{
	if (m_lastConstraints != modelPtr->GetElementIds()){
		Ids tempSelection;
		Ids selection;
		ICollectionInfo::Ids constraints = modelPtr->GetElementIds();

		switch (*m_autoSelectioModeAttrPtr){
		case ASM_NO_ACTION:
			selection = GetSelectedIds();
			break;

		case ASM_SELECT_ALL:
			for (const Id& id : constraints){
				selection += id;
			}
			break;

		case ASM_DESELECT_ALL:
			break;

		case ASM_SET_PRESET:
			for (int i = 0; i < m_autoSelectionIdsAttrPtr.GetCount(); i++){
				selection += m_autoSelectionIdsAttrPtr[i];
			}
			break;

		case ASM_UNSELECT_MISSING:
			selection = GetSelectedIds();
			for (const Id& id : selection){
				if (constraints.contains(id)){
					tempSelection += id;
				}
			}
			selection = tempSelection;
			break;
		default:
			Q_ASSERT(false);
		}

		SetSelectedIds(selection);

		m_lastConstraints = modelPtr->GetElementIds();
	}
}


// reimplemented (icomp::CComponentBase)

void CSelectionComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	switch (*m_initialSelectionModeAttrPtr){
	case 0:
		SetSelectionMode(SM_SINGLE);
		break;
	case 1:
		SetSelectionMode(SM_MULTI);
		break;
	default:
		Q_ASSERT(false);
	}

	Ids selection;

	switch (GetSelectionMode()){
	case SM_SINGLE:
		for (int i = 0; i < m_initialSelectionIdsAttrPtr.GetCount(); i++){
			SetSelectedIds({ m_initialSelectionIdsAttrPtr[i] });
		}
		break;

	case SM_MULTI:
		for (int i = 0; i < m_initialSelectionIdsAttrPtr.GetCount(); i++){
			selection += m_initialSelectionIdsAttrPtr[i];
		}

		SetSelectedIds(selection);

		break;
	}

	if (m_constraintsCompPtr.IsValid()){
		SetSelectionConstraints(m_constraintsCompPtr.GetPtr());
	}
}


void CSelectionComp::OnComponentDestroyed()
{
	SetSelectionConstraints(nullptr);

	BaseClass::OnComponentDestroyed();
}


} // namespace imtbase


