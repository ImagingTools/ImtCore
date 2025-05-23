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
class TJobProcessorPluginComponentImpl: public imtbase::TBasePluginComponentImpl<ComponentClass, imthype::IJobProcessorPlugin>
{
public:
	typedef imtbase::TBasePluginComponentImpl<ComponentClass, imthype::IJobProcessorPlugin> BaseClass;

	TJobProcessorPluginComponentImpl(const QString& pluginName, const QByteArray& typeId);

	// reimplemented (imthype::IJobProcessorPlugin)
	virtual const imthype::IJobProcessorFactory* GetJobProcessorFactory() const override;
};


// public methods
template <class ComponentClass>
TJobProcessorPluginComponentImpl<ComponentClass>::TJobProcessorPluginComponentImpl(const QString& pluginName, const QByteArray& typeId)
	:BaseClass(pluginName, typeId)
{
}


// reimplemented (imthype::IJobProcessorPlugin)

template <class ComponentClass>
const imthype::IJobProcessorFactory* TJobProcessorPluginComponentImpl<ComponentClass>::GetJobProcessorFactory() const
{
	return BaseClass::m_component.template GetInterface<imthype::IJobProcessorFactory>(QByteArray());
}


} // namespace imthype


