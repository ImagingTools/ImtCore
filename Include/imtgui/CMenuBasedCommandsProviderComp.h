#pragma once


// ACF includes
#include <ibase/ICommandsProvider.h>
#include <iqtgui/CGuiComponentBase.h>
#include <iqtgui/CHierarchicalCommand.h>
#include <ilog/TLoggerCompWrap.h>

// ImtCore includes
#include <imtgui/IWidgetProvider.h>


namespace imtgui
{


class CMenuBasedCommandsProviderComp:
		public QObject,
		public ilog::CLoggerComponentBase,
		public ibase::ICommandsProvider
{
	Q_OBJECT
public:
	typedef ilog::CLoggerComponentBase BaseClass;

	I_BEGIN_COMPONENT(CMenuBasedCommandsProviderComp);
		I_REGISTER_INTERFACE(ibase::ICommandsProvider);
		I_ASSIGN(m_menuGuiCompPtr, "MenuGuiObject", "The gui object, provides menu", true, "MenuGuiObject");
		I_ASSIGN(m_menuCommandCompPtr, "MenuCommand", "The menu command", true, "MenuCommand");
		I_ASSIGN(m_menuAnchorProviderCompPtr, "MenuAnchorProvider", "The widget provider for positioning the GUI component", true, "MenuAnchorProvider");
	I_END_COMPONENT;

	CMenuBasedCommandsProviderComp();

	// reimplemented (ibase::ICommandsProvider)
	virtual const ibase::IHierarchicalCommand* GetCommands() const override;

	// reimpemented (icomp::IComponent)
	void OnComponentCreated() override;
	void OnComponentDestroyed() override;

private Q_SLOTS:
	void OnCommandActivated(bool checked = false);

private:
	I_REF(iqtgui::IGuiObject, m_menuGuiCompPtr);
	I_REF(iqtgui::CHierarchicalCommand, m_menuCommandCompPtr);
	I_REF(imtgui::IWidgetProvider, m_menuAnchorProviderCompPtr);

	iqtgui::CHierarchicalCommand m_rootMenuCommand;
	iqtgui::CHierarchicalCommand m_mainMenuCommand;
	QWidget* m_anchorButtonPtr;
	QWidget* m_topFramePtr;
	QWidget* m_mainWidgetPtr;
};


} // namespace imtgui


