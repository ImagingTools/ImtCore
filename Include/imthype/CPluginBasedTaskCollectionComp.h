// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QMap>

// ImtCore includes
#include <imtbase/PluginInterface.h>
#include <imtbase/TPluginManagerCompBase.h>

// Acula includes
#include <imthype/CTaskCollectionCompBase.h>
#include <imthype/ITaskPlugin.h>


IMT_DECLARE_PLUGIN_INTERFACE(Task, imthype::ITaskPlugin);


namespace imthype
{


class CPluginBasedTaskCollectionComp:
			public imtbase::TPluginManagerCompBase<
						imthype::ITaskPlugin,
						IMT_CREATE_PLUGIN_FUNCTION(Task),
						IMT_DESTROY_PLUGIN_FUNCTION(Task),
						CTaskCollectionCompBase>,
			virtual public imthype::ITaskCollection
{
	using CTaskCollectionCompBase::SendCriticalMessage;
public:
	typedef imtbase::TPluginManagerCompBase<
				imthype::ITaskPlugin,
				IMT_CREATE_PLUGIN_FUNCTION(Task),
				IMT_DESTROY_PLUGIN_FUNCTION(Task),
				CTaskCollectionCompBase> BaseClass;

	I_BEGIN_COMPONENT(CPluginBasedTaskCollectionComp);
		I_REGISTER_INTERFACE(imthype::ITaskCollection);
	I_END_COMPONENT;

	CPluginBasedTaskCollectionComp();

	// reimplemented (CTaskCollectionCompBase)
	virtual QString GetTaskTypeName(const QByteArray& taskTypeId) const override;

	// reimplemented (IObjectCollectionInfo)
	virtual const iprm::IOptionsList* GetObjectTypesInfo() const override;

protected:
	// reimplemented (CTaskCollectionCompBase)
	virtual iinsp::ISupplierUniquePtr CreateTaskInstance(const QByteArray& taskTypeId) const override;

	// reimplemented (TPluginManagerCompBase)
	virtual void OnPluginsCreated();

private:
	typedef QMap<QByteArray, imthype::ITaskPlugin*> PluginsMap;
	PluginsMap m_pluginsMap;

	imod::TModelWrap<iprm::COptionsManager> m_taskFactories;
};


} // namespace imthype


