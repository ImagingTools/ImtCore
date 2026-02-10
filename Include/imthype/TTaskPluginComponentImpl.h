// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtbase/TBasePluginComponentImpl.h>

// Acula includes
#include <imthype/ITaskPlugin.h>


namespace imthype
{


/**
	Generic implementation of task plugin.
*/
template <class ComponentClass>
class TTaskPluginComponentImpl: public imtbase::TBasePluginComponentImpl<ComponentClass, ITaskPlugin>
{
public:
	typedef imtbase::TBasePluginComponentImpl<ComponentClass, ITaskPlugin> BaseClass;

	TTaskPluginComponentImpl(const QString& pluginName, const QByteArray& typeId);

	// reimplemented (ITaskPlugin)
	virtual const ITaskPlugin::ISupplierFactory* GetTaskFactory() const override;
};


// public methods
template <class ComponentClass>
TTaskPluginComponentImpl<ComponentClass>::TTaskPluginComponentImpl(const QString& pluginName, const QByteArray& typeId)
	:BaseClass(pluginName, typeId)
{
}


// reimplemented (ITaskPlugin)

template <class ComponentClass>
const ITaskPlugin::ISupplierFactory* TTaskPluginComponentImpl<ComponentClass>::GetTaskFactory() const
{
	return BaseClass::m_component.template GetInterface<ITaskPlugin::ISupplierFactory>(QByteArray());
}


} // namespace imthype


