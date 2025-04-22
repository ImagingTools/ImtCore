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
class TTaskPluginComponentImpl: public imtbase::TBasePluginComponentImpl<ComponentClass, imthype::ITaskPlugin>
{
public:
	typedef imtbase::TBasePluginComponentImpl<ComponentClass, imthype::ITaskPlugin> BaseClass;

	TTaskPluginComponentImpl(const QString& pluginName, const QByteArray& typeId);

	// reimplemented (imthype::ITaskPlugin)
	virtual const imthype::ITaskPlugin::ISupplierFactory* GetTaskFactory() const override;
};


// public methods
template <class ComponentClass>
TTaskPluginComponentImpl<ComponentClass>::TTaskPluginComponentImpl(const QString& pluginName, const QByteArray& typeId)
	:BaseClass(pluginName, typeId)
{
}


// reimplemented (imthype::ITaskPlugin)

template <class ComponentClass>
const imthype::ITaskPlugin::ISupplierFactory* TTaskPluginComponentImpl<ComponentClass>::GetTaskFactory() const
{
	return BaseClass::m_component.template GetInterface<imthype::ITaskPlugin::ISupplierFactory>(QByteArray());
}


} // namespace imthype


