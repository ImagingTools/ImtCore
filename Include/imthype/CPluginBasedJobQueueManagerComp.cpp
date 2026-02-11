// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imthype/CPluginBasedJobQueueManagerComp.h>


namespace imthype
{


// public methods

CPluginBasedJobQueueManagerComp::CPluginBasedJobQueueManagerComp()
	:BaseClass(IMT_CREATE_PLUGIN_INSTANCE_FUNCTION_NAME(TaskParamSet), IMT_DESTROY_PLUGIN_INSTANCE_FUNCTION_NAME(TaskParamSet))
{
}


// protected methods

// reimplemented (IJobQueueManager)

const iprm::IOptionsList* CPluginBasedJobQueueManagerComp::GetSupportedTasks() const
{
	return &m_taskParamsFactories;
}


iprm::IParamsSetUniquePtr CPluginBasedJobQueueManagerComp::CreateJobParameters(
	const QByteArray& /*contextId*/,
	const QByteArray& taskTypeId,
	const iprm::IParamsSet* defaultParamPtr) const
{
	if (m_pluginsMap.contains(taskTypeId)){
		const imthype::ITaskParamsPlugin::IParamSetFactory* paramsFactoryPtr = m_pluginsMap[taskTypeId]->GetTaskParamsFactory();
		if (paramsFactoryPtr != nullptr){
			iprm::IParamsSetUniquePtr newParamsPtr = paramsFactoryPtr->CreateInstance(taskTypeId);
			if (newParamsPtr.IsValid()){
				if (newParamsPtr->GetFactoryId() != taskTypeId){
					SendCriticalMessage(0, "Job parameter were created, but they have a wrong type. Please check the parameter registration");

					return nullptr;
				}

				if (defaultParamPtr != nullptr){
					if (!newParamsPtr->CopyFrom(*defaultParamPtr)){
						SendCriticalMessage(0, "Job parameter could not be initialized");

						return nullptr;
					}
				}

				return newParamsPtr;
			}

			SendCriticalMessage(0, "Job parameter could not be created");

			return paramsFactoryPtr->CreateInstance(taskTypeId);
		}
	}

	return nullptr;
}


// reimplemented (TPluginManagerCompBase)

void CPluginBasedJobQueueManagerComp::OnPluginsCreated()
{
	BaseClass::OnPluginsCreated();

	for (const PluginInfo & pluginInfo : m_plugins){
		if (pluginInfo.pluginPtr != nullptr){
			const imthype::ITaskParamsPlugin::IParamSetFactory* factoryPtr = pluginInfo.pluginPtr->GetTaskParamsFactory();
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

	for (PluginsMap::ConstIterator iter = m_pluginsMap.constBegin(); iter != m_pluginsMap.constEnd(); ++iter){
		m_taskParamsFactories.InsertOption(iter.value()->GetPluginName(), iter.key());
	}
}


} // namespace imthype


