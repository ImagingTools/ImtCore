#pragma once


// ImtCore includes
#include <imtbase/TBasePluginComponentImpl.h>
#include <imtservice/IConnectionCollectionPlugin.h>


namespace imtservice
{


/**
	Generic implementation of task plugin.
*/
template <class ComponentClass>
class TConnectionCollectionPluginComponentImpl:
			public imtbase::TBasePluginComponentImpl<ComponentClass, imtservice::IConnectionCollectionPlugin>
{
public:
	typedef imtbase::TBasePluginComponentImpl<ComponentClass, imtservice::IConnectionCollectionPlugin> BaseClass;

	TConnectionCollectionPluginComponentImpl(const QString& pluginName, const QByteArray& typeId);

	// reimplemented (imtservice::IConnectionCollectionPlugin)
	virtual const imtservice::IConnectionCollectionPlugin::IConnectionCollectionFactory* GetConnectionCollectionFactory() const override;
};


// public methods
template <class ComponentClass>
TConnectionCollectionPluginComponentImpl<ComponentClass>::TConnectionCollectionPluginComponentImpl(const QString& pluginName, const QByteArray& typeId)
	:BaseClass(pluginName, typeId)
{
}


// reimplemented (imtbase::IParamsSetPlugin)

template <class ComponentClass>
const imtservice::IConnectionCollectionPlugin::IConnectionCollectionFactory* TConnectionCollectionPluginComponentImpl<ComponentClass>::GetConnectionCollectionFactory() const
{
	return BaseClass::m_component.template GetInterface<imtservice::IConnectionCollectionPlugin::IConnectionCollectionFactory>(QByteArray());
}




} // namespace imtservice


