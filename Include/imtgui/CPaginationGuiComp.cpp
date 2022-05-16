#include <imtgui/CPaginationGuiComp.h>


// ACF includes
#include <iprm/IOptionsList.h>
#include <iwidgets/iwidgets.h>


namespace imtgui
{


CPaginationGuiComp::CPaginationGuiComp()
	:m_firstVisiblePageIndex(0)
{
}


// protected methods

// reimplemented (iqtgui::CGuiComponentBase)

void CPaginationGuiComp::OnGuiCreated()
{
	BaseClass::OnGuiCreated();

	connect(First, &QToolButton::clicked, this, &CPaginationGuiComp::OnFirstClicked);
	connect(Prev, &QToolButton::clicked, this, &CPaginationGuiComp::OnPrev10Clicked);
	connect(Next, &QToolButton::clicked, this, &CPaginationGuiComp::OnNext10Clicked);
	connect(Last, &QToolButton::clicked, this, &CPaginationGuiComp::OnLastClicked);

	First->setText("");
	Last->setText("");
}


// reimplemented (imod::CSingleModelObserverBase)

void CPaginationGuiComp::UpdateGui(const istd::IChangeable::ChangeSet& changeSet)
{
	iprm::ISelectionParam* pageSelectionPtr = GetObservedObject();
	if (pageSelectionPtr != nullptr){
		const iprm::IOptionsList* pageListPtr = pageSelectionPtr->GetSelectionConstraints();
		if (pageListPtr != nullptr){
			if (m_firstVisiblePageIndex >= pageListPtr->GetOptionsCount()){
				m_firstVisiblePageIndex = pageListPtr->GetOptionsCount() / 10;
			}
		}
	}

	UpdateNavigationButtons();
	UpdatePageButtons();
}


// reimplemented (imod::IObserver)

void CPaginationGuiComp::OnGuiModelDetached()
{
	BaseClass::OnGuiModelDetached();

	UpdateNavigationButtons();
	UpdatePageButtons();
}


// reimplemented (ibase::TDesignSchemaHandlerWrap)

void CPaginationGuiComp::OnDesignSchemaChanged()
{
	UpdateIcons();
}


// protected methods

void CPaginationGuiComp::UpdateIcons()
{
	First->setIcon(QIcon(":/Icons/MoveFirst"));
	Last->setIcon(QIcon(":/Icons/MoveLast"));
}


void CPaginationGuiComp::UpdateNavigationButtons()
{
	if (IsGuiCreated()){
		iprm::ISelectionParam* pageSelectionPtr = GetObservedObject();
		if (pageSelectionPtr != nullptr){
			const iprm::IOptionsList* pageListPtr = pageSelectionPtr->GetSelectionConstraints();
			if (pageListPtr != nullptr){
				int pageCount = pageListPtr->GetOptionsCount();

				First->setEnabled(m_firstVisiblePageIndex >= 10);
				Prev->setEnabled(m_firstVisiblePageIndex >= 10);
				Next->setEnabled(m_firstVisiblePageIndex + 10 < pageCount);
				Last->setEnabled(m_firstVisiblePageIndex + 10 < pageCount);

				return;
			}
		}
	}
}


void CPaginationGuiComp::UpdatePageButtons()
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
				
				for (int i = 0; m_firstVisiblePageIndex + i < pageCount && i < 10; i++){
					int pageIndex = m_firstVisiblePageIndex + i;

					QToolButton* itemPtr = new QToolButton(PageButtonsFrame);
					itemPtr->setText(QString::number(pageIndex + 1));
					itemPtr->setAutoRaise(true);
					connect(itemPtr, &QToolButton::clicked, this, &CPaginationGuiComp::OnPageClicked, Qt::QueuedConnection);

					if (pageIndex == pageSelectionPtr->GetSelectedOptionIndex()){
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
			const iprm::IOptionsList* pageListPtr = selectionPtr->GetSelectionConstraints();
			if (pageListPtr != nullptr){
				UpdateBlocker blocker(this);

				m_firstVisiblePageIndex = 0;
				pageListPtr->GetOptionsCount() > 0 ? selectionPtr->SetSelectedOptionIndex(0) : selectionPtr->SetSelectedOptionIndex(-1);

				UpdateNavigationButtons();
				UpdatePageButtons();
			}
		}
	}
}


void CPaginationGuiComp::OnPrev10Clicked()
{
	if (!IsUpdateBlocked()){
		iprm::ISelectionParam* selectionPtr = GetObservedObject();
		if (selectionPtr != nullptr){
			const iprm::IOptionsList* pageListPtr = selectionPtr->GetSelectionConstraints();
			if (pageListPtr != nullptr){
				UpdateBlocker blocker(this);

				if (m_firstVisiblePageIndex >= 10){
					m_firstVisiblePageIndex -= 10;
				}

				selectionPtr->SetSelectedOptionIndex(selectionPtr->GetSelectedOptionIndex() - 10);

				UpdateNavigationButtons();
				UpdatePageButtons();
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

					UpdatePageButtons();
				}
			}
		}
	}
}


void CPaginationGuiComp::OnNext10Clicked()
{
	if (!IsUpdateBlocked()){
		iprm::ISelectionParam* selectionPtr = GetObservedObject();
		if (selectionPtr != nullptr){
			const iprm::IOptionsList* pageListPtr = selectionPtr->GetSelectionConstraints();
			if (pageListPtr != nullptr){
				UpdateBlocker blocker(this);

				if (m_firstVisiblePageIndex + 10 < pageListPtr->GetOptionsCount()){
					m_firstVisiblePageIndex += 10;
				}

				int pageCount = pageListPtr->GetOptionsCount();
				int newSelectedPageIndex = selectionPtr->GetSelectedOptionIndex() + 10;
				if (newSelectedPageIndex >= pageCount){
					newSelectedPageIndex = pageCount - 1;
				}

				selectionPtr->SetSelectedOptionIndex(newSelectedPageIndex);

				UpdateNavigationButtons();
				UpdatePageButtons();
			}
		}
	}
}


void CPaginationGuiComp::OnLastClicked()
{
	if (!IsUpdateBlocked()){
		iprm::ISelectionParam* selectionPtr = GetObservedObject();
		if (selectionPtr != nullptr){
			const iprm::IOptionsList* pageListPtr = selectionPtr->GetSelectionConstraints();
			if (pageListPtr != nullptr){
				UpdateBlocker blocker(this);

				m_firstVisiblePageIndex = pageListPtr->GetOptionsCount() / 10 * 10;

				int pageCount = pageListPtr->GetOptionsCount();
				pageCount > 0 ? selectionPtr->SetSelectedOptionIndex(pageCount - 1) : selectionPtr->SetSelectedOptionIndex(-1);

				UpdateNavigationButtons();
				UpdatePageButtons();
			}
		}
	}
}


} // namespace imtgui


