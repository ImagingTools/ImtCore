#pragma once


// Acf includes
#include <icomp/CComponentBase.h>
#include <iprm/IParamsSet.h>

// ImtCore includes
#include <imtauth/CUserSettings.h>


namespace imtauth
{


class CUserSettingsComp:
		public icomp::CComponentBase,
		public CUserSettings
{
public:
	typedef icomp::CComponentBase BaseClass;
	typedef CUserSettings BaseClass2;

	I_BEGIN_COMPONENT(CUserSettingsComp);
		I_REGISTER_INTERFACE(IUserSettings);
		I_ASSIGN(m_settingsParamsSetCompPtr, "SettingsParamsSet", "Params set of the settings", true, "SettingsParamsSet");
	I_END_COMPONENT;

protected:
	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;
	virtual void OnComponentDestroyed() override;

private:
	I_REF(iprm::IParamsSet, m_settingsParamsSetCompPtr);
};


} // namespace imtauth


