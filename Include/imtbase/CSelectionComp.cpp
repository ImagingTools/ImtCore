#include <imtbase/CSelectionComp.h>


namespace imtbase
{


// protected methods

// reimplemented (icomp::CComponentBase)

void CSelectionComp::OnComponentCreated()
{
	if (m_constraintsCompPtr.IsValid()){
		SetSelectionConstraints(m_constraintsCompPtr.GetPtr());
	}
}


void CSelectionComp::OnComponentDestroyed()
{
	SetSelectionConstraints(nullptr);
}


} // namespace imtbase


