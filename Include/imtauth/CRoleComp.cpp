// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauth/CRoleComp.h>


namespace imtauth
{


// protected methods

// reimplemented (icomp::CComponentBase)

void CRoleComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	if (m_parentRoleProviderCompPtr.IsValid()){
		SetParentRoleProvider(m_parentRoleProviderCompPtr.GetPtr());
	}
}


void CRoleComp::OnComponentDestroyed()
{
	BaseClass::OnComponentDestroyed();
}


} // namespace imtauth


