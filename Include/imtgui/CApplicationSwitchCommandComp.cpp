// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtgui/CApplicationSwitchCommandComp.h>


// Qt includes
#include <QtWidgets/QMessageBox>
#include <QProcess>

// ImtCore includes
#include <imtwidgets/CWindowSystem.h>


namespace imtgui
{


// public methods

CApplicationSwitchCommandComp::CApplicationSwitchCommandComp()
	:m_switchCommand("", 100, ibase::ICommand::CF_GLOBAL_MENU | ibase::ICommand::CF_TOOLBAR)
{
}


// reimplemented (imod::CMultiModelDispatcherBase)

void CApplicationSwitchCommandComp::OnModelChanged(int modelId, const istd::IChangeable::ChangeSet& /*changeSet*/)
{
	switch (modelId){
		case MI_VISIBILITY:
		{
			bool isVisible = m_menuItemVisibilityCompPtr->IsEnabled();
			m_switchCommand.setVisible(isVisible);
			break;
		}
		case MI_NAME:
		{
			this->OnLanguageChanged();
			break;
		}
	}
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
	m_switchCommand.SetVisuals(*m_menuNameAttrPtr, *m_menuNameAttrPtr, *m_menuDescriptionAttrPtr, GetIcon(*m_actionIconAttrPtr));

	m_rootMenuCommand.InsertChild(&m_mainMenuCommand);
	m_mainMenuCommand.InsertChild(&m_switchCommand);

	m_switchCommand.setVisible(m_executablePathCompPtr.IsValid());

	if (m_actionShortcutAttrPtr.IsValid()){
		m_switchCommand.setShortcut(*m_actionShortcutAttrPtr);

		if (m_switchCommand.toolTip().size())
			m_switchCommand.setToolTip(m_switchCommand.toolTip() + " (" + *m_actionShortcutAttrPtr + ")");
	}

	connect(&m_switchCommand, SIGNAL(triggered()), this, SLOT(OnCommandActivated()));

	EnableLocalization(true);
	EnableDesignHandler(true);

	if (m_executablePathModelCompPtr.IsValid()){
		BaseClass2::RegisterModel(m_executablePathModelCompPtr.GetPtr(), MI_EXECUTABLE_PATH);
	}
	if (m_menuItemVisibilityCompPtr.IsValid() && m_menuItemVisibilityModelCompPtr.IsValid()){
		BaseClass2::RegisterModel(m_menuItemVisibilityModelCompPtr.GetPtr(), MI_VISIBILITY);
	}
}


void CApplicationSwitchCommandComp::OnComponentDestroyed()
{
	BaseClass2::UnregisterAllModels();

	BaseClass::OnComponentDestroyed();
}


// protected methods

// reimpemented (ibase::TLocalizableWrap)

void CApplicationSwitchCommandComp::OnLanguageChanged()
{
	m_switchCommand.SetVisuals(*m_menuNameAttrPtr, *m_menuNameAttrPtr, *m_menuDescriptionAttrPtr, GetIcon(*m_actionIconAttrPtr));
}


void CApplicationSwitchCommandComp::OnDesignSchemaChanged(const QByteArray& themeId)
{
	BaseClass::OnDesignSchemaChanged(themeId);

	m_switchCommand.setIcon(GetIcon(*m_actionIconAttrPtr));
}


// private slots

void CApplicationSwitchCommandComp::OnCommandActivated()
{
	Q_ASSERT(m_executablePathCompPtr.IsValid());

	if (m_executablePathCompPtr.IsValid()){
		QString executablePath = m_executablePathCompPtr->GetPath();

		QString processTitle = QFileInfo(executablePath).baseName();
		QString processTitleExe = QFileInfo(executablePath).fileName();

		bool ok = imtwidgets::CWindowSystem::RaiseWindowByProcessTitle(processTitleExe);

		if (!ok)
		{
			ok = imtwidgets::CWindowSystem::RaiseWindowByTitle(processTitle);

			// if !ok then run the app
			if (!ok){
				QStringList params;
				if (m_parametersCompPtr.IsValid()){
					params = m_parametersCompPtr->GetName().split(";");
				}

				ok = QProcess::startDetached(executablePath, params);
			}
		}
	}
}


} // namespace imtgui


