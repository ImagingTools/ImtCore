// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtassist/CAssistantActionRegistry.h>


// ImtCore includes
#include <imtassist/IAssistantAction.h>


namespace imtassist
{


bool CAssistantActionRegistry::RegisterAction(IAssistantAction* actionPtr)
{
	if (actionPtr == nullptr){
		return false;
	}

	QByteArray actionId = actionPtr->GetActionId();
	if (!IsValidActionId(actionId) || m_actions.contains(actionId)){
		return false;
	}

	if (actionPtr->GetParameterSchema().GetVersion() <= 0){
		return false;
	}

	m_actions.insert(actionId, actionPtr);

	return true;
}


bool CAssistantActionRegistry::IsValidActionId(const QByteArray& actionId)
{
	if (actionId.isEmpty() || (actionId.size() > 128)){
		return false;
	}

	char first = actionId.at(0);
	if (!(((first >= 'a') && (first <= 'z')) || ((first >= 'A') && (first <= 'Z')))){
		return false;
	}

	for (char character: actionId){
		bool isAllowed =
					((character >= 'a') && (character <= 'z')) ||
					((character >= 'A') && (character <= 'Z')) ||
					((character >= '0') && (character <= '9')) ||
					(character == '.') || (character == '_') || (character == '-');
		if (!isAllowed){
			return false;
		}
	}

	return true;
}


// reimplemented (imtassist::IAssistantActionRegistry)

QByteArrayList CAssistantActionRegistry::GetActionIds() const
{
	return m_actions.keys();
}


IAssistantAction* CAssistantActionRegistry::GetAction(const QByteArray& actionId) const
{
	return m_actions.value(actionId, nullptr);
}


} // namespace imtassist


