#include <imtauth/CRoleComp.h>


namespace imtauth
{


// protected methods

// reimplemented (icomp::CComponentBase)

void CRoleComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	if (m_featuresCompPtr.IsValid()){
		m_permissionProviderPtr = m_featuresCompPtr.GetPtr();
	}
    SetRoleId(m_roleIdAttrPtr->GetValue().toUtf8());
    SetRoleName(m_roleNameAttrPtr->GetValue());
}


} // namespace imtauth


