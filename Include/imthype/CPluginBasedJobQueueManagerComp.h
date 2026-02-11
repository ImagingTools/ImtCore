// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
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
	using CJobQueueManagerCompBase::SendCriticalMessage;
public:
	typedef imtbase::TPluginManagerCompBase<
				imthype::ITaskParamsPlugin,
				IMT_CREATE_PLUGIN_FUNCTION(TaskParamSet),
				IMT_DESTROY_PLUGIN_FUNCTION(TaskParamSet),
				CJobQueueManagerCompBase> BaseClass;

	I_BEGIN_COMPONENT(CPluginBasedJobQueueManagerComp);
	I_END_COMPONENT;

	CPluginBasedJobQueueManagerComp();

	// reimplemented (IJobQueueManager)
	virtual const iprm::IOptionsList* GetSupportedTasks() const override;
	virtual iprm::IParamsSetUniquePtr CreateJobParameters(
				const QByteArray& contextId,
				const QByteArray& taskTypeId,
				const iprm::IParamsSet* defaultParamPtr) const override;

protected:
	// reimplemented (TPluginManagerCompBase)
	virtual void OnPluginsCreated();

private:
	typedef QMap<QByteArray, imthype::ITaskParamsPlugin*> PluginsMap;
	PluginsMap m_pluginsMap;

	imod::TModelWrap<iprm::COptionsManager> m_taskParamsFactories;
};


} // namespace imthype


