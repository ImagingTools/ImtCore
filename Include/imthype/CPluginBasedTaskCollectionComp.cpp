// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imthype/CPluginBasedTaskCollectionComp.h>


namespace imthype
{


// public methods

CPluginBasedTaskCollectionComp::CPluginBasedTaskCollectionComp()
	:BaseClass(IMT_CREATE_PLUGIN_INSTANCE_FUNCTION_NAME(Task), IMT_DESTROY_PLUGIN_INSTANCE_FUNCTION_NAME(Task))
{
}


// reimplemented (CTaskCollectionCompBase)

QString CPluginBasedTaskCollectionComp::GetTaskTypeName(const QByteArray& taskTypeId) const
{
	if (m_pluginsMap.contains(taskTypeId)){
		m_pluginsMap[taskTypeId]->GetPluginName();
	}

	return QString();
}


// reimplemented (IObjectCollectionInfo)

const iprm::IOptionsList* CPluginBasedTaskCollectionComp::GetObjectTypesInfo() const
{
	return &m_taskFactories;
}


// protected methods

// reimplemented (CTaskCollectionCompBase)

iinsp::ISupplierUniquePtr CPluginBasedTaskCollectionComp::CreateTaskInstance(const QByteArray& taskTypeId) const
{
	if (m_pluginsMap.contains(taskTypeId)){
		const ITaskPlugin::ISupplierFactory* taskFactoryPtr = m_pluginsMap[taskTypeId]->GetTaskFactory();
		if (taskFactoryPtr != nullptr){
			return taskFactoryPtr->CreateInstance(taskTypeId);
		}
	}

	return nullptr;
}


// reimplemented (TPluginManagerCompBase)

void CPluginBasedTaskCollectionComp::OnPluginsCreated()
{
	BaseClass::OnPluginsCreated();

	for (const PluginInfo & pluginInfo : m_plugins){
		if (pluginInfo.pluginPtr != nullptr){
			const ITaskPlugin::ISupplierFactory* factoryPtr = pluginInfo.pluginPtr->GetTaskFactory();
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
		m_taskFactories.InsertOption(iter.value()->GetPluginName(), iter.key());
	}
}


} // namespace imthype


