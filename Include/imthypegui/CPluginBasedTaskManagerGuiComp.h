// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtbase/PluginInterface.h>
#include <imtbase/TPluginManagerCompBase.h>

// Acula includes
#include <imthypegui/ITaskEditorPlugin.h>
#include <imthypegui/CTaskManagerGuiComp.h>


IMT_DECLARE_PLUGIN_INTERFACE(TaskEditor, imthypegui::ITaskEditorPlugin);


namespace imthypegui
{


class CPluginBasedTaskManagerGuiComp:
			public imtbase::TPluginManagerCompBase<ITaskEditorPlugin,
						IMT_CREATE_PLUGIN_FUNCTION(TaskEditor),
						IMT_DESTROY_PLUGIN_FUNCTION(TaskEditor),
						ilog::TLoggerCompWrap<CTaskManagerGuiComp>>
{
	Q_OBJECT
public:
	typedef imtbase::TPluginManagerCompBase<ITaskEditorPlugin,
				IMT_CREATE_PLUGIN_FUNCTION(TaskEditor),
				IMT_DESTROY_PLUGIN_FUNCTION(TaskEditor),
				ilog::TLoggerCompWrap<CTaskManagerGuiComp>> BaseClass;

	I_BEGIN_COMPONENT(CPluginBasedTaskManagerGuiComp);
	I_END_COMPONENT;

	CPluginBasedTaskManagerGuiComp();

protected:
	// reimplemented (TPluginManagerCompBase)
	virtual void OnPluginsCreated() override;

	// reimplemented (iqtgui::CGuiComponentBase)
	virtual void OnGuiCreated() override;
	virtual void OnGuiDestroyed() override;
	virtual void OnGuiRetranslate() override;

private:
	typedef QMap<QByteArray, ITaskEditorPlugin*> PluginsMap;
	PluginsMap m_pluginsMap;
};


} // namespace imthypegui


