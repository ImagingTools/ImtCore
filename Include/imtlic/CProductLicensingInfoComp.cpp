#include <imtlic/CProductLicensingInfoComp.h>


namespace imtlic
{


// protected methods

// reimplemented (icomp::CComponentBase)

void CProductLicensingInfoComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	if (m_featurePackagesCompPtr.IsValid()){
		m_featurePackagesPtr = m_featurePackagesCompPtr.GetPtr();
	}
}


} // namespace imtlic


