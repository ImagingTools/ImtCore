#include <imtlic/CFeatureInfoComp.h>


namespace imtlic
{


// protected methods

// reimplemented (icomp::CComponentBase)

void CFeatureInfoComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	if (m_featurePackageCompPtr.IsValid()){
		m_packagePtr = m_featurePackageCompPtr.GetPtr();
	}
}


} // namespace imtlic


