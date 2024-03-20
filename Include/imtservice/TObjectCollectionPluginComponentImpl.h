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
			public imtbase::TBasePluginComponentImpl<ComponentClass, imtservice::IObjectCollectionPlugin>
{
public:
	typedef imtbase::TBasePluginComponentImpl<ComponentClass, imtservice::IObjectCollectionPlugin> BaseClass;

	TObjectCollectionPluginComponentImpl(const QString& pluginName, const QByteArray& typeId);

	// reimplemented (imtservice::IObjectCollectionPlugin)
	virtual const imtservice::IObjectCollectionPlugin::IObjectCollectionFactory* GetObjectCollectionFactory() const override;
};


// public methods
template <class ComponentClass>
TObjectCollectionPluginComponentImpl<ComponentClass>::TObjectCollectionPluginComponentImpl(const QString& pluginName, const QByteArray& typeId)
	:BaseClass(pluginName, typeId)
{
}


// reimplemented (imtbase::IParamsSetPlugin)

template <class ComponentClass>
const imtservice::IObjectCollectionPlugin::IObjectCollectionFactory* TObjectCollectionPluginComponentImpl<ComponentClass>::GetObjectCollectionFactory() const
{
	return BaseClass::m_component.template GetInterface<imtservice::IObjectCollectionPlugin::IObjectCollectionFactory>(QByteArray());
}




} // namespace imtservice


