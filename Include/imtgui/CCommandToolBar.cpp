#include <imtgui/CCommandToolBar.h>


// Qt includes
#include <QtWidgets/QLayout>

// ACF includes
#include <iwidgets/iwidgets.h>
#include <iqtgui/CCommandTools.h>


namespace imtgui
{


CCommandToolBar::CCommandToolBar()
	:m_pageCommandsObserver(*this)
{
}


void CCommandToolBar::RegisterCommands(QWidget* parentWidgetPtr, const ibase::ICommandsProvider* commandsPtr)
{
	if (commandsPtr != nullptr){
		if (m_parentWidgetPtr != nullptr){
			m_commandsToolBar->deleteLater();

			m_commandsToolBar = nullptr;
		}

		m_parentWidgetPtr = parentWidgetPtr;

		m_pageCommandsObserver.RegisterObject(commandsPtr, &CCommandToolBar::OnCommandsChanged);
	}
}


void CCommandToolBar::UnregisterCommands()
{
	UpdateCommands(nullptr);

	m_pageCommandsObserver.UnregisterAllObjects();
}


// protected methods

void CCommandToolBar::OnCommandsChanged(const istd::IChangeable::ChangeSet& /*changeSet*/, const ibase::ICommandsProvider* commandsPtr)
{
	UpdateCommands(commandsPtr);
}


void CCommandToolBar::UpdateCommands(const ibase::ICommandsProvider* commandsPtr)
{
	if (m_commandsToolBar != nullptr){
		m_commandsToolBar->clear();
	}

	if (m_parentWidgetPtr == nullptr){
		return;
	}

	if (commandsPtr != nullptr){
		const iqtgui::CHierarchicalCommand* commandPtr = dynamic_cast<const iqtgui::CHierarchicalCommand*>(commandsPtr->GetCommands());
		if (commandPtr != nullptr){
			if (m_commandsToolBar == nullptr){
				m_commandsToolBar = new QToolBar(m_parentWidgetPtr);
				m_commandsToolBar->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
				m_commandsToolBar->setIconSize(QSize(24, 24));

				QLayout* layoutPtr = m_parentWidgetPtr->layout();
				if (layoutPtr != nullptr){
					layoutPtr->addWidget(m_commandsToolBar);
				}
			}

			iqtgui::CCommandTools::SetupToolbar(*commandPtr, *m_commandsToolBar);
		}
	}
}


} // namespace imtgui


