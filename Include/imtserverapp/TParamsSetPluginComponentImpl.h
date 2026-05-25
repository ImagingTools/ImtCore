// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtbase/TBasePluginComponentImpl.h>
#include <imtserverapp/IParamsSetPlugin.h>


namespace imtserverapp
{


/**
	Generic implementation of task plugin.
*/
template <class ComponentClass>
class TParamsSetPluginComponentImpl: public imtbase::TBasePluginComponentImpl<ComponentClass, IParamsSetPlugin>
{
public:
	typedef imtbase::TBasePluginComponentImpl<ComponentClass, IParamsSetPlugin> BaseClass;

	TParamsSetPluginComponentImpl(const QString& pluginName, const QByteArray& typeId);

	// reimplemented (imtserverapp::IParamsSetPlugin)
	virtual const IParamsSetPlugin::IParamsSetFactory* GetParamsSetFactory() const override;
	virtual const IParamsSetPlugin::IRepresentationControllerFactory* GetRepresentationControllerFactory() const override;
};


// public methods
template <class ComponentClass>
TParamsSetPluginComponentImpl<ComponentClass>::TParamsSetPluginComponentImpl(const QString& pluginName, const QByteArray& typeId)
	:BaseClass(pluginName, typeId)
{
}


// reimplemented (imtserverapp::IParamsSetPlugin)

template <class ComponentClass>
const IParamsSetPlugin::IParamsSetFactory* TParamsSetPluginComponentImpl<ComponentClass>::GetParamsSetFactory() const
{
	return BaseClass::m_component.template GetInterface<IParamsSetPlugin::IParamsSetFactory>(QByteArray());
}


template <class ComponentClass>
const IParamsSetPlugin::IRepresentationControllerFactory* TParamsSetPluginComponentImpl<ComponentClass>::GetRepresentationControllerFactory() const
{
	return BaseClass::m_component.template GetInterface<IParamsSetPlugin::IRepresentationControllerFactory>(QByteArray());
}


} // namespace imtserverapp


