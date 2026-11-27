// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtassist/CAssistantActionRegistryComp.h>


namespace imtassist
{


// protected methods

// reimplemented (icomp::CComponentBase)

void CAssistantActionRegistryComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	int count = m_actionsCompPtr.GetCount();
	for (int i = 0; i < count; ++i){
		IAssistantAction* actionPtr = m_actionsCompPtr[i];
		if (!RegisterAction(actionPtr)){
			QString actionId = (actionPtr != nullptr) ? QString::fromUtf8(actionPtr->GetActionId()) : QString();
			SendErrorMessage(0, QStringLiteral("Assistant action %1 ('%2') was rejected: invalid, malformed ID, unversioned schema or duplicate").arg(i).arg(actionId), "AssistantActionRegistry");
		}
	}
}


} // namespace imtassist


