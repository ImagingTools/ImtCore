#include <imtlic/CProductInstanceInfoComp.h>


namespace imtlic
{


// protected methods

// reimplemented (icomp::CComponentBase)

void CProductInstanceInfoComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	if (m_productCollectionCompPtr.IsValid()){
		m_productCollectionPtr = m_productCollectionCompPtr.GetPtr();
	}

	if (m_productCollectionCompPtr.IsValid()){
		m_customerCollectionPtr = m_customerCollectionCompPtr.GetPtr();
	}
}


} // namespace imtlic


