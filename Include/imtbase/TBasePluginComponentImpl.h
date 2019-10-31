#pragma once


// Qt includes
#include <QtCore/QString>


namespace imtbase
{


template <class ComponentClass, class PluginInterface>
class TBasePluginComponentImpl: virtual public PluginInterface
{
public:
	TBasePluginComponentImpl(const QString& puginName);

	// reimplemented (imtbase::IPluginInfo)
	virtual QString GetPluginName() const;

protected:
	mutable ComponentClass m_component;
	QString m_pluginName;
};


// public methods

template <class ComponentClass, class PluginInterface>
TBasePluginComponentImpl<ComponentClass, PluginInterface>::TBasePluginComponentImpl(const QString& pluginName)
	:m_pluginName(pluginName)
{
}


// reimplemented (imtbase::IPluginInfo)

template <class ComponentClass, class PluginInterface>
QString TBasePluginComponentImpl<ComponentClass, PluginInterface>::GetPluginName() const
{
	return m_pluginName;
}


} // namespace imtbase


