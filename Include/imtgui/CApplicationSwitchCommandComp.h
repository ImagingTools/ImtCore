// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <imod/CMultiModelDispatcherBase.h>
#include <icomp/CComponentBase.h>
#include <ibase/ICommandsProvider.h>
#include <ifile/IFileNameParam.h>
#include <iqtgui/IIconProvider.h>
#include <iqtgui/CHierarchicalCommand.h>
#include <iqtgui/TMakeIconProviderCompWrap.h>
#include <ibase/TLocalizableWrap.h>
#include <iprm/IEnableableParam.h>
#include <iprm/INameParam.h>


namespace imtgui
{


/**
	Component for switching between applications.
	\ingroup ApplicationFramework
*/
class CApplicationSwitchCommandComp:
			public QObject,
			public iqtgui::TMakeIconProviderCompWrap<ibase::TLocalizableWrap<icomp::CComponentBase> >,
			public imod::CMultiModelDispatcherBase,
			virtual public ibase::ICommandsProvider
{
	Q_OBJECT

public:
	typedef iqtgui::TMakeIconProviderCompWrap<ibase::TLocalizableWrap<icomp::CComponentBase> > BaseClass;
	typedef imod::CMultiModelDispatcherBase BaseClass2;

	I_BEGIN_COMPONENT(CApplicationSwitchCommandComp);
		I_REGISTER_INTERFACE(ibase::ICommandsProvider);
		I_ASSIGN(m_actionIconAttrPtr, "ActionIcon", "Icon used for command menu", true, ":/Icons");
		I_ASSIGN(m_actionShortcutAttrPtr, "ActionShortcut", "Keyboard shortcut", false, "");
		I_ASSIGN(m_menuNameAttrPtr, "MenuName", "Name of the menu for the action group", true, "Application Switch");
		I_ASSIGN(m_menuDescriptionAttrPtr, "MenuDescription", "Description for the action group", true, "Swtich between applications");
		I_ASSIGN(m_rootMenuNameAttrPtr, "RootMenu", "Name of the root command", true, "");
		I_ASSIGN(m_executablePathCompPtr, "ExecutablePath", "Path to process exectuable", true, "ExecutablePath");
		I_ASSIGN_TO(m_executablePathModelCompPtr, m_executablePathCompPtr, true);
		I_ASSIGN(m_menuItemVisibilityCompPtr, "MenuItemVisibility", "Visibility of menu item (true = visibile)", false, "MenuItemVisibility");
		I_ASSIGN_TO(m_menuItemVisibilityModelCompPtr, m_menuItemVisibilityCompPtr, true);
		I_ASSIGN(m_parametersCompPtr, "Parameters", "List of parameters to pass to the application split by ;", false, "");
	I_END_COMPONENT;

	enum ModelIds
	{
		MI_EXECUTABLE_PATH,
		MI_VISIBILITY,
		MI_NAME
	};

	CApplicationSwitchCommandComp();

	// reimplemented (imod::CMultiModelDispatcherBase)
	virtual void OnModelChanged(int modelId, const istd::IChangeable::ChangeSet& changeSet) override;

	// reimpemented (ibase::ICommandsProvider)
	virtual const ibase::IHierarchicalCommand* GetCommands() const override;

	// reimpemented (icomp::IComponent)
	virtual void OnComponentCreated() override;
	virtual void OnComponentDestroyed() override;

protected:
	// reimpemented (ibase::TLocalizableWrap)
	virtual void OnLanguageChanged() override;
	virtual void OnDesignSchemaChanged(const QByteArray& themeId) override;

private Q_SLOTS:
	void OnCommandActivated();

protected:
	I_ATTR(QByteArray, m_actionIconAttrPtr);
	I_TEXTATTR(m_menuNameAttrPtr);
	I_TEXTATTR(m_menuDescriptionAttrPtr);
	I_TEXTATTR(m_rootMenuNameAttrPtr);
	I_TEXTATTR(m_actionShortcutAttrPtr);
	I_REF(ifile::IFileNameParam, m_executablePathCompPtr);
	I_REF(imod::IModel, m_executablePathModelCompPtr);
	I_REF(iprm::IEnableableParam, m_menuItemVisibilityCompPtr);
	I_REF(imod::IModel, m_menuItemVisibilityModelCompPtr);
	I_REF(iprm::INameParam, m_parametersCompPtr);

	iqtgui::CHierarchicalCommand m_rootMenuCommand;
	iqtgui::CHierarchicalCommand m_mainMenuCommand;
	iqtgui::CHierarchicalCommand m_switchCommand;
};


} // namespace imtgui


