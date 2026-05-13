// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
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


void CCommandToolBar::RegisterCommands(QWidget* parentWidgetPtr, const ibase::ICommandsProvider* commandsPtr, int commandGroupId)
{
	m_groupId = commandGroupId;

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
	m_pageCommandsObserver.UnregisterAllObjects();

	if (m_parentWidgetPtr != nullptr){
		m_commandsToolBar->deleteLater();

		m_commandsToolBar = nullptr;
	}

	m_parentWidgetPtr = nullptr;
}


QToolBar* CCommandToolBar::GetToolBar() const
{
	return m_commandsToolBar;
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

		if (m_groupId >= 0){
			commandPtr = dynamic_cast<const iqtgui::CHierarchicalCommand*>(FindCommand(commandPtr, m_groupId));
		}

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


const ibase::IHierarchicalCommand* CCommandToolBar::FindCommand(const ibase::IHierarchicalCommand* commandPtr, int groupId)
{
	if (commandPtr == nullptr){
		return nullptr;
	}

	if (commandPtr->GetGroupId() == groupId){
		return commandPtr;
	}

	int count = commandPtr->GetChildsCount();
	for (int i = 0; i < count; i++){
		const ibase::IHierarchicalCommand* childPtr = dynamic_cast<const ibase::IHierarchicalCommand*>(commandPtr->GetChild(i));

		const ibase::IHierarchicalCommand* foundPtr = FindCommand(childPtr, groupId);

		if (foundPtr != nullptr){
			return foundPtr;
		}
	}

	return nullptr;
}


} // namespace imtgui


