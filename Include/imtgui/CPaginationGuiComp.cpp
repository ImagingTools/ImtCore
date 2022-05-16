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

	UpdateWidget();
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
			UpdateBlocker blocker(this);

			m_firstVisiblePageIndex = 0;

			UpdateWidget();
		}
	}
}


void CPaginationGuiComp::OnPrev10Clicked()
{
	if (!IsUpdateBlocked()){
		iprm::ISelectionParam* selectionPtr = GetObservedObject();
		if (selectionPtr != nullptr){
			UpdateBlocker blocker(this);

			if (m_firstVisiblePageIndex >= 10){
				m_firstVisiblePageIndex -= 10;
			}

			UpdateWidget();
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

					UpdateWidget();
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
			const iprm::IOptionsList* optionsPtr = selectionPtr->GetSelectionConstraints();
			if (optionsPtr != nullptr){
				UpdateBlocker blocker(this);

				if (m_firstVisiblePageIndex + 10 < optionsPtr->GetOptionsCount()){
					m_firstVisiblePageIndex += 10;
				}

				UpdateWidget();
			}
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

				m_firstVisiblePageIndex = optionsPtr->GetOptionsCount() / 10 * 10;

				UpdateWidget();
			}
		}
	}
}


} // namespace imtgui


