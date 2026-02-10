// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtloggui/CGroupProviderComp.h>


namespace imtloggui
{


// protected methods

// reimplemented (icomp::CComponentBase)

void CGroupProviderComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	int count = qMin(m_idAttrPtr.GetCount(), m_nameAttrPtr.GetCount());
	count = qMin(count, m_groupCompPtr.GetCount());

	for (int i = 0; i < count; i++){
		Q_ASSERT(!GetElementIds().contains(m_idAttrPtr[i]));
		RegisterObject(m_idAttrPtr[i], "", m_nameAttrPtr[i], "", m_groupCompPtr[i]);
	}
}


} // namespace imtloggui


