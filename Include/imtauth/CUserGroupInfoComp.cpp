#include <imtauth/CUserGroupInfoComp.h>


namespace imtauth
{


// protected methods

// reimplemented (icomp::CComponentBase)

void CUserGroupInfoComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	if (m_roleProviderCompPtr.IsValid()){
		m_roleProviderPtr = m_roleProviderCompPtr.GetPtr();
	}

	if (m_userGroupProviderCompPtr.IsValid()){
		m_userGroupInfoProviderPtr = m_userGroupProviderCompPtr.GetPtr();
	}

	if (m_userProviderCompPtr.IsValid()){
		m_userInfoProviderPtr = m_userProviderCompPtr.GetPtr();
	}
}


} // namespace imtauth
