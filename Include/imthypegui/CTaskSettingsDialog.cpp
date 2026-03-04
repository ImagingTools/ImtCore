// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imthypegui/CTaskSettingsDialog.h>


namespace imthypegui
{


// public methods

CTaskSettingsDialog::CTaskSettingsDialog(QWidget* parent)
	:BaseClass(parent),
	m_taskInputsObserver(*this)
{
	setupUi(this);
}


CTaskSettingsDialog::~CTaskSettingsDialog()
{
	m_taskInputsObserver.UnregisterAllObjects();
}


void CTaskSettingsDialog::SetCurrentSelectedInputId(const QByteArray& currentSelectedInputId)
{
	m_currentSelectedInputId = currentSelectedInputId;
}


void CTaskSettingsDialog::RegisterInputsCollection(const imtbase::IObjectCollection* inputsCollectionPtr)
{
	m_taskInputsObserver.UnregisterAllObjects();

	if (inputsCollectionPtr != nullptr){
		m_taskInputsObserver.RegisterObject(inputsCollectionPtr, &CTaskSettingsDialog::OnTaskInputsChanged);
	}
}


// private methods

void CTaskSettingsDialog::OnTaskInputsChanged(const istd::IChangeable::ChangeSet& /*changeset*/, const imtbase::IObjectCollection* inputsCollectionPtr)
{
	if (InputSelector->count() != 0){
		m_currentSelectedInputId = InputSelector->currentText().toUtf8();
	}

	InputSelector->clear();

	imtbase::IObjectCollection::Ids inputIds = inputsCollectionPtr->GetElementIds();
	int selectedIndex = -1;
	for (int i = 0; i < inputIds.count(); ++i){
		QByteArray inputId = inputsCollectionPtr->GetElementInfo(inputIds[i], imtbase::ICollectionInfo::EIT_NAME).toString().toUtf8();
		InputSelector->addItem(inputId, inputId);
		if (m_currentSelectedInputId == inputId){
			selectedIndex = i;
		}
	}

	InputSelector->setCurrentIndex(selectedIndex);
}


} // namespace imthypegui


