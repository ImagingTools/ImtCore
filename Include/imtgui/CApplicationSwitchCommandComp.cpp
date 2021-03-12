#include <imtgui/CApplicationSwitchCommandComp.h>


// Qt includes
#include <QtWidgets/QMessageBox>

// ImtCore includes
#include <imtwidgets/CWindowSystem.h>


namespace imtgui
{


// public methods

CApplicationSwitchCommandComp::CApplicationSwitchCommandComp()
	:m_switchCommand("", 100, ibase::ICommand::CF_GLOBAL_MENU | ibase::ICommand::CF_TOOLBAR)
{
}


// reimpemented (ibase::ICommandsProvider)

const ibase::IHierarchicalCommand* CApplicationSwitchCommandComp::GetCommands() const
{
	return &m_rootMenuCommand;
}


// reimpemented (icomp::IComponent)

void CApplicationSwitchCommandComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	m_mainMenuCommand.SetName(*m_rootMenuNameAttrPtr);
	m_switchCommand.SetVisuals(*m_menuNameAttrPtr, *m_menuNameAttrPtr, *m_menuDescriptionAttrPtr, QIcon(*m_actionIconAttrPtr));

	m_rootMenuCommand.InsertChild(&m_mainMenuCommand);
	m_mainMenuCommand.InsertChild(&m_switchCommand);

	m_switchCommand.setVisible(m_executablePathCompPtr.IsValid());

	connect(&m_switchCommand, SIGNAL(triggered()), this, SLOT(OnCommandActivated()));
}


// private slots

void CApplicationSwitchCommandComp::OnCommandActivated()
{
	Q_ASSERT(m_executablePathCompPtr.IsValid());

	if (m_executablePathCompPtr.IsValid()){
		QString executablePath = m_executablePathCompPtr->GetPath();

		QString processTitle = QFileInfo(executablePath).baseName();

		imtwidgets::CWindowSystem::RaiseWindowByTitle(processTitle);
	}
}


} // namespace imtgui


