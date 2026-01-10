#pragma once


// ImtCore includes
#include <imtbase/TBasePluginComponentImpl.h>
#include <imtservice/IObjectCollectionPlugin.h>


namespace imtservice
{


/**
	Generic implementation of task plugin.
*/
template <class ComponentClass>
class TObjectCollectionPluginComponentImpl:
			public imtbase::TBasePluginComponentImpl<ComponentClass, IObjectCollectionPlugin>
{
public:
	typedef imtbase::TBasePluginComponentImpl<ComponentClass, IObjectCollectionPlugin> BaseClass;

	TObjectCollectionPluginComponentImpl(const QString& pluginName, const QByteArray& typeId);

	// reimplemented (IObjectCollectionPlugin)
	virtual const IObjectCollectionPlugin::IObjectCollectionFactory* GetObjectCollectionFactory() const override;
};


// public methods
template <class ComponentClass>
TObjectCollectionPluginComponentImpl<ComponentClass>::TObjectCollectionPluginComponentImpl(const QString& pluginName, const QByteArray& typeId)
	:BaseClass(pluginName, typeId)
{
}


// reimplemented (imtbase::IParamsSetPlugin)

template <class ComponentClass>
const IObjectCollectionPlugin::IObjectCollectionFactory* TObjectCollectionPluginComponentImpl<ComponentClass>::GetObjectCollectionFactory() const
{
	return BaseClass::m_component.template GetInterface<IObjectCollectionPlugin::IObjectCollectionFactory>(QByteArray());
}




} // namespace imtservice


