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

	if (m_rolesCompPtr.IsValid()){
		m_roleProviderPtr = m_rolesCompPtr.GetPtr();
	}
}


} // namespace imtauth
