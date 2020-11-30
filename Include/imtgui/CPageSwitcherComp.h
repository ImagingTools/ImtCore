#pragma once


// ACF includes
#include <iauth/ILogin.h>
#include <iprm/ISelectionParam.h>
#include <icomp/TMakeComponentWrap.h>
#include <imod/TSingleModelObserverBase.h>


namespace imtgui
{


class CPageSwitcherComp:
			public icomp::TMakeComponentWrap<
						imod::TSingleModelObserverBase<iauth::ILogin>>
{
public:
	typedef icomp::TMakeComponentWrap<
				imod::TSingleModelObserverBase<iauth::ILogin>> BaseClass;


	I_BEGIN_COMPONENT(CPageSwitcherComp);
		I_ASSIGN(m_pageSelectorCompPtr, "PageSelector", "Page selector", true, "PageSelector");
		I_ASSIGN(m_onLoginPageIndexAttrPtr, "OnLoginPageIndex", "Switch to page index on login", false, 0);
		I_ASSIGN(m_onLogoutPageIndexAttrPtr, "OnLogoutPageIndex", "Switch to page index on logout", false, 0);
	I_END_COMPONENT;

protected:
	// reimplemented (imod::CSingleModelObserverBase)
	virtual void OnUpdate(const istd::IChangeable::ChangeSet& changeSet) override;

private:
	I_REF(iprm::ISelectionParam, m_pageSelectorCompPtr);
	I_ATTR(int, m_onLoginPageIndexAttrPtr);
	I_ATTR(int, m_onLogoutPageIndexAttrPtr);
};


} // namespace imtgui


