// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtbase/TBasePluginComponentImpl.h>

// Acula includes
#include <imthypegui/ITaskEditorPlugin.h>


namespace imthypegui
{


template <class ComponentClass>
class TTaskEditorPluginComponentImpl: public imtbase::TBasePluginComponentImpl<ComponentClass, ITaskEditorPlugin>
{
public:
	typedef imtbase::TBasePluginComponentImpl<ComponentClass, ITaskEditorPlugin> BaseClass;

	TTaskEditorPluginComponentImpl(const QString& pluginName, const QByteArray& typeId);

	// reimplemented (imthypegui::ITaskEditorPlugin)
	virtual const istd::TIFactory<iqtgui::IGuiObject>* GetTaskEditorFactory() const override;
};


// public methods

template <class ComponentClass>
TTaskEditorPluginComponentImpl<ComponentClass>::TTaskEditorPluginComponentImpl(const QString& pluginName, const QByteArray& typeId)
	:BaseClass(pluginName, typeId)
{
}


// reimplemented (imthypegui::ITaskEditorPlugin)

template <class ComponentClass>
const istd::TIFactory<iqtgui::IGuiObject>* TTaskEditorPluginComponentImpl<ComponentClass>::GetTaskEditorFactory() const
{
	return BaseClass::m_component.template GetInterface<istd::TIFactory<iqtgui::IGuiObject>>(QByteArray());
}


} // namespace imthypegui


