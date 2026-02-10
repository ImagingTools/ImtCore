// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <icomp/CComponentBase.h>

// ImtCore includes
#include <imtauth/IRoleInfoProvider.h>
#include <imtauth/IUserInfoProvider.h>
#include <imtauth/IUserGroupInfoProvider.h>
#include <imtauth/CUserInfo.h>


namespace imtauth
{


class CUserInfoComp:
			public icomp::CComponentBase,
			public CIdentifiableUserInfo
{
public:
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_COMPONENT(CUserInfoComp);
		I_REGISTER_INTERFACE(IUserInfo);
		I_REGISTER_INTERFACE(iser::ISerializable);
		I_ASSIGN(m_roleProviderCompPtr, "RoleProvider", "Role provider", false, "RoleProvider");
		I_ASSIGN(m_userGroupProviderCompPtr, "UserGroupProvider", "User group provider", false, "UserGroupProvider");
		I_ASSIGN(m_adminIdAttrPtr, "AdminId", "Id for admin user", false, "admin");
	I_END_COMPONENT;

protected:
	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;

	// reimplemented (iser::IUserInfo)
	virtual bool IsAdmin() const override;

private:
	I_REF(imtauth::IRoleInfoProvider, m_roleProviderCompPtr);
	I_REF(imtauth::IUserGroupInfoProvider, m_userGroupProviderCompPtr);
	I_ATTR(QByteArray, m_adminIdAttrPtr);
};


} // namespace imtauth
