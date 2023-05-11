#include <imtbase/CSelectionComp.h>


namespace imtbase
{


// protected methods

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

	if (m_constraintsCompPtr.IsValid()){
		SetSelectionConstraints(m_constraintsCompPtr.GetPtr());
	}
}


void CSelectionComp::OnComponentDestroyed()
{
	SetSelectionConstraints(nullptr);
}


} // namespace imtbase


