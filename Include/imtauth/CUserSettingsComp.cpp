// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauth/CUserSettingsComp.h>


namespace imtauth
{


// protected methods

// reimplemented (icomp::CComponentBase)

void CUserSettingsComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	if(m_settingsFactCompPtr.IsValid()){
		m_settingsPtr.FromUnique(m_settingsFactCompPtr.CreateInstance());
	}
}


void CUserSettingsComp::OnComponentDestroyed()
{
	BaseClass::OnComponentDestroyed();
}


} // namespace imtauth


