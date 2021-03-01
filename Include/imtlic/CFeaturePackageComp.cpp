#include <imtlic/CFeaturePackageComp.h>


namespace imtlic
{


// protected methods

// reimplemented (icomp::CComponentBase)

void CFeaturePackageComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	if (m_ownerCollectionCompPtr.IsValid()){
		m_ownerPtr = m_ownerCollectionCompPtr.GetPtr();
	}
}


} // namespace imtlic


