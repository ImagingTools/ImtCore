// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtgui/CMultiSelectionEditorComp.h>


// ACF includes
#include <iwidgets/iwidgets.h>


namespace imtgui
{


// public methods

CMultiSelectionEditorComp::CMultiSelectionEditorComp()
{
}


// protected methods

// reimplemented (imod::CMultiModelDispatcherBase)

void CMultiSelectionEditorComp::OnModelChanged(int /*modelId*/, const istd::IChangeable::ChangeSet& /*changeSet*/)
{
	Q_ASSERT(IsGuiCreated());

	if (!IsUpdateBlocked() && IsModelAttached()){
		UpdateBlocker updateBlocker(this);

		UpdateGui(istd::IChangeable::GetAllChanges());
	}
}


// reimplemented (iqtgui::TGuiObserverWrap)

void CMultiSelectionEditorComp::OnGuiModelAttached()
{
	BaseClass::OnGuiModelAttached();

	imtbase::ISelection* selectionParamsPtr = GetObservedObject();
	if (selectionParamsPtr != NULL){
		const imtbase::ICollectionInfo* constraintsPtr = selectionParamsPtr->GetSelectionConstraints();
		if (constraintsPtr != NULL){
			const imod::IModel* constraintsModelPtr = dynamic_cast<const imod::IModel*>(constraintsPtr);
			if (constraintsModelPtr != NULL){
				RegisterModel(const_cast<imod::IModel*>(constraintsModelPtr));
			}
		}
	}
}


void CMultiSelectionEditorComp::OnGuiModelDetached()
{
	UnregisterAllModels();

	BaseClass::OnGuiModelDetached();
}


void CMultiSelectionEditorComp::UpdateGui(const istd::IChangeable::ChangeSet& /*changeSet*/)
{
	while (!m_checkBoxMap.isEmpty()){
		QPointer<QCheckBox> optionCheckPtr = m_checkBoxMap.take(m_checkBoxMap.firstKey());
		if (!optionCheckPtr.isNull()){
			disconnect(optionCheckPtr, &QCheckBox::stateChanged, this, &CMultiSelectionEditorComp::OnCheckBoxStateChanged);
		}
	}
	
	QLayout* layoutPtr = SelectorFrame->layout();
	iwidgets::ClearLayout(layoutPtr);

	const imtbase::ISelection* objectPtr = GetObservedObject();
	Q_ASSERT(objectPtr != nullptr);

	const imtbase::ICollectionInfo* collectionInfoPtr = objectPtr->GetSelectionConstraints();
	if (collectionInfoPtr != nullptr){
		imtbase::ISelection::Ids selectedOptions = objectPtr->GetSelectedIds();
		imtbase::ICollectionInfo::Ids optionIds = collectionInfoPtr->GetElementIds();
		for (const imtbase::ICollectionInfo::Id& optionId : optionIds){
			QString optionName = collectionInfoPtr->GetElementInfo(optionId, imtbase::ICollectionInfo::EIT_NAME).toString();

			QCheckBox* optionCheckPtr = new QCheckBox(SelectorFrame);
			optionCheckPtr->setText(optionName);
			optionCheckPtr->setChecked(selectedOptions.contains(optionId));

			Q_ASSERT(!m_checkBoxMap.contains(optionId));

			m_checkBoxMap.insert(optionId, optionCheckPtr);
			connect(optionCheckPtr, &QCheckBox::stateChanged, this, &CMultiSelectionEditorComp::OnCheckBoxStateChanged);

			layoutPtr->addWidget(optionCheckPtr);
		}
	}
}


// reimplemented (iqtgui::CGuiComponentBase)

void CMultiSelectionEditorComp::OnGuiCreated()
{
	QBoxLayout* layoutPtr = nullptr;
	if(*m_horizontalOrientationFlagAttrPtr){
		layoutPtr = new QHBoxLayout();
	}
	else{
		layoutPtr = new QVBoxLayout();
	}

	layoutPtr->setContentsMargins(0, 0, 0, 0);

	SelectorFrame->setLayout(layoutPtr);

	BaseClass::OnGuiCreated();
}


void CMultiSelectionEditorComp::OnGuiRetranslate()
{
	BaseClass::OnGuiRetranslate();

	UpdateGui(istd::IChangeable::GetAllChanges());
}


// private slots

void CMultiSelectionEditorComp::OnCheckBoxStateChanged(int checkState)
{
	if (!IsUpdateBlocked()){
		UpdateBlocker blockUpdate(this);

		QCheckBox* senderPtr = dynamic_cast<QCheckBox*>(sender());
		Q_ASSERT(senderPtr != nullptr);

		const imtbase::ICollectionInfo::Id checkBoxOptionId = m_checkBoxMap.key(senderPtr);
		Q_ASSERT(!checkBoxOptionId.isEmpty());

		imtbase::ISelection* objectPtr = GetObservedObject();
		Q_ASSERT(objectPtr != nullptr);

		const imtbase::ICollectionInfo* collectionInfoPtr = objectPtr->GetSelectionConstraints();
		if (collectionInfoPtr == nullptr){
			return;
		}

		imtbase::ISelection::Ids selectedOptions = objectPtr->GetSelectedIds();

		switch (checkState){
		case Qt::Checked:
			if (!selectedOptions.contains(checkBoxOptionId)){
				selectedOptions << checkBoxOptionId;
			}
			break;

		case Qt::Unchecked:
			if (selectedOptions.contains(checkBoxOptionId)){
				selectedOptions.remove(checkBoxOptionId);
			}
			break;

		default:
			break;
		}

		objectPtr->SetSelectedIds(selectedOptions);
	}
}


void CMultiSelectionEditorComp::on_SelectAllButton_clicked()
{
	imtbase::ISelection* objectPtr = GetObservedObject();
	Q_ASSERT(objectPtr != nullptr);

	const imtbase::ICollectionInfo* collectionInfoPtr = objectPtr->GetSelectionConstraints();
	if (collectionInfoPtr != nullptr){
		imtbase::ICollectionInfo::Ids optionIds = collectionInfoPtr->GetElementIds();

		imtbase::ISelection::Ids selectedIds;
		for (const imtbase::ICollectionInfo::Id& selectionId : optionIds){
			selectedIds.insert(selectionId);
		}

		objectPtr->SetSelectedIds(selectedIds);
	}
}


void CMultiSelectionEditorComp::on_DeselectAllButton_clicked()
{
	imtbase::ISelection* objectPtr = GetObservedObject();
	Q_ASSERT(objectPtr != nullptr);

	objectPtr->SetSelectedIds(imtbase::ISelection::Ids());
}


void CMultiSelectionEditorComp::on_InvertButton_clicked()
{
	imtbase::ISelection* objectPtr = GetObservedObject();
	Q_ASSERT(objectPtr != nullptr);

	imtbase::ISelection::Ids invertedIds;

	imtbase::ISelection::Ids selectedIds = objectPtr->GetSelectedIds();

	const imtbase::ICollectionInfo* collectionInfoPtr = objectPtr->GetSelectionConstraints();
	if (collectionInfoPtr != nullptr){
		imtbase::ICollectionInfo::Ids optionIds = collectionInfoPtr->GetElementIds();
		for (const imtbase::ICollectionInfo::Id& optionId : optionIds){
			if (!selectedIds.contains(optionId)){
				invertedIds.insert(optionId);
			}
		}
	}

	objectPtr->SetSelectedIds(invertedIds);
}


void CMultiSelectionEditorComp::on_ExclusiveModeButton_toggled(bool /*toggled*/)
{
}


} // namespace imtgui


