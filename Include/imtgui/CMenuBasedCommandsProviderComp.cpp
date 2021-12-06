#include <imtgui/CMenuBasedCommandsProviderComp.h>

// Qt includes
#include <QtWidgets/QFrame>


namespace imtgui
{


// public methods

CMenuBasedCommandsProviderComp::CMenuBasedCommandsProviderComp()
{
}


const ibase::IHierarchicalCommand* CMenuBasedCommandsProviderComp::GetCommands() const
{
	return &m_rootMenuCommand;
}


// reimpemented (ibase::TRuntimeStatusHanderCompWrap)

void CMenuBasedCommandsProviderComp::OnSystemStarted()
{
	BaseClass::OnSystemStarted();

	m_mainMenuCommand.SetName(m_menuCommandCompPtr->GetName());
	m_rootMenuCommand.InsertChild(&m_mainMenuCommand);
	m_mainMenuCommand.InsertChild(m_menuCommandCompPtr.GetPtr());

	m_mainWidgetPtr = m_menuAnchorProviderCompPtr->GetWidgetPtr("Main");
	m_topFramePtr = m_menuAnchorProviderCompPtr->GetWidgetPtr("TopFrame");
	m_anchorButtonPtr = m_menuAnchorProviderCompPtr->GetWidgetPtr("AdditionalCommandsFrame");

	Q_ASSERT(m_mainWidgetPtr != nullptr);
	Q_ASSERT(m_topFramePtr != nullptr);
	Q_ASSERT(m_anchorButtonPtr != nullptr);

	Q_ASSERT(m_menuGuiCompPtr.IsValid() && m_menuCommandCompPtr.IsValid());
	if(m_menuGuiCompPtr->CreateGui(m_mainWidgetPtr)){
		QObject::connect(m_menuCommandCompPtr.GetPtr(), &QAction::triggered, this, &CMenuBasedCommandsProviderComp::OnCommandActivated);
	}
	else{
		SendCriticalMessage(0, QString("Unable to create GUI for component ").append(m_menuGuiCompPtr->GetWidget()->objectName()));
	}
}


void CMenuBasedCommandsProviderComp::OnSystemShutdown()
{
	if (m_menuGuiCompPtr.IsValid() && m_menuGuiCompPtr->IsGuiCreated()){
		m_menuGuiCompPtr->DestroyGui();
	}

	BaseClass::OnSystemShutdown();
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


} // namespace imtgui


