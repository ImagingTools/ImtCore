// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
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
