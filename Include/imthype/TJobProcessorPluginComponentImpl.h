// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtbase/TBasePluginComponentImpl.h>

// Acula includes
#include <imthype/IJobProcessorPlugin.h>


namespace imthype
{


/**
	Generic implementation of task plugin.
*/
template <class ComponentClass>
class TJobProcessorPluginComponentImpl: public imtbase::TBasePluginComponentImpl<ComponentClass, IJobProcessorPlugin>
{
public:
	typedef imtbase::TBasePluginComponentImpl<ComponentClass, IJobProcessorPlugin> BaseClass;

	TJobProcessorPluginComponentImpl(const QString& pluginName, const QByteArray& typeId);

	// reimplemented (IJobProcessorPlugin)
	virtual const IJobProcessorFactory* GetJobProcessorFactory() const override;
};


// public methods
template <class ComponentClass>
TJobProcessorPluginComponentImpl<ComponentClass>::TJobProcessorPluginComponentImpl(const QString& pluginName, const QByteArray& typeId)
	:BaseClass(pluginName, typeId)
{
}


// reimplemented (IJobProcessorPlugin)

template <class ComponentClass>
const IJobProcessorFactory* TJobProcessorPluginComponentImpl<ComponentClass>::GetJobProcessorFactory() const
{
	return BaseClass::m_component.template GetInterface<IJobProcessorFactory>(QByteArray());
}


} // namespace imthype


