// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once

// Qt includes
#include <QtCore/QMap>

// ImtCore includes
#include <imtassist/IAssistantActionRegistry.h>


namespace imtassist
{


/**
	Registry of assistant actions. The registry does not own the actions.

	\ingroup imtassist
*/
class CAssistantActionRegistry: virtual public IAssistantActionRegistry
{
public:
	/**
		Register an action.
		\return False if the action is invalid, its ID is malformed or already registered.
	*/
	bool RegisterAction(IAssistantAction* actionPtr);

	/**
		Check the format of an action ID: letters, digits, '.', '_' and '-', starting with a letter.
	*/
	static bool IsValidActionId(const QByteArray& actionId);

	// reimplemented (imtassist::IAssistantActionRegistry)
	virtual QByteArrayList GetActionIds() const override;
	virtual IAssistantAction* GetAction(const QByteArray& actionId) const override;

private:
	QMap<QByteArray, IAssistantAction*> m_actions;
};


} // namespace imtassist


