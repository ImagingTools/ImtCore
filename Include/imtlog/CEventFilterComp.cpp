// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtlog/CEventFilterComp.h>


namespace imtlog
{


// protected methods

// reimplemented (icomp::CComponentBase)

void CEventFilterComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	if (m_messageIdsAttrPtr.IsValid()){
		QSet<int> ids;
		for (int i = 0; i < m_messageIdsAttrPtr.GetCount(); i++){
			ids.insert(m_messageIdsAttrPtr[i]);
		}

		SetMessageFilterIds(ids);
	}

	if (*m_includeModeAttrPtr){
		SetFilterMode(FM_INCLUDE);
	}
	else{
		SetFilterMode(FM_EXCLUDE);
	}
}


} // namespace imtlog


