#pragma once


// ImtCore includes
#include <imtbase/TBasePluginComponentImpl.h>
#include <imtbase/IParamsSetPlugin.h>


namespace imtbase
{


/**
	Generic implementation of task plugin.
*/
template <class ComponentClass>
class TParamsSetPluginComponentImpl: public imtbase::TBasePluginComponentImpl<ComponentClass, imtbase::IParamsSetPlugin>
{
public:
	typedef imtbase::TBasePluginComponentImpl<ComponentClass, imtbase::IParamsSetPlugin> BaseClass;

	TParamsSetPluginComponentImpl(const QString& pluginName, const QByteArray& typeId);

	// reimplemented (imtbase::IParamsSetPlugin)
	virtual const imtbase::IParamsSetPlugin::IParamsSetFactory* GetParamsSetFactory() const override;
	virtual const imtbase::IParamsSetPlugin::IRepresentationControllerFactory* GetRepresentationControllerFactory() const override;
};


// public methods
template <class ComponentClass>
TParamsSetPluginComponentImpl<ComponentClass>::TParamsSetPluginComponentImpl(const QString& pluginName, const QByteArray& typeId)
	:BaseClass(pluginName, typeId)
{
}


// reimplemented (imtbase::IParamsSetPlugin)

template <class ComponentClass>
const imtbase::IParamsSetPlugin::IParamsSetFactory* TParamsSetPluginComponentImpl<ComponentClass>::GetParamsSetFactory() const
{
	return BaseClass::m_component.GetInterface<imtbase::IParamsSetPlugin::IParamsSetFactory>(QByteArray());
}


template <class ComponentClass>
const imtbase::IParamsSetPlugin::IRepresentationControllerFactory* TParamsSetPluginComponentImpl<ComponentClass>::GetRepresentationControllerFactory() const
{
	return BaseClass::m_component.GetInterface<imtbase::IParamsSetPlugin::IRepresentationControllerFactory>(QByteArray());
}


} // namespace imtbase


