// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imthypegui/CTaskSettingsGuiComp.h>


// Qt includes
#include <QtWidgets/QMessageBox>

// ACF includes
#include <iqtgui/CGuiComponentDialog.h>

// ImtCore includes
#include <imtbase/MetaTypes.h>

// Acula includes
#include <imthype/ITaskCollection.h>


namespace imthypegui
{


// public methods

CTaskSettingsGuiComp::CTaskSettingsGuiComp()
	:m_inputsObserver(*this)
{
#if QT_VERSION > 0x060000
	m_taskNameValidator.setRegularExpression(QRegularExpression("^[^\\\\/:\\*\\?\"\\<\\>\\|\\+]+$"));
#else
	m_taskNameValidator.setRegExp(QRegExp("^[^\\\\/:\\*\\?\"\\<\\>\\|\\+]+$"));
#endif
}


// reimplemented (iqtgui::TGuiObserverWrap)

void CTaskSettingsGuiComp::OnGuiModelAttached()
{
	BaseClass::OnGuiModelAttached();

	const imtbase::IObjectCollection* inputsCollectionPtr = GetObservedObject()->GetTaskInputs();
	if (inputsCollectionPtr != nullptr){
		m_inputsObserver.RegisterObject(inputsCollectionPtr, &CTaskSettingsGuiComp::OnInputsChanged);
	}

	InputPageStack->setCurrentIndex((inputsCollectionPtr != nullptr) ? 0 : 1);

	imthype::ITaskSettings* taskSettingsPtr = GetObservedObject();
	if (taskSettingsPtr != nullptr){
		m_taskId = taskSettingsPtr->GetUserTaskId();
	}
}


void CTaskSettingsGuiComp::OnGuiModelDetached()
{
	m_inputsObserver.UnregisterAllObjects();

	BaseClass::OnGuiModelDetached();
}


void CTaskSettingsGuiComp::UpdateModel() const
{
	imthype::ITaskSettings* taskSettingsPtr = GetObservedObject();
	if (taskSettingsPtr != nullptr){
		int pos = 0;
		QString taskName = TaskNameEdit->text().trimmed();
		if (m_taskNameValidator.validate(taskName, pos) == QValidator::Acceptable){
			taskSettingsPtr->SetTaskName(taskName);
		}

		taskSettingsPtr->SetTaskDescription(TaskDescriptionEdit->text());
		taskSettingsPtr->SetTaskEnabled(ActiveCheck->isChecked());
		taskSettingsPtr->SetUserTaskId(TaskUserIdEdit->text().toUtf8());
		m_taskId = taskSettingsPtr->GetUserTaskId();

		if (taskSettingsPtr->GetTaskInputs() == nullptr){
			taskSettingsPtr->SetTaskInputId(InputIdEdit->text().toUtf8());
		}
		else {
			taskSettingsPtr->SetTaskInputId(InputSelector->currentText().toUtf8());
		}
	}
}


void CTaskSettingsGuiComp::UpdateGui(const istd::IChangeable::ChangeSet& /*changeSet*/)
{
	imthype::ITaskSettings* taskSettingsPtr = GetObservedObject();
	if (taskSettingsPtr != nullptr){
		TaskNameEdit->setText(taskSettingsPtr->GetTaskName());
		TaskDescriptionEdit->setText(taskSettingsPtr->GetTaskDescription());
		ActiveCheck->setChecked(taskSettingsPtr->GetTaskEnabled());
		TaskUserIdEdit->setText(taskSettingsPtr->GetUserTaskId());
		InputIdEdit->setText(taskSettingsPtr->GetTaskInputId());

		int oldIndex = InputSelector->currentIndex();
		InputSelector->blockSignals(true);
		InputSelector->setCurrentIndex(-1);
		for (int i = 0; i < InputSelector->count(); i++){
			if (InputSelector->itemText(i) == taskSettingsPtr->GetTaskInputId()){
				InputSelector->setCurrentIndex(i);
				break;
			}
		}
		InputSelector->blockSignals(false);

		if (oldIndex != InputSelector->currentIndex()){
			taskSettingsPtr->SetTaskInputId(InputSelector->currentText().toUtf8());
		}
	}
}


// reimplemented (iqtgui::CGuiComponentBase)

void CTaskSettingsGuiComp::OnGuiCreated()
{
	BaseClass::OnGuiCreated();

	TaskNameEdit->setValidator(&m_taskNameValidator);

	ShowAcquisitionManagerButton->setVisible(m_taskInputManagerGuiCompPtr.IsValid() && m_taskInputManagerObserverCompPtr.IsValid());
}


void CTaskSettingsGuiComp::OnGuiDesignChanged()
{
	BaseClass::OnGuiDesignChanged();

	ShowAcquisitionManagerButton->setIcon(GetIcon(":/Icons/Edit"));
}


// reimplemented (iqtgui::IGuiObject)

void CTaskSettingsGuiComp::OnTryClose(bool* ignoredPtr)
{
	if (ignoredPtr != nullptr){
		int pos;
		QString name = TaskNameEdit->text().trimmed();
		if (m_taskNameValidator.validate(name, pos) != QValidator::Acceptable){
			QMessageBox::critical(nullptr, tr("Error"), tr("The task name contains some not allowed characters"));

			*ignoredPtr = true;
		}
	}
}


// private Q_SLOTS

void CTaskSettingsGuiComp::on_TaskNameEdit_editingFinished()
{
	DoUpdateModel();
}


void CTaskSettingsGuiComp::on_TaskDescriptionEdit_editingFinished()
{
	DoUpdateModel();
}


void CTaskSettingsGuiComp::on_TaskUserIdEdit_editingFinished()
{
	if (TaskUserIdEdit->text().isEmpty()){
		TaskUserIdEdit->setText(m_taskId);
	}

	DoUpdateModel();
}


void CTaskSettingsGuiComp::on_InputIdEdit_editingFinished()
{
	DoUpdateModel();
}


void CTaskSettingsGuiComp::on_InputSelector_currentTextChanged(const QString& /*text*/)
{
	DoUpdateModel();
}


void CTaskSettingsGuiComp::on_ActiveCheck_stateChanged(int /*state*/)
{
	DoUpdateModel();
}


void CTaskSettingsGuiComp::on_ShowAcquisitionManagerButton_clicked()
{
	ShowInputsManager();
}


// private methods

void CTaskSettingsGuiComp::OnInputsChanged(const istd::IChangeable::ChangeSet& /*changeset*/, const imtbase::IObjectCollection* inputsCollectionPtr)
{
	InputSelector->blockSignals(true);
	InputSelector->clear();

	imtbase::IObjectCollection::Ids inputIds = inputsCollectionPtr->GetElementIds();
	for (int i = 0; i < inputIds.count(); ++i){
		QByteArray inputId = inputsCollectionPtr->GetElementInfo(inputIds[i], imtbase::ICollectionInfo::EIT_NAME).toString().toUtf8();
		InputSelector->addItem(inputId, inputId);
	}

	InputSelector->blockSignals(false);

	UpdateBlocker updateBlocker(this);
	Q_UNUSED(updateBlocker);

	UpdateGui(istd::IChangeable::ChangeSet(istd::IChangeable::CF_ANY));
}


void CTaskSettingsGuiComp::ShowInputsManager()
{
	if (m_taskInputManagerGuiCompPtr.IsValid() && m_taskInputManagerObserverCompPtr.IsValid()){
		imthype::ITaskSettings* taskSettingsPtr = GetObservedObject();
		if (taskSettingsPtr != nullptr){
			const imtbase::IObjectCollection* collectionPtr = taskSettingsPtr->GetTaskInputs();
			const imod::IModel* modelPtr = dynamic_cast<const imod::IModel*>(collectionPtr);

			if (collectionPtr != nullptr && modelPtr != nullptr){
				if (m_taskInputManagerGuiCompPtr.IsValid()){
					if ((const_cast<imod::IModel*>(modelPtr))->AttachObserver(m_taskInputManagerObserverCompPtr.GetPtr())){
						iqtgui::CGuiComponentDialog dialog(m_taskInputManagerGuiCompPtr.GetPtr(), QDialogButtonBox::Close, false, GetWidget());
						dialog.SetDialogGeometry(0.5);
						dialog.setWindowTitle(tr("Task Input Manager"));

						dialog.exec();

						(const_cast<imod::IModel*>(modelPtr))->DetachObserver(m_taskInputManagerObserverCompPtr.GetPtr());
					}
				}
			}
		}
	}
}


} // namespace imthypegui


