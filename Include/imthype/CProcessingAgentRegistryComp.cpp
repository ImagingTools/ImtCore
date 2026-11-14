// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imthype/CProcessingAgentRegistryComp.h>


namespace imthype
{


// reimplemented (imthype::IProcessingAgentRegistry)

int CProcessingAgentRegistryComp::GetAgentsCount() const
{
	return m_agentsCompPtr.GetCount();
}


IProcessingAgent* CProcessingAgentRegistryComp::GetAgentAt(int index) const
{
	if ((index < 0) || (index >= m_agentsCompPtr.GetCount())){
		return nullptr;
	}

	return m_agentsCompPtr[index];
}


IProcessingAgent* CProcessingAgentRegistryComp::FindAgentForTask(const QByteArray& taskTypeId) const
{
	for (int i = 0; i < m_agentsCompPtr.GetCount(); ++i){
		IProcessingAgent* agentPtr = m_agentsCompPtr[i];
		if (agentPtr == nullptr){
			continue;
		}

		if (!agentPtr->IsTaskSupported(taskTypeId)){
			continue;
		}

		IProcessingAgent::AgentState state = agentPtr->GetAgentState();
		if ((state == IProcessingAgent::AS_IDLE) || (state == IProcessingAgent::AS_BUSY)){
			return agentPtr;
		}
	}

	return nullptr;
}


} // namespace imthype
