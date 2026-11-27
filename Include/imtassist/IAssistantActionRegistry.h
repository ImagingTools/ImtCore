// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once

// Qt includes
#include <QtCore/QByteArrayList>

// ACF includes
#include <istd/IPolymorphic.h>


namespace imtassist
{


class IAssistantAction;


/**
	Registry of the actions available to the assistant.
	Only registered actions can be called; there is no generic function execution.

	\ingroup imtassist
*/
class IAssistantActionRegistry: virtual public istd::IPolymorphic
{
public:
	virtual QByteArrayList GetActionIds() const = 0;

	/**
		Get a registered action.
		\return Action, or nullptr if no action with this ID is registered.
	*/
	virtual IAssistantAction* GetAction(const QByteArray& actionId) const = 0;
};


} // namespace imtassist


