// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <icomp/CComponentBase.h>

// Acula includes
#include <imthype/IProcessingAgentRegistry.h>
#include <imthype/IProcessingAgent.h>


namespace imthype
{


/**
	Registry of processing agents based on a statically connected list of agents.

	The component collects the agents referenced over the "Agents" multi-reference
	and selects, for a requested task type, the first agent that supports the task
	and is currently able to accept new work.
*/
class CProcessingAgentRegistryComp:
			public icomp::CComponentBase,
			virtual public IProcessingAgentRegistry
{
public:
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_COMPONENT(CProcessingAgentRegistryComp);
		I_REGISTER_INTERFACE(IProcessingAgentRegistry);
		I_ASSIGN_MULTI_0(m_agentsCompPtr, "Agents", "List of available processing agents", true);
	I_END_COMPONENT;

	// reimplemented (imthype::IProcessingAgentRegistry)
	virtual int GetAgentsCount() const override;
	virtual IProcessingAgent* GetAgentAt(int index) const override;
	virtual IProcessingAgent* FindAgentForTask(const QByteArray& taskTypeId) const override;

private:
	/**
		Reference to the list of available processing agents.
	*/
	I_MULTIREF(IProcessingAgent, m_agentsCompPtr);
};


} // namespace imthype
