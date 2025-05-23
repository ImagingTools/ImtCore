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
class TTaskParamsPluginComponentImpl : public imtbase::TBasePluginComponentImpl<ComponentClass, imthype::ITaskParamsPlugin>
{
public:
	typedef imtbase::TBasePluginComponentImpl<ComponentClass, imthype::ITaskParamsPlugin> BaseClass;

	TTaskParamsPluginComponentImpl(const QString& pluginName, const QByteArray& typeId);

	// reimplemented (imthype::ITaskParamsPlugin)
	virtual const imthype::ITaskParamsPlugin::IParamSetFactory* GetTaskParamsFactory() const override;
};


// public methods
template <class ComponentClass>
TTaskParamsPluginComponentImpl<ComponentClass>::TTaskParamsPluginComponentImpl(const QString& pluginName, const QByteArray& typeId)
	:BaseClass(pluginName, typeId)
{
}


// reimplemented (imthype::ITaskPlugin)

template <class ComponentClass>
const imthype::ITaskParamsPlugin::IParamSetFactory* TTaskParamsPluginComponentImpl<ComponentClass>::GetTaskParamsFactory() const
{
	return BaseClass::m_component.template GetInterface<imthype::ITaskParamsPlugin::IParamSetFactory>(QByteArray());
}


} // namespace imthype


