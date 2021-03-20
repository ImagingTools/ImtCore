#include <imtauth/CAccountInfoComp.h>


namespace imtauth
{


// protected methods

// reimplemented (icomp::CComponentBase)

void CAccountInfoComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	if (m_accountOwnerCompPtr.IsValid()){
		m_accountOwnerPtr = m_accountOwnerCompPtr.GetPtr();
	}
}


} // namespace imtauth


