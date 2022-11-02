#include <imtauth/CUserInfoComp.h>


namespace imtauth
{


// protected methods

// reimplemented (icomp::CComponentBase)

void CUserInfoComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	if (m_featuresCompPtr.IsValid()){
		m_permissionProviderPtr = m_featuresCompPtr.GetPtr();
	}

	if (m_roleProviderCompPtr.IsValid()){
		m_roleProviderPtr = m_roleProviderCompPtr.GetPtr();
	}
}


} // namespace imtauth
