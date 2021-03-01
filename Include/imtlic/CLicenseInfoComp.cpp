#include <imtlic/CLicenseInfoComp.h>


namespace imtlic
{


// protected methods

// reimplemented (icomp::CComponentBase)

void CLicenseInfoComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	if (m_featurePackageCollectionCompPtr.IsValid()){
		m_featurePackageCollectionPtr = m_featurePackageCollectionCompPtr.GetPtr();
	}
}


} // namespace imtlic


