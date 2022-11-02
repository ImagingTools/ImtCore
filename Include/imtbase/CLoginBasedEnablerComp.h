#pragma once


// ACF includes
#include <icomp/CComponentBase.h>
#include <imod/TSingleModelObserverBase.h>
#include <iprm/IEnableableParam.h>

// ACF-Solutions includes
#include <iauth/ILogin.h>


namespace imtbase
{


class CLoginBasedEnablerComp:
			public icomp::CComponentBase,
			virtual public iprm::IEnableableParam,
			protected imod::TSingleModelObserverBase<iauth::ILogin>
{
public:
	typedef icomp::CComponentBase BaseClass;
	typedef imod::TSingleModelObserverBase<iauth::ILogin> BaseClass2;

	I_BEGIN_COMPONENT(CLoginBasedEnablerComp);
		I_REGISTER_INTERFACE(iprm::IEnableableParam);
		I_ASSIGN(m_loginCompPtr, "Login", "Login", true, "Login");
		I_ASSIGN_TO(m_loginModelCompPtr, m_loginCompPtr, true);
	I_END_COMPONENT;

	CLoginBasedEnablerComp();

	// reimplemented (iprm::IEnableableParam)
	virtual bool IsEnabled() const;
	virtual bool IsEnablingAllowed() const;
	virtual bool SetEnabled(bool isEnabled = true);

	// reimplemented (iser::ISerializable)
	virtual bool Serialize(iser::IArchive& archive);

protected:
	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated();
	virtual void OnComponentDestroyed();

	// reimplemented (imod::TSingleModelObserverBase)
	virtual void OnUpdate(const istd::IChangeable::ChangeSet& changeSet);

private:
	I_REF(iauth::ILogin, m_loginCompPtr);
	I_REF(imod::IModel, m_loginModelCompPtr);

	bool m_isEnabled;
};


} // namespace imtbase


