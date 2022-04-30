#include <imtgui/CFullScreenCommandComp.h>


namespace imtgui
{


// public methods

CFullScreenCommandComp::CFullScreenCommandComp()
	:m_switchCommand("", 100, ibase::ICommand::CF_GLOBAL_MENU | ibase::ICommand::CF_TOOLBAR)
{
}


// reimpemented (ibase::ICommandsProvider)

const ibase::IHierarchicalCommand* CFullScreenCommandComp::GetCommands() const
{
	return &m_rootMenuCommand;
}


// reimpemented (icomp::IComponent)

void CFullScreenCommandComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	m_mainMenuCommand.SetName(*m_rootMenuNameAttrPtr);
	m_switchCommand.SetVisuals(*m_menuNameAttrPtr, *m_menuNameAttrPtr, *m_menuDescriptionOffAttrPtr, QIcon(*m_actionIconScreenOffAttrPtr));
	m_rootMenuCommand.InsertChild(&m_mainMenuCommand);
	m_mainMenuCommand.InsertChild(&m_switchCommand);

	connect(&m_switchCommand, SIGNAL(triggered()), this, SLOT(OnCommandActivated()));

	EnableLocalization(true);

}


// protected methods

bool CFullScreenCommandComp::CheckIsFullScreen()
{
	if (!m_guiObjectCompPtr.IsValid()){
		return false;
	}

	if (!m_guiObjectCompPtr->IsGuiCreated()){
		return false;
	}

	bool retVal = true;

	QWidget* mainWidgetPtr = m_guiObjectCompPtr->GetWidget();
	Q_ASSERT(mainWidgetPtr != nullptr);

	while (mainWidgetPtr->parentWidget() != nullptr){
		mainWidgetPtr = mainWidgetPtr->parentWidget();
	}

	if (mainWidgetPtr->isFullScreen() == false){
		retVal = false;
	}

	return retVal;
}


// reimpemented (iqtgui::TDesignSchemaHandlerWrap)

void CFullScreenCommandComp::OnDesignSchemaChanged()
{
	bool isFullScreen = CheckIsFullScreen();
	if (isFullScreen){
		m_switchCommand.SetVisuals(*m_menuNameAttrPtr, *m_menuNameAttrPtr, *m_menuDescriptionOffAttrPtr, GetIcon(*m_actionIconScreenOffAttrPtr));
	}
	else {
		m_switchCommand.SetVisuals(*m_menuNameAttrPtr, *m_menuNameAttrPtr, *m_menuDescriptionOnAttrPtr, GetIcon(*m_actionIconScreenOnAttrPtr));
	}
}


// reimpemented (ibase::TLocalizableWrap)

void CFullScreenCommandComp::OnLanguageChanged()
{
	OnDesignSchemaChanged();
}


// private slots

void CFullScreenCommandComp::OnCommandActivated()
{
	QWidget* mainWidgetPtr = m_guiObjectCompPtr->GetWidget();
	Q_ASSERT(mainWidgetPtr != nullptr);

	while (mainWidgetPtr->parentWidget() != nullptr){
		mainWidgetPtr = mainWidgetPtr->parentWidget();
	}

	if (mainWidgetPtr->isFullScreen()){
		mainWidgetPtr->showMaximized();
		if (m_actionIconScreenOnAttrPtr.IsValid()){
			m_switchCommand.SetVisuals(*m_menuNameAttrPtr, *m_menuNameAttrPtr, *m_menuDescriptionOnAttrPtr, QIcon(*m_actionIconScreenOnAttrPtr));
		}
	}
	else{
		mainWidgetPtr->showFullScreen();
		if (m_actionIconScreenOnAttrPtr.IsValid()){
			m_switchCommand.SetVisuals(*m_menuNameAttrPtr, *m_menuNameAttrPtr, *m_menuDescriptionOffAttrPtr, QIcon(*m_actionIconScreenOffAttrPtr));
		}
	}
}


} // namespace imtgui


