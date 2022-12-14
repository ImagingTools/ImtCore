#include <imtauth/CUserSettingsComp.h>


namespace imtauth
{


// protected methods

// reimplemented (icomp::CComponentBase)

void CUserSettingsComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	if(m_settingsFactCompPtr.IsValid()){
		m_settingsPtr = m_settingsFactCompPtr.CreateInstance();
	}
}


void CUserSettingsComp::OnComponentDestroyed()
{
	BaseClass::OnComponentDestroyed();
}


} // namespace imtauth


