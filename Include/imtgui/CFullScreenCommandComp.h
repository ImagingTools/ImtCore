#pragma once


// Qt includes
#include <QtCore/QProcess>

// ACF includes
#include <imod/TSingleModelObserverBase.h>
#include <icomp/CComponentBase.h>
#include <ibase/ICommandsProvider.h>
#include <ifile/IFileNameParam.h>
#include <iqtgui/IIconProvider.h>
#include <iqtgui/CHierarchicalCommand.h>
#include <ibase/TLocalizableWrap.h>
#include <iqtgui/IGuiObject.h>


namespace imtgui
{


/**
	Component for switching between applications.
*/
class CFullScreenCommandComp:
			public QObject,
			public ibase::TLocalizableWrap<icomp::CComponentBase>,
			virtual public ibase::ICommandsProvider
{
	Q_OBJECT
public:
	typedef ibase::TLocalizableWrap<icomp::CComponentBase> BaseClass;
	
	I_BEGIN_COMPONENT(CFullScreenCommandComp);
		I_REGISTER_INTERFACE(ibase::ICommandsProvider);
		I_ASSIGN(m_actionIconScreenOnAttrPtr, "ActionFullScreenOnIcon", "Icon screen on used command menu", true, ":/Icons/FullScreenOnNormal");
		I_ASSIGN(m_actionIconScreenOffAttrPtr, "ActionFullScreenOffIcon", "Icon screen off used command menu", true, ":/Icons/FullScreenOffNormal");
		I_ASSIGN(m_menuNameAttrPtr, "MenuName", "Name of the menu for the action group", true, "MenuName");
		I_ASSIGN(m_menuDescriptionOnAttrPtr, "MenuDescriptionOn", "Description for the full screen On", true, "MenuDescription");
		I_ASSIGN(m_menuDescriptionOffAttrPtr, "MenuDescriptionOff", "Description for the full screen Off", true, "MenuDescription");
		I_ASSIGN(m_rootMenuNameAttrPtr, "RootMenu", "Name of the root command", true, "");
		I_ASSIGN(m_guiObjectCompPtr, "GuiObject", "Any of the gui object", true, "GuiObject");
	I_END_COMPONENT;

	CFullScreenCommandComp();

	// reimpemented (ibase::ICommandsProvider)
	virtual const ibase::IHierarchicalCommand* GetCommands() const;

	// reimpemented (icomp::IComponent)
	virtual void OnComponentCreated();

protected:
	virtual bool CheckIsFullScreen();

	// reimpemented (ibase::TLocalizableWrap)
	virtual void OnLanguageChanged() override;

private Q_SLOTS:
	void OnCommandActivated();

protected:
	I_ATTR(QByteArray, m_actionIconScreenOnAttrPtr);
	I_ATTR(QByteArray, m_actionIconScreenOffAttrPtr);
	I_TEXTATTR(m_menuNameAttrPtr);
	I_TEXTATTR(m_menuDescriptionOnAttrPtr);
	I_TEXTATTR(m_menuDescriptionOffAttrPtr);
	I_TEXTATTR(m_rootMenuNameAttrPtr);
	I_REF(iqtgui::IGuiObject, m_guiObjectCompPtr);

	iqtgui::CHierarchicalCommand m_rootMenuCommand;
	iqtgui::CHierarchicalCommand m_mainMenuCommand;
	iqtgui::CHierarchicalCommand m_switchCommand;

};


} // namespace imtgui


