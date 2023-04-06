#include <imtauth/CUserInfoComp.h>


namespace imtauth
{


// protected methods

// reimplemented (icomp::CComponentBase)

void CUserInfoComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	if (m_roleProviderCompPtr.IsValid()){
		m_roleProviderPtr = m_roleProviderCompPtr.GetPtr();
	}
}


bool CUserInfoComp::IsAdmin() const
{
	bool isAdmin = false;

	if (m_adminIdAttrPtr.IsValid()){
		isAdmin = *m_adminIdAttrPtr == GetId();
	}

	return isAdmin;
}


} // namespace imtauth
