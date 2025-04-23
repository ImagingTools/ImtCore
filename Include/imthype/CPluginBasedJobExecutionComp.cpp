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
	if (m_jobProcessorMap.contains(workerTypeId)) {
		return m_jobProcessorMap[workerTypeId].get();
	}

	if (m_pluginsMap.contains(workerTypeId)){
		const imthype::IJobProcessorFactory* jobProcessorFactoryPtr = m_pluginsMap[workerTypeId]->GetJobProcessorFactory();
		if (jobProcessorFactoryPtr != nullptr){
			istd::TDelPtr<IJobProcessor> processorPtr(jobProcessorFactoryPtr->CreateInstance(workerTypeId));
			if (processorPtr.IsValid()){
				if (processorPtr->GetTaskTypeId() != workerTypeId){
					SendCriticalMessage(0, "Job processor were created, but they have a wrong type. Please check the processor component registration");

					return nullptr;
				}

				m_jobProcessorMap[workerTypeId] = JobProcessorPtr(processorPtr.PopPtr());

				return m_jobProcessorMap[workerTypeId].get();
			}
			else {
				SendCriticalMessage(0, "Job processor could not be created");
			}
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


// reimplemented (icomp::CComponentBase)

void CPluginBasedJobExecutionComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	TPluginManager::SetLogPtr(CJobExecutionControllerCompBase::GetLogPtr());
}


} // namespace imthype


