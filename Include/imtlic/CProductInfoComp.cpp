#include <imtlic/CProductInfoComp.h>



namespace imtlic
{


// protected methods

// reimplemented (icomp::CComponentBase)

void CProductInfoComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	if (m_featureInfoProviderCompPtr.IsValid()){
		m_featureInfoProviderPtr = m_featureInfoProviderCompPtr.GetPtr();
	}
}


} // namespace imtlic


