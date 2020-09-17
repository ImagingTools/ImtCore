#pragma once


// Qt includes
#include <QtCore/QString>


namespace imtbase
{


template <class ComponentClass, class PluginInterface>
class TBasePluginComponentImpl: virtual public PluginInterface
{
public:
	TBasePluginComponentImpl(const QString& puginName, const QByteArray& typeId);

	// reimplemented (imtbase::IPluginInfo)
	virtual QString GetPluginName() const override;
	virtual QByteArray GetPluginTypeId() const override;

protected:
	mutable ComponentClass m_component;
	QString m_pluginName;
	QByteArray m_pluginTypeId;
};


// public methods

template <class ComponentClass, class PluginInterface>
TBasePluginComponentImpl<ComponentClass, PluginInterface>::TBasePluginComponentImpl(const QString& pluginName, const QByteArray& typeId)
	:m_pluginName(pluginName),
	m_pluginTypeId(typeId)
{
}


// reimplemented (imtbase::IPluginInfo)

template <class ComponentClass, class PluginInterface>
QString TBasePluginComponentImpl<ComponentClass, PluginInterface>::GetPluginName() const
{
	return m_pluginName;
}


template <class ComponentClass, class PluginInterface>
QByteArray TBasePluginComponentImpl<ComponentClass, PluginInterface>::GetPluginTypeId() const
{
	return m_pluginTypeId;
}


} // namespace imtbase


