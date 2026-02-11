// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtbase/TBasePluginComponentImpl.h>

// Acula includes
#include <imthype/ITaskParamsPlugin.h>


namespace imthype
{


/**
	Generic implementation of task plugin.
*/
template <class ComponentClass>
class TTaskParamsPluginComponentImpl : public imtbase::TBasePluginComponentImpl<ComponentClass, ITaskParamsPlugin>
{
public:
	typedef imtbase::TBasePluginComponentImpl<ComponentClass, ITaskParamsPlugin> BaseClass;

	TTaskParamsPluginComponentImpl(const QString& pluginName, const QByteArray& typeId);

	// reimplemented (ITaskParamsPlugin)
	virtual const ITaskParamsPlugin::IParamSetFactory* GetTaskParamsFactory() const override;
};


// public methods
template <class ComponentClass>
TTaskParamsPluginComponentImpl<ComponentClass>::TTaskParamsPluginComponentImpl(const QString& pluginName, const QByteArray& typeId)
	:BaseClass(pluginName, typeId)
{
}


// reimplemented (ITaskPlugin)

template <class ComponentClass>
const ITaskParamsPlugin::IParamSetFactory* TTaskParamsPluginComponentImpl<ComponentClass>::GetTaskParamsFactory() const
{
	return BaseClass::m_component.template GetInterface<ITaskParamsPlugin::IParamSetFactory>(QByteArray());
}


} // namespace imthype


