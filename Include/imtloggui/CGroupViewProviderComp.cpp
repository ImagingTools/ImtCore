// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtloggui/CGroupViewProviderComp.h>


// ACF includes
#include <iprm/IIdParam.h>


namespace imtloggui
{


// protected methods

// reimplemented (icomp::CComponentBase)

void CGroupViewProviderComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	int count = qMin(m_idAttrPtr.GetCount(), m_nameAttrPtr.GetCount());
	count = qMin(count, m_groupViewCompPtr.GetCount());

	for (int i = 0; i < count; i++){
		Q_ASSERT(!GetElementIds().contains(m_idAttrPtr[i]));

		iprm::IIdParam* idParamPtr = dynamic_cast<iprm::IIdParam*>(m_groupViewCompPtr[i]);
		if (idParamPtr != nullptr){
			RegisterObject(m_idAttrPtr[i], "", m_nameAttrPtr[i], "", m_groupViewCompPtr[i]);
			idParamPtr->SetId(m_idAttrPtr[i]);
		}
	}
}


} // namespace imtloggui


