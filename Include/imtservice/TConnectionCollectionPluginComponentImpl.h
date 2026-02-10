// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
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
			public imtbase::TBasePluginComponentImpl<ComponentClass, IConnectionCollectionPlugin>
{
public:
	typedef imtbase::TBasePluginComponentImpl<ComponentClass, IConnectionCollectionPlugin> BaseClass;

	TConnectionCollectionPluginComponentImpl(const QString& pluginName, const QByteArray& typeId);

	// reimplemented (IConnectionCollectionPlugin)
	virtual const IConnectionCollectionPlugin::IConnectionCollectionFactory* GetConnectionCollectionFactory() const override;
};


// public methods
template <class ComponentClass>
TConnectionCollectionPluginComponentImpl<ComponentClass>::TConnectionCollectionPluginComponentImpl(const QString& pluginName, const QByteArray& typeId)
	:BaseClass(pluginName, typeId)
{
}


// reimplemented (imtbase::IParamsSetPlugin)

template <class ComponentClass>
const IConnectionCollectionPlugin::IConnectionCollectionFactory* TConnectionCollectionPluginComponentImpl<ComponentClass>::GetConnectionCollectionFactory() const
{
	return BaseClass::m_component.template GetInterface<IConnectionCollectionPlugin::IConnectionCollectionFactory>(QByteArray());
}


} // namespace imtservice


