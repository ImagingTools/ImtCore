#include <imtgui/CVirtualKeyboardShowCommandComp.h>


namespace imtgui
{


// public methods

CVirtualKeyboardShowCommandComp::CVirtualKeyboardShowCommandComp()
	:m_switchCommand("", 100, ibase::ICommand::CF_GLOBAL_MENU | ibase::ICommand::CF_TOOLBAR)
{
}


// reimpemented (ibase::ICommandsProvider)

const ibase::IHierarchicalCommand* CVirtualKeyboardShowCommandComp::GetCommands() const
{
	return &m_rootMenuCommand;
}


// reimpemented (icomp::IComponent)

void CVirtualKeyboardShowCommandComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	m_mainMenuCommand.SetName(*m_rootMenuNameAttrPtr);
	m_switchCommand.SetVisuals(*m_menuNameAttrPtr, *m_menuNameAttrPtr, *m_menuDescriptionAttrPtr, QIcon(*m_actionIconAttrPtr));

	m_rootMenuCommand.InsertChild(&m_mainMenuCommand);
	m_mainMenuCommand.InsertChild(&m_switchCommand);

	connect(&m_switchCommand, SIGNAL(triggered()), this, SLOT(OnCommandActivated()));

	EnableLocalization(true);

	SetKeyboardCommandPath();
}


// protected methods

// reimpemented (ibase::TLocalizableWrap)

void CVirtualKeyboardShowCommandComp::OnLanguageChanged()
{
	m_switchCommand.SetVisuals(*m_menuNameAttrPtr, *m_menuNameAttrPtr, *m_menuDescriptionAttrPtr, QIcon(*m_actionIconAttrPtr));
}


// private slots

void CVirtualKeyboardShowCommandComp::OnCommandActivated()
{
	ShowKeyboard();
}


// private

void CVirtualKeyboardShowCommandComp::ShowKeyboard()
{
	HideKeyboard();

	m_keyboardProcessPtr.SetPtr(new QProcess(this));

	connect(	m_keyboardProcessPtr.GetPtr(),
				SIGNAL(stateChanged(QProcess::ProcessState)),
				this,
				SLOT(OnVirtualKeyboardStateChanged(QProcess::ProcessState)));

	m_keyboardProcessPtr->setProgram(m_winKeyboardPath);

	m_keyboardProcessPtr->startDetached();
}


void CVirtualKeyboardShowCommandComp::HideKeyboard()
{
	if (m_keyboardProcessPtr != nullptr){
		m_keyboardProcessPtr->terminate();
		m_keyboardProcessPtr->waitForFinished(2000);
		if (!m_keyboardProcessPtr->isOpen()){
			m_keyboardProcessPtr->kill();
		}
		m_keyboardProcessPtr.Reset();
	}
}


void CVirtualKeyboardShowCommandComp::SetKeyboardCommandPath()
{
	QString processPath;

	if (m_winKeyboardPath.isEmpty()){
		processPath = getenv("SystemRoot");
		if (processPath.isEmpty()){
			processPath = "C:\\Windows";
		}

		processPath += "\\System32\\osk.exe";

		QFileInfo processFileInfo(processPath);
		if (processFileInfo.exists()){
			m_winKeyboardPath = processPath;
		}
	}

}


} // namespace imtgui


