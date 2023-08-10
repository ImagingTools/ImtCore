#include <imtgui/CFullScreenCommandComp.h>


namespace imtgui
{


// public methods

CFullScreenCommandComp::CFullScreenCommandComp()
	:m_switchCommand("", 100, ibase::ICommand::CF_GLOBAL_MENU | ibase::ICommand::CF_TOOLBAR)
{
}


// reimplemented (QObject)

bool CFullScreenCommandComp::eventFilter(QObject* watchedPtr, QEvent* eventPtr)
{
	QWidget* mainWidgetPtr = GetMainWidget();
	const bool isFullScreen = CheckIsFullScreen();
	if (watchedPtr == mainWidgetPtr){
		if (eventPtr->type() == QEvent::WindowStateChange){
			const Qt::WindowStates windowStates = mainWidgetPtr->windowState();
			if (!windowStates.testFlag(Qt::WindowState::WindowFullScreen)){
				m_mainWidgetLastState = windowStates;
			}
			UpdateVisualStatus();
		}
	}

	return BaseClass2::eventFilter(watchedPtr, eventPtr);
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
	m_switchCommand.SetVisuals(*m_menuNameAttrPtr, *m_menuNameAttrPtr, *m_menuDescriptionOffAttrPtr, GetIcon(*m_actionIconScreenOffAttrPtr));
	m_rootMenuCommand.InsertChild(&m_mainMenuCommand);
	m_mainMenuCommand.InsertChild(&m_switchCommand);

	connect(&m_switchCommand, SIGNAL(triggered()), this, SLOT(OnCommandActivated()));

	EnableLocalization(true);

	QWidget* mainWidgetPtr = GetMainWidget();
	if(mainWidgetPtr != nullptr){
		mainWidgetPtr->installEventFilter(this);
	}
}


// protected methods

bool CFullScreenCommandComp::CheckIsFullScreen() const
{
	QWidget* mainWidgetPtr = GetMainWidget();

	if(mainWidgetPtr == nullptr){
		return false;
	}

	const bool retVal = mainWidgetPtr->isFullScreen();

	return retVal;
}


QWidget* CFullScreenCommandComp::GetMainWidget(QWidget* fromWidgetPtr) const
{
	// can the main widget change during execution?
	if(!m_mainWidgetPtr.isNull()){
		return m_mainWidgetPtr;
	}

	if(fromWidgetPtr == nullptr){
		if (!m_guiObjectCompPtr.IsValid()){
			return nullptr;
		}

		if (!m_guiObjectCompPtr->IsGuiCreated()){
			return nullptr;
		}
		m_mainWidgetPtr = m_guiObjectCompPtr->GetWidget();
	}
	else {
		m_mainWidgetPtr = fromWidgetPtr;
	}

	Q_ASSERT(m_mainWidgetPtr != nullptr);

	while (m_mainWidgetPtr->parentWidget() != nullptr){
		m_mainWidgetPtr = m_mainWidgetPtr->parentWidget();
	}

	return m_mainWidgetPtr;
}


void CFullScreenCommandComp::UpdateVisualStatus()
{
	bool isFullScreen = CheckIsFullScreen();
	if (isFullScreen){
		m_switchCommand.SetVisuals(*m_menuNameAttrPtr, *m_menuNameAttrPtr, *m_menuDescriptionOffAttrPtr, GetIcon(*m_actionIconScreenOffAttrPtr));
	}
	else {
		m_switchCommand.SetVisuals(*m_menuNameAttrPtr, *m_menuNameAttrPtr, *m_menuDescriptionOnAttrPtr, GetIcon(*m_actionIconScreenOnAttrPtr));
	}
}


// reimpemented (iqtgui::TDesignSchemaHandlerWrap)

void CFullScreenCommandComp::OnDesignSchemaChanged(const QByteArray& /*themeId*/)
{
	UpdateVisualStatus();
}


// reimpemented (ibase::TLocalizableWrap)

void CFullScreenCommandComp::OnLanguageChanged()
{
	OnDesignSchemaChanged(GetCurrentThemeId());
}


// private slots

void CFullScreenCommandComp::OnCommandActivated()
{
	QWidget* mainWidgetPtr = GetMainWidget();
	Q_ASSERT(mainWidgetPtr != nullptr);

	const bool isFullScreen = CheckIsFullScreen();
	if (isFullScreen){
		Q_ASSERT(!m_mainWidgetLastState.testFlag(Qt::WindowState::WindowFullScreen));

		mainWidgetPtr->setWindowState(m_mainWidgetLastState);
	}
	else{
		mainWidgetPtr->showFullScreen();
	}
}


} // namespace imtgui


