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
    SetId(m_userIdAttrPtr->GetValue().toUtf8());
    SetUsername(m_usernameAttrPtr->GetValue());
    SetName(m_nameAttrPtr->GetValue());
    SetPasswordHash(m_passwordHashIdAttrPtr->GetValue().toUtf8());
    SetMail(m_mailAttrPtr->GetValue());
}


} // namespace imtauth
