#include <imtgui/CPageSwitcherComp.h>


namespace imtgui
{


// public methods

// reimplemented (imod::CSingleModelObserverBase)

void CPageSwitcherComp::OnUpdate(const istd::IChangeable::ChangeSet& changeSet)
{
	iauth::ILogin* loginPtr = GetObjectPtr();
	if (loginPtr != nullptr){
		if (changeSet.ContainsExplicit(iauth::ILogin::CF_LOGIN)){
			if (m_pageSelectorCompPtr.IsValid()){
				if (m_onLoginPageIndexAttrPtr.IsValid()){
					m_pageSelectorCompPtr->SetSelectedOptionIndex(*m_onLoginPageIndexAttrPtr);
				}
			}
		}

		if (changeSet.ContainsExplicit(iauth::ILogin::CF_LOGOUT)){
			if (m_pageSelectorCompPtr.IsValid()){
				if (m_onLogoutPageIndexAttrPtr.IsValid()){
					m_pageSelectorCompPtr->SetSelectedOptionIndex(*m_onLogoutPageIndexAttrPtr);
				}
			}
		}
	}
}


} // namespace imtgui


