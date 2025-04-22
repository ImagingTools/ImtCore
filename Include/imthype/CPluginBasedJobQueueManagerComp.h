#pragma once


// Qt includes
#include <QtCore/QMap>


// ACF includes
#include <iprm/COptionsManager.h>

// ImtCore includes
#include <imtbase/PluginInterface.h>
#include <imtbase/TPluginManagerCompBase.h>

// Acula includes
#include <imthype/CJobQueueManagerCompBase.h>
#include <imthype/ITaskParamsPlugin.h>


IMT_DECLARE_PLUGIN_INTERFACE(TaskParamSet, imthype::ITaskParamsPlugin);


namespace imthype
{


class CPluginBasedJobQueueManagerComp:
			public imtbase::TPluginManagerCompBase<
						imthype::ITaskParamsPlugin,
						IMT_CREATE_PLUGIN_FUNCTION(TaskParamSet),
						IMT_DESTROY_PLUGIN_FUNCTION(TaskParamSet),
						CJobQueueManagerCompBase>
{
public:
	typedef imtbase::TPluginManagerCompBase<
				imthype::ITaskParamsPlugin,
				IMT_CREATE_PLUGIN_FUNCTION(TaskParamSet),
				IMT_DESTROY_PLUGIN_FUNCTION(TaskParamSet),
				CJobQueueManagerCompBase> BaseClass;

	I_BEGIN_COMPONENT(CPluginBasedJobQueueManagerComp);
		I_ASSIGN(m_logCompPtr, "JobManagerLog", "Job management log", false, "JobManagerLog");
	I_END_COMPONENT;

	CPluginBasedJobQueueManagerComp();

	// reimplemented (IJobQueueManager)
	virtual const iprm::IOptionsList* GetSupportedTasks() const override;
	virtual iprm::IParamsSet* CreateJobParameters(
				const QByteArray& contextId,
				const QByteArray& taskTypeId,
				const iprm::IParamsSet* defaultParamPtr) const override;

protected:
	// reimplemented (TPluginManagerCompBase)
	virtual void OnPluginsCreated();

private:
	I_REF(ilog::IMessageConsumer, m_logCompPtr);

	typedef QMap<QByteArray, imthype::ITaskParamsPlugin*> PluginsMap;
	PluginsMap m_pluginsMap;

	imod::TModelWrap<iprm::COptionsManager> m_taskParamsFactories;
};


} // namespace imthype


