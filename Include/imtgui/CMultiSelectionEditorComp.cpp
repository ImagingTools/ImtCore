#include <imtgui/CMultiSelectionEditorComp.h>


// Qt includes
#include <QtWidgets/QCheckBox>

// ACF includes
#include <iwidgets/iwidgets.h>

// ImtCore includes
#include <imtbase/ICollectionInfo.h>


namespace imtgui
{


// public methods

CMultiSelectionEditorComp::CMultiSelectionEditorComp()
{
}


// protected methods

// reimplemented (imod::CMultiModelDispatcherBase)

void CMultiSelectionEditorComp::OnModelChanged(int modelId, const istd::IChangeable::ChangeSet& changeSet)
{
	Q_ASSERT(IsGuiCreated());

	if (!IsUpdateBlocked() && IsModelAttached()) {
		UpdateBlocker updateBlocker(this);

		UpdateGui(istd::IChangeable::GetAllChanges());
	}
}


// reimplemented (iqtgui::TGuiObserverWrap)

void CMultiSelectionEditorComp::OnGuiModelAttached()
{
	BaseClass::OnGuiModelAttached();

	imtbase::ISelection* selectionParamsPtr = GetObservedObject();
	if (selectionParamsPtr != NULL) {
		const imtbase::ICollectionInfo* constraintsPtr = selectionParamsPtr->GetSelectionConstraints();
		if (constraintsPtr != NULL) {
			const imod::IModel* constraintsModelPtr = dynamic_cast<const imod::IModel*>(constraintsPtr);
			if (constraintsModelPtr != NULL) {
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
	QLayout* layoutPtr = SelectorFrame->layout();
	iwidgets::ClearLayout(layoutPtr);

	const imtbase::ISelection* objectPtr = GetObservedObject();
	Q_ASSERT(objectPtr != nullptr);

	const imtbase::ICollectionInfo* collectionInfoPtr = objectPtr->GetSelectionConstraints();
	if (collectionInfoPtr != nullptr) {
		imtbase::ISelection::Ids selectedOptions = objectPtr->GetSelectedIds();
		imtbase::ICollectionInfo::Ids optionIds = collectionInfoPtr->GetElementIds();
		for (const imtbase::ICollectionInfo::Id& optionId : optionIds) {
			QString optionName = collectionInfoPtr->GetElementInfo(optionId, imtbase::ICollectionInfo::EIT_NAME).toString();

			QCheckBox* optionCheck= new QCheckBox(SelectorFrame);
			optionCheck->setText(optionName);
			optionCheck->setChecked(selectedOptions.contains(optionId));

			layoutPtr->addWidget(optionCheck);
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


} // namespace imtgui


