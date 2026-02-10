// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
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
	virtual bool IsEnabled() const override;
	virtual bool IsEnablingAllowed() const override;
	virtual bool SetEnabled(bool isEnabled = true) override;

	// reimplemented (iser::ISerializable)
	virtual bool Serialize(iser::IArchive& archive) override;

protected:
	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;
	virtual void OnComponentDestroyed() override;

	// reimplemented (imod::TSingleModelObserverBase)
	virtual void OnUpdate(const istd::IChangeable::ChangeSet& changeSet) override;

private:
	I_REF(iauth::ILogin, m_loginCompPtr);
	I_REF(imod::IModel, m_loginModelCompPtr);

	bool m_isEnabled;
};


} // namespace imtbase


