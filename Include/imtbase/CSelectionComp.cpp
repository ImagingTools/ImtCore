#include <imtbase/CSelectionComp.h>


namespace imtbase
{


// protected methods

// reimplemented (CSelection)

void CSelectionComp::OnConstraintsUpdated(const istd::IChangeable::ChangeSet& changeset, const ICollectionInfo* modelPtr)
{
	Ids tempSelection;
	Ids selection;
	ICollectionInfo::Ids constraints = modelPtr->GetElementIds();

	switch (*m_autoSelectioModeAttrPtr){
	case 0:
		selection = GetSelectedIds();
		break;
	case 1:
		for (Id id : constraints){
			selection += id;
		}
		break;
	case 2:
		break;
	case 3:
		for (int i = 0; i < m_autoSelectionIdsAttrPtr.GetCount(); i++){
			selection += m_autoSelectionIdsAttrPtr[i];
		}
		break;
	case 4:
		selection = GetSelectedIds();
		for (Id id : selection){
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
}


// reimplemented (icomp::CComponentBase)

void CSelectionComp::OnComponentCreated()
{
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
}


} // namespace imtbase


