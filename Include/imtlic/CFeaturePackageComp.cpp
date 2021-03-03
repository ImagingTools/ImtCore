#include <imtlic/CFeaturePackageComp.h>


namespace imtlic
{


// protected methods

// reimplemented (icomp::CComponentBase)

void CFeaturePackageComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	if (m_ownerCollectionCompPtr.IsValid()){
		m_featurePackageCollectionPtr = m_ownerCollectionCompPtr.GetPtr();
	}
}


} // namespace imtlic


