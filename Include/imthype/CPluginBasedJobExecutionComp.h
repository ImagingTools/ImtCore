// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QMap>

// ImtCore includes
#include <imtbase/PluginInterface.h>
#include <imtbase/TPluginManagerCompBase.h>

// Acula includes
#include <imthype/CJobExecutionControllerCompBase.h>
#include <imthype/IJobProcessorPlugin.h>


IMT_DECLARE_PLUGIN_INTERFACE(JobProcessor, imthype::IJobProcessorPlugin);


namespace imthype
{


class CPluginBasedJobExecutionComp:
			public imtbase::TPluginManagerCompBase<
						imthype::IJobProcessorPlugin,
						IMT_CREATE_PLUGIN_FUNCTION(JobProcessor),
						IMT_DESTROY_PLUGIN_FUNCTION(JobProcessor),
						CJobExecutionControllerCompBase>
{
	using CJobExecutionControllerCompBase::SendCriticalMessage;
public:
	typedef imtbase::TPluginManagerCompBase<
				imthype::IJobProcessorPlugin,
				IMT_CREATE_PLUGIN_FUNCTION(JobProcessor),
				IMT_DESTROY_PLUGIN_FUNCTION(JobProcessor),
				CJobExecutionControllerCompBase> BaseClass;

	I_BEGIN_COMPONENT(CPluginBasedJobExecutionComp);
	I_END_COMPONENT;

	CPluginBasedJobExecutionComp();

protected:
	// reimplemented (CJobExecutionControllerCompBase)
	virtual const IJobProcessor* GetTaskProcessor(const QByteArray& workerTypeId) const override;

	// reimplemented (TPluginManagerCompBase)
	virtual void OnPluginsCreated();

private:
	typedef QMap<QByteArray, imthype::IJobProcessorPlugin*> PluginsMap;
	PluginsMap m_pluginsMap;

	typedef IJobProcessorSharedPtr JobProcessorPtr;
	typedef QMap<QByteArray, JobProcessorPtr> JobProcessorMap;

	mutable JobProcessorMap m_jobProcessorMap;
};


} // namespace imthype


