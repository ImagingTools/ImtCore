#include <imtgui/CMenuBasedCommandsProviderComp.h>

// Qt includes
#include <QtWidgets/QFrame>


namespace imtgui
{


// public methods

CMenuBasedCommandsProviderComp::CMenuBasedCommandsProviderComp()
	:m_menuCommand(nullptr),
	  m_menuCommandVisualStatusObserver(*this)
{
}


const ibase::IHierarchicalCommand* CMenuBasedCommandsProviderComp::GetCommands() const
{
	return &m_rootMenuCommand;
}


// reimpemented (icomp::TRuntimeStatusHanderCompWrap)

void CMenuBasedCommandsProviderComp::OnSystemStarted()
{
	BaseClass::OnSystemStarted();

	istd::CChangeNotifier notifier(this);

	if (m_menuCommandVisualStatusProviderCompPtr.IsValid()){
		m_menuCommand = new iqtgui::CHierarchicalCommand(m_menuCommandVisualStatusProviderCompPtr->GetVisualStatus()->GetStatusText(), 100, ibase::ICommand::CF_GLOBAL_MENU | ibase::ICommand::CF_TOOLBAR);
		m_mainMenuCommand.SetName(m_menuCommandVisualStatusProviderCompPtr->GetVisualStatus()->GetStatusText());
	}
	else{
		m_menuCommand = new iqtgui::CHierarchicalCommand("", 100, ibase::ICommand::CF_GLOBAL_MENU | ibase::ICommand::CF_TOOLBAR);
	}

	m_rootMenuCommand.InsertChild(&m_mainMenuCommand);
	m_mainMenuCommand.InsertChild(m_menuCommand);
	m_menuCommand->SetCommandId(*m_commandIdAttrPtr);

	m_mainWidgetPtr = m_menuAnchorProviderCompPtr->GetWidgetPtr("Main");
	m_topFramePtr = m_menuAnchorProviderCompPtr->GetWidgetPtr("TopFrame");
	m_anchorButtonPtr = m_menuAnchorProviderCompPtr->GetWidgetPtr("AdditionalCommandsFrame");

	Q_ASSERT(m_mainWidgetPtr != nullptr);
	Q_ASSERT(m_topFramePtr != nullptr);
	Q_ASSERT(m_anchorButtonPtr != nullptr);

	Q_ASSERT(m_menuGuiCompPtr.IsValid());
	if (m_menuGuiCompPtr->CreateGui(m_mainWidgetPtr)){
		QObject::connect(m_menuCommand, &QAction::triggered, this, &CMenuBasedCommandsProviderComp::OnCommandActivated);
	}
	else{
		SendCriticalMessage(0, QString("Unable to create GUI for component ").append(m_menuGuiCompPtr->GetWidget()->objectName()));
	}

	if (m_menuCommandVisualStatusProviderCompPtr.IsValid()){
		const iqtgui::IVisualStatus* visualStatusPtr = m_menuCommandVisualStatusProviderCompPtr->GetVisualStatus();
		imod::IModel* visualStatusModelPtr = const_cast<imod::IModel*>(dynamic_cast<const imod::IModel*>(visualStatusPtr));
		if (visualStatusModelPtr != nullptr){
			m_menuCommandVisualStatusObserver.RegisterModel(visualStatusModelPtr);
		}
	}
}


void CMenuBasedCommandsProviderComp::OnSystemShutdown()
{
	if (m_menuGuiCompPtr.IsValid() && m_menuGuiCompPtr->IsGuiCreated()){
		m_menuGuiCompPtr->DestroyGui();
	}

	BaseClass::OnSystemShutdown();
}


void CMenuBasedCommandsProviderComp::UpdateMenuCommandVisualStatus()
{

	if (!m_menuCommandVisualStatusProviderCompPtr.IsValid()){
		return;
	}

	const iqtgui::IVisualStatus* visualStatus = m_menuCommandVisualStatusProviderCompPtr->GetVisualStatus();
	if(visualStatus == nullptr){
		return;
	}

	m_menuCommand->setIcon(visualStatus->GetStatusIcon());
	m_menuCommand->setText(visualStatus->GetStatusText());
}


// private slots

void CMenuBasedCommandsProviderComp::OnCommandActivated(bool)
{
	QWidget* menuPtr = m_menuGuiCompPtr->GetWidget();
	if (menuPtr != nullptr){
		QPoint menuPos = m_anchorButtonPtr->mapToGlobal(m_anchorButtonPtr->rect().bottomRight());
		menuPtr->resize(10, 10);
		menuPos.rx() -= menuPtr->sizeHint().width();
		menuPos.setY(m_topFramePtr->mapToGlobal(m_topFramePtr->rect().bottomRight()).y());
		QFrame* framePtr = qobject_cast<QFrame*>(m_topFramePtr);
		if(framePtr != nullptr){
			menuPos.ry() += qMax(framePtr->lineWidth(), framePtr->midLineWidth());
		}
		menuPtr->move(menuPos);

		if (menuPtr->isVisible()){
			menuPtr->hide();
		}
		else{
			menuPtr->show();
		}
	}
}


CMenuBasedCommandsProviderComp::MenuCommandVisualStatusObserver::MenuCommandVisualStatusObserver(CMenuBasedCommandsProviderComp& parent)
	:m_parent(parent)
{
}


void CMenuBasedCommandsProviderComp::MenuCommandVisualStatusObserver::OnModelChanged(int /*modelId*/, const istd::IChangeable::ChangeSet& /*changeSet*/)
{
	m_parent.UpdateMenuCommandVisualStatus();
}


} // namespace imtgui


