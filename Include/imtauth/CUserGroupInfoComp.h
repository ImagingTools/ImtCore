#pragma once


// ACF includes
#include <icomp/CComponentBase.h>

// ImtCore includes
#include <imtauth/CUserGroupInfo.h>
#include <imtauth/IRoleInfoProvider.h>


namespace imtauth
{


class CUserGroupInfoComp:
			public icomp::CComponentBase,
			public CIdentifiableUserGroupInfo
{
public:
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_COMPONENT(CUserGroupInfoComp);
		I_REGISTER_INTERFACE(IUserGroupInfo);
		I_REGISTER_INTERFACE(iser::ISerializable);
		I_ASSIGN(m_roleProviderCompPtr, "RoleProvider", "Role provider", false, "RoleProvider");
		I_ASSIGN(m_userGroupProviderCompPtr, "UserGroupProvider", "User group provider", false, "UserGroupProvider");
		I_ASSIGN(m_userProviderCompPtr, "UserProvider", "User provider", false, "UserProvider");
	I_END_COMPONENT;

protected:
	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;

private:
	I_REF(imtauth::IRoleInfoProvider, m_roleProviderCompPtr);
	I_REF(imtauth::IUserGroupInfoProvider, m_userGroupProviderCompPtr);
	I_REF(imtauth::IUserInfoProvider, m_userProviderCompPtr);
};


} // namespace imtauth


