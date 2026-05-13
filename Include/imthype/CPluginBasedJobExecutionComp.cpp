// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imthype/CPluginBasedJobExecutionComp.h>


namespace imthype
{


// public methods

CPluginBasedJobExecutionComp::CPluginBasedJobExecutionComp()
	:BaseClass(IMT_CREATE_PLUGIN_INSTANCE_FUNCTION_NAME(JobProcessor), IMT_DESTROY_PLUGIN_INSTANCE_FUNCTION_NAME(JobProcessor))
{
}


// protected methods

// reimplemented (CJobExecutionControllerCompBase)

const IJobProcessor* CPluginBasedJobExecutionComp::GetTaskProcessor(const QByteArray& workerTypeId) const
{
	if (m_jobProcessorMap.contains(workerTypeId)){
		return m_jobProcessorMap[workerTypeId].GetPtr();
	}

	if (m_pluginsMap.contains(workerTypeId)){
		const imthype::IJobProcessorFactory* jobProcessorFactoryPtr = m_pluginsMap[workerTypeId]->GetJobProcessorFactory();
		if (jobProcessorFactoryPtr != nullptr){
			IJobProcessorUniquePtr processorPtr(jobProcessorFactoryPtr->CreateInstance(workerTypeId));
			if (processorPtr.IsValid()){
				if (processorPtr->GetTaskTypeId() != workerTypeId){
					SendCriticalMessage(0, "Job processor were created, but they have a wrong type. Please check the processor component registration");

					return nullptr;
				}

				m_jobProcessorMap[workerTypeId].FromUnique(processorPtr);

				return m_jobProcessorMap[workerTypeId].GetPtr();
			}

			SendCriticalMessage(0, "Job processor could not be created");
		}
	}

	return nullptr;
}


// reimplemented (TPluginManagerCompBase)

void CPluginBasedJobExecutionComp::OnPluginsCreated()
{
	BaseClass::OnPluginsCreated();

	for (const PluginInfo & pluginInfo : m_plugins){
		if (pluginInfo.pluginPtr != nullptr){
			const imthype::IJobProcessorFactory* factoryPtr = pluginInfo.pluginPtr->GetJobProcessorFactory();
			if (factoryPtr != nullptr){
				istd::IFactoryInfo::KeyList keys = factoryPtr->GetFactoryKeys();
				for (const QByteArray& key : keys){
					Q_ASSERT(!key.isEmpty());

					if (!key.isEmpty()){
						m_pluginsMap[key] = pluginInfo.pluginPtr;
					}
				}
			}
		}
	}
}


} // namespace imthype


