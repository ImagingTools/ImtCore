#pragma once


// ACF includes
#include <icomp/CComponentBase.h>

// ImtCore includes
#include <imtauth/CUserInfo.h>
#include <imtlic/IFeatureInfoProvider.h>
#include <imtauth/IRoleInfoProvider.h>
#include <imtbase/IObjectCollection.h>

namespace imtauth
{


class CUserInfoComp:
            public icomp::CComponentBase,
            public CUserInfo
{
public:
    typedef icomp::CComponentBase BaseClass;

    I_BEGIN_COMPONENT(CUserInfoComp);
        I_REGISTER_INTERFACE(IUserInfo);
        I_REGISTER_INTERFACE(iser::ISerializable);
        I_ASSIGN(m_featuresCompPtr, "Features", "All features avaliable of this user", true, "Features");
        I_ASSIGN(m_rolesCompPtr, "Roles", "All roles avaliable of this user", true, "Roles");
        I_ASSIGN(m_userIdAttrPtr, "UserId", "User id", true, "");
        I_ASSIGN(m_usernameAttrPtr, "Username", "Username", true, "");
        I_ASSIGN(m_nameAttrPtr, "Name", "Name", true, "");
        I_ASSIGN(m_passwordHashIdAttrPtr, "PasswordHash", "Password Hash", true, "");
        I_ASSIGN(m_mailAttrPtr, "Mail", "Mail", true, "");
    I_END_COMPONENT;

protected:
    // reimplemented (icomp::CComponentBase)
    virtual void OnComponentCreated() override;

private:
    I_REF(imtlic::IFeatureInfoProvider, m_featuresCompPtr);
    I_REF(imtbase::IObjectCollection, m_rolesCompPtr);
    I_TEXTATTR(m_userIdAttrPtr);
    I_TEXTATTR(m_usernameAttrPtr);
    I_TEXTATTR(m_nameAttrPtr);
    I_TEXTATTR(m_passwordHashIdAttrPtr);
    I_TEXTATTR(m_mailAttrPtr);
};


} // namespace imtauth
