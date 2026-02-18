// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauth/CUserRecentActionComp.h>


namespace imtauth
{


// protected methods

// reimplemented (icomp::CComponentBase)

void CUserRecentActionComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	if (m_actionDataFactPtr.IsValid() && m_actionTypeIdsAttrPtr.IsValid()){
		SetActionDataFactory([this](const QByteArray& actionTypeId) -> iser::ISerializableSharedPtr {
			int index = m_actionTypeIdsAttrPtr.FindValue(actionTypeId);
			if (index < 0){
				Q_ASSERT(false);
				return nullptr;
			}

			if (index >= m_actionDataFactPtr.GetCount()){
				Q_ASSERT(false);
				return nullptr;
			}

			return m_actionDataFactPtr.CreateInstance(index);
		});
	}
}


} // namespace imtauth


