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
		I_REGISTER_INTERFACE(iser::IObject);
		I_REGISTER_INTERFACE(iser::ISerializable);
		I_ASSIGN(m_settingsFactCompPtr, "SettingsFactory", "Factory used for creation of the user related settings instance", true, "SettingsFactory");
	I_END_COMPONENT;

protected:
	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;
	virtual void OnComponentDestroyed() override;

private:
	I_FACT(iprm::IParamsSet, m_settingsFactCompPtr);
};


} // namespace imtauth


