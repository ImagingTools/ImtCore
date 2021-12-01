#pragma once


// Qt includes
#include <QtCore/QMap>
#include <QtCore/QTimer>
#include <QtCore/QProcess>
#include <QtWidgets/QListView>
#include <QtWidgets/QMenu>
#include <QtGui/QStandardItemModel>
#include <QtGui/QStandardItem>

// ACF includes
#include <imod/CMultiModelDispatcherBase.h>
#include <ilog/IMessageConsumer.h>
#include <iqtgui/TDesignerGuiCompBase.h>
#include <ibase/ICommandsProvider.h>
#include <iprm/ISelectionParam.h>
#include <iqtgui/CGuiComponentBase.h>
#include <iqtgui/CHierarchicalCommand.h>
#include <iqtgui/TRestorableGuiWrap.h>
#include <imeas/INumericValue.h>
#include <iauth/ILogin.h>
#include <iauth/IRightsProvider.h>
#include <ilog/TLoggerCompWrap.h>

// ImtCore includes
#include <imtgui/IWidgetProvider.h>
#include <imtgui/IPageNavigationController.h>
#include <imtgui/CThumbPageItemGuiDelegate.h>



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
		I_ASSIGN(m_guiObjectCompPtr, "GuiObject", "The gui object, provides menu", true, "GuiObject");
		I_ASSIGN(m_commandCompPtr, "Command", "The command", true, "CHierarchicalCommand");
		I_ASSIGN(m_widgetProviderCompPtr, "WidgetProviderCompPtr", "The widget provider for positioning the GUI component", true, "WidgetProvider");
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
	I_REF(iqtgui::CHierarchicalCommand, m_commandCompPtr);
	I_REF(iqtgui::IGuiObject, m_guiObjectCompPtr);
	I_REF(imtgui::IWidgetProvider, m_widgetProviderCompPtr);

	iqtgui::CHierarchicalCommand m_rootMenuCommand;
	iqtgui::CHierarchicalCommand m_mainMenuCommand;
	QWidget* m_controlButton;
	QWidget* m_topFrame;
	QWidget* m_mainWidget;

};




} // namespace imtgui




