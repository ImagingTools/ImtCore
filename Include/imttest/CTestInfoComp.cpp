#include <imttest/CTestInfoComp.h>


namespace imttest
{


// protected methods

// reimplemented (icomp::CComponentBase)

void CTestInfoComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	if (m_testCollectionCompPtr.IsValid()){
		m_testCollectionPtr = m_testCollectionCompPtr.GetPtr();
	}
}


} // namespace imtlic


