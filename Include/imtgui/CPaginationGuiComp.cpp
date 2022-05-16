#include <imtgui/CPaginationGuiComp.h>


// ACF includes
#include <iprm/IOptionsList.h>
#include <iwidgets/iwidgets.h>


namespace imtgui
{


// protected methods

// reimplemented (iqtgui::CGuiComponentBase)

void CPaginationGuiComp::OnGuiCreated()
{
	BaseClass::OnGuiCreated();

	connect(First, &QToolButton::clicked, this, &CPaginationGuiComp::OnFirstClicked);
	connect(Prev, &QToolButton::clicked, this, &CPaginationGuiComp::OnPrevClicked);
	connect(Next, &QToolButton::clicked, this, &CPaginationGuiComp::OnNextClicked);
	connect(Last, &QToolButton::clicked, this, &CPaginationGuiComp::OnLastClicked);

	UpdateWidget();
}


// reimplemented (imod::CSingleModelObserverBase)

void CPaginationGuiComp::UpdateGui(const istd::IChangeable::ChangeSet& changeSet)
{
	UpdateWidget();
}


// reimplemented (imod::IObserver)

void CPaginationGuiComp::OnGuiModelDetached()
{
	BaseClass::OnGuiModelDetached();

	UpdateWidget();
}


// protected methods

void CPaginationGuiComp::UpdateWidget()
{
	if (IsGuiCreated()){
		QHBoxLayout* layoutPtr = dynamic_cast<QHBoxLayout*>(PageButtonsFrame->layout());
		Q_ASSERT(layoutPtr != nullptr);

		iwidgets::ClearLayout(layoutPtr);

		iprm::ISelectionParam* pageSelectionPtr = GetObservedObject();
		if (pageSelectionPtr != nullptr){
			const iprm::IOptionsList* pageListPtr = pageSelectionPtr->GetSelectionConstraints();
			if (pageListPtr != nullptr){
				int pageCount = pageListPtr->GetOptionsCount();
				for (int i = 0; i < pageCount; i++){
					QString text = QString::number(i + 1);
					QToolButton* itemPtr = new QToolButton(PageButtonsFrame);
					itemPtr->setText(QString::number(i + 1));
					itemPtr->setAutoRaise(true);
					connect(itemPtr, &QToolButton::clicked, this, &CPaginationGuiComp::OnPageClicked, Qt::QueuedConnection);

					if (i == pageSelectionPtr->GetSelectedOptionIndex()){
						itemPtr->setStyleSheet("font-weight: bold;");
					}

					layoutPtr->addWidget(itemPtr);
				}

				ControlsFrame->setVisible(pageCount > 0);

				return;
			}
		}

		ControlsFrame->setVisible(false);
	}
}


// private slots

void CPaginationGuiComp::OnFirstClicked()
{
	if (!IsUpdateBlocked()){
		iprm::ISelectionParam* selectionPtr = GetObservedObject();
		if (selectionPtr != nullptr){
			UpdateBlocker blocker(this);

			selectionPtr->SetSelectedOptionIndex(0);
		}
	}
}


void CPaginationGuiComp::OnPrevClicked()
{
	if (!IsUpdateBlocked()){
		iprm::ISelectionParam* selectionPtr = GetObservedObject();
		if (selectionPtr != nullptr){
			int currentSelection = selectionPtr->GetSelectedOptionIndex();
			if (currentSelection > 0){
				UpdateBlocker blocker(this);

				selectionPtr->SetSelectedOptionIndex(currentSelection - 1);
			}
		}
	}
}


void CPaginationGuiComp::OnPageClicked()
{
	if (!IsUpdateBlocked()){
		iprm::ISelectionParam* selectionPtr = GetObservedObject();
		if (selectionPtr != nullptr){
			QToolButton* itemPtr = dynamic_cast<QToolButton*>(sender());
			if (itemPtr != nullptr){
				QString text = itemPtr->text();
				bool isOk;
				int page = text.toInt(&isOk);
				if (isOk){
					UpdateBlocker blocker(this);

					selectionPtr->SetSelectedOptionIndex(page - 1);
				}
			}
		}
	}
}


void CPaginationGuiComp::OnNextClicked()
{
	if (!IsUpdateBlocked()){
		iprm::ISelectionParam* selectionPtr = GetObservedObject();
		if (selectionPtr != nullptr){
			UpdateBlocker blocker(this);

			int currentSelection = selectionPtr->GetSelectedOptionIndex();
			selectionPtr->SetSelectedOptionIndex(currentSelection + 1);
		}
	}
}


void CPaginationGuiComp::OnLastClicked()
{
	if (!IsUpdateBlocked()){
		iprm::ISelectionParam* selectionPtr = GetObservedObject();
		if (selectionPtr != nullptr){
			const iprm::IOptionsList* optionsPtr = selectionPtr->GetSelectionConstraints();
			if (optionsPtr != nullptr){
				UpdateBlocker blocker(this);

				selectionPtr->SetSelectedOptionIndex(optionsPtr->GetOptionsCount() - 1);
			}
		}
	}
}


} // namespace imtgui


