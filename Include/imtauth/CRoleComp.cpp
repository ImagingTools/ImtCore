#include <imtauth/CRoleComp.h>


namespace imtauth
{


// protected methods

// reimplemented (icomp::CComponentBase)

void CRoleComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	if (m_parentRoleProviderCompPtr.IsValid()){
		SetParentRoleProvider( m_parentRoleProviderCompPtr.GetPtr());
	}
}


void CRoleComp::OnComponentDestroyed()
{
	BaseClass::OnComponentDestroyed();
}


} // namespace imtauth


