#include <imtgui/CMenuBasedCommandsProviderComp.h>


namespace imtgui
{


CMenuBasedCommandsProviderComp::CMenuBasedCommandsProviderComp()
{

}


const ibase::IHierarchicalCommand* CMenuBasedCommandsProviderComp::GetCommands() const
{
	return &m_rootMenuCommand;
}


// reimpemented (icomp::IComponent)

void CMenuBasedCommandsProviderComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	m_mainMenuCommand.SetName("Login");
//	m_commandCompPtr->SetVisuals("Connection status", "Connection status", "Connection status", QIcon(":/Icons/AutoUpdate"));
	m_rootMenuCommand.InsertChild(&m_mainMenuCommand);
	m_mainMenuCommand.InsertChild(m_commandCompPtr.GetPtr());

	m_mainWidget = m_widgetProviderCompPtr->GetWidgetPtr("Main");
	m_topFrame = m_widgetProviderCompPtr->GetWidgetPtr("TopFrame");
	m_controlButton = m_widgetProviderCompPtr->GetWidgetPtr("AdditionalCommandsFrame");

	Q_ASSERT(m_mainWidget != nullptr);
	Q_ASSERT(m_topFrame != nullptr);
	Q_ASSERT(m_controlButton != nullptr);

	Q_ASSERT(m_guiObjectCompPtr.IsValid() && m_commandCompPtr.IsValid());
	if(m_guiObjectCompPtr->CreateGui(m_mainWidget)){
		QObject::connect(m_commandCompPtr.GetPtr(), &QAction::triggered, this, &CMenuBasedCommandsProviderComp::OnCommandActivated);
	}
	else{
		SendCriticalMessage(0, QString("Unable to create GUI for component"));
	}
}


void CMenuBasedCommandsProviderComp::OnComponentDestroyed()
{
	if (m_guiObjectCompPtr.IsValid() && m_guiObjectCompPtr->IsGuiCreated()){
		m_guiObjectCompPtr->DestroyGui();
	}

	BaseClass::OnComponentDestroyed();
}


// private slots

void CMenuBasedCommandsProviderComp::OnCommandActivated(bool)
{
	QWidget* menuPtr = m_guiObjectCompPtr->GetWidget();
	if (menuPtr != nullptr){
		QPoint menuPos = m_controlButton->mapToGlobal(m_controlButton->rect().bottomRight());
		menuPtr->resize(10, 10);
		menuPos.rx() -= menuPtr->sizeHint().width();
		menuPos.setY(m_topFrame->mapToGlobal(m_topFrame->rect().bottomRight()).y());
		menuPtr->move(menuPos);
		menuPtr->show();
	}
}


} // namespace imtgui


