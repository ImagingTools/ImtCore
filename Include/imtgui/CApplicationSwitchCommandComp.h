#pragma once


// ACF includes
#include <imod/TSingleModelObserverBase.h>
#include <icomp/CComponentBase.h>
#include <ibase/ICommandsProvider.h>
#include <ifile/IFileNameParam.h>
#include <iqtgui/IIconProvider.h>
#include <iqtgui/CHierarchicalCommand.h>
#include <ibase/TLocalizableWrap.h>


namespace imtgui
{


/**
	Component for switching between applications.
*/
class CApplicationSwitchCommandComp:
			public QObject,
			public ibase::TLocalizableWrap<icomp::CComponentBase>,
			virtual public ibase::ICommandsProvider
{
	Q_OBJECT
public:
	typedef ibase::TLocalizableWrap<icomp::CComponentBase> BaseClass;
	
	I_BEGIN_COMPONENT(CApplicationSwitchCommandComp);
		I_REGISTER_INTERFACE(ibase::ICommandsProvider);
		I_ASSIGN(m_actionIconAttrPtr, "ActionIcon", "Icon used command menu", true, ":/Icons");
		I_ASSIGN(m_menuNameAttrPtr, "MenuName", "Name of the menu for the action group", true, "Application Switch");
		I_ASSIGN(m_menuDescriptionAttrPtr, "MenuDescription", "Description for the action group", true, "Swtich between applications");
		I_ASSIGN(m_rootMenuNameAttrPtr, "RootMenu", "Name of the root command", true, "");
		I_ASSIGN(m_executablePathCompPtr, "ExecutablePath", "Path to process exectuable", true, "ExecutablePath");
	I_END_COMPONENT;

	CApplicationSwitchCommandComp();

	// reimpemented (ibase::ICommandsProvider)
	virtual const ibase::IHierarchicalCommand* GetCommands() const;

	// reimpemented (icomp::IComponent)
	virtual void OnComponentCreated();

protected:
	// reimpemented (ibase::TLocalizableWrap)
	virtual void OnLanguageChanged() override;

private Q_SLOTS:
	void OnCommandActivated();

protected:
	I_ATTR(QByteArray, m_actionIconAttrPtr);
	I_TEXTATTR(m_menuNameAttrPtr);
	I_TEXTATTR(m_menuDescriptionAttrPtr);
	I_TEXTATTR(m_rootMenuNameAttrPtr);
	I_REF(ifile::IFileNameParam, m_executablePathCompPtr);

	iqtgui::CHierarchicalCommand m_rootMenuCommand;
	iqtgui::CHierarchicalCommand m_mainMenuCommand;
	iqtgui::CHierarchicalCommand m_switchCommand;
};


} // namespace imtgui


