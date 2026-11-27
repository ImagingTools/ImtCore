// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once

// ACF includes
#include <ilog/TLoggerCompWrap.h>

// ImtCore includes
#include <imtassist/CAssistantActionRegistry.h>
#include <imtassist/IAssistantAction.h>


namespace imtassist
{


/**
	Component of the action registry. The actions are the components assigned to \c Actions.

	\ingroup imtassist
*/
class CAssistantActionRegistryComp:
			public ilog::CLoggerComponentBase,
			public CAssistantActionRegistry
{
public:
	typedef ilog::CLoggerComponentBase BaseClass;

	I_BEGIN_COMPONENT(CAssistantActionRegistryComp);
		I_REGISTER_INTERFACE(IAssistantActionRegistry);
		I_ASSIGN_MULTI_0(m_actionsCompPtr, "Actions", "Actions available to the assistant", false);
	I_END_COMPONENT;

protected:
	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;

private:
	I_MULTIREF(IAssistantAction, m_actionsCompPtr);
};


} // namespace imtassist


