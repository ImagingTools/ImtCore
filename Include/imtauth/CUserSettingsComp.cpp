#include <imtauth/CUserSettingsComp.h>


namespace imtauth
{


// protected methods

// reimplemented (icomp::CComponentBase)

void CUserSettingsComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	if(m_settingsParamsSetCompPtr.IsValid()){
		BaseClass2::SetSettings(m_settingsParamsSetCompPtr.GetPtr());
	}
}


void CUserSettingsComp::OnComponentDestroyed()
{
	BaseClass::OnComponentDestroyed();
}


} // namespace imtauth
